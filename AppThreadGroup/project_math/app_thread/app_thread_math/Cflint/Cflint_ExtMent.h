#ifndef CFLINT_EXT_MENT_H
#define CFLINT_EXT_MENT_H

/* 蒙哥马利约减(X,R,N,NP) */
void Cflint_Ment_ModRec(Cflint_Type *Ret, uint32_t Len, int64_t R,
                        Cflint_Type *X, Cflint_Type *N, Cflint_Type *NP);

/* 蒙哥马利模乘(X,Y,N,NP,R_N) */
void Cflint_Ment_ModMult(Cflint_Type *Ret, Cflint_Type *X, Cflint_Type *Y, Cflint_Type *N,
                         Cflint_Type *NP, Cflint_Type *RR_N, int64_t R, uint32_t Len);

#endif
