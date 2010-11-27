#ifndef __MINER_H__
#define __MINER_H__

#include <stdbool.h>
#include <jansson.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

extern bool opt_protocol;
extern json_t *json_rpc_call(const char *url, const char *userpass,
			     const char *rpc_req);
extern char *bin2hex(unsigned char *p, size_t len);
extern bool hex2bin(unsigned char *p, const char *hexstr, size_t len);

extern unsigned int ScanHash_4WaySSE2(unsigned char *pmidstate,
	unsigned char *pdata, unsigned char *phash1, unsigned char *phash,
	unsigned int *nHashesDone);

#endif /* __MINER_H__ */
