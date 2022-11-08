#include "evm.h"

#ifndef FEE_SCHEDULE_H_
#define FEE_SCHEDULE_H_

#define G_ZERO 0
#define G_JUMP_DEST 1
#define G_BASE 2
#define G_VERY_LOW 3
#define G_LOW 5
#define G_MID 8
#define G_HIGH 10
#define G_WARM_ACCESS 100
#define G_ACCESS_LIST_ADDR 2400
#define G_ACCESS_LIST_STORAGE 1900
#define G_COLD_ACCOUNT_ACCESS 2600
#define G_COLD_SLOAD 2100
#define G_SSET 20000
#define G_SRESET 2900
#define R_SCLEAR 15000
#define R_SELF_DESTRUCT 24000
#define G_SELF_DESTRUCT 5000
#define G_CREATE 32000
#define G_CODE_DEPOSIT 200
#define G_CALL_VALUE 9000
#define G_CALL_STIPEND 2300
#define G_NEW_ACCOUNT 25000
#define G_EXP 10
#define G_EXP_BYTE 50
#define G_MEMORY 3
#define G_TX_CREATE 32000
#define G_TX_DATA_ZERO 4
#define G_TX_DATA_NONZERO 16
#define G_TX 21000
#define G_LOG 375
#define G_LOG_DATA 8
#define G_LOG_TOPIC 375
#define G_KECCAK256 30
#define G_KECCAK256_WORD 6
#define G_COPY 3
#define G_BLOCKHASH 20

int64_t get_gas_fee(uint8_t op)
{
    switch(op) {
        case OP_STOP:
            return 0;
        case OP_ADD:
            return 3;
        case OP_MUL:
        case OP_SUB:
        case OP_DIV:
        case OP_SDIV:
        case OP_MOD:
        case OP_SMOD:
        case OP_ADDMOD:
        case OP_MULMOD:
        case OP_EXP:
        case OP_SIGNEXTEND:
        case OP_LT:
        case OP_GT:
        case OP_SLT:
        case OP_SGT:
        case OP_EQ:
        case OP_ISZERO:
        case OP_AND:
        case OP_OR:
        case OP_XOR:
        case OP_NOT:
        case OP_BYTE:
        case OP_SHL:
        case OP_SHR:
        case OP_SAR:
        case OP_KECCAK256:
        case OP_ADDRESS:
        case OP_BALANCE:
        case OP_ORIGIN:
        case OP_CALLER:
        case OP_CALLVALUE:
        case OP_CALLDATALOAD:
        case OP_CALLDATASIZE:
        case OP_CALLDATACOPY:
        case OP_CODESIZE:
        case OP_CODECOPY:
        case OP_GASPRICE:
        case OP_POP:
        case OP_MLOAD:
        case OP_MSTORE:
        case OP_MSTORE8:
        case OP_SLOAD:
        case OP_SSTORE:
            return 22100;
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
            return 3;
        case OP_CREATE:
        case OP_RETURN:
            return 0;
        case OP_REVERT:
            return 0;
        default:
            return INT_FAST64_MAX;
    }
}

#endif