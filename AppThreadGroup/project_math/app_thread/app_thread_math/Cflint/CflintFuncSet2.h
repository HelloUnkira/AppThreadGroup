#ifndef CFLINT_FUNC_SET2_H
#define CFLINT_FUNC_SET2_H

/* 和运算:Op += Ow */
Cflint_Type Cflint_AddBit(Cflint_Type *Op, uint32_t Len, Cflint_Type Ow);

/* 差运算:Op -= Ow */
Cflint_Type Cflint_SubBit(Cflint_Type *Op, uint32_t Len, Cflint_Type Ow);

/* 翻转数:模对称翻转数 */
Cflint_Type Cflint_Rev(Cflint_Type *Op, uint32_t Len);

/* 和运算:Ret = Op_1 + Op_2 */
Cflint_Type Cflint_Add(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len);

/* 差运算:Ret = Op_1 - Op_2 */
Cflint_Type Cflint_Sub(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len);

/* 乘运算:Ret = Op_1 * Op_2; 结果的长度实际是Length * 2 */
void Cflint_Mult(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len);

/* 方运算:Ret = Op * Op; 结果的长度实际是Length * 2 */
void Cflint_Square(Cflint_Type *Ret, Cflint_Type *Op, uint32_t Len);

/* 带余除运算:Quo = Op_0 / Op_1; Mod = Op_0 % Op_1 */
void Cflint_Devide(Cflint_Type *Quo, Cflint_Type *Mod, Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len);

/* 基础模运算:Mod = Op_0 % Op_1 */
void Cflint_Mod(Cflint_Type *Mod, Cflint_Type *Op_0, Cflint_Type *Op_1,  uint32_t  Len);

/* 模和运算:Ret = (Op_0 + Op_1) % Mod */
void Cflint_ModAdd(Cflint_Type *Ret, Cflint_Type *Mod, Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len);

/* 模差运算:Ret = (Op_0 - Op_1) % Mod */
void Cflint_ModSub(Cflint_Type *Ret, Cflint_Type *Mod, Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len);

/* 模乘运算:Ret = (Op_0 * Op_1) % Mod */
void Cflint_ModMult(Cflint_Type *Ret, Cflint_Type *Mod, Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len);

/* 模方运算:Ret = (Op * Op) % Mod */
void Cflint_ModSquare(Cflint_Type *Ret, Cflint_Type *Mod, Cflint_Type *Op, uint32_t Len);

/* 模幂运算:Ret = (Op**Exp) % Mod */
void Cflint_ModExp(Cflint_Type *Ret, Cflint_Type *Mod, Cflint_Type *Op, Cflint_Type *Exp, uint32_t Len);

/* 模逆运算: Ret = (1 / Op) % Mod */
/* 要求:Op < Module且互素 */
void Cflint_ModInv(Cflint_Type *Ret, Cflint_Type *Op, Cflint_Type *Mod, uint32_t Len);

#endif
