#include <assert.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "evm.h"
#include "keccak256.h"

void pop1(unsigned char* a)
{
    memcpy(a, stack[stack_top], WORD_SIZE);
    pc++;
}

void pop2(unsigned char* a, unsigned char* b)
{
    memcpy(a, stack[stack_top], WORD_SIZE);
    memcpy(b, stack[stack_top-1], WORD_SIZE);
    stack_top--; 
    pc++;
}

void pop3(unsigned char* a, unsigned char* b, unsigned char* c)
{
    memcpy(a, stack[stack_top], WORD_SIZE);
    memcpy(b, stack[stack_top-1], WORD_SIZE);
    memcpy(c, stack[stack_top-2], WORD_SIZE);
    stack_top-=2; 
    pc++;
}

void no_op() {}

void op_stop()
{
    pc = bytecode_len;
}

void op_add()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    add_int256(a, b, stack[stack_top]);
}

void op_mul()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    mul_int256(a, b, stack[stack_top]);
}

void op_sub()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    sub_int256(a, b, stack[stack_top]);
}

void op_div()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    unsigned char lt[WORD_SIZE];
    lt_int256(a, b, lt);
    memset(stack[stack_top], 0, WORD_SIZE);
    if (!uint256_is_zero(b)) {
        div_int256(a, b, stack[stack_top]);
    }
}

void op_sdiv()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    memset(stack[stack_top], 0, WORD_SIZE);
    unsigned char min_int256[WORD_SIZE]; memset(min_int256, 0x0, WORD_SIZE); min_int256[0] = 0x80;
    unsigned char neg_1[WORD_SIZE]; memset(neg_1, 0xff, WORD_SIZE);
    unsigned char is_a_min_int256[WORD_SIZE]; memset(is_a_min_int256, 0, WORD_SIZE);
    unsigned char is_b_neg_1[WORD_SIZE]; memset(is_b_neg_1, 0, WORD_SIZE);
    if (!uint256_is_zero(b)) {
        eq_int256(a, min_int256, is_a_min_int256);
        eq_int256(b, neg_1, is_b_neg_1);
        if (is_a_min_int256[WORD_SIZE-1] && is_b_neg_1[WORD_SIZE-1]) {
            memcpy(stack+stack_top, min_int256, WORD_SIZE);
        } else {
            unsigned char abs_a[WORD_SIZE], abs_b[WORD_SIZE];
            abs_int256(a, abs_a);
            abs_int256(b, abs_b);
            div_int256(abs_a, abs_b, stack[stack_top]);
            unsigned char sign = (a[0]>>7)^(b[0]>>7);
            if (sign) {
                neg_int256(stack[stack_top], stack[stack_top]);
            }
        }
    }
}

void op_mod()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    memset(stack[stack_top], 0, WORD_SIZE);
    if (!uint256_is_zero(b)) {
        mod_int256(a, b, stack[stack_top]);
    }
}

void op_smod()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    memset(stack[stack_top], 0, WORD_SIZE);
    if (!uint256_is_zero(b)) {
        unsigned char abs_a[WORD_SIZE], abs_b[WORD_SIZE];
        abs_int256(a, abs_a);
        abs_int256(b, abs_b);
        mod_int256(abs_a, abs_b, stack[stack_top]);
        unsigned char sign = a[0]>>7;
        if (sign) {
            neg_int256(stack[stack_top], stack[stack_top]);
        }
    }
}

void op_addmod()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE], c[WORD_SIZE];
    pop3(a, b, c);
    unsigned char sum[WORD_SIZE];
    memset(sum, 0, WORD_SIZE);
    memset(stack[stack_top], 0, WORD_SIZE);
    if (!uint256_is_zero(c)) {
        add_int256(a, b, sum);
        mod_int256(sum, c, stack[stack_top]);
    }
}

void op_mulmod()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE], c[WORD_SIZE];
    pop3(a, b, c);
    unsigned char prod[WORD_SIZE];
    memset(prod, 0, WORD_SIZE);
    memset(stack[stack_top], 0, WORD_SIZE);
    if (!uint256_is_zero(c)) {
        mul_int256(a, b, prod);
        mod_int256(prod, c, stack[stack_top]);
    }
}

void op_exp()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    memset(stack[stack_top], 0, WORD_SIZE);
    exp_uint256(a, b, stack[stack_top]);
}

void op_signextend()
{    
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    memset(stack[stack_top], 0, WORD_SIZE);

    assert(msb(a)>=WORD_SIZE-2);
    unsigned int uintb = to_uint(a);
    unsigned int t = 256-(8*(uintb+1));

    for (unsigned int i = 0; i < 32; i++) {
        if (i <= t/8) {
            stack[stack_top][i] = b[t/8];
        } else {
            stack[stack_top][i] = b[i];
        }
    }
}

void op_lt()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    lt_int256(a, b, stack[stack_top]);
}

void op_gt()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    gt_int256(a, b, stack[stack_top]);
}

void op_slt()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    unsigned char sa = a[0]>>7;
    unsigned char sb = b[0]>>7;
    if (sa == 0x1 && sb == 0x0) {
        uint_to_uint256(1, stack[stack_top]);
    } else if (sa == 0x0 && sb == 0x1) {
        uint_to_uint256(0, stack[stack_top]);
    } else {
        lt_int256(a, b, stack[stack_top]);
    }
}

void op_sgt()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    unsigned char sa = a[0]>>7;
    unsigned char sb = b[0]>>7;
    if (sa == 0x1 && sb == 0x0) {
        uint_to_uint256(0, stack[stack_top]);
    } else if (sa == 0x0 && sb == 0x1) {
        uint_to_uint256(1, stack[stack_top]);
    } else {
        gt_int256(a, b, stack[stack_top]);
    }
}

void op_eq()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    eq_int256(a, b, stack[stack_top]);
}

void op_iszero()
{
    unsigned char a[WORD_SIZE];
    pop1(a);
    uint_to_uint256(uint256_is_zero(stack[stack_top]), stack[stack_top]);
}

void op_and()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    and_int256(a, b, stack[stack_top]);
}

void op_or()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    or_int256(a, b, stack[stack_top]);
}

void op_xor()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    xor_int256(a, b, stack[stack_top]);
}

void op_not()
{
    unsigned char a[WORD_SIZE];
    pop1(a);
    not_int256(stack[stack_top], stack[stack_top]);
}

void op_byte()
{
    unsigned char i[WORD_SIZE], x[WORD_SIZE];
    pop2(i, x);
    memset(stack+stack_top, 0, WORD_SIZE);
    unsigned int uinti = to_uint(i);
    stack[stack_top][WORD_SIZE-1] = x[N_BYTES-uinti-1];
}

void op_shl()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    shl_int256(a, b, stack[stack_top]);
}

void op_shr()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    memset(stack+stack_top, 0, WORD_SIZE);
    shr_int256(a, b, stack[stack_top]);
}

void op_sar()
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    pop2(a, b);
    memset(stack+stack_top, 0, WORD_SIZE);
    unsigned char abs_a[WORD_SIZE], abs_b[WORD_SIZE];
    abs_int256(a, abs_a);
    abs_int256(b, abs_b);
    shr_int256(abs_a, abs_b, stack[stack_top]);
    unsigned char sign = b[0]>>7;
    if (sign) {
        neg_int256(stack[stack_top], stack[stack_top]);
    }
}

void op_sha3()
{
    unsigned char uint256_offset[WORD_SIZE], uint256_length[WORD_SIZE];
    pop2(uint256_offset, uint256_length);
    size_t offset = to_uint64(uint256_offset);
    size_t length = to_uint64(uint256_length);
    memcpy(stack+stack_top, keccak_256(length, (unsigned char*) memory+offset), 32);
}

void op_pop()
{
    stack_top--; pc++;
}

void op_mload()
{
    unsigned char uint256_offset[WORD_SIZE];
    pop1(uint256_offset);
    size_t offset = to_uint64(uint256_offset);
    memcpy(stack[stack_top], memory+offset, WORD_SIZE);
}

void op_mstore()
{
    unsigned char uint256_offset[WORD_SIZE], value[WORD_SIZE];
    pop2(uint256_offset, value); stack_top--;
    size_t offset = to_uint64(uint256_offset);
    if (offset+WORD_SIZE > mem_size) {
        size_t old_mem_size = mem_size;
        mem_size = offset+WORD_SIZE;
        memory = (unsigned char*) realloc(memory, mem_size);
        memset(memory+old_mem_size, 0, mem_size-old_mem_size);
    }
    memcpy(memory+offset, value, WORD_SIZE);
}

void op_mstore8()
{
    unsigned char uint256_offset[WORD_SIZE], uint256_256[WORD_SIZE], uint256_mod[WORD_SIZE], value[WORD_SIZE];
    pop2(uint256_offset, value); stack_top--;
    size_t offset = to_uint64(uint256_offset);
    if (offset+1 > mem_size) {
        size_t old_mem_size = mem_size;
        mem_size = offset+1;
        memory = (unsigned char*) realloc(memory, mem_size);
        memset(memory+old_mem_size, 0, mem_size-old_mem_size);
    }
    uint_to_uint256(256, uint256_256);
    mod_int256(value, uint256_256, uint256_mod);
    memset(memory+offset, uint256_mod[WORD_SIZE-1], 1);
}

void op_sload()
{
    unsigned char key[WORD_SIZE];
    pop1(key); stack_top--;
}

void op_sstore()
{
    pc++;
}

void op_push(size_t size)
{
    pc++;
    unsigned char* val = (unsigned char*) malloc(size);
    size_t pcs = pc;
    for (; pc < pcs+size; pc++) {
        val[pc-pcs] = bytecode[pc];
    }
    stack_top++;
    memset(stack+stack_top, 0, WORD_SIZE);
    memcpy(*(stack+stack_top)+WORD_SIZE-size, val, size);
    free(val);
}
void op_push1() { op_push(1); }
void op_push2() { op_push(2); }
void op_push3() { op_push(3); }
void op_push4() { op_push(4); }
void op_push5() { op_push(5); }
void op_push6() { op_push(6); }
void op_push7() { op_push(7); }
void op_push8() { op_push(8); }
void op_push9() { op_push(9); }
void op_push10() { op_push(10); }
void op_push11() { op_push(11); }
void op_push12() { op_push(12); }
void op_push13() { op_push(13); }
void op_push14() { op_push(14); }
void op_push15() { op_push(15); }
void op_push16() { op_push(16); }
void op_push17() { op_push(17); }
void op_push18() { op_push(18); }
void op_push19() { op_push(19); }
void op_push20() { op_push(20); }
void op_push21() { op_push(21); }
void op_push22() { op_push(22); }
void op_push23() { op_push(23); }
void op_push24() { op_push(24); }
void op_push25() { op_push(25); }
void op_push26() { op_push(26); }
void op_push27() { op_push(27); }
void op_push28() { op_push(28); }
void op_push29() { op_push(29); }
void op_push30() { op_push(30); }
void op_push31() { op_push(31); }
void op_push32() { op_push(32); }

void op_dup(size_t n)
{
    unsigned char value[WORD_SIZE];
    memcpy(value, stack+stack_top-n+1, WORD_SIZE);
    stack_top++; pc++;
    memcpy(stack+stack_top, value, WORD_SIZE);
}
void op_dup1() { op_dup(1); }
void op_dup2() { op_dup(2); }
void op_dup3() { op_dup(3); }
void op_dup4() { op_dup(4); }
void op_dup5() { op_dup(5); }
void op_dup6() { op_dup(6); }
void op_dup7() { op_dup(7); }
void op_dup8() { op_dup(8); }
void op_dup9() { op_dup(9); }
void op_dup10() { op_dup(10); }
void op_dup11() { op_dup(11); }
void op_dup12() { op_dup(12); }
void op_dup13() { op_dup(13); }
void op_dup14() { op_dup(14); }
void op_dup15() { op_dup(15); }
void op_dup16() { op_dup(16); }

void op_swap(size_t n)
{
    unsigned char a[WORD_SIZE], b[WORD_SIZE];
    memcpy(a, stack+stack_top, WORD_SIZE);
    memcpy(b, stack+stack_top-n, WORD_SIZE);
    memcpy(stack+stack_top, b, WORD_SIZE);
    memcpy(stack+stack_top-n, a, WORD_SIZE);
    pc++;
}
void op_swap1() { op_swap(1); }
void op_swap2() { op_swap(2); }
void op_swap3() { op_swap(3); }
void op_swap4() { op_swap(4); }
void op_swap5() { op_swap(5); }
void op_swap6() { op_swap(6); }
void op_swap7() { op_swap(7); }
void op_swap8() { op_swap(8); }
void op_swap9() { op_swap(9); }
void op_swap10() { op_swap(10); }
void op_swap11() { op_swap(11); }
void op_swap12() { op_swap(12); }
void op_swap13() { op_swap(13); }
void op_swap14() { op_swap(14); }
void op_swap15() { op_swap(15); }
void op_swap16() { op_swap(16); }

void op_return()
{
    unsigned long long offset = to_uint64(stack[stack_top]);
    unsigned char length = stack[stack_top-1][WORD_SIZE-1];
    r_val = (unsigned char*) malloc(length);
    stack_top-=2;
    memcpy(r_val, memory+offset, length);
}

void exec_op(unsigned char opcode)
{
    void (*opcode_table[256])() = 
    {
        /*0x00*/ &op_stop,
        /*0x01*/ &op_add,
        /*0x02*/ &op_mul,
        /*0x03*/ &op_sub,
        /*0x04*/ &op_div,
        /*0x05*/ &op_sdiv,
        /*0x06*/ &op_mod,
        /*0x07*/ &op_smod,
        /*0x08*/ &op_addmod,
        /*0x09*/ &op_mulmod,
        /*0x0a*/ &op_exp,
        /*0x0b*/ &op_signextend,

        /*0x0c*/ &no_op,
        /*0x0d*/ &no_op,
        /*0x0e*/ &no_op,
        /*0x0f*/ &no_op,

        /*0x10*/ &op_lt,
        /*0x11*/ &op_gt,
        /*0x12*/ &op_slt,
        /*0x13*/ &op_sgt,
        /*0x14*/ &op_eq,
        /*0x15*/ &op_iszero,
        /*0x16*/ &op_and,
        /*0x17*/ &op_or,
        /*0x18*/ &op_xor,
        /*0x19*/ &op_not,
        /*0x1a*/ &op_byte,
        /*0x1b*/ &op_shl,
        /*0x1c*/ &op_shr,
        /*0x1d*/ &op_sar,

        /*0x1e*/ &no_op,
        /*0x1f*/ &no_op,

        /*0x20*/ &op_sha3,

        /*0x21*/ &no_op,
        /*0x22*/ &no_op,
        /*0x23*/ &no_op,
        /*0x24*/ &no_op,
        /*0x25*/ &no_op,
        /*0x26*/ &no_op,
        /*0x27*/ &no_op,
        /*0x28*/ &no_op,
        /*0x29*/ &no_op,
        /*0x2a*/ &no_op,
        /*0x2b*/ &no_op,
        /*0x2c*/ &no_op,
        /*0x2d*/ &no_op,
        /*0x2e*/ &no_op,
        /*0x2f*/ &no_op,

        /*0x30*/ &no_op,
        /*0x31*/ &no_op,
        /*0x32*/ &no_op,
        /*0x33*/ &no_op,
        /*0x34*/ &no_op,
        /*0x35*/ &no_op,
        /*0x36*/ &no_op,
        /*0x37*/ &no_op,
        /*0x38*/ &no_op,
        /*0x39*/ &no_op,
        /*0x3a*/ &no_op,
        /*0x3b*/ &no_op,
        /*0x3c*/ &no_op,
        /*0x3d*/ &no_op,
        /*0x3e*/ &no_op,
        /*0x3f*/ &no_op,
        /*0x40*/ &no_op,
        /*0x41*/ &no_op,
        /*0x42*/ &no_op,
        /*0x43*/ &no_op,
        /*0x44*/ &no_op,
        /*0x45*/ &no_op,
        /*0x46*/ &no_op,
        /*0x47*/ &no_op,
        /*0x48*/ &no_op,

        /*0x49*/ &no_op,
        /*0x4a*/ &no_op,
        /*0x4b*/ &no_op,
        /*0x4c*/ &no_op,
        /*0x4d*/ &no_op,
        /*0x4e*/ &no_op,
        /*0x4f*/ &no_op,

        /*0x50*/ &op_pop,
        /*0x51*/ &op_mload,
        /*0x52*/ &op_mstore,
        /*0x53*/ &op_mstore8,
        /*0x54*/ &op_sload,
        /*0x55*/ &op_sstore,
        /*0x56*/ &no_op,
        /*0x57*/ &no_op,
        /*0x58*/ &no_op,
        /*0x59*/ &no_op,
        /*0x5a*/ &no_op,
        /*0x5b*/ &no_op,

        /*0x5c*/ &no_op,
        /*0x5d*/ &no_op,
        /*0x5e*/ &no_op,
        /*0x5f*/ &no_op,

        /*0x60*/ &op_push1,
        /*0x61*/ &op_push2,
        /*0x62*/ &op_push3,
        /*0x63*/ &op_push4,
        /*0x64*/ &op_push5,
        /*0x65*/ &op_push6,
        /*0x66*/ &op_push7,
        /*0x67*/ &op_push8,
        /*0x68*/ &op_push9,
        /*0x69*/ &op_push10,
        /*0x6a*/ &op_push11,
        /*0x6b*/ &op_push12,
        /*0x6c*/ &op_push13,
        /*0x6d*/ &op_push14,
        /*0x6e*/ &op_push15,
        /*0x6f*/ &op_push16,
        /*0x70*/ &op_push17,
        /*0x71*/ &op_push18,
        /*0x72*/ &op_push19,
        /*0x73*/ &op_push20,
        /*0x74*/ &op_push21,
        /*0x75*/ &op_push22,
        /*0x76*/ &op_push23,
        /*0x77*/ &op_push24,
        /*0x78*/ &op_push25,
        /*0x79*/ &op_push26,
        /*0x7a*/ &op_push27,
        /*0x7b*/ &op_push28,
        /*0x7c*/ &op_push29,
        /*0x7d*/ &op_push30,
        /*0x7e*/ &op_push31,
        /*0x7f*/ &op_push32,
        /*0x80*/ &op_dup1,
        /*0x81*/ &op_dup2,
        /*0x82*/ &op_dup3,
        /*0x83*/ &op_dup4,
        /*0x84*/ &op_dup5,
        /*0x85*/ &op_dup6,
        /*0x86*/ &op_dup7,
        /*0x87*/ &op_dup8,
        /*0x88*/ &op_dup9,
        /*0x89*/ &op_dup10,
        /*0x8a*/ &op_dup11,
        /*0x8b*/ &op_dup12,
        /*0x8c*/ &op_dup13,
        /*0x8d*/ &op_dup14,
        /*0x8e*/ &op_dup15,
        /*0x8f*/ &op_dup16,
        /*0x90*/ &op_swap1,
        /*0x91*/ &op_swap2,
        /*0x92*/ &op_swap3,
        /*0x93*/ &op_swap4,
        /*0x94*/ &op_swap5,
        /*0x95*/ &op_swap6,
        /*0x96*/ &op_swap7,
        /*0x97*/ &op_swap8,
        /*0x98*/ &op_swap9,
        /*0x99*/ &op_swap10,
        /*0x9a*/ &op_swap11,
        /*0x9b*/ &op_swap12,
        /*0x9c*/ &op_swap13,
        /*0x9d*/ &op_swap14,
        /*0x9e*/ &op_swap15,
        /*0x9f*/ &op_swap16,
        /*0xa0*/ &no_op,
        /*0xa1*/ &no_op,
        /*0xa2*/ &no_op,
        /*0xa3*/ &no_op,
        /*0xa4*/ &no_op,

        /*0xa5*/ &no_op,
        /*0xa6*/ &no_op,
        /*0xa7*/ &no_op,
        /*0xa8*/ &no_op,
        /*0xa9*/ &no_op,
        /*0xaa*/ &no_op,
        /*0xab*/ &no_op,
        /*0xac*/ &no_op,
        /*0xad*/ &no_op,
        /*0xae*/ &no_op,
        /*0xaf*/ &no_op,

        /*0xb0*/ &no_op,
        /*0xb1*/ &no_op,
        /*0xb2*/ &no_op,

        /*0xb3*/ &no_op,
        /*0xb4*/ &no_op,
        /*0xb5*/ &no_op,
        /*0xb6*/ &no_op,
        /*0xb7*/ &no_op,
        /*0xb8*/ &no_op,
        /*0xb9*/ &no_op,
        /*0xba*/ &no_op,
        /*0xbb*/ &no_op,
        /*0xbc*/ &no_op,
        /*0xbd*/ &no_op,
        /*0xbe*/ &no_op,
        /*0xbf*/ &no_op,
        /*0xc0*/ &no_op,
        /*0xc1*/ &no_op,
        /*0xc2*/ &no_op,
        /*0xc3*/ &no_op,
        /*0xc4*/ &no_op,
        /*0xc5*/ &no_op,
        /*0xc6*/ &no_op,
        /*0xc7*/ &no_op,
        /*0xc8*/ &no_op,
        /*0xc9*/ &no_op,
        /*0xca*/ &no_op,
        /*0xcb*/ &no_op,
        /*0xcc*/ &no_op,
        /*0xcd*/ &no_op,
        /*0xce*/ &no_op,
        /*0xcf*/ &no_op,
        /*0xd0*/ &no_op,
        /*0xd1*/ &no_op,
        /*0xd2*/ &no_op,
        /*0xd3*/ &no_op,
        /*0xd4*/ &no_op,
        /*0xd5*/ &no_op,
        /*0xd6*/ &no_op,
        /*0xd7*/ &no_op,
        /*0xd8*/ &no_op,
        /*0xd9*/ &no_op,
        /*0xda*/ &no_op,
        /*0xdb*/ &no_op,
        /*0xdc*/ &no_op,
        /*0xdd*/ &no_op,
        /*0xde*/ &no_op,
        /*0xdf*/ &no_op,
        /*0xe0*/ &no_op,
        /*0xe1*/ &no_op,
        /*0xe2*/ &no_op,
        /*0xe3*/ &no_op,
        /*0xe4*/ &no_op,
        /*0xe5*/ &no_op,
        /*0xe6*/ &no_op,
        /*0xe7*/ &no_op,
        /*0xe8*/ &no_op,
        /*0xe9*/ &no_op,
        /*0xea*/ &no_op,
        /*0xeb*/ &no_op,
        /*0xec*/ &no_op,
        /*0xed*/ &no_op,
        /*0xee*/ &no_op,
        /*0xef*/ &no_op,

        /*0xf0*/ &no_op,
        /*0xf1*/ &no_op,
        /*0xf2*/ &no_op,
        /*0xf3*/ &op_return,
        /*0xf4*/ &no_op,
        /*0xf5*/ &no_op,

        /*0xf6*/ &no_op,
        /*0xf7*/ &no_op,
        /*0xf8*/ &no_op,
        /*0xf9*/ &no_op,

        /*0xfa*/ &no_op,

        /*0xfb*/ &no_op,
        /*0xfc*/ &no_op,

        /*0xfd*/ &no_op,

        /*0xfe*/ &no_op,

        /*0xff*/ &no_op
    };
    (*opcode_table[opcode])();
}

void evm_exec(const char* code)
{
    assert(strlen(code)%2==0);

    bytecode_len = strlen(code)/2;
    bytecode = (unsigned char*) malloc(bytecode_len+1);
    for (int i = 0; i < strlen(code); i+=2) {
        bytecode[i/2] = (to_byte(code[i])<<4)+to_byte(code[i+1]);
    }
    bytecode[bytecode_len] = '\0';

    pc = 0;
    stack_top = -1;
    memset(stack, 0, WORD_SIZE*STACK_SIZE);
    mem_size = 0;
    memory = (unsigned char*) malloc(sizeof(unsigned char*));
    r_val = NULL;
    while (pc < bytecode_len && r_val == NULL) {
        exec_op(bytecode[pc]);
    }
}