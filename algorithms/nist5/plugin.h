#ifndef NIST5_PLUGIN_H
#define NIST5_PLUGIN_H

#define PLUGIN_NAME_NIST5 "NIST5"
#define PLUGIN_DESC_NIST5 "\tNIST5 algorithm"

int init_NIST5();
void* thread_init_NIST5(int* error, void* extra_param);

int scanhash_NIST5(int thr_id, uint32_t *pdata,
        void *thread_local_data, const uint32_t *ptarget,
        uint32_t max_nonce, unsigned long *hashes_done, void* extra_param);

void *param_default_NIST5();

void *param_parse_NIST5( const char *str, int *error);

#endif // NIST5_PLUGIN_H
