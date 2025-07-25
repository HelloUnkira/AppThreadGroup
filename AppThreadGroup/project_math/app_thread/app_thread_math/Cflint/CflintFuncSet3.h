#ifndef CFLINT_FUNC_SET3_H
#define CFLINT_FUNC_SET3_H

/* 最大公因数为1检查(互素检查):GCD(A,B) == 1 */
bool Cflint_GCDCheck(Cflint_Type *A, Cflint_Type *B, Cflint_Type *Temp[3], uint32_t Len);

/* 最大公因数:GCD(A, B) */
void Cflint_GCD(Cflint_Type *Ret, Cflint_Type *A, Cflint_Type *B,
                Cflint_Type *Temp[3],   uint32_t  Len);

/* 最小公倍数:LCM(A, B) = A * B / GCD(A, B) */
void Cflint_LCM(Cflint_Type *Ret, Cflint_Type *A, Cflint_Type *B,
                Cflint_Type *Temp[5],   uint32_t  Len);

/* 扩展欧几里得算法:A * X + B * Y = GCD(A, B) */
/* Flag为0表示正数,Flag为1表示负数,要求X,Y,Temp[3~6]的长度为(Len+1)*2 */
void Cflint_GCDExtend(Cflint_Type *A,    Cflint_Type *B, Cflint_Type *GCD,
                      Cflint_Type *X,    Cflint_Type *X_Flag,
                      Cflint_Type *Y,    Cflint_Type *Y_Flag,
                      Cflint_Type *Temp[7], uint32_t  Len);

/* 欧几里得乘法逆: Inverse = (1 / A) % N, GCD = GCD(A, N) */
/* 要求Temp(7,8)的长度为Length*2 */
void Cflint_GCDInverse(Cflint_Type *A,       Cflint_Type *N,
                       Cflint_Type *Inverse, Cflint_Type *GCD,    
                       Cflint_Type *Temp[8],    uint32_t  Len);

#endif
