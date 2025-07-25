#ifndef CFLINT_FUNC_SET1_H
#define CFLINT_FUNC_SET1_H

/* 交换:Op_0 <=> Op_1 */
void Cflint_Swap(Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len);

/* 拷贝:Op_0 = Op_1 */
void Cflint_Copy(Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len);

/* 比较:((>:1);(==:0);(<:-1)) */
int8_t Cflint_Compare(Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len);

/* 相等判断:((==:1);(!=:0)) */
bool Cflint_Equal(Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len);

/* 为0判断:Op == 0 */
bool Cflint_IsZero(Cflint_Type *Op, uint32_t Len);

/* 设值:类似memset */
void Cflint_SetValue(Cflint_Type *Op, uint32_t Len, Cflint_Type Val);

/* 检查一个数是否为2的幂次方 */
bool Cflint_IsExp2(Cflint_Type *Op, uint32_t Len);

/* 计算2进制最高位索引 */
int64_t Cflint_Nums2(Cflint_Type *Op, uint32_t Len);

/* 位或运算 */
void Cflint_OR(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len);

/* 位与运算 */
void Cflint_AND(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len);

/* 位异或运算 */
void Cflint_XOR(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len);

/* 位反运算 */
void Cflint_NOT(Cflint_Type *Op, uint32_t Len);

/* 位检查 */
bool Cflint_Bit2Check(Cflint_Type *Op, uint32_t Len, int64_t Bits2);

/* 位设置 */
void Cflint_Bit2Set(Cflint_Type *Op, uint32_t Len, int64_t Bits2);

/* 位清除 */
void Cflint_Bit2Reset(Cflint_Type *Op, uint32_t Len, int64_t Bits2);

/* 偶数判断 */
bool Cflint_IsEven(Cflint_Type *Op, uint32_t Len);

/* 左移位运算:Op <<= Bits2(2进制位) */
void Cflint_ShiftL2(Cflint_Type *Op, uint32_t Len, uint64_t Bits2);

/* 右移位运算:Op >>= Bits2(2进制位) */
void Cflint_ShiftR2(Cflint_Type *Op, uint32_t Len, uint64_t Bits2);

/* 左移位运算:Op <<= BitsN(N进制位) */
void Cflint_ShiftLN(Cflint_Type *Op, uint32_t Len, uint32_t BitsN);

/* 右移位运算:Op >>= BitsN(N进制位) */
void Cflint_ShiftRN(Cflint_Type *Op, uint32_t Len, uint32_t BitsN);

/* 数分解运算Op1==(2**Ret)*Op_2 */
int64_t Cflint_Factor2(Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len);

/* 掩码运算:Op &= ((2**K - 1) == ((1 << Bits2) - 1)) */
void Cflint_Mask2(Cflint_Type *Op, uint32_t Len, int64_t Bits2);

/* Bytes转Native格式流 */
void Cflint_Native1ToBytes(uint8_t *Bytes, uint8_t  *Native, uint32_t Len);
void Cflint_BytesToNative1(uint8_t *Bytes, uint8_t  *Native, uint32_t Len);
void Cflint_Native2ToBytes(uint8_t *Bytes, uint16_t *Native, uint32_t Len);
void Cflint_BytesToNative2(uint8_t *Bytes, uint16_t *Native, uint32_t Len);
void Cflint_Native4ToBytes(uint8_t *Bytes, uint32_t *Native, uint32_t Len);
void Cflint_BytesToNative4(uint8_t *Bytes, uint32_t *Native, uint32_t Len);
void Cflint_Native8ToBytes(uint8_t *Bytes, uint64_t *Native, uint32_t Len);
void Cflint_BytesToNative8(uint8_t *Bytes, uint64_t *Native, uint32_t Len);

/* 逆序操作数,操作数与NativeX互转(X==Cflint_Byte) */
void Cflint_OpRev(Cflint_Type *Op, uint32_t Len);

/* 操作数转化为NativeX(X==Type) */
void Cflint_OpToNative(Cflint_Type *Op, uint32_t Len, uint8_t Type);

/* NativeX转化为操作数(X==Type) */
void Cflint_NativeToOp(Cflint_Type *Op, uint32_t Len, uint8_t Type);

/* Native Num数组转为逆Native Hex数组(Bytes) */
/* 可作为十进制数组生成操作数的前置接口 */
void Cflint_NumToHex(uint8_t *Hex, uint8_t *Num, uint8_t *Temp[2], uint32_t Len);

/* 待定 */
void Cflint_HexToNum(uint8_t *Hex, uint8_t *Num, uint8_t *Temp[2], uint32_t Len);

#endif
