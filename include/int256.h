#include <string.h>

#ifndef INT_256_H_
#define INT_256_H_

#define N_BYTES 32

static void print_word(unsigned char* word)
{
    printf("bytes: ");
    for (int i = 0; i < N_BYTES; i++) printf("%x ", word[i]);
    long lval = 0;
    for (int i = 1; i <= 4; i++) lval += word[N_BYTES-i]<<(8*(i-1));
    printf("int: %li", lval);
    printf("\n");
}

static unsigned int msb(unsigned char* v1)
{
    unsigned int i = 0;
    for (; v1[i] == 0x00 && i < N_BYTES; i++);
    return i;
}

static int uint256_is_zero(unsigned char* v1)
{
    for (int i = 0; i < N_BYTES; i++) {
        if (v1[i] != 0x00) return 0;
    }
    return 1;
}

static void add_int256(unsigned char* v1, unsigned char* v2, unsigned char* out);
static void sub_int256(unsigned char* v1, unsigned char* v2, unsigned char* out);

static void inc_int256(unsigned char *v1, unsigned char* out)
{
    unsigned char one[N_BYTES];
    memset(one, 0, N_BYTES);
    one[N_BYTES-1] = 0x01;
    add_int256(v1, one, out);
}

static void dec_int256(unsigned char *v1, unsigned char* out)
{
    unsigned char one[N_BYTES];
    memset(one, 0, N_BYTES);
    one[N_BYTES-1] = 0x01;
    sub_int256(v1, one, out);
}

static void uint_to_uint256(unsigned int v1, unsigned char* v2)
{
    memset(v2, 0, N_BYTES);
    v2[N_BYTES-1] = v1&0xff;
    v2[N_BYTES-2] = (v1>>8)&0xff;
}

static void neg_int256(unsigned char* v1, unsigned char* out)
{
    memcpy(out, v1, N_BYTES);
    for (int i = 0; i < N_BYTES; i++) {
        out[i] = out[i]^0xff;
    }
    inc_int256(out, out);
}

static void abs_int256(unsigned char* v1, unsigned char* out)
{
    memcpy(out, v1, N_BYTES);
    if (v1[0]>>7==0x1) {
        neg_int256(v1, out);
    }    
}

static void eq_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    for (int i = 0; i < N_BYTES; i++) {
        if (v1[i] != v2[i]) {
            memset(out, 0, N_BYTES);
            return;
        }
    }
    memset(out, 0, N_BYTES);
    out[N_BYTES-1] = 0x1;
}

static void gt_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    memset(out, 0, N_BYTES);
    for (int i = 0; i < N_BYTES; i++) {
        if (v1[i] > v2[i]) {
            out[N_BYTES-1] = 0x1;
            return;
        } else if (v1[i] < v2[i]) {
            return;
        }
    }
}

static void lt_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    gt_int256(v1, v2, out);
    out[N_BYTES-1] = !out[N_BYTES-1];
}

static void and_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    for (int i = 0; i < N_BYTES; i++) {
        out[i] = v1[i]&v2[i];
    }
}

static void or_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    for (int i = 0; i < N_BYTES; i++) {
        out[i] = v1[i]|v2[i];
    }
}

static void xor_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    for (int i = 0; i < N_BYTES; i++) {
        out[i] = v1[i]^v2[i];
    }
}

static void not_int256(unsigned char* v1, unsigned char* out)
{
    for (int i = 0; i < N_BYTES; i++) {
        out[i] = !v1[i];
    }
}

static void add_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    unsigned char carry = 0;
    for (int i = N_BYTES-1; i >= 0; i--) {
        unsigned int sum = v1[i]+v2[i]+carry;
        carry = (v1[i]+v2[i]+carry)>>8;
        out[i] = sum&0xff;
    }
}

static void sub_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    unsigned char neg_v2[N_BYTES];
    neg_int256(v2, neg_v2);
    add_int256(v1, neg_v2, out);
}

static void mul_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    unsigned char v3[32], iter[32];
    if (msb(v1) > msb(v2)) {
        memcpy(v3, v2, N_BYTES);
        memcpy(iter, v1, N_BYTES);
    } else {
        memcpy(v3, v1, N_BYTES);
        memcpy(iter, v2, N_BYTES);
    }
    memset(out, 0, N_BYTES);
    while(!uint256_is_zero(v3) && !uint256_is_zero(iter)) {
        add_int256(out, v3, out);
        dec_int256(iter, iter);
    }
}

static void exp_uint256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    unsigned char exp[N_BYTES];
    memcpy(exp, v2, N_BYTES);
    memset(out, 0, N_BYTES);
    out[N_BYTES-1] = 0x01;
    while (!uint256_is_zero(exp)) {
        mul_int256(out, v1, out);
        dec_int256(exp, exp);
    }
}

static void shl_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    unsigned char k2[N_BYTES];
    unsigned char pow2[N_BYTES];
    uint_to_uint256(2, k2);
    exp_uint256(k2, v1, pow2);
    mul_int256(v2, pow2, out);
}

static void div_helper(size_t dvdend_len, unsigned char* dvdend, unsigned char* dvisor, unsigned char* out)
{
    size_t dvisor_len = N_BYTES-msb(dvisor);
    unsigned char v3[N_BYTES];
    mul_int256(dvisor, out, v3);
    sub_int256(dvdend, v3, v3);
    unsigned char cdvdend[N_BYTES]; memset(cdvdend, 0, N_BYTES); memcpy(cdvdend+N_BYTES-dvdend_len, v3, dvdend_len);
    unsigned char cout[N_BYTES]; memset(cout, 0, N_BYTES);
    unsigned char lt[N_BYTES], eq[N_BYTES];
    unsigned char one[N_BYTES]; memset(one, 0, N_BYTES); one[N_BYTES-1] = 0x01;
    lt_int256(cdvdend, dvisor, lt);
    eq_int256(cdvdend, dvisor, eq);
    if (dvdend_len > N_BYTES) {
        return;
    } else if (dvisor_len < dvdend_len && (!lt[N_BYTES-1] || eq[N_BYTES-1])) {
        while (!lt[N_BYTES-1] || eq[N_BYTES-1]) {
            add_int256(cout, one, cout);
            mul_int256(dvisor, cout, v3);
            lt_int256(cdvdend, v3, lt);
            eq_int256(cdvdend, v3, eq);
        }
        if (!eq[N_BYTES-1]) {
            sub_int256(cout, one, cout);
        }
        memset(v3, 0, N_BYTES);
        uint_to_uint256((N_BYTES-dvdend_len)*8, v3);
        shl_int256(v3, cout, cout);
        add_int256(out, cout, out);
    }
    div_helper(dvdend_len+1, dvdend, dvisor, out);
}

static void div_int256(unsigned char* dvdend, unsigned char* dvisor, unsigned char* out)
{
    div_helper(1, dvdend, dvisor, out);
}

static void shr_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    unsigned char k2[N_BYTES];
    unsigned char pow2[N_BYTES];
    uint_to_uint256(2, k2);
    exp_uint256(k2, v1, pow2);
    div_int256(v2, pow2, out);
}

static void mod_int256(unsigned char* v1, unsigned char* v2, unsigned char* out)
{
    unsigned char quot[N_BYTES];
    memset(quot, 0, N_BYTES);
    div_int256(v1, v2, quot);
    mul_int256(v2, quot, out);
    sub_int256(v1, out, out);
}

static unsigned long long to_uint64(unsigned char* word)
{    
    unsigned long long val = 0;
    for (int i = N_BYTES-8; i < N_BYTES; i++) {        
        val += word[i]<<((N_BYTES-i-1)*8);
    }
    return val;
}

static unsigned int to_uint(unsigned char* word)
{
    unsigned int uintb = 0;
    uintb += word[N_BYTES-2]<<8;
    uintb += word[N_BYTES-1];
    return uintb;
}

static int uint64_is_overflowed(unsigned char* word)
{
    unsigned long long v1 = 0;
    unsigned long long v2 = 0;
    unsigned long long v3 = 0;
    memcpy(&v1, word, sizeof(unsigned long long));
    memcpy(&v2, word+8, sizeof(unsigned long long));
    memcpy(&v3, word+16, sizeof(unsigned long long));
    return (v1 | v2 | v3) != 0;
}

#endif