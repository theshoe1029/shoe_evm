#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include/evm.h"

int main(int argc, unsigned char** argv)
{
    evm_exec(argv[1]);
    print_stack();
    if (r_val != NULL) {
        print_word(r_val);
    }
}