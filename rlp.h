#ifndef RLP_H_
#define RLP_H_

typedef struct rlp_struct {
    char* data;
    struct rlp_struct* sub_arr;
    struct rlp_struct* next;
} rlp_struct;

rlp_struct* new_rlp_list();
rlp_struct* new_rlp_str(const char* data);
void rlp_list_add(rlp_struct* root, rlp_struct* next);
void print_rlp_str(rlp_struct* str);
void print_rlp_list(rlp_struct* root);
char* r_b(rlp_struct* root);
char* r_c(rlp_struct* root);
char* rlp(rlp_struct* val);

#endif