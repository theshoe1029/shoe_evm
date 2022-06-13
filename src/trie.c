#include <stdlib.h>

#include "hex_prefix.h"
#include "keccak256.h"
#include "trie.h"

void get_j(size_t i, input_set* input, size_t* j)
{
    unsigned char test_nibble = nibble_at(*j, input->keys[0].data);
    for (size_t ik = 1; ik < input->card; ik++) {
        unsigned char next_nibble = nibble_at(*j, input->keys[ik].data);
        if (test_nibble != next_nibble) {
            return;
        }
    }
    *j += 1;
    get_j(i, input, j);
}

unsigned char* nibble_subarr(size_t i, size_t j, input_set* input, size_t* output_len)
{
    *output_len = BYTE_ARR_LEN(j-i);
    unsigned char* data = input->keys[0].data;
    unsigned char* shared_bytes = (unsigned char*) malloc(*output_len);
    memset(shared_bytes, 0, *output_len);
    for (size_t is = i; is < j; is++) {
        unsigned char c = nibble_at(is, data);
        shared_bytes[(is-i)/2] += (is-i)%2 ? c : c<<4;
    }
    return shared_bytes;
}

rlp_struct* construct_v(input_set* input, unsigned int i)
{
    if (input != NULL) {
        for (size_t i = 0; i < input->card; i++) {
            if (input->keys[i].size == i) {
                unsigned char* data = input->values[i].data;
                size_t size = input->values[i].size;
                rlp_struct* val = new_rlp_str(size, data);
                return val;
            }
        }
    }
    return NULL;
}

rlp_struct* construct_node(input_set* input, unsigned int i)
{
    if (input == NULL) {
        return NULL;
    } else {
        rlp_struct* trie = construct_trie(input, i);
        size_t size = 0;
        unsigned char* encoded_trie = rlp_encode(&size, trie);
        if (size < 32) {
            return new_rlp_str(size, encoded_trie);
        } else {
            unsigned char* encoded_root = keccak_256(size, encoded_trie);
            return new_rlp_str(32, encoded_root);
        }
    }
}

rlp_struct* construct_trie(input_set* input, unsigned int i)
{
    rlp_struct* trie = new_rlp_list();
    if (input->card == 1) {
        size_t key_size = 0;
        unsigned char* key_data = nibble_subarr(i, input->keys[0].size, input, &key_size);
        rlp_struct* key = new_rlp_str(key_size, key_data);
        rlp_struct* value = new_rlp_str(input->values[0].size, input->values[0].data);
        rlp_list_add(trie, key);
        rlp_list_add(trie, value);
    } else {
        size_t j = i;
        get_j(i, input, &j);
        if (i != j) {
            size_t key_data_len = 0;
            unsigned char* key_data = nibble_subarr(i, j, input, &key_data_len);
            rlp_struct* key = new_rlp_str(key_data_len, key_data);
            rlp_struct* value = construct_node(input, j);
            rlp_list_add(trie, key);
            rlp_list_add(trie, value);
        } else {
            for (unsigned int branch_nibble = 0x0; branch_nibble <= 0xf; branch_nibble++) {
                input_set* branch_input = NULL;
                for (size_t ik = 0; ik < input->card; ik++) {
                    if (nibble_at(i, input->keys[ik].data) == branch_nibble) {
                        if (branch_input == NULL) {
                            branch_input = (input_set*) malloc(sizeof(input_set));
                            branch_input->keys = (trie_val*) malloc(sizeof(trie_val));
                            branch_input->values = (trie_val*) malloc(sizeof(trie_val));
                            branch_input->card = 1;
                        } else {
                            branch_input->card++;
                            branch_input->keys = (trie_val*) realloc(branch_input->keys, sizeof(trie_val)*branch_input->card);
                            branch_input->values = (trie_val*) realloc(branch_input->keys, sizeof(trie_val)*branch_input->card);
                        }
                        memcpy(branch_input->keys+branch_input->card-1, input->keys+ik, sizeof(trie_val));
                        memcpy(branch_input->values+branch_input->card-1, input->values+ik, sizeof(trie_val));
                    }
                }
                rlp_list_add(trie, construct_node(branch_input, i+1));
            }
            rlp_list_add(trie, construct_v(input, i));
        }
    }
    return trie;
}

unsigned char* get_trie_hash(input_set* input)
{
    rlp_struct* trie = construct_trie(input, 0);
    size_t size = 0;
    unsigned char* encoded_trie = rlp_encode(&size, trie);
    unsigned char* encoded_root = keccak_256(size, encoded_trie);
    return encoded_root;
}