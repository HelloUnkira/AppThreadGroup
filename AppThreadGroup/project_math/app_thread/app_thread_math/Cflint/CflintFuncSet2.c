
#include "Cflint.h"

/*备注:和,差,乘,方,逆运算实现来源于uECC.c.h */

/* 和运算:数加一个位 */
Cflint_Type Cflint_AddBit(Cflint_Type *Op, uint32_t Len, Cflint_Type Ow)
{
    /* 计算数累加一个位 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Cflint_Type Temp = Op[Idx] + Ow;
        /* 检查进位溢出 */
        if (Temp < Op[Idx])
            Ow = 1;
        if (Temp > Op[Idx])
            Ow = 0;
        /* 保存运算结果 */
        Op[Idx] = Temp;
        /* 不存在进位溢出时即可退出 */
        if (Ow == 0)
            return 0;
    }
    return 1;
}

/* 差运算:数减一个位 */
Cflint_Type Cflint_SubBit(Cflint_Type *Op, uint32_t Len, Cflint_Type Ow)
{
    /* 计算数累减一个位 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Cflint_Type Temp = Op[Idx] - Ow;
        /* 检查借位溢出 */
        if (Temp > Op[Idx])
            Ow = 1;
        if (Temp < Op[Idx])
            Ow = 0;
        /* 保存运算结果 */
        Op[Idx] = Temp;
        /* 不存在借位溢出时即可退出 */
        if (Ow == 0)
            return 0;
    }
    return 1;
}

/* 翻转数:模对称指定位翻转数 */
Cflint_Type Cflint_Rev(Cflint_Type *Op, uint32_t Len)
{
    Cflint_Type Ow = 0;
    /* 计算数累减一个模 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Cflint_Type Temp = 0 - Op[Idx] - Ow;
        /* 检查借位溢出 */
        if (Temp > Op[Idx])
            Ow = 1;
        if (Temp < Op[Idx])
            Ow = 0;
        /* 保存运算结果 */
        Op[Idx] = Temp;
    }
    return 1;
}

/* 和运算 */
Cflint_Type Cflint_Add(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len)
{
    Cflint_Type Ow = 0;
    /* 计算俩数和 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        /* 计算和结果,暂存到临时区 */
        Cflint_Type Temp = Op_1[Idx] + Op_2[Idx] + Ow;
        /* 检查进位溢出 */
        if (Temp < Op_1[Idx] || Temp < Op_2[Idx])
            Ow = 1;
        if (Temp > Op_1[Idx] && Temp > Op_2[Idx])
            Ow = 0;
        /* 保存运算结果 */
        Ret[Idx] = Temp;
    }
    return Ow;
}

/* 差运算 */
Cflint_Type Cflint_Sub(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len)
{
    Cflint_Type Ow = 0;
    /* 计算俩数差 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        /* 计算差结果,暂存到临时区 */
        Cflint_Type Temp = Op_1[Idx] - Op_2[Idx] - Ow;
        /* 检查借位溢出 */
        if (Temp > Op_1[Idx])
            Ow = 1;
        else
            Ow = 0;
        /* 保存运算结果 */
        Ret[Idx] = Temp;
    }
    return Ow;
}

/* 乘运算(计算位乘法器) */
static void Cflint_Multiply1(Cflint_Type  Data1,  Cflint_Type  Data2,
                             Cflint_Type *Carry1, Cflint_Type *Carry2,
                             Cflint_Type *Ow)
{
#ifdef Cflint_Type2
//利用关键字优化策略
    Cflint_Type2 Result_XY = 0;
    Cflint_Type2 Result_01 = 0;
    //1.计算乘数结果
    Result_XY |= ((Cflint_Type2)Data1);
    Result_XY *= ((Cflint_Type2)Data2);
    //2.计算上一位的进位
    Result_01 |= ((Cflint_Type2)(*Carry1));
    Result_01 |= ((Cflint_Type2)(*Carry2) << Cflint_Bits);
    //3.计算最终结果
    Result_01 += Result_XY;
    //4.本次计算是否高位产生溢出
    if (Result_01 <  Result_XY) (*Ow) = 1;
    if (Result_01 >= Result_XY) (*Ow) = 0;
    //5.保存本次计算的低位和高位
    (*Carry1) = (Cflint_Type)(Result_01 >> 0);
    (*Carry2) = (Cflint_Type)(Result_01 >> Cflint_Bits);
#else
//不利用关键字优化策略
    Cflint_Type Result_XY0 = 0;
    Cflint_Type Result_XY1 = 0;
    //1.获得乘数的高位与低位
    Cflint_Type X0 = Data1 & CFLINT_ByteLH;
    Cflint_Type X1 = Data1 >> (Cflint_Bits / 2);
    Cflint_Type Y0 = Data2 & CFLINT_ByteLH;
    Cflint_Type Y1 = Data2 >> (Cflint_Bits / 2);
    //2.分别计算高位与低位的交乘
    Cflint_Type I0 = X0 * Y0;
    Cflint_Type I1 = X0 * Y1;
    Cflint_Type I2 = X1 * Y0;
    Cflint_Type I3 = X1 * Y1;
    //3.计算溢出
    Cflint_Type Overflow0 = I0 >> (Cflint_Bits / 2); //低位的进位
    Cflint_Type Result_0  = I1 + I2 + Overflow0;        //低位的值
    Cflint_Type Result_1  = I3;                         //高位的值
    if (Result_0 < I1)                                   //低位溢出
        Result_1 += ((Cflint_Type)1 << (Cflint_Bits / 2));
    //4.计算终点值
    Result_XY0 |= (I0 & CFLINT_ByteLH);
    Result_XY0 |= (Result_0 << (Cflint_Bits / 2));
    Result_XY1 |= (Result_1);
    Result_XY1 += (Result_0 >> (Cflint_Bits / 2));
    //5.保存本次计算的低位和高位
    (*Carry1) += Result_XY0;
    (*Carry2) += Result_XY1;
    //6.本次计算是否低位产生溢出
    if ((*Carry1) < Result_XY0)
        (*Carry2) += 1;
    //7.本次计算是否高位产生溢出
    if (((*Carry2)  < Result_XY1) ||
        ((*Carry2) == Result_XY1 && (*Carry1) < Result_XY0))
         (*Ow) += 1;
#endif
}

/* 方运算(计算位乘方器) */
static void Cflint_Square1(Cflint_Type  Data1,  Cflint_Type  Data2,
                           Cflint_Type *Carry1, Cflint_Type *Carry2,
                           Cflint_Type *Ow)
{
#ifdef Cflint_Type2
//利用关键字优化策略
    Cflint_Type2 Result_XY = 0;
    Cflint_Type2 Result_01 = 0;
    //1.计算乘数结果
    Result_XY |= ((Cflint_Type2)Data1);
    Result_XY *= ((Cflint_Type2)Data2);
    //2.计算上一位的进位
    Result_01 |= ((Cflint_Type2)(*Carry1));
    Result_01 |= ((Cflint_Type2)(*Carry2) << Cflint_Bits);
    //3.计算进位
    (*Ow) += (Result_XY >> (Cflint_Bits * 2 - 1));
    //4.计算最终结果
    Result_XY *= 2;
    Result_01 += Result_XY;
    //4.本次计算是否高位产生溢出
    if (Result_01 <  Result_XY) (*Ow) += 1;
    if (Result_01 >= Result_XY) (*Ow) += 0;
    //5.保存本次计算的低位和高位
    (*Carry1) = (Cflint_Type)(Result_01 >> 0);
    (*Carry2) = (Cflint_Type)(Result_01 >> Cflint_Bits);
#else
//不利用关键字优化策略
    Cflint_Type Result_XY0 = 0;
    Cflint_Type Result_XY1 = 0;
    //1.获得乘数的高位与低位
    Cflint_Type X0 = Data1 & CFLINT_ByteLH;
    Cflint_Type X1 = Data1 >> (Cflint_Bits / 2);
    Cflint_Type Y0 = Data2 & CFLINT_ByteLH;
    Cflint_Type Y1 = Data2 >> (Cflint_Bits / 2);
    //2.分别计算高位与低位的交乘
    Cflint_Type I0 = X0 * Y0;
    Cflint_Type I1 = X0 * Y1;
    Cflint_Type I2 = X1 * Y0;
    Cflint_Type I3 = X1 * Y1;
    //3.计算溢出
    Cflint_Type Overflow0 = I0 >> (Cflint_Bits / 2); //低位的进位
    Cflint_Type Result_0  = I1 + I2 + Overflow0;        //低位的值
    Cflint_Type Result_1  = I3;                         //高位的值
    if (Result_0 < I1)                                   //低位溢出
        Result_1 += ((Cflint_Type)1 << (Cflint_Bits / 2));
    //4.计算终点值
    Result_XY0 |= (I0 & CFLINT_ByteLH);
    Result_XY0 |= (Result_0 << (Cflint_Bits / 2));
    Result_XY1 |= (Result_1);
    Result_XY1 += (Result_0 >> (Cflint_Bits / 2));
    //5.计算进位
    (*Ow) += (Result_XY1 >> (Cflint_Bits - 1));
    //5.保存本次计算的低位和高位
    Result_XY1 <<= 1;
    Result_XY1  |= (Result_XY0 >> (Cflint_Bits - 1));
    Result_XY0 <<= 1;

    (*Carry1) += Result_XY0;
    (*Carry2) += Result_XY1;
    //6.本次计算是否低位产生溢出
    if ((*Carry1) < Result_XY0)
        (*Carry2) += 1;
    //7.本次计算是否高位产生溢出
    if (((*Carry2)  < Result_XY1) ||
        ((*Carry2) == Result_XY1 && (*Carry1) < Result_XY0))
         (*Ow) += 1;
#endif
}

/* 乘运算 */
void Cflint_Mult(Cflint_Type *Ret, Cflint_Type *Op_1,
                     Cflint_Type *Op_2,  uint32_t  Len)
{
    Cflint_Type Carry1 = 0;
    Cflint_Type Carry2 = 0;
    Cflint_Type Ow = 0;

    for (uint32_t K = 0; K < Len; K++) {
        for (uint32_t I = 0; I <= K; I++)
            /* 使用位乘法器 */
            Cflint_Multiply1(Op_1[I], Op_2[K - I],
                             &Carry1, &Carry2, &Ow);
        /* 结果保留 */
        Ret[K]   = Carry1;   //保留低位
        Carry1      = Carry2;   //高位变低位
        Carry2      = Ow; //进位变高位
        Ow    = 0;        //进位清0
    }
    for (uint32_t K = Len; K < Len * 2 - 1; K++) {
        for (uint32_t I = (K + 1) - Len; I < Len; I++)
            /* 使用位乘法器 */
            Cflint_Multiply1(Op_1[I], Op_2[K - I],
                             &Carry1, &Carry2, &Ow);
        /* 结果保留 */
        Ret[K]   = Carry1;   //保留低位
        Carry1      = Carry2;   //高位变低位
        Carry2      = Ow; //进位变高位
        Ow    = 0;        //进位清0
    }

    Ret[Len * 2 - 1] = Carry1;
}

/* 方运算 */
void Cflint_Square(Cflint_Type *Ret, Cflint_Type *Op, uint32_t Len)
{
    Cflint_Type Carry1 = 0;
    Cflint_Type Carry2 = 0;
    Cflint_Type Ow = 0;

    for (uint32_t K = 0; K < Len * 2 - 1; K++) {
         uint32_t Min = (K < Len ? 0 : ((K + 1) - Len));
        for (uint32_t I = Min; I <= K && I <= K - I; I++) {
            /* 使用位乘法器 */
            if (I <  K - I)
                Cflint_Square1(Op[I], Op[K - I],
                               &Carry1, &Carry2, &Ow);
            if (I >= K - I)
                Cflint_Multiply1(Op[I], Op[K - I],
                                 &Carry1, &Carry2, &Ow);
        }
        /* 结果保留 */
        Ret[K]   = Carry1;   //保留低位
        Carry1      = Carry2;   //高位变低位
        Carry2      = Ow; //进位变高位
        Ow    = 0;        //进位清0
    }

    Ret[Len * 2 - 1] = Carry1;
}

/* 带余除运算 */
void Cflint_Devide(Cflint_Type *Quotient, Cflint_Type *Module,
                   Cflint_Type *Op_0, Cflint_Type *Op_1,
                      uint32_t  Len)
{
    /* 初始化时模为被除数,商为0 */
    Cflint_Copy(Module, Op_0, Len);
    Cflint_SetValue(Quotient, Len, 0);
    Cflint_Type *Op = Op_1;
    /* 特例:除数为0检查 */
    if (Cflint_IsZero(Op, Len))
        return;
    /* 除数移位记录 */
    int64_t MovedBits2 = 0;
    /* 主迭代运行 */
    do {
        /* 比较被除数和除数 */
        int8_t CompareResult = Cflint_Compare(Module, Op, Len);
        /* 被除数等于除数,恰好被整除时 */
        if (CompareResult == 0) {
            /* 商累加1个单元 */
            Cflint_AddBit(Quotient, Len, 1);
            if (MovedBits2 != 0) {
                /* 商进所有位,除数回退所有位 */
                Cflint_ShiftL2(Quotient, Len, MovedBits2);
                Cflint_ShiftR2(Op, Len, MovedBits2);
            }
            Cflint_SetValue(Module, Len, 0);
            return;
        }
        /* 被除数小于除数 */
        if (CompareResult == -1) {
            if (MovedBits2 != 0) {
                int64_t Numbers0 = Cflint_Nums2(Module, Len);
                int64_t Numbers1 = Cflint_Nums2(Op, Len);
                /* 在同一水位线时,,商进1个位,除数回退1个位 */
                if (Numbers0 == Numbers1) {
                    Cflint_ShiftL2(Quotient, Len, 1);
                    Cflint_ShiftR2(Op, Len, 1);
                    MovedBits2 -= 1;
                }
                /* 不在同一水位线时,缺少退位时,商进N个位,除数回退N个位 */
                if (Numbers0 != Numbers1 && Numbers1 - Numbers0 > MovedBits2) {
                    Cflint_ShiftL2(Quotient, Len, MovedBits2);
                    Cflint_ShiftR2(Op, Len, MovedBits2);
                    MovedBits2 = 0;
                }
                /* 不在同一水位线时,冗余退位时,商进N个位,除数回退N个位 */
                if (Numbers0 != Numbers1 && Numbers1 - Numbers0 <= MovedBits2) {
                    Cflint_ShiftL2(Quotient, Len, Numbers1 - Numbers0);
                    Cflint_ShiftR2(Op, Len, Numbers1 - Numbers0);
                    MovedBits2 -= Numbers1 - Numbers0;
                }
                continue;
            }
            return;
        }
        /* 被除数大于除数 */
        if (CompareResult == 1) {
            /* 计算被除数和除数有效2进制位数 */
            int64_t Numbers0 = Cflint_Nums2(Module, Len);
            int64_t Numbers1 = Cflint_Nums2(Op, Len);
            /* 如果它们未处在同一级别,除数放大且小于被除数 */
            if (Numbers0 > Numbers1 + 1) {
                Cflint_ShiftL2(Op, Len, Numbers0 - Numbers1);
                MovedBits2 += Numbers0 - Numbers1;
                continue;
            }
            /* 现在被除数和除数至少处于同一级别, 执行约减 */
            Cflint_Sub(Module, Module, Op, Len);
            /* 商累加1个单元 */
            Cflint_AddBit(Quotient, Len, 1);
            continue;
        }
    } while (1);
}

/* 基础模运算 */
void Cflint_Mod(Cflint_Type *Module, Cflint_Type *Op_0,
                   Cflint_Type *Op_1,  uint32_t  Len)
{
    /* 初始化时模为被除数,商为0 */
    if (Module != Op_0)
        Cflint_Copy(Module, Op_0, Len);
    Cflint_Type *Op = Op_1;
    /* 特例:除数为0检查 */
    if (Cflint_IsZero(Op, Len))
        return;
    /* 除数移位记录 */
    int64_t MovedBits2 = 0;
    /* 主迭代运行 */
    do {
        /* 比较被除数和除数 */
        int8_t CompareResult = Cflint_Compare(Module, Op, Len);
        /* 被除数等于除数,恰好被整除时 */
        if (CompareResult == 0) {
            if (MovedBits2 != 0) {
                /* 除数回退所有位 */
                Cflint_ShiftR2(Op, Len, MovedBits2);
            }
            Cflint_SetValue(Module, Len, 0);
            return;
        }
        /* 被除数小于除数 */
        if (CompareResult == -1) {
            if (MovedBits2 != 0) {
                int64_t Numbers0 = Cflint_Nums2(Module, Len);
                int64_t Numbers1 = Cflint_Nums2(Op, Len);
                /* 在同一水位线时,后退一个单元 */
                if (Numbers0 == Numbers1) {
                    Cflint_ShiftR2(Op, Len, 1);
                    MovedBits2 -= 1;
                }
                /* 不在同一水位线时,缺少退位时,除数回退N个位 */
                if (Numbers0 != Numbers1 && Numbers1 - Numbers0 > MovedBits2) {
                    Cflint_ShiftR2(Op, Len, MovedBits2);
                    MovedBits2 = 0;
                }
                /* 不在同一水位线时,冗余退位时,除数回退N个位 */
                if (Numbers0 != Numbers1 && Numbers1 - Numbers0 <= MovedBits2) {
                    Cflint_ShiftR2(Op, Len, Numbers1 - Numbers0);
                    MovedBits2 -= Numbers1 - Numbers0;
                }
                continue;
            }
            return;
        }
        /* 被除数大于除数 */
        if (CompareResult == 1) {
            /* 计算被除数和除数有效2进制位数 */
            int64_t Numbers0 = Cflint_Nums2(Module, Len);
            int64_t Numbers1 = Cflint_Nums2(Op, Len);
            /* 如果它们未处在同一级别,除数放大且小于被除数 */
            if (Numbers0 > Numbers1 + 1) {
                Cflint_ShiftL2(Op, Len, Numbers0 - Numbers1);
                MovedBits2 += Numbers0 - Numbers1;
                continue;
            }
            /* 现在被除数和除数至少处于同一级别, 执行约减 */
            Cflint_Sub(Module, Module, Op, Len);
            continue;
        }
    } while (1);
}

/* 模和运算 */
void Cflint_ModAdd(Cflint_Type *Ret,   Cflint_Type *Module,
                           Cflint_Type *Op_0, Cflint_Type *Op_1,
                           Cflint_Type *Temp[2],     uint32_t  Len)
{
    Cflint_Mod(Temp[0], Op_0, Module, Len);
    Cflint_Mod(Temp[1], Op_1, Module, Len);
    
    if (Cflint_Add(Temp[0], Temp[0], Temp[1], Len) != 0 ||
        Cflint_Compare(Temp[0], Module, Len) != -1)
        Cflint_Sub(Temp[0], Temp[0], Module, Len);
    Cflint_Copy(Ret, Temp[0], Len);
}

/* 模差运算 */
void Cflint_ModSub(Cflint_Type *Ret,   Cflint_Type *Module,
                              Cflint_Type *Op_0, Cflint_Type *Op_1,
                              Cflint_Type *Temp[2],     uint32_t  Len)
{
    Cflint_Mod(Temp[0], Op_0, Module, Len);
    Cflint_Mod(Temp[1], Op_1, Module, Len);
    
    if (Cflint_Sub(Temp[0], Temp[0], Temp[1], Len) != 0)
        Cflint_Add(Temp[0], Temp[0], Module, Len);
    Cflint_Copy(Ret, Temp[0], Len);
}

/* 模乘运算 */
void Cflint_ModMult(Cflint_Type *Ret,   Cflint_Type *Module,
                           Cflint_Type *Op_0, Cflint_Type *Op_1,
                           Cflint_Type *Temp[2],     uint32_t  Len)
{
    Cflint_Mult(Temp[0], Op_0, Op_1, Len);
    Cflint_SetValue(Temp[1], Len * 2, 0);
    Cflint_Copy(Temp[1], Module, Len);
    Cflint_Mod(Temp[0], Temp[0], Temp[1], Len * 2);
    Cflint_Copy(Ret, Temp[0], Len);
}

/* 模方运算 */
void Cflint_ModSquare(Cflint_Type *Ret,  Cflint_Type *Module,
                         Cflint_Type *Op, Cflint_Type *Temp[2],
                            uint32_t  Len)
{
    Cflint_Square(Temp[0], Op, Len);
    Cflint_SetValue(Temp[1], Len * 2, 0);
    Cflint_Copy(Temp[1], Module, Len);
    Cflint_Mod(Temp[0], Temp[0], Temp[1], Len * 2);
    Cflint_Copy(Ret, Temp[0], Len);
}

/* 模幂运算 */
void Cflint_ModExp(Cflint_Type *Ret,  Cflint_Type *Module,
                           Cflint_Type *Op, Cflint_Type *Exponent,
                           Cflint_Type *Temp[4],    uint32_t  Len)
{
    /* 固有开销 */
    Cflint_Type *X = Temp[0];  //保留Result
    Cflint_Type *Y = Temp[1];  //保留Module
    Cflint_Type *A = Temp[2];  //保留Operand
    Cflint_Type *B = Temp[3];  //保留Operand
    
    /* 特例:除数为0检查 */
    if (Cflint_IsZero(Module, Len))
        return;
    /* 初始化:X == 1, Y == Module */
    Cflint_SetValue(X, Len * 2, 0);
    Cflint_AddBit(X, Len * 2, 1);
    Cflint_SetValue(Y, Len * 2, 0);
    Cflint_Copy(Y, Module, Len);
    /* 初始化:A == Op % Module, B == 0 */
    Cflint_SetValue(A, Len * 2, 0);
    Cflint_Mod(A, Op, Module, Len);
    Cflint_SetValue(B, Len * 2, 0);
    /* 计算:2**Numbers <= Exponent < 2**(Numbers + 1) */
    int64_t Numbers = Cflint_Nums2(Exponent, Len);
    /* 检查:2**K == Exponent */
    bool NumbersOnlyOne = Cflint_IsExp2(Exponent, Len);
    /* 特殊优化场景:指数为2的幂 */
    if ( NumbersOnlyOne) {
        /* 一个数的0次幂为1 */
        if (Numbers == -1) {
            Cflint_Copy(Ret, X, Len);
            return;
        }
        /* 一个数的1次幂为本身 */
        if (Numbers == 0) {
            Cflint_Mod(X, A, Y, Len * 2);
            Cflint_Copy(Ret, X, Len);
            return;
        }
        /* 顺向迭代 */
        for (int64_t Bits2 = 1; Bits2 <= Numbers; Bits2++) {
            /* 计算:X = A**2 % Y */
            Cflint_Square(B, A, Len);
            Cflint_Copy(A, B, Len);
            Cflint_Mod(X, A, Y, Len * 2);
        }
        Cflint_Copy(Ret, X, Len);
        return;
    }
    /* 普通场景 */
    if (!NumbersOnlyOne) {
        for (int64_t Bits2 = 0; Bits2 <= Numbers; Bits2++) {
            /* 索引计算 */
            uint32_t BitN = Bits2 / Cflint_Bits;
            uint32_t Bit2 = Bits2 % Cflint_Bits;
            /* 逆向迭代 */
            if ((Exponent[BitN] & (1 << Bit2)) != 0) {
                /* 计算:X = X * A % Y */
                Cflint_Mult(B, X, A, Len);
                Cflint_Mod(X, B, Y, Len * 2);
            }
            /* 计算:A = A**2 % Y */
            Cflint_Square(B, A, Len);
            Cflint_Mod(A, B, Y, Len * 2);
        }
        Cflint_Copy(Ret, X, Len);
        return;
    }
}

/* 模逆运算 */
void Cflint_ModInv(Cflint_Type *Ret, Cflint_Type *Op,
                          Cflint_Type *Module, Cflint_Type *Temp[4],
                             uint32_t  Len)
{
    /* 固有额外空间开销 */
    Cflint_Type *A = Temp[0];
    Cflint_Type *B = Temp[1];
    Cflint_Type *U = Temp[2];
    Cflint_Type *V = Temp[3];

    /* 1.除数为0检查 */
    if (Cflint_IsZero(Op, Len)) {
        Cflint_SetValue(Ret, Len, 0);
        return;
    }
    /* 2.初始化:A = Op,B = Module */
    Cflint_Copy(A, Op, Len);
    Cflint_Copy(B, Module,  Len);
    /* 3.初始化:V = 0,U = 1 */
    Cflint_SetValue(V, Len, 0);
    Cflint_SetValue(U, Len, 0);
    Cflint_AddBit(U, Len, 1);
    /* 4.准备状态量 */
    int8_t CompareResult = 0;
    /* 5.开始主流程 */
    while ((CompareResult = Cflint_Compare(A, B, Len)) != 0) {
        if ((A[0] & 1) == 0 || (B[0] & 1) == 0) {
            /* 场景1:A最低位为0,或,B最低位为0 */
            Cflint_Type *AB = NULL, *UV = NULL;
            /* 合并场景,这里先查B后查A */
            if ((B[0] & 1) == 0) { AB = B; UV = V;}
            if ((A[0] & 1) == 0) { AB = A; UV = U;}
            /* AB >>= 1 */
            Cflint_ShiftR2(AB, Len, 1);
            /* UV最低位不为0:UV += Module */
            Cflint_Type Ow = 0;
            if ((UV[0] & 1) != 0)
                Ow = Cflint_Add(UV, UV, Module, Len);
            /* UV >>= 1 */
            Cflint_ShiftR2(UV, Len, 1);
            /* 溢出检查 */
            if (Ow != 0)
                UV[Len - 1] |= CFLINT_ByteH1;
            continue;
        }
        /* 场景2.A>B,或,A<=B */
        {
            Cflint_Type *AB_Max = NULL, *UV_Max = NULL;
            Cflint_Type *AB_Min = NULL, *UV_Min = NULL;
            /* 合并场景 */
            if (CompareResult == 1) {
                AB_Max = A; UV_Max = U;
                AB_Min = B; UV_Min = V;
            }
            if (CompareResult != 1) {
                AB_Max = B; UV_Max = V;
                AB_Min = A; UV_Min = U;
            }
            /* AB_Max -= AB_Min, AB_Max >>= 1 */
            Cflint_Sub(AB_Max, AB_Max, AB_Min, Len);
            Cflint_ShiftR2(AB_Max, Len, 1);
            /* 对UV进行蒙哥马利约减:(UV_Max - UV_Min) % Module */
            if (Cflint_Compare(UV_Max, UV_Min, Len) == -1)
                Cflint_Add(UV_Max, UV_Max, Module, Len);
            Cflint_Sub(UV_Max, UV_Max, UV_Min, Len);
            /* Max1最低位不为0:UV_Max += Module */
            Cflint_Type Ow = 0;
            if ((UV_Max[0] & 1) != 0)
                Ow = Cflint_Add(UV_Max, UV_Max, Module, Len);
            /* UV_Max >>= 1 */
            Cflint_ShiftR2(UV_Max, Len, 1);
            /* 溢出检查 */
            if (Ow != 0)
                UV_Max[Len - 1] |= CFLINT_ByteH1;
            continue;
        }
    }
    /* 拷贝数据结果 */
    Cflint_Copy(Ret, U, Len);
}

/* 带符号与运算:Ret * (*ResultFlag) = Op_1 * (Operand1_Flag) + */
/*                                       Op_2 * (Operand2_Flag)   */
/* 备注:大数运算本身是无符号语义运算,这里是为某些数学运算额外扩展 */
Cflint_Type Cflint_FlagSum(Cflint_Type *Ret,   Cflint_Type *ResultFlag,
                           Cflint_Type *Op_1, Cflint_Type  Operand1_Flag,
                           Cflint_Type *Op_2, Cflint_Type  Operand2_Flag,
                              uint32_t  Len)
{
    if (Operand1_Flag == 0 && Operand2_Flag == 0) {
        *ResultFlag = 0;
        return Cflint_Add(Ret, Op_1, Op_2, Len);
    }
    if (Operand1_Flag == 1 && Operand2_Flag == 1) {
        *ResultFlag = 1;
        return Cflint_Add(Ret, Op_1, Op_2, Len);
    }
    int8_t CompareResult = 0;
    if (Operand1_Flag == 0 && Operand2_Flag == 1) {
        CompareResult = Cflint_Compare(Op_1, Op_2, Len);
        if (CompareResult != -1) {
            *ResultFlag = 0;
            return Cflint_Sub(Ret, Op_1, Op_2, Len);
        }
        if (CompareResult == -1) {
            *ResultFlag = 1;
            return Cflint_Sub(Ret, Op_2, Op_1, Len);
        }
    }
    if (Operand1_Flag == 1 && Operand2_Flag == 0) {
        CompareResult = Cflint_Compare(Op_2, Op_1, Len);
        if (CompareResult != -1) {
            *ResultFlag = 0;
            return Cflint_Sub(Ret, Op_2, Op_1, Len);
        }
        if (CompareResult == -1) {
            *ResultFlag = 1;
            return Cflint_Sub(Ret, Op_1, Op_2, Len);
        }
    }
}

/* 带符号模运算:Ret = Op_1 * (Operand1_Flag) % Op_2 */
/* 备注:大数运算本身是无符号语义运算,这里是为某些数学运算额外扩展 */
void Cflint_FlagMod(Cflint_Type *Ret,   Cflint_Type *Op_1,
                       Cflint_Type *Op_2, Cflint_Type  Operand1_Flag,
                          uint32_t  Len)
{
    if (Operand1_Flag == 0) {
        Cflint_Mod(Ret, Op_1, Op_2, Len);
        return;
    }
    if (Operand1_Flag == 1) {
        Cflint_Mod(Ret, Op_1, Op_2, Len);
        Cflint_Sub(Ret, Op_1, Ret, Len);
        return;
    }
}

