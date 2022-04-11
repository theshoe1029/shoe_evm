#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rlp.h"

int main(int argc, char** argv) 
{    
    rlp_struct* root = new_rlp_list();
    print_rlp_list(root);
    printf("\n");
    
    rlp_struct* s1 = new_rlp_str(3, "dog");
    rlp_list_add(root, s1);
    rlp_struct* s2 = new_rlp_str(3, "cat");
    rlp_list_add(root, s2);
    print_rlp_list(root);
    printf("\n");

    rlp_struct* child1 = new_rlp_list();
    rlp_list_add(root, child1);
    rlp_struct* s3 = new_rlp_str(5, "goose");
    rlp_list_add(child1, s3);
    print_rlp_list(child1);
    print_rlp_list(root);
    printf("\n");

    rlp_struct* s4 = new_rlp_str(5, "horse");
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

    unsigned long long int output_size;
    char* output;

    rlp_struct* s5 = new_rlp_str(56, "Lorem ipsum dolor sit amet, consectetur adipisicing elit");
    output_size = 0;
    output = rlp_encode(&output_size, s5);
    for (int i = 0; i < output_size; i++) printf("%x ", output[i]);
    printf("\n");

    rlp_struct* root2 = new_rlp_list();
    rlp_struct* s6 = new_rlp_str(3, "cat");
    rlp_struct* s7 = new_rlp_str(3, "dog");
    rlp_list_add(root2, s6);
    rlp_list_add(root2, s7);
    output_size = 0;
    output = rlp_encode(&output_size, root2);
    for (int i = 0; i < output_size; i++) printf("%x ", output[i]);
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
    output_size = 0;
    output = rlp_encode(&output_size, l1);
    for (int i = 0; i < output_size; i++) printf("%x ", output[i]);
    printf("\n");

    rlp_struct* n1 = new_rlp_int(15);
    output_size = 0;
    output = rlp_encode(&output_size, n1);
    for (int i = 0; i < output_size; i++) printf("%x ", output[i]);
    printf("\n");

    rlp_struct* n2 = new_rlp_int(1024);
    output_size = 0;
    output = rlp_encode(&output_size, n2);
    for (int i = 0; i < output_size; i++) printf("%x ", output[i]);
    printf("\n");

    //[ "zw", [ 4 ], 1 ]
    rlp_struct* l9 = new_rlp_list();
    rlp_struct* l10 = new_rlp_list();
    rlp_struct* s8 = new_rlp_str(2, "zw");
    rlp_struct* n3 = new_rlp_int(4);
    rlp_struct* n4 = new_rlp_int(1);
    rlp_list_add(l10, n3);
    rlp_list_add(l9, s8);
    rlp_list_add(l9, l10);
    rlp_list_add(l9, n4);
    output_size = 0;
    output = rlp_encode(&output_size, l9);
    for (int i = 0; i < output_size; i++) printf("%x ", output[i]);
    printf("\n");

    unsigned long long int decoded_size;

    output_size = 3;
    print_rlp_str(rlp_decode(&decoded_size, rlp_encode(&output_size, s6)));
    printf("\n");

    output_size = 56;
    print_rlp_str(rlp_decode(&decoded_size, rlp_encode(&output_size, s5)));
    printf("\n");

    output_size = 0;
    output = rlp_encode(&output_size, root2);
    print_rlp_list(rlp_decode(&decoded_size, output));
    printf("\n");

    output_size = 0;
    output = rlp_encode(&output_size, l9);
    print_rlp_list(rlp_decode(&decoded_size, output));
    printf("\n");

    output_size = 0;
    output = rlp_encode(&output_size, l1);
    print_rlp_list(rlp_decode(&decoded_size, output));
    printf("\n");

    rlp_struct* l11 = new_rlp_list();
    rlp_struct* s9 = new_rlp_str(56, "Lorem ipsum dolor sit amet, consectetur adipisicing elit");
    rlp_list_add(l11, s9);
    output_size = 0;
    output = rlp_encode(&output_size, l11);
    for (int i = 0; i < output_size; i++) printf("%x ", output[i]);
    printf("\n");
    return 0;
}