#include <stdio.h>
#include <string.h>
#include <sys/signalfd.h>

/* The only public function */
void sha256_hash(char hash_hexstring[65], const uint8_t* buf, const uint64_t nbyte);

/* Private function prototypes */
void sha256_init(uint32_t hash[8]);
void sha256_process(uint32_t message_block[16], uint32_t hash[8]);
void sha256_process_final(uint64_t nbyte, short leftover_bytes,
                          uint32_t last_block[16], uint32_t hash[8]);
void uint32_array_to_hex_string(char* out, uint32_t* in, unsigned long length);