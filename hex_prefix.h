#ifndef HEX_PREFIX_H_
#define HEX_PREFIX_H_

typedef struct nibble {
    unsigned int first : 4;
    unsigned int second : 4;
} nibble;

unsigned char* hp_encode(size_t arr_size, nibble* arr, unsigned int t, size_t* output_size);
nibble* hp_decode(size_t arr_size, unsigned char* arr, size_t* output_size);
void print_nibble_arr(size_t arr_size, nibble* arr);

#endif