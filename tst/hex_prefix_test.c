#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hex_prefix.h"

void hex_prefix_test()
{
    {
        printf("encode and decode nibble arr of len 5\n");
        nibble n1 = (nibble) {.first=0x1, .second=0x2};
        nibble n2 = (nibble) {.first=0x3, .second=0x4};
        nibble n3 = (nibble) {.first=0x5, .second=0x0};
        nibble input[3] = {n1, n2, n3};
        size_t output_size;
        unsigned char* output = hp_encode(5, input, 0x0, &output_size);
        size_t nibble_output_size;
        nibble* nibble_output = hp_decode(output_size, output, &nibble_output_size);
        assert(nibble_arr_eq(nibble_output_size, nibble_output, output_size, input));
    }
    {
        printf("encode and decode nibble arr of len 6\n");
        nibble n1 = (nibble) {.first=0x0, .second=0x1};
        nibble n2 = (nibble) {.first=0x2, .second=0x3};
        nibble n3 = (nibble) {.first=0x4, .second=0x5};
        nibble input[3] = {n1, n2, n3};
        size_t output_size;
        unsigned char* output = hp_encode(6, input, 0x0, &output_size);
        size_t nibble_output_size;
        nibble* nibble_output = hp_decode(output_size, output, &nibble_output_size);
        assert(nibble_arr_eq(nibble_output_size, nibble_output, nibble_output_size, input));
    }
}