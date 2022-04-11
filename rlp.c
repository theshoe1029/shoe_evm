#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "rlp.h"

rlp_struct* new_rlp_list()
{
    rlp_struct* list = (rlp_struct*) malloc(sizeof(rlp_struct));
    list->data = 0;
    list->sub_arr = malloc(sizeof(rlp_struct*));
    list->next = 0;
    return list;
}

rlp_struct* new_rlp_str(const char* data)
{
    rlp_struct* str = (rlp_struct*) malloc(sizeof(rlp_struct));
    str->data = malloc(strlen(data));
    strcpy(str->data, data);
    str->sub_arr = 0;
    str->next = 0;
}

void rlp_list_add(rlp_struct* root, rlp_struct* next)
{
    rlp_struct* tmp = root->sub_arr;
    for (; tmp->next; tmp = tmp->next)
        ;
    tmp->next = next;
}

long long get_rlp_list_size(rlp_struct* root)
{
    long long size = 0;
    rlp_struct* tmp = root;
    while (tmp->next) {
        tmp = tmp->next;
        if (tmp->data) {
            size += strlen(tmp->data);
        } else {
            size += get_rlp_list_size(tmp);
        }
    }
    return size;
}

void print_rlp_str(rlp_struct* str)
{
    printf("%s", str->data);
    if (str->next) {
        printf(", ");
    }
}

void print_rlp_list(rlp_struct* root)
{
    rlp_struct* tmp = root->sub_arr;
    printf(" [size: %lld ", get_rlp_list_size(tmp));    
    while (tmp->next) {
        tmp = tmp->next;
        if (tmp->data) {
            print_rlp_str(tmp);
        } else {
            print_rlp_list(tmp);
        }
    }
    printf("] ");
}

char* convert_be(long long size)
{
    char* arr = malloc(8);
    for (int i = 0; i < 8; ++i) {
        arr[i] = (unsigned char)((((unsigned long long) size) >> (56 - (8*i))) & 0xFFu);
    }
}

long long get_s_size(rlp_struct* root)
{
    long long size = 0;
    rlp_struct* tmp = root;
    while (tmp) {
        tmp = tmp->next;
        if (tmp) {
            char* rlp_output = rlp(tmp);
            size += strlen(rlp_output);
        }
    }
    return size;
}

char* s(rlp_struct* root)
{
    char* output = malloc(get_s_size(root));    
    char* list_head = output;
    rlp_struct* tmp = root;
    while (tmp) {
        tmp = tmp->next;
        if (tmp) {
            char* rlp_output = rlp(tmp);
            if (rlp_output) {
                memcpy(list_head, rlp_output, strlen(rlp_output));
                list_head += strlen(rlp_output);
            } else {
                return 0;
            }
        }
    }
    return output;
}

char* r_b(rlp_struct* root)
{
    long long size = strlen(root->data);
    if (size == 1 && root->data[0] < 128) {
        return root->data[0];
    } else if (size < 56) {
        char* output = malloc(size+1);
        output[0] = 0x80 + size;
        memcpy(output+1, root->data, size);
        return output;
    } else if (size >= 56 && size < 0xffffffff) {
        char* be_size_arr = convert_be(size);
        char* output = malloc(size+strlen(be_size_arr)+1);        
        output[0] = 183 + strlen(be_size_arr);
        memcpy(output+1, be_size_arr, strlen(be_size_arr));
        memcpy(output+strlen(be_size_arr)+1, root->data, size);
        return output;
    } else {
        return 0;
    }
}

char* r_l(rlp_struct* root)
{
    long long size = get_rlp_list_size(root);
    if (root) {
        if (size < 56) {
            char* s_output = s(root);
            char* output = malloc(strlen(s_output)+1);
            output[0] = 192 + strlen(s_output);
            memcpy(output+1, s_output, strlen(s_output));
            return output;
        } else if (size >= 56 && size < 0xffffffff) {
            char* s_output = s(root);
            char* be_size_arr = convert_be(strlen(s_output));
            char* output = malloc(strlen(s_output)+strlen(be_size_arr)+1);
            output[0] = 247 + strlen(be_size_arr);
            memcpy(output+1, be_size_arr, strlen(be_size_arr));
            memcpy(output+strlen(be_size_arr)+1, s_output, strlen(s_output));
            return output;
        }
    }
    return 0;
}

char* rlp(rlp_struct* val)
{
    return val->data ? r_b(val) : r_l(val->sub_arr);
}