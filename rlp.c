#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "rlp.h"

rlp_struct* new_rlp_list()
{
    rlp_struct* list = (rlp_struct*) malloc(sizeof(rlp_struct));
    list->int_val = 0;
    list->data = 0;
    list->type = LIST;
    list->size = 0;
    list->sub_arr = (rlp_struct*) malloc(sizeof(rlp_struct));
    list->next = 0;
}

rlp_struct* new_rlp_str(size_t size, const char* data)
{
    rlp_struct* str = (rlp_struct*) malloc(sizeof(rlp_struct));
    str->int_val = 0;
    str->data = (char*) malloc(size);
    strcpy(str->data, data);
    str->type = BYTE_ARR;
    str->size = size;
    str->sub_arr = 0;
    str->next = 0;
}

rlp_struct* new_rlp_int(unsigned int int_val)
{
    rlp_struct* num = (rlp_struct*) malloc(sizeof(rlp_struct));
    num->int_val = int_val;
    num->data = 0;
    num->type = UNSIGNED_INT;
    get_be_size(&(num->size), int_val);
    num->sub_arr = 0;
    num->next = 0;
}

void rlp_list_add(rlp_struct* root, rlp_struct* next)
{
    rlp_struct* tmp = root->sub_arr;
    for (; tmp->next; tmp = tmp->next)
        ;
    tmp->next = next;
}

size_t get_rlp_list_size(rlp_struct* root)
{
    size_t size = 0;
    rlp_struct* tmp = root;
    while (tmp && tmp->next) {
        tmp = tmp->next;
        switch (tmp->type) {
            case LIST:
                size += get_rlp_list_size(tmp);
                break;
            default:
                size += tmp->size;
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

void print_rlp_int(rlp_struct* num)
{
    printf("%llu", num->int_val);
    if (num->next) {
        printf(", ");
    }
}

void print_rlp_list(rlp_struct* root)
{
    rlp_struct* tmp = root->sub_arr;
    printf(" [size: %lld ", get_rlp_list_size(tmp));
    while (tmp && tmp->next) {
        tmp = tmp->next;
        switch (tmp->type) {
            case BYTE_ARR:
                print_rlp_str(tmp);
                break;
            case UNSIGNED_INT:
                print_rlp_int(tmp);
                break;
            case LIST:
                print_rlp_list(tmp);
                break;
        }
    }
    printf("] ");
}

void get_be_size(size_t* size, unsigned long long int val)
{
    *size = 0;
    for (int i = 0; i < 8; i++) {
        if (val >> (56 - (8*i)) & 0xFF) {
            *size = 8 - i;
        }
    }
}

char* convert_be(size_t* size, unsigned long long int val)
{
    get_be_size(size, val);
    char* arr = (char*) malloc(*size);
    for (int i = 0; i < *size; i++) {
        arr[i] = (char)(val >> (8*(*size-i-1)) & 0xFF);
    }
    return arr;
}

char* s(size_t* size, rlp_struct* root)
{    
    char* output = (char*) malloc(sizeof(char*));
    char* list_head = output;
    rlp_struct* tmp = root;
    while (tmp) {
        tmp = tmp->next;
        if (tmp) {
            size_t tmp_size = 0;
            char* rlp_output = rlp_encode(&tmp_size, tmp);
            if (rlp_output) {
                *size = *size + tmp_size;
                output = (char*) realloc(output, *size);
                memcpy(list_head, rlp_output, tmp_size);
                list_head += tmp_size;
            } else {
                return 0;
            }
        }
    }
    return output;
}

char* r_b(size_t* output_size, rlp_struct* root)
{
    size_t size = root->size;
    if (size == 1 && root->data[0] < 128) {
        *output_size = 1;        
        char* output = (char*) malloc(*output_size);
        output[0] = root->data[0];
        return output;
    } else if (size < 56) {
        *output_size = size + 1;
        char* output = (char*) malloc(*output_size);
        output[0] = 0x80 + size;
        memcpy(output+1, root->data, size);
        return output;
    } else if (size >= 56 && size < 0xffffffff) {
        size_t be_size_arr_len;
        char* be_size_arr = convert_be(&be_size_arr_len, size);
        *output_size = size+be_size_arr_len+1;
        char* output = (char*) malloc(*output_size);
        output[0] = 183 + be_size_arr_len;
        memcpy(output+1, be_size_arr, be_size_arr_len);
        memcpy(output+be_size_arr_len+1, root->data, size);
        return output;
    } else {
        return 0;
    }
}

char* r_l(size_t* output_size, rlp_struct* root)
{
    size_t size = get_rlp_list_size(root);
    if (root) {
        if (size < 56) {
            char* s_output = s(output_size, root);
            char* output = (char*) malloc(*output_size+1);
            output[0] = 192 + *output_size;
            memcpy(output+1, s_output, *output_size);
            *output_size += 1;
            return output;
        } else if (size >= 56 && size < 0xffffffff) {
            char* s_output = s(output_size, root);
            size_t be_size_arr_len;
            char* be_size_arr = convert_be(&be_size_arr_len, *output_size);
            char* output = (char*) malloc(*output_size+be_size_arr_len+1);
            output[0] = 247 + be_size_arr_len;
            memcpy(output+1, be_size_arr, be_size_arr_len);
            memcpy(output+be_size_arr_len+1, s_output, *output_size);
            *output_size += be_size_arr_len + 1;
            return output;
        }
    }
    return 0;
}

char* rlp_encode(size_t* size, rlp_struct* val)
{
    switch (val->type) {        
        case BYTE_ARR:
            return r_b(size, val);
        case UNSIGNED_INT:
            val->data = convert_be(&(val->size), val->int_val);
            val->int_val = 0;
            val->type = BYTE_ARR;
            return rlp_encode(size, val);
        case LIST:
            return r_l(size, val->sub_arr);
    }
}