#include "sha256.h"

/*****************************************************************************/
/* SHA-256 Hashing, implemented by Steven Tang */
/* Reference: RFC 6234 */

/* SHA-256 Functions: RFC 6234, FIPS 180-3 section 4.1.2 */
#define SHA_Ch(x, y, z) (((x) & (y)) ^ ((~(x)) & (z)))
#define SHA_Maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define SHA256_SHR(bits, word) ((word) >> (bits))
#define SHA256_ROTR(bits, word)                                               \
	(((word) >> (bits)) | ((word) << (32 - (bits))))

#define SHA256_BSIG0(word)                                                    \
	(SHA256_ROTR(2 , word) ^ SHA256_ROTR(13, word) ^ SHA256_ROTR(22, word))
#define SHA256_BSIG1(word)                                                    \
	(SHA256_ROTR(6 , word) ^ SHA256_ROTR(11, word) ^ SHA256_ROTR(25, word))
#define SHA256_SSIG0(word)                                                    \
	(SHA256_ROTR(7 , word) ^ SHA256_ROTR(18, word) ^ SHA256_SHR (3, word ))
#define SHA256_SSIG1(word)                                                    \
	(SHA256_ROTR(17, word) ^ SHA256_ROTR(19, word) ^ SHA256_SHR (10, word))

/* SHA-256 Initial Hash Values: FIPS 180-3 section 5.3.3 */
static uint32_t SHA256_H0[8] = {0x6A09E667, 0xBB67AE85, 0x3C6EF372,
                                0xA54FF53A, 0x510E527F, 0x9B05688C,
                                0x1F83D9AB, 0x5BE0CD19};

/* SHA-256 Constants: FIPS 180-3, section 4.2.2 */
static const uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
        0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
        0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
        0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
        0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
        0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
#ifdef DEBUG
void print_uint32_array(uint32_t* arr, unsigned long length);
#endif

void sha256_hash(char hash_hexstring[65], const uint8_t* buf, const uint64_t nbyte) {
    uint64_t i, num_blocks;
    uint32_t hash[8], last_block[16];
    short leftover_bytes;

    // each block is 512 bits, or 64 bytes;
    // last block needs to be processed differently for padding
    leftover_bytes = nbyte % 64;
    num_blocks = leftover_bytes == 0 ? nbyte / 64 : nbyte / 64 + 1;

#ifdef DEBUG
    fprintf(stderr, "Blocks: %ld, Leftover bytes %d\n", num_blocks,
			leftover_bytes);
#endif

    // initialization
    sha256_init(hash);

    // process: 64 = 512 bits per block / 8 bits for uint8_t
    for (i = 0; i < (leftover_bytes == 0 ? num_blocks : num_blocks - 1); i++) {
        sha256_process((uint32_t*)(buf + i * 64), hash);
    }

    // final block
    memset(last_block, 0, 64);
    if (leftover_bytes != 0) {
        memcpy(last_block, buf + i * 64, leftover_bytes);
    }
    sha256_process_final(nbyte, leftover_bytes, last_block, hash);

    // print to buffer as hex string
    uint32_array_to_hex_string(hash_hexstring, hash, 8);
}

/* SHA-256 Initialization.
 * Source: https://www.rfc-editor.org/rfc/rfc6234#section-6.1
 */
void sha256_init(uint32_t hash[8]) {
    int i;
    for (i = 0; i < 8; i++) {
        hash[i] = SHA256_H0[i];
    }
}

/* SHA-256 Processing. For each 32 * 16 = 512 block of bytes
 * Source: https://www.rfc-editor.org/rfc/rfc6234#section-6.2
 */
void sha256_process(uint32_t message_block[16], uint32_t hash[8]) {
    int t;
    uint32_t a, b, c, d, e, f, g, h, t1, t2;
    uint32_t w[64];
    uint8_t* message_ptr;

    // prepare message schedule
    for (t = 0; t < 64; t++) {
        if (t < 16) {
            // w[t] = ntohl(message_block[t]);
            message_ptr = (uint8_t*)&message_block[t];
            w[t] = message_ptr[0] << 24 | message_ptr[1] << 16 |
                   message_ptr[2] << 8 | message_ptr[3];
        } else {
            w[t] = SHA256_SSIG1(w[t - 2]) + w[t - 7] +
                   SHA256_SSIG0(w[t - 15]) + w[t - 16];
        }
    }

    // initialize working variables
    a = hash[0];
    b = hash[1];
    c = hash[2];
    d = hash[3];
    e = hash[4];
    f = hash[5];
    g = hash[6];
    h = hash[7];

    // main hash computation
    for (t = 0; t < 64; t++) {
        t1 = h + SHA256_BSIG1(e) + SHA_Ch(e, f, g) + K[t] + w[t];
        t2 = SHA256_BSIG0(a) + SHA_Maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    // calculate intermediate hash value H(i)
    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
    hash[5] += f;
    hash[6] += g;
    hash[7] += h;
}

/* SHA-256 processing for final block. Padding - "1" followed by m "0"s followed by 64-bit integer.
 * Source: https://www.rfc-editor.org/rfc/rfc6234#section-4.1
 */
void sha256_process_final(uint64_t nbyte, short leftover_bytes, uint32_t last_block[16], uint32_t hash[8]) {
    // Append "1" bit
    ((uint8_t*)last_block)[leftover_bytes] = 1 << 7;

    // Length will be in next block since it doesn't fit
    if (leftover_bytes > 64 - 8 - 1) {
        sha256_process(last_block, hash);
        memset(last_block, 0, 64);
    }

    // Set length, process last block
    /*
        ((uint64_t*)last_block)[7] = htobe64(nbyte * 8);
        last_block[14] = htonl(nbyte * 8 >> 32);
        last_block[15] = htonl(nbyte * 8 & 0xFFFFFFF0);
    */
    ((uint8_t*)last_block)[56] = (nbyte * 8 >> (64 - 8 )) & 0xFF;
    ((uint8_t*)last_block)[57] = (nbyte * 8 >> (64 - 16)) & 0xFF;
    ((uint8_t*)last_block)[58] = (nbyte * 8 >> (64 - 24)) & 0xFF;
    ((uint8_t*)last_block)[59] = (nbyte * 8 >> (64 - 32)) & 0xFF;
    ((uint8_t*)last_block)[60] = (nbyte * 8 >> (64 - 40)) & 0xFF;
    ((uint8_t*)last_block)[61] = (nbyte * 8 >> (64 - 48)) & 0xFF;
    ((uint8_t*)last_block)[62] = (nbyte * 8 >> (64 - 56)) & 0xFF;
    ((uint8_t*)last_block)[63] = (nbyte * 8 >> (64 - 64)) & 0xFF;

    sha256_process(last_block, hash);
}

#ifdef DEBUG
/* Prints a uint32 array */
void print_uint32_array(uint32_t* arr, unsigned long length) {
	unsigned long i;
	for (i = 0; i < length; i++) {
		fprintf(stderr, "%08x", arr[i]);
	}
	sprintf(stderr, "\n");
}
#endif

/* Returns a uint32 array in hex notation */
void uint32_array_to_hex_string(char* out, uint32_t* in, unsigned long length) {
    int i;
    for (i = 0; i < length; i++) {
        sprintf(out + (i * 8), "%08x", in[i]);
    }
    out[64] = 0;
}
