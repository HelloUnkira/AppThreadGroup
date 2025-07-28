
#include "Cflint.h"

/* 通过目标素数对构造公钥(N, E)和私钥(N, D) */
void RSA_Make(Cflint_Type *P, Cflint_Type *Q, Cflint_Type *N,
              Cflint_Type *E, Cflint_Type *D, Cflint_Type *R,
              Cflint_Type *Temp[9], uint32_t Len)
{
    /* 计算公共模数:N = P * Q */
    Cflint_Mult(N, P, Q, Len);
    /* 计算欧拉数:R = (P - 1) * (Q - 1) */
    Cflint_SubBit(P, Len, 1);
    Cflint_SubBit(Q, Len, 1);
    Cflint_Mult(R, P, Q, Len);
    Cflint_AddBit(P, Len, 1);
    Cflint_AddBit(Q, Len, 1);
    /* 寻找一个小于R的整数E满足GCD(E, R) == 1 */
    Cflint_Copy(E, R, Len * 2);
    do {
        Cflint_SubBit(E, Len * 2, 1);
    } while (Cflint_GCDCheck(R, E, Len * 2) == false);
    /* 计算乘法逆:D = (1 / E) % R */
    Cflint_InvGCD(E, N, D, Temp[0], Temp + 1, Len * 2);
}

void RSA_Encrypt(Cflint_Type *N, Cflint_Type *E, Cflint_Type *I, Cflint_Type *O,
                 Cflint_Type *Temp[4], uint32_t Len)
{
    Cflint_ModExp(O, N, I, E, Temp, Len);
}

void RSA_Decrypt(Cflint_Type *N, Cflint_Type *D, Cflint_Type *I, Cflint_Type *O,
                 Cflint_Type *Temp[4], uint32_t Len)
{
    Cflint_ModExp(O, N, I, D, Temp, Len);
}

