#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/keccak256.h"

int main(int argc, char** argv) 
{    
    int total_str_len = 0;
    for (int i = 1; i < argc; i++) total_str_len += strlen(argv[i]);
    total_str_len += argc - 2;
    char* input = (char*) malloc(sizeof(char)*total_str_len);
    memset(input, 0, sizeof(char)*total_str_len);
    for (int i = 1; i < argc; i++) {
        strcat(input, argv[i]);
        if (i != argc - 1) {
            strcat(input, " ");
        }        
    }

    unsigned char* output = keccak_256(input);
    printf("%p\n", output);
    for (size_t i = 0; i < 32; ++i) printf("%02x", output[i]);
    printf("\n");
    return 0;
}