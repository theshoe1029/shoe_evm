#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NIBBLE_H_
#define NIBBLE_H_

#define FIRST_NIBBLE(byte) (unsigned char) (byte>>4)
#define SECOND_NIBBLE(byte) (unsigned char) (byte&0x0f)
#define BYTE_ARR_LEN(nibble_arr_len) (size_t) (nibble_arr_len+1)/2

typedef struct nibble {
    unsigned int first : 4;
    unsigned int second : 4;
} nibble;

static void print_nibble_arr(size_t arr_size, nibble* arr)
{
    for (int i = 0; i < arr_size; i++) {
        printf("first: %x, second: %x\n", arr[i].first, arr[i].second);
    }    
}

static unsigned char nibble_at(size_t i, unsigned char* arr)
{
    return i%2 ? SECOND_NIBBLE(arr[i/2]) : FIRST_NIBBLE(arr[i/2]);
}

static nibble byte_to_nibble(unsigned char byte)
{
    (nibble) {.first=FIRST_NIBBLE(byte), .second=SECOND_NIBBLE(byte)};
}

static nibble* byte_arr_to_nibble_arr(size_t size, unsigned char* arr, size_t* output_size)
{
    *output_size = BYTE_ARR_LEN(size);
    nibble* nibble_arr = (nibble*) malloc(sizeof(nibble)*(*output_size));
    for (int i = 0; i < *output_size; i++) {
        nibble n = byte_to_nibble(arr[i]);
        memcpy(nibble_arr+i, &n, sizeof(nibble));
    }
    return nibble_arr;
}

#endif