#ifndef _BITS_H_
#define _BITS_H_
#include <stdio.h>
#define F_VAX 0
#define F_IEEE754 1
typedef struct {
    int m_data;
    int m_sign; //1
    int m_expn; //k
    int m_mant; //p
    int m_left; //v
    int m_type; //IEEE754 or F_VAX
} real_t ;

void p_bits(const void* p, size_t n);
void to_bin(double x, int n, int prec);
void p_bits_int(int x);
void p_bits_hex(int x);
void p_bits_oct(int x);
void p_bits_float(float x);
void p_bits_double(double x);
void p_bits_real(real_t r);
void p_arr_int(int*, int n);


int mkbits(const char* bin);

real_t mkreal(int e, int m, int type);
real_t mkrealf(const char* fmt, int type);

real_t r_int(real_t r, int x);
real_t r_add(real_t r1, real_t r2);

real_t r_set_bits(real_t r, int bits);
int r_get_bits(real_t r);
int r_get_exp(real_t r);
int r_get_man(real_t r);
int r_get_sign(real_t r);


double real2f(real_t r);

#endif
