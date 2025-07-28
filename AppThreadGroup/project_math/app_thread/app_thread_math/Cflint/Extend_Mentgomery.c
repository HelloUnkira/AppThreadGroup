
#include "Cflint.h"

/* 蒙哥马利约减(X,R,N,NP) */
/* X,Temp1,Temp2长度为Length*2 */
void Mentgomery_ModuloReduction(Cflint_Type *Ret, uint32_t Len, int64_t R,
                                Cflint_Type *X, Cflint_Type *N, Cflint_Type *NP,
                                Cflint_Type *Temp1, Cflint_Type *Temp2)
{
    /* 1.计算:m = (((X % R) * NP) % R */
    Cflint_Copy(Temp2, X, Len * 2);
    Cflint_Mask2(Temp2, Len * 2, R);
    Cflint_Mult(Temp1, Temp2, NP, Len);
    Cflint_Mask2(Temp1, Len * 2, R);
    /* 2.计算:y = (X + m * N) / R */
    Cflint_Mult(Temp2, Temp1, N, Len);
    Cflint_Add(Temp1, X, Temp2, Len * 2);
    Cflint_ShiftR2(Temp1, Len * 2, R);
    /* 3.y > N, y -= N */
    if (Cflint_Cmp(Temp1, N, Len) == 1)
        Cflint_Sub(Temp1, Temp1, N, Len);
    /* 4.拷贝结果即可 */
    Cflint_Copy(Ret, Temp1, Len);
}

/* 蒙哥马利模乘(X,Y,N,NP,R_N) */
/* Temp1,Temp2,Temp3,Temp4长度为Length*2 */
void Mentgomery_ModuloMultiply(Cflint_Type *Ret, Cflint_Type *X, Cflint_Type *Y,
                               Cflint_Type *N, Cflint_Type *NP, Cflint_Type *RR_N,
                               Cflint_Type *Temp1, Cflint_Type *Temp2,
                               Cflint_Type *Temp3, Cflint_Type *Temp4,
                               int64_t R, uint32_t Len)
{
    /* 1.计算X' = REDC(X * RR_N),Y' = REDC(Y * RR_N) */
    Cflint_Mult(Temp3, X, RR_N, Len);
    Cflint_Mult(Temp4, Y, RR_N, Len);
    Mentgomery_ModuloReduction(Temp3, Len, R, Temp3, N, NP, Temp1, Temp2);
    Mentgomery_ModuloReduction(Temp4, Len, R, Temp4, N, NP, Temp1, Temp2);
    /* 3.计算Z = X’·Y’ */
    Cflint_Mult(Temp1, Temp3, Temp4, Len);
    /* 4.计算Z’=REDC(Z),Z’’=REDC(Z’) */
    Mentgomery_ModuloReduction(Temp2, Len, R, Temp1, N, NP, Temp3, Temp4);
    Mentgomery_ModuloReduction(Temp1, Len, R, Temp2, N, NP, Temp3, Temp4);
    /* 5.拷贝结果 */
    Cflint_Copy(Ret, Temp1, Len);
}

