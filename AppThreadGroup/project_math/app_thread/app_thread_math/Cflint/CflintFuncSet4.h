#ifndef CFLINT_FUNC_SET4_H
#define CFLINT_FUNC_SET4_H

/* 2次方根整数部运算:[Sqrt(Op)] */
void Cflint_Root2Int(Cflint_Type *RootI, Cflint_Type *Op, uint32_t Len);

/* 2次方数判别检查:Sqrt(Op)**2 == Op */
bool Cflint_Root2Check(Cflint_Type *RootI, Cflint_Type *Op, uint32_t Len);

/* Jacobi符号计算:Jacobi(Op_1/Op_2) */
int8_t Cflint_JacobiFlag(Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len);

/* 二次剩余计算:((Root**2) % Op_2 == Op_1 % Op_2), 模为Op2 > 2的奇素数, */
bool Cflint_ModP1Root2(Cflint_Type *Op_1, Cflint_Type *Op_2, Cflint_Type *Root, uint32_t Len);

/* 扩展二次剩余计算:(Root(K)**2) % Op_2 == Op_1 % (Op_2**K) */
bool Cflint_ModPkRoot2(Cflint_Type *Op_1, Cflint_Type *Op_2, Cflint_Type *Root, int64_t Exp, uint32_t Len);

/* 二次剩余计算:((Root**2) % (Op_2 * Op3) == (Op_1) % (Op_2 * Op3)) */
/* 这里是求最小平方根, 意外返回: 当Root == 0时不满足二次剩余 */
/* 模为Op2, Op3 > 2的奇素数, Op_1, Root为Length * 2 */
bool Cflint_Mod1Root2(Cflint_Type *Op_1, Cflint_Type *Op_2, Cflint_Type *Op3, Cflint_Type *Root, uint32_t Len);

/* 线性同余方程组计算:X == Ai % Mi,当i != j时, GCD(Mi, Mj) == 1 */
int8_t Cflint_LCE(Cflint_Type **Ops, Cflint_Type *X, uint64_t Num, uint32_t Len, uint32_t LenMax);

#endif
