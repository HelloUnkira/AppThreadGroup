#ifndef CFLINT_FUNC_SET3_H
#define CFLINT_FUNC_SET3_H

/* 带符号与运算:Ret * (*RetFlag) = Op_1 * (Op1Flag) + Op_2 * (Op2Flag) */
/* 备注:大数运算本身是无符号语义运算,这里是为某些数学运算额外扩展 */
Cflint_Type Cflint_FlagSum(Cflint_Type *Ret,  Cflint_Flag *RetFlag,
                           Cflint_Type *Op_1, Cflint_Flag  Op1Flag,
                           Cflint_Type *Op_2, Cflint_Flag  Op2Flag,
                              uint32_t  Len);

/* 带符号模运算:Ret = Op * (OpFlag) % Mod */
/* 备注:大数运算本身是无符号语义运算,这里是为某些数学运算额外扩展 */
void Cflint_FlagMod(Cflint_Type *Ret, Cflint_Type *Op,
                    Cflint_Type *Mod, Cflint_Flag  OpFlag,
                       uint32_t  Len);

/* 最大公因数为1检查(互素检查):GCD(A,B) == 1 */
bool Cflint_GCDCheck(Cflint_Type *A, Cflint_Type *B, uint32_t Len);

/* 最大公因数:GCD(A, B) */
void Cflint_GCD(Cflint_Type *Ret, Cflint_Type *A, Cflint_Type *B, uint32_t Len);

/* 最小公倍数:LCM(A, B) = A * B / GCD(A, B) */
void Cflint_LCM(Cflint_Type *Ret, Cflint_Type *A, Cflint_Type *B, uint32_t Len);

/* 扩展欧几里得算法:A * X + B * Y = GCD(A, B) */
void Cflint_ExtGCD(Cflint_Type *A, Cflint_Type *B, Cflint_Type *GCD,
                   Cflint_Type *X, Cflint_Flag *X_Flag,
                   Cflint_Type *Y, Cflint_Flag *Y_Flag,
                   uint32_t Len);

/* 欧几里得乘法逆: Inv = (1 / A) % N, GCD = GCD(A, N) */
void Cflint_InvGCD(Cflint_Type *A, Cflint_Type *N, Cflint_Type *Inv, Cflint_Type *GCD, uint32_t Len);

#endif
