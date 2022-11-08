#include <assert.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/evm.h"
#include "include/keccak256.h"

void destroy(struct evmc_vm *vm)
{
    free(vm);
}

void print_stack()
{
    for (int i = stack_top; i >= 0; i--) {
        printf("%x: ", i);
        print_word(stack[i]);
    }
}

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

struct evmc_result execute(struct evmc_vm* instance, 
    const struct evmc_host_interface* host,
    struct evmc_host_context* context,
    enum evmc_revision rev,
    const struct evmc_message* msg,
    const uint8_t* code,
    size_t code_size)
{
    pc = 0;
    stack_top = -1;
    memset(stack, 0, WORD_SIZE*STACK_SIZE);
    mem_size = 0;
    memory = (unsigned char*) malloc(sizeof(unsigned char*));
    struct evmc_result result = {
        .gas_left = msg->gas,
        .status_code = EVMC_SUCCESS,
        .create_address = 0,
        .output_data = 0,
        .output_size = 0,
        .padding = 0,
        .release = 0
    };    
    while (pc < code_size) {
        unsigned char a[WORD_SIZE], b[WORD_SIZE], c[WORD_SIZE], d[WORD_SIZE];
        uint8_t op = code[pc];
        switch(op) {
            case OP_STOP:
                pc = code_size;
                result.status_code = EVMC_SUCCESS;
                break;
            case OP_ADD:
                pop2(a, b);
                add_int256(a, b, stack[stack_top]);
                break;
            case OP_MUL:
                pop2(a, b);
                mul_int256(a, b, stack[stack_top]);
                break;
            case OP_SUB:
                pop2(a, b);
                sub_int256(a, b, stack[stack_top]);
                break;
            case OP_DIV:
                pop2(a, b);
                lt_int256(a, b, c);
                memset(stack[stack_top], 0, WORD_SIZE);
                if (!uint256_is_zero(b)) {
                    div_int256(a, b, stack[stack_top]);
                }
                break;
            case OP_SDIV:
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
                break;
            case OP_MOD:
                pop2(a, b);
                memset(stack[stack_top], 0, WORD_SIZE);
                if (!uint256_is_zero(b)) {
                    mod_int256(a, b, stack[stack_top]);
                }
                break;
            case OP_SMOD:
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
                break;
            case OP_ADDMOD:
                pop3(a, b, c);
                unsigned char sum[WORD_SIZE];
                memset(sum, 0, WORD_SIZE);
                memset(stack[stack_top], 0, WORD_SIZE);
                if (!uint256_is_zero(c)) {
                    add_int256(a, b, sum);
                    mod_int256(sum, c, stack[stack_top]);
                }
                break;
            case OP_MULMOD:
                pop3(a, b, c);
                unsigned char prod[WORD_SIZE];
                memset(prod, 0, WORD_SIZE);
                memset(stack[stack_top], 0, WORD_SIZE);
                if (!uint256_is_zero(c)) {
                    mul_int256(a, b, prod);
                    mod_int256(prod, c, stack[stack_top]);
                }
                break;
            case OP_EXP:
                pop2(a, b);
                memset(stack[stack_top], 0, WORD_SIZE);
                exp_uint256(a, b, stack[stack_top]);
                break;
            case OP_SIGNEXTEND:
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
                break;
            case OP_LT:
                pop2(a, b);
                lt_int256(a, b, stack[stack_top]);
                break;
            case OP_GT:
                pop2(a, b);
                gt_int256(a, b, stack[stack_top]);
                break;
            case OP_SLT:
                pop2(a, b);
                {
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
                break;
            case OP_SGT:
                pop2(a, b);
                {
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
                break;
            case OP_EQ:
                pop2(a, b);
                eq_int256(a, b, stack[stack_top]);
                break;
            case OP_ISZERO:
                pop1(a);
                uint_to_uint256(uint256_is_zero(stack[stack_top]), stack[stack_top]);
                break;
            case OP_AND:
                pop2(a, b);
                and_int256(a, b, stack[stack_top]);
                break;
            case OP_OR:
                pop2(a, b);
                or_int256(a, b, stack[stack_top]);
                break;
            case OP_XOR:
                pop2(a, b);
                xor_int256(a, b, stack[stack_top]);
                break;
            case OP_NOT:
                pop1(a);
                not_int256(stack[stack_top], stack[stack_top]);
                break;
            case OP_BYTE:
                pop2(a, b);
                memset(stack+stack_top, 0, WORD_SIZE);
                unsigned int uinti = to_uint(a);
                stack[stack_top][WORD_SIZE-1] = b[N_BYTES-uinti-1];
                break;
            case OP_SHL:
                pop2(a, b);
                shl_int256(a, b, stack[stack_top]);
                break;
            case OP_SHR:
                pop2(a, b);
                memset(stack+stack_top, 0, WORD_SIZE);
                shr_int256(a, b, stack[stack_top]);
                break;
            case OP_SAR:
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
                break;
            case OP_KECCAK256:
                pop2(a, b);
                {
                    size_t offset = to_uint64(a);
                    size_t length = to_uint64(b);
                    memcpy(stack+stack_top, keccak_256(length, (unsigned char*) memory+offset), WORD_SIZE);
                }
                break;
            case OP_ADDRESS:
                stack_top++; pc++;
                {
                    size_t addr_len = sizeof(msg->recipient.bytes);
                    memcpy(*(stack+stack_top)+WORD_SIZE-addr_len, msg->recipient.bytes, addr_len);
                }
                break;
            case OP_BALANCE:
                pop1(a);
                {
                    evmc_address *addr = (evmc_address*) malloc(sizeof(evmc_address));
                    memcpy(addr->bytes, stack[stack_top]+WORD_SIZE-sizeof(addr->bytes), sizeof(addr->bytes));                    
                    evmc_uint256be balance = (*(host->get_balance))(context, addr);
                    free(addr);
                    memcpy(stack+stack_top, balance.bytes, WORD_SIZE);
                }
            case OP_ORIGIN:
                stack_top++; pc++;
                {
                    struct evmc_tx_context tx_context = (*(host->get_tx_context))(context);
                    size_t origin_len = sizeof(tx_context.tx_origin.bytes);
                    memcpy(*(stack+stack_top)+WORD_SIZE-origin_len, tx_context.tx_origin.bytes, origin_len);
                }
                break;
            case OP_CALLER:
                stack_top++; pc++;
                {
                    size_t addr_len = sizeof(msg->sender.bytes);
                    memcpy(*(stack+stack_top)+WORD_SIZE-addr_len, msg->sender.bytes, addr_len);
                }
                break;
            case OP_CALLVALUE:
                stack_top++; pc++;
                memcpy(stack+stack_top, msg->value.bytes, WORD_SIZE);
                break;
            case OP_CALLDATALOAD:
                pop1(a);
                {
                    size_t i = to_uint64(a);
                    memcpy(stack+stack_top, msg->input_data+i, WORD_SIZE);
                }
                break;
            case OP_CALLDATASIZE:
                stack_top++; pc++;
                memcpy(*(stack+stack_top)+WORD_SIZE-sizeof(size_t), &(msg->input_size), sizeof(size_t));
                break;
            case OP_CALLDATACOPY:
                pop3(a, b, c); stack_top--;
                {
                    size_t dest_offset = to_uint64(a);
                    size_t offset = to_uint64(b);
                    size_t length = to_uint64(c);
                    if (dest_offset+(length*WORD_SIZE) > mem_size) {
                        size_t old_mem_size = mem_size;
                        mem_size = dest_offset+(length*WORD_SIZE);
                        memory = (unsigned char*) realloc(memory, mem_size);
                        memset(memory+old_mem_size, 0, mem_size-old_mem_size);
                    }
                    memcpy(memory+dest_offset, msg->input_data+offset, length);
                }
                break;
            case OP_CODESIZE:
                stack_top++; pc++;
                memcpy(*(stack+stack_top)+WORD_SIZE-sizeof(size_t), &code_size, sizeof(size_t));
                break;
            case OP_CODECOPY:
                pop3(a, b, c); stack_top--;
                {
                    size_t dest_offset = to_uint64(a);
                    size_t offset = to_uint64(b);
                    size_t length = to_uint64(c);
                    if (dest_offset+(length*WORD_SIZE) > mem_size) {
                        size_t old_mem_size = mem_size;
                        mem_size = dest_offset+(length*WORD_SIZE);
                        memory = (unsigned char*) realloc(memory, mem_size);
                        memset(memory+old_mem_size, 0, mem_size-old_mem_size);
                    }
                    memcpy(memory+dest_offset, code+offset, length);
                }
                break;
            case OP_GASPRICE:
                stack_top++; pc++;
                {
                    struct evmc_tx_context tx_context = (*(host->get_tx_context))(context);
                    print_word(tx_context.tx_gas_price.bytes);
                    memcpy(stack+stack_top, tx_context.tx_gas_price.bytes, WORD_SIZE);
                }
                break;
            case OP_POP:
                stack_top--; pc++;
                break;
            case OP_MLOAD:
                pop1(a);
                {
                    size_t offset = to_uint64(a);
                    memcpy(stack[stack_top], memory+offset, WORD_SIZE);   
                }
                break;
            case OP_MSTORE:
                pop2(a, b); stack_top--;
                {
                    size_t offset = to_uint64(a);
                    if (offset+WORD_SIZE > mem_size) {
                        size_t old_mem_size = mem_size;
                        mem_size = offset+WORD_SIZE;
                        memory = (unsigned char*) realloc(memory, mem_size);
                        memset(memory+old_mem_size, 0, mem_size-old_mem_size);
                    }
                    memcpy(memory+offset, b, WORD_SIZE);
                }
                break;
            case OP_MSTORE8:
                pop2(a, b); stack_top--;
                {
                    size_t offset = to_uint64(a);
                    if (offset+1 > mem_size) {
                        size_t old_mem_size = mem_size;
                        mem_size = offset+1;
                        memory = (unsigned char*) realloc(memory, mem_size);
                        memset(memory+old_mem_size, 0, mem_size-old_mem_size);
                    }
                    uint_to_uint256(256, c);
                    mod_int256(b, c, d);
                    memset(memory+offset, d[WORD_SIZE-1], 1);
                }
                break;
            case OP_SLOAD:
                pop1(a); stack_top--;
                printf("SLOAD not implemented\n");
                break;
            case OP_SSTORE:
                pop2(a, b); stack_top--;
                {
                    evmc_address *addr = (evmc_address*) malloc(sizeof(evmc_address));
                    memcpy(addr, msg->recipient.bytes, sizeof(evmc_address));
                    struct evmc_bytes32* key = (struct evmc_bytes32*) malloc(sizeof(struct evmc_bytes32));
                    struct evmc_bytes32* val = (struct evmc_bytes32*) malloc(sizeof(struct evmc_bytes32));
                    memcpy(key->bytes, a, sizeof(key->bytes));
                    memcpy(val->bytes, b, sizeof(key->bytes));
                    host->set_storage(context, addr, key, val);
                }
                break;
            case OP_PUSH0:
            case OP_PUSH1:
            case OP_PUSH2:
            case OP_PUSH3:
            case OP_PUSH4:
            case OP_PUSH5:
            case OP_PUSH6:
            case OP_PUSH7:
            case OP_PUSH8:
            case OP_PUSH9:
            case OP_PUSH10:
            case OP_PUSH11:
            case OP_PUSH12:
            case OP_PUSH13:
            case OP_PUSH14:
            case OP_PUSH15:
            case OP_PUSH16:
            case OP_PUSH17:
            case OP_PUSH18:
            case OP_PUSH19:
            case OP_PUSH20:
            case OP_PUSH21:
            case OP_PUSH22:
            case OP_PUSH23:
            case OP_PUSH24:
            case OP_PUSH25:
            case OP_PUSH26:
            case OP_PUSH27:
            case OP_PUSH28:
            case OP_PUSH29:
            case OP_PUSH30:
            case OP_PUSH31:
            case OP_PUSH32:
                {
                    size_t n_bytes = ((size_t) op) - OP_PUSH1 + 1;
                    pc++;
                    unsigned char* val = (unsigned char*) malloc(n_bytes);
                    size_t pcs = pc;
                    for (; pc < pcs+n_bytes; pc++) {
                        val[pc-pcs] = code[pc];
                    }
                    stack_top++;
                    memset(stack+stack_top, 0, WORD_SIZE);
                    memcpy(*(stack+stack_top)+WORD_SIZE-n_bytes, val, n_bytes);
                    free(val);
                }
                break;
            case OP_DUP1:
            case OP_DUP2:
            case OP_DUP3:
            case OP_DUP4:
            case OP_DUP5:
            case OP_DUP6:
            case OP_DUP7:
            case OP_DUP8:
            case OP_DUP9:
            case OP_DUP10:
            case OP_DUP11:
            case OP_DUP12:
            case OP_DUP13:
            case OP_DUP14:
            case OP_DUP15:
            case OP_DUP16:
                {
                    size_t n_bytes = ((size_t) op) - OP_DUP1 + 1;
                    unsigned char value[WORD_SIZE];
                    memcpy(value, stack+stack_top-n_bytes+1, WORD_SIZE);
                    stack_top++; pc++;
                    memcpy(stack+stack_top, value, WORD_SIZE);
                }
                break;
            case OP_SWAP1:
            case OP_SWAP2:
            case OP_SWAP3:
            case OP_SWAP4:
            case OP_SWAP5:
            case OP_SWAP6:
            case OP_SWAP7:
            case OP_SWAP8:
            case OP_SWAP9:
            case OP_SWAP10:
            case OP_SWAP11:
            case OP_SWAP12:
            case OP_SWAP13:
            case OP_SWAP14:
            case OP_SWAP15:
            case OP_SWAP16:
                {
                    size_t n_bytes = ((size_t) op) - OP_SWAP1 + 1;
                    memcpy(a, stack+stack_top, WORD_SIZE);
                    memcpy(b, stack+stack_top-n_bytes, WORD_SIZE);
                    memcpy(stack+stack_top, b, WORD_SIZE);
                    memcpy(stack+stack_top-n_bytes, a, WORD_SIZE);
                    pc++;   
                }
                break;
            case OP_CREATE:
                pop3(a, b, c);
                {
                    
                }
            case OP_RETURN:
                {
                    pc = code_size;
                    size_t offset = to_uint64(stack[stack_top]);
                    size_t length = stack[stack_top-1][WORD_SIZE-1];
                    result.output_data = (uint8_t*) malloc(length);
                    stack_top-=2;
                    memcpy((uint8_t*) result.output_data, memory+offset, length);
                    result.output_size = length;
                }
                break;
            default:
                printf("opcode %02x not implmented\n", op);
                pc = code_size;
        }
    }
    return result;
}

evmc_capabilities_flagset get_capabilities(struct evmc_vm* evm)
{
    return EVMC_CAPABILITY_EVM1;
}

EVMC_EXPORT struct evmc_vm* evmc_create()
{
    shoe_vm = (struct evmc_vm*) malloc(sizeof(struct evmc_vm));
    *((int *) &(shoe_vm->abi_version)) = 7;
    shoe_vm->name = "shoe_evm";
    shoe_vm->version = "0.0.0";
    shoe_vm->destroy = &destroy;
    shoe_vm->execute = &execute;
    shoe_vm->get_capabilities = &get_capabilities;
    shoe_vm->set_option = 0;
    return shoe_vm;
}