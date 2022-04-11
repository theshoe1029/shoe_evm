#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rlp.h"

int main(int argc, char** argv) 
{
    char* output;

    rlp_struct* root = new_rlp_list();
    print_rlp_list(root);
    printf("\n");
    rlp_struct* s1 = new_rlp_str("dog");
    rlp_list_add(root, s1);
    rlp_struct* s2 = new_rlp_str("cat");
    rlp_list_add(root, s2);
    print_rlp_list(root);
    printf("\n");
    rlp_struct* child1 = new_rlp_list();
    rlp_list_add(root, child1);
    rlp_struct* s3 = new_rlp_str("goose");
    rlp_list_add(child1, s3);
    print_rlp_list(child1);
    print_rlp_list(root);
    printf("\n");
    rlp_struct* s4 = new_rlp_str("horse");
    rlp_list_add(root, s4);
    print_rlp_list(root);
    printf("\n");
    rlp_struct* child2 = new_rlp_list();
    rlp_list_add(root, child2);
    print_rlp_list(root);
    printf("\n");
    rlp_struct* child3 = new_rlp_list();
    rlp_list_add(child2, child3);
    print_rlp_list(root);
    printf("\n");

    rlp_struct* s5 = new_rlp_str("Lorem ipsum dolor sit amet, consectetur adipisicing elit");
    output = rlp(s5);
    for (int i = 0; i < strlen(output); i++) printf("%x ", output[i]);
    printf("\n");

    rlp_struct* root2 = new_rlp_list();
    rlp_struct* s6 = new_rlp_str("cat");
    rlp_struct* s7 = new_rlp_str("dog");
    rlp_list_add(root2, s6);
    rlp_list_add(root2, s7);
    output = rlp(root2);
    for (int i = 0; i < strlen(output); i++) printf("%x ", output[i]);
    printf("\n");

    //[ [], [[]], [ [], [[]] ] ]
    rlp_struct* l1 = new_rlp_list();
    rlp_struct* l2 = new_rlp_list();
    rlp_struct* l3 = new_rlp_list();
    rlp_struct* l4 = new_rlp_list();
    rlp_struct* l5 = new_rlp_list();
    rlp_struct* l6 = new_rlp_list();
    rlp_struct* l7 = new_rlp_list();
    rlp_struct* l8 = new_rlp_list();
    rlp_list_add(l7, l8);    
    rlp_list_add(l5, l6);
    rlp_list_add(l5, l7);
    rlp_list_add(l3, l4);
    rlp_list_add(l1, l2);
    rlp_list_add(l1, l3);
    rlp_list_add(l1, l5);
    output = rlp(l1);
    for (int i = 0; i < strlen(output); i++) printf("%x ", output[i]);
    printf("\n");
    return 0;
}