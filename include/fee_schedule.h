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

#endif