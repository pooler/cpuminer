#ifndef X15_PLUGIN_H
#define X15_PLUGIN_H

#define PLUGIN_NAME_X15 "X15"
#define PLUGIN_DESC_X15 "\tX15 (bitblock) algorithm"

int init_X15();
void* thread_init_X15(int* error, void* extra_param);

int scanhash_X15(int thr_id, uint32_t *pdata,
        void *thread_local_data, const uint32_t *ptarget,
        uint32_t max_nonce, unsigned long *hashes_done, void* extra_param);

void *param_default_X15();

void *param_parse_X15( const char *str, int *error);

#endif // X15_PLUGIN_H
