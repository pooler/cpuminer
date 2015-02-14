#ifndef EXAMPLE_PLUGIN_H
#define EXAMPLE_PLUGIN_H

#define PLUGIN_NAME_EXAMPLE "example"
#define PLUGIN_DESC_EXAMPLE "example(N, x), default: N=1024, x=12.1"

int init_EXAMPLE();
void* thread_init_EXAMPLE(int* error, void *extra_param);

int scanhash_EXAMPLE(int thr_id, uint32_t *pdata,
        void *thread_local_data, const uint32_t *ptarget,
        uint32_t max_nonce, unsigned long *hashes_done, void* extra_param);

void *param_default_EXAMPLE();

void *param_parse_EXAMPLE( const char *str, int *error);

#endif // EXAMPLE_PLUGIN_H
