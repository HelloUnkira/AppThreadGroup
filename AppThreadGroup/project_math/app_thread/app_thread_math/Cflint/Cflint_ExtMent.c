
#include "Cflint.h"

/* 蒙哥马利约减(X,R,N,NP) */
void Cflint_Ment_ModRec(Cflint_Type *Ret, uint32_t Len, int64_t R,
                        Cflint_Type *X, Cflint_Type *N, Cflint_Type *NP)
{
    Cflint_Type *T_1 = Cflint_Alloc(Cflint_Byte * Len * 2);
    Cflint_Type *T_2 = Cflint_Alloc(Cflint_Byte * Len * 2);
    
    /* 1.计算:m = (((X % R) * NP) % R */
    Cflint_Copy(T_2, X, Len * 2);
    Cflint_Mask2(T_2, Len * 2, R);
    Cflint_Mult(T_1, T_2, NP, Len);
    Cflint_Mask2(T_1, Len * 2, R);
    /* 2.计算:y = (X + m * N) / R */
    Cflint_Mult(T_2, T_1, N, Len);
    Cflint_Add(T_1, X, T_2, Len * 2);
    Cflint_ShiftR2(T_1, Len * 2, R);
    /* 3.y > N, y -= N */
    if (Cflint_Cmp(T_1, N, Len) == 1)
        Cflint_Sub(T_1, T_1, N, Len);
    /* 4.拷贝结果即可 */
    Cflint_Copy(Ret, T_1, Len);
    
    Cflint_Free(T_1);
    Cflint_Free(T_2);
}

/* 蒙哥马利模乘(X,Y,N,NP,R_N) */
void Cflint_Ment_ModMult(Cflint_Type *Ret, Cflint_Type *X, Cflint_Type *Y, Cflint_Type *N,
                         Cflint_Type *NP, Cflint_Type *RR_N, int64_t R, uint32_t Len)
{
    Cflint_Type *T_1 = Cflint_Alloc(Cflint_Byte * Len * 2);
    Cflint_Type *T_2 = Cflint_Alloc(Cflint_Byte * Len * 2);
    Cflint_Type *T_3 = Cflint_Alloc(Cflint_Byte * Len * 2);
    Cflint_Type *T_4 = Cflint_Alloc(Cflint_Byte * Len * 2);
    
    /* 1.计算X' = REDC(X * RR_N),Y' = REDC(Y * RR_N) */
    Cflint_Mult(T_3, X, RR_N, Len);
    Cflint_Mult(T_4, Y, RR_N, Len);
    Cflint_Ment_ModRec(T_3, Len, R, T_3, N, NP, T_1, T_2);
    Cflint_Ment_ModRec(T_4, Len, R, T_4, N, NP, T_1, T_2);
    /* 3.计算Z = X’·Y’ */
    Cflint_Mult(T_1, T_3, T_4, Len);
    /* 4.计算Z’=REDC(Z),Z’’=REDC(Z’) */
    Cflint_Ment_ModRec(T_2, Len, R, T_1, N, NP, T_3, T_4);
    Cflint_Ment_ModRec(T_1, Len, R, T_2, N, NP, T_3, T_4);
    /* 5.拷贝结果 */
    Cflint_Copy(Ret, T_1, Len);
    
    Cflint_Free(T_1);
    Cflint_Free(T_2);
    Cflint_Free(T_3);
    Cflint_Free(T_4);
}

