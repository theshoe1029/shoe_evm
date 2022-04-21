#include <stdio.h>

#include "int256.h"

#ifndef EVM_H_
#define EVM_H_

#define WORD_SIZE 32
#define STACK_SIZE 1024

size_t pc;
unsigned int stack_top;
unsigned char stack[STACK_SIZE][WORD_SIZE];
size_t bytecode_len;
unsigned char* bytecode;
unsigned int* memory;
size_t mem_size;
unsigned char* r_val;

void evm_exec(unsigned char* bytecode);

static void print_stack()
{
    for (int i = stack_top; i >= 0; i--) {
        printf("%x: ", i);
        print_word(stack[i]);
    }
}

static unsigned char to_byte(unsigned char c)
{
    if (c < 'a') {
        return c - '0';
    } else {
        return c - 'a' + 10;
    }
}

#endif