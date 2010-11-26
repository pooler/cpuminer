
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
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <getopt.h>
#include <jansson.h>
#include "miner.h"

#define PROGRAM_NAME		"minerd"
#define DEF_RPC_URL		"http://127.0.0.1:8332/"
#define DEF_RPC_USERPASS	"rpcuser:rpcpass"

#include "sha256_generic.c"

enum {
	STAT_SLEEP_INTERVAL		= 10,
	STAT_CTR_INTERVAL		= 10000000,
};

static bool opt_debug;
bool opt_protocol = false;
static bool program_running = true;
static const bool opt_time = true;
static int opt_n_threads = 1;
static pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;
static uint64_t hash_ctr;
static char *rpc_url = DEF_RPC_URL;
static char *userpass = DEF_RPC_USERPASS;


struct option_help {
	const char	*name;
	const char	*helptext;
};

static struct option_help options_help[] = {
	{ "help",
	  "(-h) Display this help text" },

	{ "debug",
	  "(-D) Enable debug output" },

	{ "protocol-dump",
	  "(-P) Verbose dump of protocol-level activities" },

	{ "threads",
	  "(-t N) Number of miner threads (default: 1)" },

	{ "url",
	  "URL for bitcoin JSON-RPC server "
	  "(default: " DEF_RPC_URL ")" },

	{ "userpass",
	  "Username:Password pair for bitcoin JSON-RPC server "
	  "(default: " DEF_RPC_USERPASS ")" },
};

static struct option options[] = {
	{ "help", 0, NULL, 'h' },
	{ "debug", 0, NULL, 'D' },
	{ "protocol-dump", 0, NULL, 'P' },
	{ "threads", 1, NULL, 't' },
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

static void submit_work(struct work *work)
{
	char *hexstr = NULL;
	json_t *val, *res;
	char s[256];

	printf("PROOF OF WORK FOUND?  submitting...\n");

	/* build hex string */
	hexstr = bin2hex(work->data, sizeof(work->data));
	if (!hexstr)
		goto out;

	/* build JSON-RPC request */
	sprintf(s,
	      "{\"method\": \"getwork\", \"params\": [ \"%s\" ], \"id\":1}\r\n",
		hexstr);

	if (opt_debug)
		fprintf(stderr, "DBG: sending RPC call:\n%s", s);

	/* issue JSON-RPC request */
	val = json_rpc_call(rpc_url, userpass, s);
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

static void inc_stats(uint64_t n_hashes)
{
	pthread_mutex_lock(&stats_mutex);

	hash_ctr += n_hashes;

	pthread_mutex_unlock(&stats_mutex);
}

static void runhash(void *state, void *input, const void *init)
{
	memcpy(state, init, 32);
	sha256_transform(state, input);
}

static const uint32_t init_state[8] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

/* suspiciously similar to ScanHash* from bitcoin */
static bool scanhash(unsigned char *midstate, unsigned char *data,
		     unsigned char *hash1, unsigned char *hash)
{
	uint32_t *hash32 = (uint32_t *) hash;
	uint32_t *nonce = (uint32_t *)(data + 12);
	uint32_t n = 0;
	unsigned long stat_ctr = 0;

	while (1) {
		n++;
		*nonce = n;

		runhash(hash1, data, midstate);
		runhash(hash, hash1, init_state);

		if (hash32[7] == 0) {
			char *hexstr;

			hexstr = bin2hex(hash, 32);
			fprintf(stderr,
				"DBG: found zeroes in hash:\n%s\n",
				hexstr);
			free(hexstr);

			return true;
		}

		stat_ctr++;
		if (stat_ctr >= STAT_CTR_INTERVAL) {
			inc_stats(STAT_CTR_INTERVAL);
			stat_ctr = 0;
		}

		if ((n & 0xffffff) == 0) {
			inc_stats(stat_ctr);

			if (opt_debug)
				fprintf(stderr, "DBG: end of nonce range\n");
			return false;
		}
	}
}

static void *miner_thread(void *dummy)
{
	static const char *rpc_req =
		"{\"method\": \"getwork\", \"params\": [], \"id\":0}\r\n";

	while (1) {
		struct work work __attribute__((aligned(128)));
		json_t *val;
		bool rc;

		/* obtain new work from bitcoin */
		val = json_rpc_call(rpc_url, userpass, rpc_req);
		if (!val) {
			fprintf(stderr, "json_rpc_call failed\n");
			return NULL;
		}

		/* decode result into work state struct */
		rc = work_decode(json_object_get(val, "result"), &work);
		if (!rc) {
			fprintf(stderr, "work decode failed\n");
			return NULL;
		}

		json_decref(val);

		/* scan nonces for a proof-of-work hash */
		rc = scanhash(work.midstate, work.data + 64,
			      work.hash1, work.hash);

		/* if nonce found, submit work */
		if (rc)
			submit_work(&work);
	}

	return NULL;
}

static void show_usage(void)
{
	int i;

	printf("Summary: minerd [options]\n\nSupported options:\n");
	for (i = 0; i < ARRAY_SIZE(options_help); i++) {
		struct option_help *h;

		h = &options_help[i];
		printf("--%s\n%s\n\n", h->name, h->helptext);
	}

	exit(1);
}

static void parse_arg (int key, char *arg)
{
	int v;

	switch(key) {
	case 'D':
		opt_debug = true;
		break;
	case 'P':
		opt_protocol = true;
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

		rpc_url = arg;
		break;
	case 1002:			/* --userpass */
		if (!strchr(arg, ':'))
			show_usage();

		userpass = arg;
		break;
	default:
		show_usage();
	}
}

static void parse_cmdline(int argc, char *argv[])
{
	int key;

	while (1) {
		key = getopt_long(argc, argv, "DPt:h?", options, NULL);
		if (key < 0)
			break;

		parse_arg(key, optarg);
	}
}

static void calc_stats(void)
{
	uint64_t hashes;
	long double hd, sd;

	pthread_mutex_lock(&stats_mutex);

	hashes = hash_ctr;
	hash_ctr = 0;

	pthread_mutex_unlock(&stats_mutex);

	hashes = hashes / 1000;

	hd = hashes;
	sd = STAT_SLEEP_INTERVAL;

	fprintf(stderr, "wildly inaccurate HashMeter: %.2Lf khash/sec\n", hd / sd);
}

int main (int argc, char *argv[])
{
	int i;

	/* parse command line */
	parse_cmdline(argc, argv);

	/* set our priority to the highest (aka "nicest, least intrusive") */
	if (setpriority(PRIO_PROCESS, 0, 19))
		perror("setpriority");

	/* start mining threads */
	for (i = 0; i < opt_n_threads; i++) {
		pthread_t t;

		if (pthread_create(&t, NULL, miner_thread, NULL)) {
			fprintf(stderr, "thread %d create failed\n", i);
			return 1;
		}

		sleep(1);	/* don't pound RPC server all at once */
	}

	fprintf(stderr, "%d miner threads started.\n", opt_n_threads);

	/* main loop */
	while (program_running) {
		sleep(STAT_SLEEP_INTERVAL);
		calc_stats();
	}

	return 0;
}

