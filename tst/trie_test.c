#include <stdlib.h>
#include <string.h>

#include "trie.h"

void trie_test()
{
    trie_val* keys = (trie_val*) malloc(sizeof(trie_val)*4);
    unsigned char key1[] = {0xa7, 0x11, 0x35, 0x50};
    unsigned char key2[] = {0xa7, 0x7d, 0x33, 0x70};
    unsigned char key3[] = {0xa7, 0xf9, 0x36, 0x50};
    unsigned char key4[] = {0xa7, 0x7d, 0x39, 0x70};
    keys[0] = (trie_val){.data=key1, .size=7};
    keys[1] = (trie_val){.data=key2, .size=7};
    keys[2] = (trie_val){.data=key3, .size=7};
    keys[3] = (trie_val){.data=key4, .size=7};
    trie_val* values = (trie_val*) malloc(sizeof(trie_val)*4);
    values[0] = (trie_val){.data="45.0", .size=4};
    values[1] = (trie_val){.data="1.0", .size=3};
    values[2] = (trie_val){.data="1.1", .size=3};
    values[3] = (trie_val){.data="0.12", .size=4};
    input_set input = {.keys=keys, .values=values, .card=4};

    size_t i = 0;
    size_t j = 0;
    get_j(0, &input, &j);
    printf("%lu\n", j);
    size_t arr_len = 0;
    unsigned char* arr = nibble_subarr(i, j, &input, &arr_len);
    for (size_t k = 0; k < arr_len; k++) printf("%x ", arr[k]);
    printf("\n");

    i = 2;
    j = 2;
    get_j(i, &input, &j);
    printf("%lu\n", j);

    for (int i = 0; i < 32; i++) printf("%x ", get_trie_hash(&input)[i]);
    printf("\n");

    return 0;
}