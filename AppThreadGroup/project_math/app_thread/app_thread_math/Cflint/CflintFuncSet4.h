#ifndef CFLINT_FUNC_SET4_H
#define CFLINT_FUNC_SET4_H

/* 2次方根整数部运算:[Sqrt(Op)] */
void Cflint_Root2Integer(Cflint_Type *Ret, Cflint_Type *Op,
                         Cflint_Type *Temp[4],   uint32_t  Len);

/* 2次方数判别检查:Sqrt(Op)**2 == Op */
/* 要求Temp[0]的长度为Length*2 */
bool Cflint_Root2Check(Cflint_Type *Ret, Cflint_Type *Op,
                       Cflint_Type *Temp[4],   uint32_t  Len);

/* Jacobi符号计算:Jacobi(Op_1/Op_2) */
int8_t Cflint_JacobiFlag(Cflint_Type *Op_1, Cflint_Type *Op_2,
                         Cflint_Type *Temp[3],     uint32_t  Len);

/* 二次剩余计算:((Ret**2) % Op_2 == Op_1 % Op_2) */
/* 模为Operand2 > 2的奇素数, 要求Temp[6~9]为Length * 2 */
bool Cflint_ModuloP1Root2(Cflint_Type *Op_1,  Cflint_Type *Op_2,
                          Cflint_Type *Ret,    Cflint_Type *Temp[10],
                             uint32_t  Len);

/* 扩展二次剩余计算:(X(K)**2) % P == A % (P**K) */
bool Cflint_ModuloPkRoot2(Cflint_Type *Op_1,  Cflint_Type *Op_2,
                          Cflint_Type *Ret,    Cflint_Type *Temp[10],
                              int64_t  Exponent,     uint32_t  Len);

/* 二次剩余计算:((Ret**2) % (Op_2 * Operand3) ==  */
/*               (Op_1)  % (Op_2 * Operand3))    */
/* 这里是求最小平方根,意外返回:当Result == 0时不满足二次剩余 */
/* 模为Operand2,Operand3 > 2的奇素数, Op_1, Result为Length * 2 */
/* 要求Temp[3~10] 为(Len+1)*2 */
/* 要求Temp[11~12]为Length * 4 */
bool Cflint_Modulo1Root2(Cflint_Type *Op_1,  Cflint_Type *Op_2,
                         Cflint_Type *Operand3,  Cflint_Type *Ret,
                         Cflint_Type *Temp[13],     uint32_t  Len);

/* 线性同余方程组计算:X == Ai % Mi,当i != j时, GCD(Mi, Mj) == 1 */
/* Ret,Temps[0~5]为LengthMax,返回值-2表示LengthMax过小 */
/* 要求Temp[3~4]为(Len+1)*2 */
/* 要求Temp[5~9]为(Len+1)*4 */
int8_t Cflint_LCE(Cflint_Type **Operands, Cflint_Type *Ret,   uint64_t Number,
                  Cflint_Type  *Temps[5], Cflint_Type *Temp[10], uint32_t Len,
                     uint32_t   LengthMax);

#endif
