
#include "Cflint.h"

/* 交换:Op_0 <=> Op_1 */
void Cflint_Swap(Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len)
{
    Cflint_Type Temp0 = 0, Temp1 = 0;
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Temp0 = Op_0[Idx]; Temp1 = Op_1[Idx];
        Op_0[Idx] = Temp1; Op_1[Idx] = Temp0;
    }
}

/* 拷贝:Op_0 = Op_1 */
void Cflint_Copy(Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        Op_0[Idx] = Op_1[Idx];
}

/* 比较:((>:1);(==:0);(<:-1)) */
int8_t Cflint_Compare(Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len)
{
    for (int64_t Idx = (int64_t)Len - 1; Idx >= 0; Idx--)
        if (Op_0[Idx] != Op_1[Idx]) {
            if (Op_0[Idx] > Op_1[Idx]) return +1;
            if (Op_0[Idx] < Op_1[Idx]) return -1;
        }
    return 0;
}

/* 相等判断:((==:1);(!=:0)) */
bool Cflint_Equal(Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len)
{
    Cflint_Type Ret = 0;
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        Ret |= Op_0[Idx] ^ Op_1[Idx];
    return Ret == 0 ? true : false;
}

/* 为0判断:Op == 0 */
bool Cflint_IsZero(Cflint_Type *Op, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        if (Op[Idx] != 0)
            return false;
    return true;
}

/* 设值:类似memset */
void Cflint_SetValue(Cflint_Type *Op, uint32_t Len, Cflint_Type Val)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        Op[Idx] = Val;
}

/* 检查一个数是否为2的幂次方 */
bool Cflint_IsExp2(Cflint_Type *Op, uint32_t Len)
{
    uint32_t UnZeroWord = 0;
    /* 1.每一个字都满足2的幂次方;2.只有一个非0字 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        if ((Op[Idx] == 0))
            continue;
        if ((Op[Idx] & (Op[Idx] - 1)) != 0)
            return false;
        if (UnZeroWord++ != 0)
            return false;
    }
    if (UnZeroWord == 1)
        return true;
    return false;
}

/* 计算2进制最高位索引 */
int64_t Cflint_Nums2(Cflint_Type *Op, uint32_t Len)
{
    /* 先计算前导0,这里使用普通的方式,使用折半查找效果会更好 */
    int64_t Idx1 = 0, Idx2 = 0;
    /* 粗粒度计算 */
    for (Idx1 = (int64_t)Len - 1; Idx1 >= 0; Idx1--)
        if (Op[Idx1] != 0)
            break;
    /* 查验,全空时最高位索引为-1 */
    if (Idx1 < 0) return -1;
    /* 不存在全空时,查验落点位 */
    for (Idx2 = (int64_t)Cflint_Bits - 1; Idx2 >= 0; Idx2--)
        if ((Op[Idx1] & (1 << Idx2)) != 0)
            break;
    /* 计算最高位 */
    return (Idx1 * Cflint_Bits + Idx2);
}

/* 位或运算 */
void Cflint_OR(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        Ret[Idx] = Op_1[Idx] | Op_2[Idx];
}

/* 位与运算 */
void Cflint_AND(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        Ret[Idx] = Op_1[Idx] & Op_2[Idx];
}

/* 位异或运算 */
void Cflint_XOR(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        Ret[Idx] = Op_1[Idx] ^ Op_2[Idx];
}

/* 位反运算 */
void Cflint_NOT(Cflint_Type *Op, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        Op[Idx] = ~Op[Idx];
}

/* 位检查 */
bool Cflint_Bit2Check(Cflint_Type *Op, uint32_t Len, int64_t Bits2)
{
    int64_t Bits_N = Bits2 / Cflint_Bits;
    int64_t Bits_2 = Bits2 % Cflint_Bits;
    
    return Bits_N < Len ? ((Op[Bits_N] & (1 << Bits_2)) != 0) : false;
}

/* 位设置 */
void Cflint_Bit2Set(Cflint_Type *Op, uint32_t Len, int64_t Bits2)
{
    int64_t Bits_N = Bits2 / Cflint_Bits;
    int64_t Bits_2 = Bits2 % Cflint_Bits;
    
    if (Bits_N < Len)
        Op[Bits_N] |= (1 << Bits_2);
}

/* 位清除 */
void Cflint_Bit2Reset(Cflint_Type *Op, uint32_t Len, int64_t Bits2)
{
    int64_t Bits_N = Bits2 / Cflint_Bits;
    int64_t Bits_2 = Bits2 % Cflint_Bits;
    
    if (Bits_N < Len)
        Op[Bits_N] &= ~(1 << Bits_2);
}

/* 偶数判断 */
bool Cflint_IsEven(Cflint_Type *Op, uint32_t Len)
{
    return ((Op[0] & 1) == 0);
}

/* 左移位运算 */
void Cflint_ShiftL2(Cflint_Type *Op, uint32_t Len, uint64_t Bits2)
{
    /* 特殊检查:空移位 */
    if (Bits2 == 0)
        return;
    /* 一个DIGIT_TYPE字节下进制的位有多少个2进制位 */
    int64_t Idx  = 0;
    int64_t Bits_N = Bits2 / Cflint_Bits;
    int64_t Bits_2 = Bits2 % Cflint_Bits;
    int64_t Last_2 = Cflint_Bits - Bits_2;
    Cflint_Type Bit_N1 = 0, Bit_High = 0;
    Cflint_Type Bit_N2 = 0, Bit_Low  = 0;
    
    for (Idx = (int64_t)Len - 1; Idx - Bits_N - 1 >= 0; Idx--) {
        Bit_N1    = Op[Idx - Bits_N];
        Bit_N2    = Op[Idx - Bits_N - 1];
        Bit_High  = (Bit_N1 << Bits_2);
        Bit_Low   = (Bit_N2 >> Last_2);
        Bit_Low   = (Last_2 >= Cflint_Bits) ? 0 : Bit_Low;
        Op[Idx] = Bit_High | Bit_Low;
    }
    {
        Bit_N1 = Op[Idx - Bits_N];
        //Bit_N2 = Op[Idx - Bits_N - 1];
        Bit_High  = (Bit_N1 << Bits_2);
        //Bit_Low   = (Bit_N2 >> Last_2);
        //Bit_Low   = (Last_2 >= Cflint_Bits) ? 0 : Bit_Low;
        Op[Idx] = Bit_High;// | (Bit_N2 >> Last_2);
    }
    
    for (Idx = 0; Idx - Bits_N < 0; Idx++)
        Op[Idx] = 0;
}

/* 右移位运算 */
void Cflint_ShiftR2(Cflint_Type *Op, uint32_t Len, uint64_t Bits2)
{
    /* 特殊检查:空移位 */
    if (Bits2 == 0)
        return;
    /* 一个DIGIT_TYPE字节下进制的位有多少个2进制位 */
    int64_t Idx  = 0;
    int64_t Bits_N = Bits2 / Cflint_Bits;
    int64_t Bits_2 = Bits2 % Cflint_Bits;
    int64_t Last_2 = Cflint_Bits - Bits_2;
    Cflint_Type Bit_N1 = 0, Bit_Low  = 0;
    Cflint_Type Bit_N2 = 0, Bit_High = 0;
    
    for (Idx = 0; Idx + Bits_N + 1 < (int64_t)Len; Idx++) {
        Bit_N1    = Op[Idx + Bits_N];
        Bit_N2    = Op[Idx + Bits_N + 1];
        Bit_Low   = (Bit_N1 >> Bits_2);
        Bit_High  = (Bit_N2 << Last_2);
        Bit_High  = (Last_2 >= Cflint_Bits) ? 0 : Bit_High;
        Op[Idx] = Bit_Low | Bit_High;
    }
    {
        Bit_N1 = Op[Idx + Bits_N];
        //Bit_N2 = Op[Idx + Bits_N + 1];
        Bit_Low = (Bit_N1 >> Bits_2);
        //Bit_High = (Bit_N2 << Last_2);
        //Bit_High = (Last_2 >= Cflint_Bits) ? 0 : Bit_High;
        Op[Idx] = Bit_Low;// | (Bit_N2 << Last_2);
    }
    
    for (Idx = (int64_t)Len - Bits_N; Idx < (int64_t)Len; Idx++)
        Op[Idx] = 0;
}
 
/* 左移位运算 */
void Cflint_ShiftLN(Cflint_Type *Op, uint32_t Len, uint32_t BitsN)
{
    /* 超出限制时,直接清空 */
    if (BitsN >= Len) {
        Cflint_SetValue(Op, Len, 0);
        return;
    }
    
    for (int64_t Idx = 0; Idx + BitsN < Len; Idx++)
        Op[Idx] = Op[Idx + BitsN];
    for (int64_t Idx = (int64_t)Len - BitsN; Idx < Len; Idx++)
        Op[Idx] = 0;
}

/* 右移位运算 */
void Cflint_ShiftRN(Cflint_Type *Op, uint32_t Len, uint32_t BitsN)
{
    /* 超出移位限制时,直接清空 */
    if (BitsN >= Len) {
        Cflint_SetValue(Op, Len, 0);
        return;
    }
    
    for (int64_t Idx = (int64_t)Len - 1; Idx - BitsN >= 0; Idx--)
        Op[Idx] = Op[Idx - BitsN];
    for (int64_t Idx = (int64_t)BitsN - 1; Idx >= 0; Idx--)
        Op[Idx] = 0;
}

/* 数分解运算 */
int64_t Cflint_Factor2(Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len)
{
    /* 初始化Op2=Op_1 */
    if (Op_2 != Op_1)
        Cflint_Copy(Op_2, Op_1, Len);
    /* 初始化Bits2=0 */
    int64_t Bits2 = 0;
    /* 特殊检查,Op_1==0 */
    if (Cflint_IsZero(Op_2, Len))
        return 0;
    /* 计算最低非0位所在位置 */
    while (!Cflint_Bit2Check(Op_2, Len, Bits2))
        Bits2++;
    /* 再次检查移位可行性 */
    Cflint_ShiftR2(Op_2, Len, Bits2);
    /* 返回K */
    return Bits2;
}

/* 掩码运算 */
void Cflint_Mask2(Cflint_Type *Op, uint32_t Len, int64_t Bits2)
{
    int64_t Bits_N = Bits2 / Cflint_Bits;
    int64_t Bits_2 = Bits2 % Cflint_Bits;
    Cflint_Type Bit_N = 0;
    /* 简要检查 */
    if (Bits2 > Len *Cflint_Bits || Bits2 == 0)
        return;
    /* 移除高位 */
    for (int64_t Idx = Bits_N + 1; Idx < Len; Idx++)
        Op[Idx] = 0;
    /* 移出位内高位 */
    Bit_N = Op[Bits_N];
    Op[Bits_N] <<= (Cflint_Bits - Bits_2);
    Op[Bits_N] >>= (Cflint_Bits - Bits_2);
}

/* 逆序操作数 */
void Cflint_OpRev(Cflint_Type *Op, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len / 2; Idx++) {
        uint32_t Idx1 = Idx;
        uint32_t Idx2 = Len - 1 - Idx;
        /* 交换俩个数 */
        Op[Idx1] ^= Op[Idx2];
        Op[Idx2] ^= Op[Idx1];
        Op[Idx1] ^= Op[Idx2];
    }
}

void Cflint_Native1ToBytes(uint8_t *Bytes, uint8_t *Native, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        Bytes[Idx] = Native[Len - 1 - Idx];
}

void Cflint_BytesToNative1(uint8_t *Bytes, uint8_t *Native, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        Native[Idx] = Bytes[Len - 1 - Idx];
}

void Cflint_Native2ToBytes(uint8_t *Bytes, uint16_t *Native, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Bytes[Idx * 2 + 0] = (uint8_t)(Native[Len - 1 - Idx] >> 8);
        Bytes[Idx * 2 + 1] = (uint8_t)(Native[Len - 1 - Idx] >> 0);
    }
}

void Cflint_BytesToNative2(uint8_t *Bytes, uint16_t *Native, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Native[Idx] |= (uint16_t)Bytes[(Len - 1 - Idx) * 2 + 0] << 8;
        Native[Idx] |= (uint16_t)Bytes[(Len - 1 - Idx) * 2 + 1] << 0;
    }
}

void Cflint_Native4ToBytes(uint8_t *Bytes, uint32_t *Native, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Bytes[Idx * 4 + 0] = (uint8_t)(Native[Len - 1 - Idx] >> 24);
        Bytes[Idx * 4 + 1] = (uint8_t)(Native[Len - 1 - Idx] >> 16);
        Bytes[Idx * 4 + 2] = (uint8_t)(Native[Len - 1 - Idx] >>  8);
        Bytes[Idx * 4 + 3] = (uint8_t)(Native[Len - 1 - Idx] >>  0);
    }
}

void Cflint_BytesToNative4(uint8_t *Bytes, uint32_t *Native, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Native[Idx] |= (uint32_t)Bytes[(Len - 1 - Idx) * 4 + 0] << 24;
        Native[Idx] |= (uint32_t)Bytes[(Len - 1 - Idx) * 4 + 1] << 16;
        Native[Idx] |= (uint32_t)Bytes[(Len - 1 - Idx) * 4 + 2] <<  8;
        Native[Idx] |= (uint32_t)Bytes[(Len - 1 - Idx) * 4 + 3] <<  0;
    }
}

void Cflint_Native8ToBytes(uint8_t *Bytes, uint64_t *Native, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Bytes[Idx * 8 + 0] = (uint8_t)(Native[Len - 1 - Idx] >> 56);
        Bytes[Idx * 8 + 1] = (uint8_t)(Native[Len - 1 - Idx] >> 48);
        Bytes[Idx * 8 + 2] = (uint8_t)(Native[Len - 1 - Idx] >> 40);
        Bytes[Idx * 8 + 3] = (uint8_t)(Native[Len - 1 - Idx] >> 32);
        Bytes[Idx * 8 + 4] = (uint8_t)(Native[Len - 1 - Idx] >> 24);
        Bytes[Idx * 8 + 5] = (uint8_t)(Native[Len - 1 - Idx] >> 16);
        Bytes[Idx * 8 + 6] = (uint8_t)(Native[Len - 1 - Idx] >>  8);
        Bytes[Idx * 8 + 7] = (uint8_t)(Native[Len - 1 - Idx] >>  0);
    }
}

void Cflint_BytesToNative8(uint8_t *Bytes, uint64_t *Native, uint32_t Len)
{
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Native[Idx] |= (uint64_t)Bytes[(Len - 1 - Idx) * 8 + 0] << 56;
        Native[Idx] |= (uint64_t)Bytes[(Len - 1 - Idx) * 8 + 1] << 48;
        Native[Idx] |= (uint64_t)Bytes[(Len - 1 - Idx) * 8 + 2] << 40;
        Native[Idx] |= (uint64_t)Bytes[(Len - 1 - Idx) * 8 + 3] << 32;
        Native[Idx] |= (uint64_t)Bytes[(Len - 1 - Idx) * 8 + 4] << 24;
        Native[Idx] |= (uint64_t)Bytes[(Len - 1 - Idx) * 8 + 5] << 16;
        Native[Idx] |= (uint64_t)Bytes[(Len - 1 - Idx) * 8 + 6] <<  8;
        Native[Idx] |= (uint64_t)Bytes[(Len - 1 - Idx) * 8 + 7] <<  0;
    }
}

/* 操作数转化为NativeX(X==Type) */
void Cflint_OpToNative(Cflint_Type *Op, uint32_t Len, uint8_t Type)
{
    if (Type != 1 && Type != 2 && Type != 4 && Type != 8)
        return;
    /* 跳过无必要的转换 */
    if (Cflint_Byte == Type) {
        Cflint_Type *Native = Op;
        Cflint_OpRev(Op, Len);
        return;
    }
    /* 开始转换流程 */
    Cflint_Type *Native = Op;
    uint32_t  Length1 = Len * Cflint_Byte / 1;
    uint32_t  Length2 = Len * Cflint_Byte / 2;
    uint32_t  Length4 = Len * Cflint_Byte / 4;
    uint32_t  Length8 = Len * Cflint_Byte / 8;
     uint8_t *Native1 = ( uint8_t *)Native;
    uint16_t *Native2 = (uint16_t *)Native;
    uint32_t *Native4 = (uint32_t *)Native;
    uint64_t *Native8 = (uint64_t *)Native;
    /* 操作数转Native */
    Cflint_OpRev(Op, Len);
    /* 将Native转化为Native1 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
#if 0
#elif Cflint_Byte == 1
        continue;
         uint8_t Val = Native[Idx];
        Native1[Idx * 1 + 0] = Val >>  0;
#elif Cflint_Byte == 2
        uint16_t Val = Native[Idx];
        Native1[Idx * 2 + 0] = Val >>  8;
        Native1[Idx * 2 + 1] = Val >>  0;
#elif Cflint_Byte == 4
        uint32_t Val = Native[Idx];
        Native1[Idx * 4 + 0] = Val >> 24;
        Native1[Idx * 4 + 1] = Val >> 16;
        Native1[Idx * 4 + 2] = Val >>  8;
        Native1[Idx * 4 + 3] = Val >>  0;
#elif Cflint_Byte == 8
        uint64_t Val = Native[Idx];
        Native1[Idx * 8 + 0] = Val >> 56;
        Native1[Idx * 8 + 1] = Val >> 48;
        Native1[Idx * 8 + 2] = Val >> 40;
        Native1[Idx * 8 + 3] = Val >> 32;
        Native1[Idx * 8 + 4] = Val >> 24;
        Native1[Idx * 8 + 5] = Val >> 16;
        Native1[Idx * 8 + 6] = Val >>  8;
        Native1[Idx * 8 + 7] = Val >>  0;
#else
#endif
    }
    /* 将Native1转化为目标Native */
    switch(Type) {
    case 1:
        break;
        for (uint32_t Idx = 0; Idx < Length1; Idx++) {
             uint8_t Val = 0;
            Val |= Native1[Idx * 1 + 0];
            Native1[Idx] = Val;
        }
        break;
    case 2:
        for (uint32_t Idx = 0; Idx < Length2; Idx++) {
            uint16_t Val = 0;
            Val |= Native1[Idx * 2 + 0]; Val <<= 8;
            Val |= Native1[Idx * 2 + 1];
            Native2[Idx] = Val;
        }
        break;
    case 4:
        for (uint32_t Idx = 0; Idx < Length4; Idx++) {
            uint32_t Val = 0;
            Val |= Native1[Idx * 4 + 0]; Val <<= 8;
            Val |= Native1[Idx * 4 + 1]; Val <<= 8;
            Val |= Native1[Idx * 4 + 2]; Val <<= 8;
            Val |= Native1[Idx * 4 + 3];
            Native4[Idx] = Val;
        }
        break;
    case 8:
        for (uint32_t Idx = 0; Idx < Length8; Idx++) {
            uint64_t Val = 0;
            Val |= Native1[Idx * 8 + 0]; Val <<= 8;
            Val |= Native1[Idx * 8 + 1]; Val <<= 8;
            Val |= Native1[Idx * 8 + 2]; Val <<= 8;
            Val |= Native1[Idx * 8 + 3]; Val <<= 8;
            Val |= Native1[Idx * 8 + 4]; Val <<= 8;
            Val |= Native1[Idx * 8 + 5]; Val <<= 8;
            Val |= Native1[Idx * 8 + 6]; Val <<= 8;
            Val |= Native1[Idx * 8 + 7];
            Native8[Idx] = Val;
        }
        break;
    }
    /*  */
}

/* NativeX转化为操作数(X==Type) */
void Cflint_NativeToOp(Cflint_Type *Op, uint32_t Len, uint8_t Type)
{
    if (Type != 1 && Type != 2 && Type != 4 && Type != 8)
        return;
    /* 跳过无必要的转换 */
    if (Cflint_Byte == Type) {
        Cflint_Type *Native = Op;
        Cflint_OpRev(Op, Len);
        return;
    }
    /* 开始转换流程 */
    Cflint_Type *Native = Op;
    uint32_t  Length1 = Len * Cflint_Byte / 1;
    uint32_t  Length2 = Len * Cflint_Byte / 2;
    uint32_t  Length4 = Len * Cflint_Byte / 4;
    uint32_t  Length8 = Len * Cflint_Byte / 8;
     uint8_t *Native1 = ( uint8_t *)Native;
    uint16_t *Native2 = (uint16_t *)Native;
    uint32_t *Native4 = (uint32_t *)Native;
    uint64_t *Native8 = (uint64_t *)Native;
    /* 将目标Native转化为Native1 */
    switch(Type) {
    case 1:
        break;
        for (uint32_t Idx = 0; Idx < Length1; Idx++) {
             uint8_t Val = Native1[Idx];
            Native1[Idx * 1 + 0] = Val >>  0;
        }
        break;
    case 2:
        for (uint32_t Idx = 0; Idx < Length2; Idx++) {
            uint16_t Val = Native2[Idx];
            Native1[Idx * 2 + 0] = Val >>  8;
            Native1[Idx * 2 + 1] = Val >>  0;
        }
        break;
    case 4:
        for (uint32_t Idx = 0; Idx < Length4; Idx++) {
            uint32_t Val = Native4[Idx];
            Native1[Idx * 4 + 0] = Val >> 24;
            Native1[Idx * 4 + 1] = Val >> 16;
            Native1[Idx * 4 + 2] = Val >>  8;
            Native1[Idx * 4 + 3] = Val >>  0;
        }
        break;
    case 8:
        for (uint32_t Idx = 0; Idx < Length8; Idx++) {
            uint64_t Val = Native8[Idx];
            Native1[Idx * 8 + 0] = Val >> 56;
            Native1[Idx * 8 + 1] = Val >> 48;
            Native1[Idx * 8 + 2] = Val >> 40;
            Native1[Idx * 8 + 3] = Val >> 32;
            Native1[Idx * 8 + 4] = Val >> 24;
            Native1[Idx * 8 + 5] = Val >> 16;
            Native1[Idx * 8 + 6] = Val >>  8;
            Native1[Idx * 8 + 7] = Val >>  0;
        }
        break;
    }
    /* 将Native1转化为Native */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
#if 0
#elif Cflint_Byte == 1
        continue;
         uint8_t Val = 0;
        Val |= Native1[Idx * 1 + 0];
        Native[Idx] = Val;
#elif Cflint_Byte == 2
        uint16_t Val = 0;
        Val |= Native1[Idx * 2 + 0]; Val <<= 8;
        Val |= Native1[Idx * 2 + 1];
        Native[Idx] = Val;
#elif Cflint_Byte == 4
        uint32_t Val = 0;
        Val |= Native1[Idx * 4 + 0]; Val <<= 8;
        Val |= Native1[Idx * 4 + 1]; Val <<= 8;
        Val |= Native1[Idx * 4 + 2]; Val <<= 8;
        Val |= Native1[Idx * 4 + 3];
        Native[Idx] = Val;
#elif Cflint_Byte == 8
        uint64_t Val = 0;
        Val |= Native1[Idx * 8 + 0]; Val <<= 8;
        Val |= Native1[Idx * 8 + 1]; Val <<= 8;
        Val |= Native1[Idx * 8 + 2]; Val <<= 8;
        Val |= Native1[Idx * 8 + 3]; Val <<= 8;
        Val |= Native1[Idx * 8 + 4]; Val <<= 8;
        Val |= Native1[Idx * 8 + 5]; Val <<= 8;
        Val |= Native1[Idx * 8 + 6]; Val <<= 8;
        Val |= Native1[Idx * 8 + 7];
        Native[Idx] = Val;
#else
#endif
    }
    /* Native转操作数 */
    Cflint_OpRev(Op, Len);
}

void Cflint_NumToHex(uint8_t *Hex, uint8_t *Num, uint8_t *Temp[2], uint32_t Len)
{
    uint32_t NumLen1 = 0;
    uint32_t NumLen2 = 0;
    uint8_t *Swap = NULL;
    uint8_t *Buffer1 = Temp[0]; //被除数
    uint8_t *Buffer2 = Temp[1]; //商
    /* 清零缓冲区数据,初始化Buffer1,Buffer2 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Hex[Idx] = 0;
        Buffer1[Idx] = 0;
        Buffer2[Idx] = 0;
    }
    /* 拷贝十进制数据到Buffer1 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        /* 这里只能接受纯数字字符串 */
        if (Num[Idx] == 0)
            break;
        if (Num[Idx] <= '0' && Num[Idx] >= '9')
            return;
        Buffer1[Idx] = Num[Idx] - '0';
        NumLen1 = Idx + 1;
    }
    /* 进行迭代循环 Module = Buffer1 % 255 */
    uint32_t Index0 = 0;
    while (1) {
        /* 前缀0滤除锁 */
        uint32_t PrefixLock = 0;
        /* 进行迭代循环 */
        uint32_t Idx1 = 0;
        uint16_t Module = 0;
        /* Buffer2 = Buffer1 / 255 */
        /* Module  = Buffer1 % 255 */
        do {
            /* 数据不够移动到下一个位 */
            if (Idx1 < NumLen1)
                Module  = Module * 10 + Buffer1[Idx1++];
            /* 数据不够除时,商补0 */
            /* 数据足够除时,计算商 */
            if (Module >> 8 != 0)
                PrefixLock  = 1;
            /* 前缀0滤除锁(这是必备的,它会限制商以期靠近0) */
            if (PrefixLock != 0)
                Buffer2[NumLen2++] = Module >> 8;
            Module &= 0xFF;
        } while (Idx1 < NumLen1);
        /* 保存当次最后除余 */
        Hex[Index0++] = Module;
        /* 辗转迭代直到商为0 */
        if (NumLen2 == 0)
            break;
        /* 商更新成被除数(交换指针即可) */
        Swap    = Buffer1;
        Buffer1 = Buffer2;
        Buffer2 = Swap;
        NumLen1 = NumLen2;
        NumLen2 = 0;
        /* 开始下一次迭代循环 */
    }
}

void Cflint_HexToNum(uint8_t *Hex, uint8_t *Num, uint8_t *Temp[3], uint32_t Len)
{
    uint32_t NumLen1 = 0;
    uint32_t NumLen2 = 0;
    uint8_t *Swap = NULL;
    uint8_t *Buffer1 = Temp[0]; //被除数
    uint8_t *Buffer2 = Temp[1]; //商
    uint8_t *Buffer3 = Temp[2]; //累增除数
    /* 清零缓冲区数据,初始化Buffer1,Buffer2 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Num[Idx] = 0;
        Buffer1[Idx] = 0;
        Buffer2[Idx] = 0;
        Buffer3[Idx] = 0;
    }
    /* 拷贝十进制数据到Buffer1 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Buffer1[Idx] = Hex[Idx];
        if (Hex[Idx] != 0)
            NumLen1 = Idx + 1;
    }
    /* 进行迭代循环 Module = Buffer1 % 10 */
    uint32_t Index0 = 0;
    while (1) {
        /* 进行迭代循环 */
        uint32_t Idx1 = 0;
        uint16_t Module = 0;
        /* 退出条件 */
        if (NumLen1 == 0)
            break;
        /* 注意:这里只能使用原始的除转减(计算机不支持跨进制除法) */
        /* 生成一个累赠除数,该除数恰好小于被除数偏移1位 */
        for (uint32_t Idx = 0; Idx < Len; Idx++)
            Buffer3[Idx] = 0;
        Buffer3[0] = 10;
        /* 这个累赠除数数学表示为10 * 2 ** k */
        int64_t Offset = 0;
        while (1) {
            /* 比较Buffer1和Buffer3(必须大于才可以是true) */
            int8_t Compare = 0;
            for (int64_t Idx = NumLen1 - 1; Idx >= 0; Idx--) {
                if (Buffer1[Idx] > Buffer3[Idx]) {
                    Compare = +1;
                    break;
                }
                if (Buffer1[Idx] < Buffer3[Idx]) {
                    Compare = -1;
                    break;
                }
            }
            if (Compare == 0)
                break;
            /* 如果Buffer1 > Buffer3则Buffer3左移一位(*2) */
            if (Compare == +1) {
                /* 如果Buffer3移位达到限制,再移位会产生截断时,强行中止 */
                if (Offset >= (NumLen1 - 1) * 8 + 4)
                    break;
                uint8_t Ow = 0;
                for (uint32_t Idx = 0; Idx < NumLen1; Idx++) {
                    uint8_t Val  = Buffer3[Idx];
                    Buffer3[Idx] = ((Val & 0x7F) << 1) | Ow;
                    Ow = (Val & 0x80) >> 7;
                }
                Offset++;
            }
            /* 如果Buffer1 < Buffer3则Buffer3右移一位(/2) */
            if (Compare == -1) {
                uint8_t Ow = 0;
                for (int64_t Idx = NumLen1 - 1; Idx >= 0; Idx--) {
                    uint8_t Val  = Buffer3[Idx];
                    Buffer3[Idx] = ((Val & 0xFE) >> 1) | Ow;
                    Ow = (Val & 0x01) << 7;
                }
                Offset--;
                break;
            }
        }
        /* 如果第一次小于比较都不通过(踩空了) */
        if (Offset == -1) {
            Num[Index0++] = Buffer1[0];
            /* Buffer1 = Buffer2 */
            for (uint32_t Idx = 0; Idx < Len; Idx++)
                Buffer1[Idx] = Buffer2[Idx];
            NumLen1 = NumLen2;
            /* Buffer2 = 0 */
            for (uint32_t Idx = 0; Idx < Len; Idx++)
                Buffer2[Idx] = 0;
            NumLen2 = 0;
            continue;
        }
        /* 现在的Buffer3 <= Buffer1了 */
        /* Buffer1 -= Buffer3 */
        /* Buffer2 += Buffer3 / 10 */
        {
            uint8_t Ow = 0;
            /* Buffer1 -= Buffer3 */
            for (uint32_t Idx = 0; Idx < NumLen1; Idx++) {
                /* 计算差结果 */
                uint8_t Val = Buffer1[Idx] - Buffer3[Idx] - Ow;
                /* 检查借位溢出 */
                if (Val > Buffer1[Idx])
                    Ow = 1;
                else
                    Ow = 0;
                /* 保存运算结果 */
                Buffer1[Idx] = Val;
            }
            /* 关键: */
            /* Buffer3 == 10  * 2 ** K(K == Offset) */
            /* Buffer3 /= 10 == 2 ** K(K == Offset) */
            for (uint32_t Idx = 0; Idx < Len; Idx++)
                Buffer3[Idx] = 0;
            Buffer3[Offset / 8] = 1 << (Offset % 8);
            Ow = 0;
            /* Buffer2 += Buffer3 */
            for (uint32_t Idx = 0; Idx < Len; Idx++) {
                /* 计算和结果 */
                uint8_t Val = Buffer2[Idx] + Buffer3[Idx] + Ow;
                /* 检查进位溢出 */
                if (Val < Buffer2[Idx] || Val < Buffer3[Idx])
                    Ow = 1;
                if (Val > Buffer2[Idx] && Val > Buffer3[Idx])
                    Ow = 0;
                /* 保存运算结果 */
                Buffer2[Idx] = Val;
                if (Buffer2[Idx] != 0)
                    NumLen2 = Idx + 1;
            }
        }
    }
    /* 逆序Num成Native模式 */
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        if (Num[Idx] != 0)
            NumLen1 = Idx + 1;
    for (uint32_t Idx = 0; Idx < NumLen1; Idx++)
        Num[Idx] += '0';
    
    for (uint32_t Idx = 0; Idx < NumLen1 / 2; Idx++) {
        uint32_t Idx1 = Idx;
        uint32_t Idx2 = NumLen1 - 1 - Idx;
        /* 交换俩个数 */
        Num[Idx1] ^= Num[Idx2];
        Num[Idx2] ^= Num[Idx1];
        Num[Idx1] ^= Num[Idx2];
    }
}

