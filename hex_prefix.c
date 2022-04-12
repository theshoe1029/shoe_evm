#include <stdlib.h>
#include <stdio.h>

#include "hex_prefix.h"

#define f_t(t) t == 0 ? 0 : 2

unsigned char* hp_encode(size_t arr_size, nibble* arr, unsigned int t, size_t* output_size)
{
    *output_size = (arr_size/2)+1;
    unsigned char* output = (char*) malloc(*output_size);
    nibble n = arr[0];
    if (arr_size%2 == 0) {
        output[0] = 16*f_t(t);
        for (int i = 1; i < *output_size; i++) {
            n = arr[i-1];
            output[i] = (16*n.first)+n.second;
        }
    } else {
        output[0] = (16*((f_t(t))+1))+n.first;
        for (int i = 1; i < *output_size; i++) {
            output[i] = (16*n.second);
            n = arr[i];
            output[i] += n.first;
        }
    }
    return output;
}

nibble byte_to_nibble(unsigned char byte)
{
    (nibble) {.first=(byte&0xf0)>>2, .second=byte&0x0f};
}

nibble* hp_decode(size_t arr_size, unsigned char* arr, size_t* output_size)
{
    nibble header = byte_to_nibble(arr[0]);
    nibble* nibbles;
    unsigned int t = header.second;
    if (header.first == 0) {
        *output_size = arr_size-1;
        nibbles = (nibble*) malloc(sizeof(nibble)*(*output_size));
        for (int i = 0; i < *output_size; i++) {
            nibbles[i] = (nibble) {.first=(arr[i+1]&0xf0)>>4,.second=(arr[i+1]&0x0f)};
        }
    } else if (header.second) {
        *output_size = arr_size;
        nibbles = (nibble*) malloc(sizeof(nibble)*(*output_size));
        for (int i = 0; i < *output_size; i++) {
            nibbles[i] = (nibble) {.first=arr[i]&0x0f,.second=(arr[i+1]&0xf0)>>4};
        }
    }
    return nibbles;
}

void print_nibble_arr(size_t arr_size, nibble* arr)
{
    for (int i = 0; i < arr_size; i++) {
        printf("first: %x, second: %x\n", arr[i].first, arr[i].second);
    }    
}