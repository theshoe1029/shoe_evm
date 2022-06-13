#include <stdio.h>

#include "keccak256_test.h"
#include "rlp_test.h"
#include "hex_prefix_test.h"
#include "trie_test.h"

void print_test_name(const char* test_name)
{
    printf("*****************************\n");
    printf("\tTest: %s\t\n", test_name);
    printf("*****************************\n");
}

int main(int argc, char** argv)
{
    print_test_name("keccak256");
    keccak256_test();
    print_test_name("hex prefix");
    hex_prefix_test();
    print_test_name("rlp");
    rlp_test();
    print_test_name("trie");
    trie_test();
    return 0;
}