
/*
 * Copyright 2010 Jeff Garzik
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.  See COPYING for more details.
 */

#include "cpuminer-config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#ifndef WIN32
#include <sys/resource.h>
#endif
#include <pthread.h>
#include <getopt.h>
#include <jansson.h>
#include <curl/curl.h>
#include "compat.h"
#include "miner.h"

#define PROGRAM_NAME		"minerd"
#define DEF_RPC_URL		"http://127.0.0.1:8332/"
#define DEF_RPC_USERPASS	"rpcuser:rpcpass"

enum sha256_algos {
	ALGO_C,			/* plain C */
	ALGO_4WAY,		/* parallel SSE2 */
	ALGO_VIA,		/* VIA padlock */
	ALGO_CRYPTOPP,		/* Crypto++ (C) */
	ALGO_CRYPTOPP_ASM32,	/* Crypto++ 32-bit assembly */
};

static const char *algo_names[] = {
	[ALGO_C]		= "c",
#ifdef WANT_SSE2_4WAY
	[ALGO_4WAY]		= "4way",
#endif
#ifdef WANT_VIA_PADLOCK
	[ALGO_VIA]		= "via",
#endif
	[ALGO_CRYPTOPP]		= "cryptopp",
#ifdef WANT_CRYPTOPP_ASM32
	[ALGO_CRYPTOPP_ASM32]	= "cryptopp_asm32",
#endif
};

bool opt_debug = false;
bool opt_protocol = false;
static bool opt_quiet = false;
static int opt_retries = 10;
static int opt_fail_pause = 30;
static int opt_scantime = 5;
static json_t *opt_config;
static const bool opt_time = true;
static enum sha256_algos opt_algo = ALGO_C;
static int opt_n_threads = 1;
static char *rpc_url;
static char *userpass;


struct option_help {
	const char	*name;
	const char	*helptext;
};

static struct option_help options_help[] = {
	{ "help",
	  "(-h) Display this help text" },

	{ "config FILE",
	  "(-c FILE) JSON-format configuration file (default: none)\n"
	  "See example-cfg.json for an example configuration." },

	{ "algo XXX",
	  "(-a XXX) Specify sha256 implementation:\n"
	  "\tc\t\tLinux kernel sha256, implemented in C (default)"
#ifdef WANT_SSE2_4WAY
	  "\n\t4way\t\ttcatm's 4-way SSE2 implementation"
#endif
#ifdef WANT_VIA_PADLOCK
	  "\n\tvia\t\tVIA padlock implementation"
#endif
	  "\n\tcryptopp\tCrypto++ C/C++ implementation"
#ifdef WANT_CRYPTOPP_ASM32
	  "\n\tcryptopp_asm32\tCrypto++ 32-bit assembler implementation"
#endif
	  },

	{ "quiet",
	  "(-q) Disable per-thread hashmeter output (default: off)" },

	{ "debug",
	  "(-D) Enable debug output (default: off)" },

	{ "protocol-dump",
	  "(-P) Verbose dump of protocol-level activities (default: off)" },

	{ "retries N",
	  "(-r N) Number of times to retry, if JSON-RPC call fails\n"
	  "\t(default: 10; use -1 for \"never\")" },

	{ "retry-pause N",
	  "(-R N) Number of seconds to pause, between retries\n"
	  "\t(default: 30)" },

	{ "scantime N",
	  "(-s N) Upper bound on time spent scanning current work,\n"
	  "\tin seconds. (default: 5)" },

	{ "threads N",
	  "(-t N) Number of miner threads (default: 1)" },

	{ "url URL",
	  "URL for bitcoin JSON-RPC server "
	  "(default: " DEF_RPC_URL ")" },

	{ "userpass USERNAME:PASSWORD",
	  "Username:Password pair for bitcoin JSON-RPC server "
	  "(default: " DEF_RPC_USERPASS ")" },
};

static struct option options[] = {
	{ "help", 0, NULL, 'h' },
	{ "algo", 1, NULL, 'a' },
	{ "config", 1, NULL, 'c' },
	{ "quiet", 0, NULL, 'q' },
	{ "debug", 0, NULL, 'D' },
	{ "protocol-dump", 0, NULL, 'P' },
	{ "threads", 1, NULL, 't' },
	{ "retries", 1, NULL, 'r' },
	{ "retry-pause", 1, NULL, 'R' },
	{ "scantime", 1, NULL, 's' },
	{ "url", 1, NULL, 1001 },
	{ "userpass", 1, NULL, 1002 },
	{ }
};

struct work {
	unsigned char	data[128];
	unsigned char	hash1[64];
	unsigned char	midstate[32];
	unsigned char	target[32];

	unsigned char	hash[32];
};

static bool jobj_binary(const json_t *obj, const char *key,
			void *buf, size_t buflen)
{
	const char *hexstr;
	json_t *tmp;

	tmp = json_object_get(obj, key);
	if (!tmp) {
		fprintf(stderr, "JSON key '%s' not found\n", key);
		return false;
	}
	hexstr = json_string_value(tmp);
	if (!hexstr) {
		fprintf(stderr, "JSON key '%s' is not a string\n", key);
		return false;
	}
	if (!hex2bin(buf, hexstr, buflen))
		return false;

	return true;
}

static bool work_decode(const json_t *val, struct work *work)
{
	if (!jobj_binary(val, "midstate",
			 work->midstate, sizeof(work->midstate))) {
		fprintf(stderr, "JSON inval midstate\n");
		goto err_out;
	}

	if (!jobj_binary(val, "data", work->data, sizeof(work->data))) {
		fprintf(stderr, "JSON inval data\n");
		goto err_out;
	}

	if (!jobj_binary(val, "hash1", work->hash1, sizeof(work->hash1))) {
		fprintf(stderr, "JSON inval hash1\n");
		goto err_out;
	}

	if (!jobj_binary(val, "target", work->target, sizeof(work->target))) {
		fprintf(stderr, "JSON inval target\n");
		goto err_out;
	}

	memset(work->hash, 0, sizeof(work->hash));

	return true;

err_out:
	return false;
}

static void submit_work(CURL *curl, struct work *work)
{
	char *hexstr = NULL;
	json_t *val, *res;
	char s[345];

	/* build hex string */
	hexstr = bin2hex(work->data, sizeof(work->data));
	if (!hexstr) {
		fprintf(stderr, "submit_work OOM\n");
		goto out;
	}

	/* build JSON-RPC request */
	sprintf(s,
	      "{\"method\": \"getwork\", \"params\": [ \"%s\" ], \"id\":1}\r\n",
		hexstr);

	if (opt_debug)
		fprintf(stderr, "DBG: sending RPC call:\n%s", s);

	/* issue JSON-RPC request */
	val = json_rpc_call(curl, rpc_url, userpass, s);
	if (!val) {
		fprintf(stderr, "submit_work json_rpc_call failed\n");
		goto out;
	}

	res = json_object_get(val, "result");

	printf("PROOF OF WORK RESULT: %s\n",
		json_is_true(res) ? "true (yay!!!)" : "false (booooo)");

	json_decref(val);

out:
	free(hexstr);
}

static bool get_work(CURL *curl, struct work *work)
{
	static const char *rpc_req =
		"{\"method\": \"getwork\", \"params\": [], \"id\":0}\r\n";
	json_t *val;
	bool rc;

	val = json_rpc_call(curl, rpc_url, userpass, rpc_req);
	if (!val)
		return false;

	rc = work_decode(json_object_get(val, "result"), work);

	json_decref(val);

	return rc;
}

static void hashmeter(int thr_id, const struct timeval *diff,
		      unsigned long hashes_done)
{
	double khashes, secs;

	khashes = hashes_done / 1000.0;
	secs = (double)diff->tv_sec + ((double)diff->tv_usec / 1000000.0);

	if (!opt_quiet)
		printf("HashMeter(%d): %lu hashes, %.2f khash/sec\n",
		       thr_id, hashes_done,
		       khashes / secs);
}

static void *miner_thread(void *thr_id_int)
{
	int thr_id = (unsigned long) thr_id_int;
	int failures = 0;
	uint32_t max_nonce = 0xffffff;
	CURL *curl;

	curl = curl_easy_init();
	if (!curl) {
		fprintf(stderr, "CURL initialization failed\n");
		return NULL;
	}

	while (1) {
		struct work work __attribute__((aligned(128)));
		unsigned long hashes_done;
		struct timeval tv_start, tv_end, diff;
		bool rc;

		/* obtain new work from bitcoin */
		if (!get_work(curl, &work)) {
			fprintf(stderr, "json_rpc_call failed, ");

			if ((opt_retries >= 0) && (++failures > opt_retries)) {
				fprintf(stderr, "terminating thread\n");
				return NULL;	/* exit thread */
			}

			/* pause, then restart work loop */
			fprintf(stderr, "retry after %d seconds\n",
				opt_fail_pause);
			sleep(opt_fail_pause);
			continue;
		}

		hashes_done = 0;
		gettimeofday(&tv_start, NULL);

		/* scan nonces for a proof-of-work hash */
		switch (opt_algo) {
		case ALGO_C:
			rc = scanhash_c(work.midstate, work.data + 64,
				        work.hash1, work.hash, work.target,
					max_nonce, &hashes_done);
			break;

#ifdef WANT_SSE2_4WAY
		case ALGO_4WAY: {
			unsigned int rc4 =
				ScanHash_4WaySSE2(work.midstate, work.data + 64,
						  work.hash1, work.hash,
						  work.target,
						  max_nonce, &hashes_done);
			rc = (rc4 == -1) ? false : true;
			}
			break;
#endif

#ifdef WANT_VIA_PADLOCK
		case ALGO_VIA:
			rc = scanhash_via(work.data, work.target,
					  max_nonce, &hashes_done);
			break;
#endif
		case ALGO_CRYPTOPP:
			rc = scanhash_cryptopp(work.midstate, work.data + 64,
				        work.hash1, work.hash, work.target,
					max_nonce, &hashes_done);
			break;

#ifdef WANT_CRYPTOPP_ASM32
		case ALGO_CRYPTOPP_ASM32:
			rc = scanhash_asm32(work.midstate, work.data + 64,
				        work.hash1, work.hash, work.target,
					max_nonce, &hashes_done);
			break;
#endif

		default:
			/* should never happen */
			return NULL;
		}

		/* record scanhash elapsed time */
		gettimeofday(&tv_end, NULL);
		timeval_subtract(&diff, &tv_end, &tv_start);

		hashmeter(thr_id, &diff, hashes_done);

		/* adjust max_nonce to meet target scan time */
		if (diff.tv_sec > (opt_scantime * 2))
			max_nonce /= 2;			/* large decrease */
		else if ((diff.tv_sec > opt_scantime) &&
			 (max_nonce > 1500000))
			max_nonce -= 1000000;		/* small decrease */
		else if ((diff.tv_sec < opt_scantime) &&
			 (max_nonce < 0xffffec76))
			max_nonce += 100000;		/* small increase */

		/* if nonce found, submit work */
		if (rc)
			submit_work(curl, &work);

		failures = 0;
	}

	curl_easy_cleanup(curl);

	return NULL;
}

static void show_usage(void)
{
	int i;

	printf("minerd version %s\n\n", VERSION);
	printf("Usage:\tminerd [options]\n\nSupported options:\n");
	for (i = 0; i < ARRAY_SIZE(options_help); i++) {
		struct option_help *h;

		h = &options_help[i];
		printf("--%s\n%s\n\n", h->name, h->helptext);
	}

	exit(1);
}

static void parse_arg (int key, char *arg)
{
	int v, i;

	switch(key) {
	case 'a':
		for (i = 0; i < ARRAY_SIZE(algo_names); i++) {
			if (algo_names[i] &&
			    !strcmp(arg, algo_names[i])) {
				opt_algo = i;
				break;
			}
		}
		if (i == ARRAY_SIZE(algo_names))
			show_usage();
		break;
	case 'c': {
		json_error_t err;
		if (opt_config)
			json_decref(opt_config);
		opt_config = json_load_file(arg, &err);
		if (!json_is_object(opt_config)) {
			fprintf(stderr, "JSON decode of %s failed\n", arg);
			show_usage();
		}
		break;
	}
	case 'q':
		opt_quiet = true;
		break;
	case 'D':
		opt_debug = true;
		break;
	case 'P':
		opt_protocol = true;
		break;
	case 'r':
		v = atoi(arg);
		if (v < -1 || v > 9999)	/* sanity check */
			show_usage();

		opt_retries = v;
		break;
	case 'R':
		v = atoi(arg);
		if (v < 1 || v > 9999)	/* sanity check */
			show_usage();

		opt_fail_pause = v;
		break;
	case 's':
		v = atoi(arg);
		if (v < 1 || v > 9999)	/* sanity check */
			show_usage();

		opt_scantime = v;
		break;
	case 't':
		v = atoi(arg);
		if (v < 1 || v > 9999)	/* sanity check */
			show_usage();

		opt_n_threads = v;
		break;
	case 1001:			/* --url */
		if (strncmp(arg, "http://", 7) &&
		    strncmp(arg, "https://", 8))
			show_usage();

		free(rpc_url);
		rpc_url = strdup(arg);
		break;
	case 1002:			/* --userpass */
		if (!strchr(arg, ':'))
			show_usage();

		free(userpass);
		userpass = strdup(arg);
		break;
	default:
		show_usage();
	}
}

static void parse_config(void)
{
	int i;
	json_t *val;

	if (!json_is_object(opt_config))
		return;

	for (i = 0; i < ARRAY_SIZE(options); i++) {
		if (!options[i].name)
			break;
		if (!strcmp(options[i].name, "config"))
			continue;

		val = json_object_get(opt_config, options[i].name);
		if (!val)
			continue;

		if (options[i].has_arg && json_is_string(val)) {
			char *s = strdup(json_string_value(val));
			if (!s)
				break;
			parse_arg(options[i].val, s);
			free(s);
		} else if (!options[i].has_arg && json_is_true(val))
			parse_arg(options[i].val, "");
		else
			fprintf(stderr, "JSON option %s invalid\n",
				options[i].name);
	}
}

static void parse_cmdline(int argc, char *argv[])
{
	int key;

	while (1) {
		key = getopt_long(argc, argv, "a:c:qDPr:s:t:h?", options, NULL);
		if (key < 0)
			break;

		parse_arg(key, optarg);
	}

	parse_config();
}

int main (int argc, char *argv[])
{
	int i;
	pthread_t *t_all;

	rpc_url = strdup(DEF_RPC_URL);
	userpass = strdup(DEF_RPC_USERPASS);

	/* parse command line */
	parse_cmdline(argc, argv);

	/* set our priority to the highest (aka "nicest, least intrusive") */
	if (setpriority(PRIO_PROCESS, 0, 19))
		perror("setpriority");

	t_all = calloc(opt_n_threads, sizeof(pthread_t));
	if (!t_all)
		return 1;

	/* start mining threads */
	for (i = 0; i < opt_n_threads; i++) {
		if (pthread_create(&t_all[i], NULL, miner_thread,
				   (void *)(unsigned long) i)) {
			fprintf(stderr, "thread %d create failed\n", i);
			return 1;
		}

		sleep(1);	/* don't pound RPC server all at once */
	}

	fprintf(stderr, "%d miner threads started, "
		"using SHA256 '%s' algorithm.\n",
		opt_n_threads,
		algo_names[opt_algo]);

	/* main loop - simply wait for all threads to exit */
	for (i = 0; i < opt_n_threads; i++)
		pthread_join(t_all[i], NULL);

	fprintf(stderr, "all threads dead, fred. exiting.\n");

	return 0;
}

