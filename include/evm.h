#include <stdio.h>

#include "evmc/evmc.h"
#include "evmc/helpers.h"
#include "evmc/instructions.h"
#include "evmc/utils.h"

#include "int256.h"

#ifndef EVM_H_
#define EVM_H_

#define WORD_SIZE 32
#define STACK_SIZE 1024

struct evmc_vm *shoe_vm;

size_t pc;
unsigned int stack_top;
unsigned char stack[STACK_SIZE][WORD_SIZE];
unsigned char* memory;
size_t mem_size;

#endif