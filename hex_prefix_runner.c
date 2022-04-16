#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/hex_prefix.h"

int main(int argc, char** argv) 
{    
    {
        nibble n1 = (nibble) {.first=0x1, .second=0x2};
        nibble n2 = (nibble) {.first=0x3, .second=0x4};
        nibble n3 = (nibble) {.first=0x5, .second=0x0};
        nibble arr[3] = {n1, n2, n3};
        size_t output_size;
        char* output = hp_encode(5, arr, 0x0, &output_size);
        for (size_t i = 0; i < output_size; ++i) printf("%02x ", output[i]);
        printf("\n");
        size_t nibble_output_size;
        nibble* nibble_output = hp_decode(output_size, output, &nibble_output_size);
        print_nibble_arr(nibble_output_size, nibble_output);
    }
    {
        nibble n1 = (nibble) {.first=0x0, .second=0x1};
        nibble n2 = (nibble) {.first=0x2, .second=0x3};
        nibble n3 = (nibble) {.first=0x4, .second=0x5};
        nibble arr[3] = {n1, n2, n3};
        size_t output_size;
        unsigned char* output = hp_encode(6, arr, 0x0, &output_size);
        for (size_t i = 0; i < output_size; ++i) printf("%02x ", output[i]);
        printf("\n");
        size_t nibble_output_size;
        nibble* nibble_output = hp_decode(output_size, output, &nibble_output_size);
        print_nibble_arr(nibble_output_size, nibble_output);
    }
    return 0;
}