#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "keccak256.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define i(x, y) ((x)+5*(y))
#define read_lane(x, y)  load64((uint8_t*)state+sizeof(uint64_t)*i(x, y))
#define write_lane(x, y, lane) store64((uint8_t*)state+sizeof(uint64_t)*i(x, y), lane)
#define xor_lane(x, y, lane) xor64((uint8_t*)state+sizeof(uint64_t)*i(x, y), lane)

#define ROL64(a, offset) ((((uint64_t)a) << offset) ^ (((uint64_t)a) >> (64-offset)))

static uint64_t load64(const uint8_t *x)
{
    uint64_t u=0;

    for(int i = 7; i >= 0; i--) {
        u <<= 8;
        u |= x[i];
    }
    return u;
}

static void store64(uint8_t *x, uint64_t u)
{
    for(unsigned int i = 0; i < 8; i++) {
        x[i] = u;
        u >>= 8;
    }
}

static void xor64(uint8_t *x, uint64_t u)
{
    for(unsigned int i = 0; i < 8; i++) {
        x[i] ^= u;
        u >>= 8;
    }
}

int LFSR86540(uint8_t *LFSR)
{
    int result = ((*LFSR) & 0x01) != 0;
    if (((*LFSR) & 0x80) != 0) {
        (*LFSR) = ((*LFSR) << 1) ^ 0x71;
    } else {
        (*LFSR) <<= 1;
    }
    return result;
}

void keccak_f_1600(void* state)
{
    unsigned int x, y;
    uint8_t lfsr_state = 0x01;
    // see keccak documentation for round count math
    // round count is 24 for Keccak-f(1600)
    for (unsigned int round = 0; round < 24; round++) {
        {
            uint64_t C[5], D;
            for (x = 0; x < 5; x++) {
                C[x] = read_lane(x, 0) ^ read_lane(x, 1) ^ read_lane(x, 2) ^ read_lane(x, 3) ^ read_lane(x, 4);
            }
            for (x = 0; x < 5; x++) {
                D = C[(x+4)%5] ^ ROL64(C[(x+1)%5], 1);
                for (y = 0; y < 5; y++) {
                    xor_lane(x, y, D);
                }                    
            }
        }

        {
            uint64_t current, temp;
            x = 1; y = 0;
            current = read_lane(x, y);
            for(int t = 0; t < 24; t++) {
                unsigned int r = ((t+1)*(t+2)/2)%64;
                unsigned int Y = (2*x+3*y)%5; x = y; y = Y;
                temp = read_lane(x, y);
                write_lane(x, y, ROL64(current, r));
                current = temp;
            }
        }

        {
            uint64_t temp[5];
            for(y = 0; y < 5; y++) {
                for(x = 0; x < 5; x++) {
                    temp[x] = read_lane(x, y);
                }
                for(x = 0; x < 5; x++) {
                    write_lane(x, y, temp[x] ^((~temp[(x+1)%5]) & temp[(x+2)%5]));
                }
            }
        }

        {
            for(unsigned int j = 0; j < 7; j++) {
                unsigned int bit_position = (1<<j)-1;
                if (LFSR86540(&lfsr_state)) {
                    xor_lane(0, 0, (uint64_t)1<<bit_position);
                }                    
            }                                   
        }
    }    
}

unsigned char* keccak_256(size_t input_byte_len, unsigned char* input)
{
    // 200*8 = 1600 bits for Keccac-f(1600)
    uint8_t state[200];
    memset(state, 0, sizeof(state));

    unsigned int block_size = 0;    
    // 256 bits
    size_t output_byte_len = 32;
    unsigned char* output = (unsigned char*) malloc(output_byte_len);
    unsigned char* output_head = output;

    // rate (1088) and capacity (512) sum to 1600 for Keccak-f(1600)
    unsigned int rate = 1088; unsigned int rate_in_bytes = rate / 8;
    // capacity 512 for 256 bit output
    unsigned int capacity = 512;
    // 0x01 so we use default padding
    char d = 0x01;

    while(input_byte_len > 0) {
        block_size = MIN(input_byte_len, rate_in_bytes);
        for(int i = 0; i < block_size; i++) {
            state[i] ^= input[i];
        }
        input += block_size;
        input_byte_len -= block_size;

        if (block_size == rate_in_bytes) {
            keccak_f_1600(state);
            block_size = 0;
        }
    }

    state[block_size] ^= d;
    if (((d & 0x80) != 0) && (block_size == (rate_in_bytes-1))) {
        keccak_f_1600(state);
    }        
    state[rate_in_bytes-1] ^= 0x80;
    keccak_f_1600(state);

    while(output_byte_len > 0) {
        block_size = MIN(output_byte_len, rate_in_bytes);
        memcpy(output_head, state, block_size);
        output_head += block_size;
        output_byte_len -= block_size;

        if (output_byte_len > 0) {
            keccak_f_1600(state);
        }
    }

    return output;
}