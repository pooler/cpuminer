/*
   Copyright 2010 Jeff Garzik
   Distributed under the MIT/X11 software license, see
   http://www.opensource.org/licenses/mit-license.php
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <argp.h>
#include <jansson.h>
#include <curl/curl.h>
#include <openssl/bn.h>

#define PROGRAM_NAME "minerd"

#include "sha256_generic.c"

enum {
	STAT_SLEEP_INTERVAL		= 10,
	POW_SLEEP_INTERVAL		= 1,
	STAT_CTR_INTERVAL		= 10000000,
};

static bool opt_verbose;
static bool opt_debug;
static bool program_running = true;
static const bool opt_time = true;
static int opt_n_threads = 1;
static pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;
static uint64_t hash_ctr;

static struct argp_option options[] = {
	{ "threads", 't', "N", 0,
	  "Number of miner threads" },
	{ "debug", 'D', NULL, 0,
	  "Enable debug output" },
	{ "verbose", 'v', NULL, 0,
	  "Enable verbose output" },
	{ }
};

static const char doc[] =
PROGRAM_NAME " - CPU miner for bitcoin";

static error_t parse_opt (int key, char *arg, struct argp_state *state);

static const struct argp argp = { options, parse_opt, NULL, doc };

struct data_buffer {
	void		*buf;
	size_t		len;
};

struct upload_buffer {
	const void	*buf;
	size_t		len;
};

struct work {
	unsigned char	midstate[32];
	unsigned char	data[128];
	unsigned char	hash[32];
	unsigned char	hash1[64];
	BIGNUM		*target;
};

#define ___constant_swab32(x) ((u32)(                       \
        (((u32)(x) & (u32)0x000000ffUL) << 24) |            \
        (((u32)(x) & (u32)0x0000ff00UL) <<  8) |            \
        (((u32)(x) & (u32)0x00ff0000UL) >>  8) |            \
        (((u32)(x) & (u32)0xff000000UL) >> 24)))

static inline uint32_t swab32(uint32_t v)
{
	return ___constant_swab32(v);
}

static void databuf_free(struct data_buffer *db)
{
	if (!db)
		return;
	
	free(db->buf);

	memset(db, 0, sizeof(*db));
}

static size_t all_data_cb(const void *ptr, size_t size, size_t nmemb,
			  void *user_data)
{
	struct data_buffer *db = user_data;
	size_t len = size * nmemb;
	size_t oldlen, newlen;
	void *newmem;
	static const unsigned char zero;

	if (opt_debug)
		fprintf(stderr, "DBG(%s): %p, %lu, %lu, %p\n",
			__func__, ptr, (unsigned long) size,
			(unsigned long) nmemb, user_data);

	oldlen = db->len;
	newlen = oldlen + len;

	newmem = realloc(db->buf, newlen + 1);
	if (!newmem)
		return 0;

	db->buf = newmem;
	db->len = newlen;
	memcpy(db->buf + oldlen, ptr, len);
	memcpy(db->buf + newlen, &zero, 1);	/* null terminate */

	return len;
}

static size_t upload_data_cb(void *ptr, size_t size, size_t nmemb,
			     void *user_data)
{
	struct upload_buffer *ub = user_data;
	int len = size * nmemb;

	if (opt_debug)
		fprintf(stderr, "DBG(%s): %p, %lu, %lu, %p\n",
			__func__, ptr, (unsigned long) size,
			(unsigned long) nmemb, user_data);

	if (len > ub->len)
		len = ub->len;

	if (len) {
		memcpy(ptr, ub->buf, len);
		ub->buf += len;
		ub->len -= len;
	}

	return len;
}

static json_t *json_rpc_call(const char *url, const char *userpass,
		       const char *rpc_req)
{
	CURL *curl;
	json_t *val;
	int rc;
	struct data_buffer all_data = { };
	struct upload_buffer upload_data;
	json_error_t err = { };
	struct curl_slist *headers = NULL;
	char len_hdr[64];

	curl = curl_easy_init();
	if (!curl)
		return NULL;

	if (opt_verbose)
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_ENCODING, "");
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
	curl_easy_setopt(curl, CURLOPT_TCP_NODELAY, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, all_data_cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &all_data);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, upload_data_cb);
	curl_easy_setopt(curl, CURLOPT_READDATA, &upload_data);
	if (userpass) {
		curl_easy_setopt(curl, CURLOPT_USERPWD, userpass);
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
	}
	curl_easy_setopt(curl, CURLOPT_POST, 1);

	upload_data.buf = rpc_req;
	upload_data.len = strlen(rpc_req);
	sprintf(len_hdr, "Content-Length: %lu",
		(unsigned long) upload_data.len);

	headers = curl_slist_append(headers,
		"Content-type: application/json");
	headers = curl_slist_append(headers, len_hdr);
	headers = curl_slist_append(headers, "Expect:"); /* disable Expect hdr*/

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	rc = curl_easy_perform(curl);
	if (rc)
		goto err_out;

	if (opt_debug)
		printf("====\nSERVER RETURNS:\n%s\n====\n",
			(char *) all_data.buf);

	val = json_loads(all_data.buf, &err);
	if (!val) {
		fprintf(stderr, "JSON failed(%d): %s\n", err.line, err.text);
		goto err_out;
	}

	databuf_free(&all_data);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	return val;

err_out:
	databuf_free(&all_data);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	return NULL;
}

static char *bin2hex(unsigned char *p, size_t len)
{
	int i;
	char *s = malloc((len * 2) + 1);
	if (!s)
		return NULL;
	
	for (i = 0; i < len; i++)
		sprintf(s + (i * 2), "%02x", p[i]);

	return s;
}

static bool hex2bin(unsigned char *p, const char *hexstr, size_t len)
{
	while (*hexstr && len) {
		char hex_byte[3];
		unsigned int v;

		if (!hexstr[1]) {
			fprintf(stderr, "hex2bin str truncated\n");
			return false;
		}

		hex_byte[0] = hexstr[0];
		hex_byte[1] = hexstr[1];
		hex_byte[2] = 0;

		if (sscanf(hex_byte, "%x", &v) != 1) {
			fprintf(stderr, "hex2bin sscanf '%s' failed\n",
				hex_byte);
			return false;
		}

		*p = (unsigned char) v;

		p++;
		hexstr += 2;
		len--;
	}

	return (len == 0 && *hexstr == 0) ? true : false;
}

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

static void work_free(struct work *work)
{
	if (!work)
		return;

	if (work->target)
		BN_free(work->target);

	free(work);
}

static struct work *work_decode(const json_t *val)
{
	struct work *work;

	work = calloc(1, sizeof(*work));
	if (!work)
		return NULL;

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

	if (!BN_hex2bn(&work->target,
		json_string_value(json_object_get(val, "target")))) {
		fprintf(stderr, "JSON inval target\n");
		goto err_out;
	}

	return work;

err_out:
	work_free(work);
	return NULL;
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
static uint32_t scanhash(unsigned char *midstate, unsigned char *data,
			 unsigned char *hash1, unsigned char *hash)
{
	uint32_t *hash32 = (uint32_t *) hash;
	uint32_t *nonce = (uint32_t *)(data + 12);
	uint32_t n;
	unsigned long stat_ctr = 0;

	while (1) {
		n = *nonce;
		n++;
		*nonce = n;

		runhash(hash1, data, midstate);
		runhash(hash, hash1, init_state);

		if (hash32[7] == 0) {
			if (1) {
				char *hexstr;

				hexstr = bin2hex(hash, 32);
				fprintf(stderr,
					"DBG: found zeroes in hash:\n%s\n",
					hexstr);
				free(hexstr);
			}
			return n;
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
			return 0;
		}
	}
}

static const char *url = "http://127.0.0.1:8332/";
static const char *userpass = "pretzel:smooth";

static void submit_work(struct work *work)
{
	char *hexstr = NULL, *s = NULL;
	json_t *val, *res;

	printf("PROOF OF WORK FOUND?  submitting...\n");

	/* build hex string */
	hexstr = bin2hex(work->data, sizeof(work->data));
	if (!hexstr)
		goto out;

	/* build JSON-RPC request */
	if (asprintf(&s,
	    "{\"method\": \"getwork\", \"params\": [ \"%s\" ], \"id\":1}\r\n",
	    hexstr) < 0) {
		fprintf(stderr, "asprintf failed\n");
		goto out;
	}

	if (opt_debug)
		fprintf(stderr, "DBG: sending RPC call:\n%s", s);

	/* issue JSON-RPC request */
	val = json_rpc_call(url, userpass, s);
	if (!val) {
		fprintf(stderr, "submit_work json_rpc_call failed\n");
		goto out;
	}

	res = json_object_get(val, "result");

	printf("PROOF OF WORK RESULT: %s\n",
		json_is_true(res) ? "true (yay!!!)" : "false (booooo)");

	json_decref(val);

out:
	free(s);
	free(hexstr);
}

static void *miner_thread(void *dummy)
{
	static const char *rpc_req =
		"{\"method\": \"getwork\", \"params\": [], \"id\":0}\r\n";

	while (1) {
		json_t *val;
		struct work *work;
		uint32_t nonce;

		/* obtain new work from bitcoin */
		val = json_rpc_call(url, userpass, rpc_req);
		if (!val) {
			fprintf(stderr, "json_rpc_call failed\n");
			return NULL;
		}

		if (opt_verbose) {
			char *s = json_dumps(val, JSON_INDENT(2));
			printf("JSON output:\n%s\n", s);
			free(s);
		}

		/* decode result into work state struct */
		work = work_decode(json_object_get(val, "result"));
		if (!work) {
			fprintf(stderr, "work decode failed\n");
			return NULL;
		}

		json_decref(val);

		/* scan nonces for a proof-of-work hash */
		nonce = scanhash(work->midstate, work->data + 64,
				 work->hash1, work->hash);

		/* if nonce found, submit work */
		if (nonce) {
			submit_work(work);

			fprintf(stderr, "sleeping, after proof-of-work...\n");
			sleep(POW_SLEEP_INTERVAL);
		}

		work_free(work);
	}

	return NULL;
}

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	int v;

	switch(key) {
	case 'v':
		opt_verbose = true;
		break;
	case 'D':
		opt_debug = true;
		break;
	case 't':
		v = atoi(arg);
		if (v < 1 || v > 9999)		/* sanity check */
			argp_usage(state);

		opt_n_threads = v;
		break;
	case ARGP_KEY_ARG:
		argp_usage(state);	/* too many args */
		break;
	case ARGP_KEY_END:
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
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
	error_t aprc;
	int i;

	aprc = argp_parse(&argp, argc, argv, 0, NULL, NULL);
	if (aprc) {
		fprintf(stderr, "argp_parse failed: %s\n", strerror(aprc));
		return 1;
	}

	if (setpriority(PRIO_PROCESS, 0, 19))
		perror("setpriority");

	for (i = 0; i < opt_n_threads; i++) {
		pthread_t t;

		if (pthread_create(&t, NULL, miner_thread, NULL)) {
			fprintf(stderr, "thread %d create failed\n", i);
			return 1;
		}

		sleep(1);	/* don't pound server all at once */
	}

	fprintf(stderr, "%d miner threads started.\n", opt_n_threads);

	while (program_running) {
		sleep(STAT_SLEEP_INTERVAL);
		calc_stats();
	}

	return 0;
}

