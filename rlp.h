#ifndef RLP_H_
#define RLP_H_

typedef enum rlp_type {
    BYTE_ARR,    
    UNSIGNED_INT,
    LIST
} rlp_type;

typedef struct rlp_struct {
    unsigned long long int int_val;
    char* data;
    rlp_type type;
    size_t size;

    struct rlp_struct* sub_arr;
    struct rlp_struct* next;
} rlp_struct;

rlp_struct* new_rlp_list();
rlp_struct* new_rlp_str(size_t size, const char* data);
rlp_struct* new_rlp_int(unsigned int int_val);
void rlp_list_add(rlp_struct* root, rlp_struct* next);
void print_rlp_str(rlp_struct* str);
void print_rlp_list(rlp_struct* root);
void get_be_size(size_t* size, unsigned long long int val);
char* r_b(size_t* size, rlp_struct* root);
char* r_c(size_t* size, rlp_struct* root);
char* rlp_encode(size_t* size, rlp_struct* val);

#endif