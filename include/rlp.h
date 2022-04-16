#ifndef RLP_H_
#define RLP_H_

typedef enum rlp_type {
    BYTE_ARR,    
    UNSIGNED_INT,
    LIST
} rlp_type;

typedef struct rlp_struct {
    unsigned long long int int_val;
    unsigned char* data;
    rlp_type type;
    size_t size;

    struct rlp_struct* sub_arr;
    struct rlp_struct* next;
} rlp_struct;

rlp_struct* new_rlp_list();
rlp_struct* new_rlp_str(size_t size, unsigned const char* data);
rlp_struct* new_rlp_int(unsigned int int_val);
void rlp_list_add(rlp_struct* root, rlp_struct* next);
rlp_struct* rlp_list_at(size_t target, rlp_struct* root);
void print_rlp_str(rlp_struct* str);
void print_rlp_byte_arr(rlp_struct* str);
void print_rlp_list(rlp_struct* root);
void get_be_size(size_t* size, unsigned long long int val);
unsigned char* r_b(size_t* size, rlp_struct* root);
unsigned char* r_c(size_t* size, rlp_struct* root);
unsigned char* rlp_encode(size_t* size, rlp_struct* val);
rlp_struct* rlp_decode(size_t* decoded_size, unsigned char* rlp_data);

#endif