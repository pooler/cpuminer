#include <string.h>

#include "miner.h"

typedef struct {
   int N;
   double x;
} example_param_t;

static example_param_t default_param;

int init_EXAMPLE() {
    default_param.N = 1023;
    default_param.x = 12.1;

	return 0; // 0 == success
}

void* thread_init_EXAMPLE(int* error, void *extra_param) {
    example_param_t *p = (example_param_t*) extra_param;

	void *buff = malloc((size_t)p->N * 1234);
	*error = (buff == NULL);
	return buff;
}


int scanhash_EXAMPLE(int thr_id, uint32_t *pdata,
	unsigned char *scratchbuf, const uint32_t *ptarget,
	uint32_t max_nonce, unsigned long *hashes_done, void *extra_param) {

    example_param_t *p = (example_param_t*) extra_param;
	
	// c.f. scrypt or sha256d code for usage of other parameters
	return 0;
}

void *param_default_EXAMPLE() {
	return (void*) &default_param;
}

void *param_parse_EXAMPLE(const char *str, int *error) {
    static example_param_t p;
	const char *delim = ",";
    char *pch = strtok (str, delim);
    int  i = 0;

    while( pch ) {
		switch(i) {
			case 0:
				p.N = strtol(pch, NULL, 10);
				applog(LOG_INFO, "example: Setting parameter N to %i", p.N);
				break;
			case 1:
				p.x = strtod(pch, NULL);
				applog(LOG_INFO, "example: Setting parameter x to %f", p.x);
				break;
			default:
				applog(LOG_ERR, "Too many parameters specified to example algorithm: %s", str);
				*error = 1;
				return NULL;
		}
		i++;
		pch = strtok (NULL, delim);
    }
	return (void*) &p;
}
