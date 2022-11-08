#include <stdio.h>

#include "keccak256_test.h"

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
    return 0;
}