#include "nibble.h"
#include "rlp.h"

#ifndef TRIE_H_
#define TRIE_H_

typedef struct trie_val {
    unsigned char* data;
    size_t size;
} trie_val;

typedef struct input_set {
    struct trie_val* keys;
    struct trie_val* values;
    size_t card;
} input_set;

void get_j(size_t i, input_set* input, size_t* j);
unsigned char* nibble_subarr(size_t i, size_t j, input_set* input, size_t* output_len);
rlp_struct* construct_node(input_set* input, unsigned int i);
rlp_struct* construct_trie(input_set* input, unsigned int i);
unsigned char* get_trie_hash(input_set* input);

#endif