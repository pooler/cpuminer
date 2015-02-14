#ifndef X13_PLUGIN_H
#define X13_PLUGIN_H

#define PLUGIN_NAME_X13 "X13"
#define PLUGIN_DESC_X13 "\tX13 algorithm"

int init_X13();
void* thread_init_X13(int* error, void* extra_param);

int scanhash_X13(int thr_id, uint32_t *pdata,
        void *thread_local_data, const uint32_t *ptarget,
        uint32_t max_nonce, unsigned long *hashes_done, void* extra_param);

void *param_default_X13();

void *param_parse_X13( const char *str, int *error);

#endif // X13_PLUGIN_H
