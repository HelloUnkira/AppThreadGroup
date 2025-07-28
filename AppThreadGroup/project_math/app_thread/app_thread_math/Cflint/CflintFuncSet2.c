
#include "Cflint.h"

/* 和运算:数加一个位 */
Cflint_Type Cflint_AddBit(Cflint_Type *Op, uint32_t Len, Cflint_Type Ow)
{
    /* 计算数累加一个位 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Cflint_Type T = Op[Idx] + Ow;
        if (T < Op[Idx]) Ow = 1; /* 检查进位溢出 */
        if (T > Op[Idx]) Ow = 0;
        Op[Idx] = T; /* 保存运算结果 */
        /* 不存在进位溢出时即可退出 */
        if (Ow == 0) return 0;
    }
    return 1;
}

/* 差运算:数减一个位 */
Cflint_Type Cflint_SubBit(Cflint_Type *Op, uint32_t Len, Cflint_Type Ow)
{
    /* 计算数累减一个位 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Cflint_Type T = Op[Idx] - Ow;
        if (T > Op[Idx]) Ow = 1; /* 检查借位溢出 */
        if (T < Op[Idx]) Ow = 0;
        Op[Idx] = T;/* 保存运算结果 */
        /* 不存在借位溢出时即可退出 */
        if (Ow == 0) return 0;
    }
    return 1;
}

/* 翻转数:模对称指定位翻转数 */
Cflint_Type Cflint_Rev(Cflint_Type *Op, uint32_t Len)
{
    Cflint_Type Ow = 0;
    /* 计算数累减一个模 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Cflint_Type T = 0 - Op[Idx] - Ow;
        if (T > Op[Idx]) Ow = 1; /* 检查借位溢出 */
        if (T < Op[Idx]) Ow = 0;
        Op[Idx] = T; /* 保存运算结果 */
    }
    return 1;
}

/* 和运算 */
Cflint_Type Cflint_Add(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len)
{
    Cflint_Type Ow = 0;
    /* 计算俩数和 */
    for (uint32_t Idx = 0; Idx < Len; Idx++) {
        Cflint_Type T = Op_1[Idx] + Op_2[Idx] + Ow;
        if (T < Op_1[Idx] || T < Op_2[Idx]) Ow = 1; /* 检查进位溢出 */
        if (T > Op_1[Idx] && T > Op_2[Idx]) Ow = 0;
        Ret[Idx] = T; /* 保存运算结果 */
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
        Cflint_Type T = Op_1[Idx] - Op_2[Idx] - Ow;
        if (T > Op_1[Idx]) Ow = 1; /* 检查借位溢出 */
        else Ow = 0;
        
        Ret[Idx] = T; /* 保存运算结果 */
    }
    return Ow;
}

/* 乘运算(计算位乘法器) */
static void Cflint_Mult1(Cflint_Type D1, Cflint_Type D2, Cflint_Type *C1, Cflint_Type *C2, Cflint_Type *Ow)
{
#ifdef Cflint_Type2
//利用关键字优化策略
    
    //1.计算乘数结果
    //2.计算上一位的进位
    Cflint_Type2 Ret_XY = (Cflint_Type2)D1 * (Cflint_Type2)D2;
    Cflint_Type2 Ret_01 = (Cflint_Type2)(*C1);
    Ret_01 |= (Cflint_Type2)(*C2) << Cflint_Bits;
    //3.计算最终结果
    Ret_01 += Ret_XY;
    //4.本次计算是否高位产生溢出
    if (Ret_01 <  Ret_XY) (*Ow) = 1;
    if (Ret_01 >= Ret_XY) (*Ow) = 0;
    //5.保存本次计算的低位和高位
    (*C1) = (Cflint_Type)(Ret_01 >> 0);
    (*C2) = (Cflint_Type)(Ret_01 >> Cflint_Bits);
#else
//不利用关键字优化策略
    Cflint_Type Ret_XY0 = 0;
    Cflint_Type Ret_XY1 = 0;
    //1.获得乘数的高位与低位
    Cflint_Type X0 = D1 & CFLINT_ByteLH;
    Cflint_Type X1 = D1 >> (Cflint_Bits / 2);
    Cflint_Type Y0 = D2 & CFLINT_ByteLH;
    Cflint_Type Y1 = D2 >> (Cflint_Bits / 2);
    //2.分别计算高位与低位的交乘
    Cflint_Type I0 = X0 * Y0;
    Cflint_Type I1 = X0 * Y1;
    Cflint_Type I2 = X1 * Y0;
    Cflint_Type I3 = X1 * Y1;
    //3.计算溢出
    Cflint_Type Ow0 = I0 >> (Cflint_Bits / 2); //低位的进位
    Cflint_Type Ret_0 = I1 + I2 + Ow0; //低位的值
    Cflint_Type Ret_1 = I3; //高位的值
    if (Ret_0 < I1) //低位溢出
        Ret_1 += ((Cflint_Type)1 << (Cflint_Bits / 2));
    //4.计算终点值
    Ret_XY0 |= (I0 & CFLINT_ByteLH);
    Ret_XY0 |= (Ret_0 << (Cflint_Bits / 2));
    Ret_XY1 |= (Ret_1);
    Ret_XY1 += (Ret_0 >> (Cflint_Bits / 2));
    //5.保存本次计算的低位和高位
    (*C1) += Ret_XY0;
    (*C2) += Ret_XY1;
    //6.本次计算是否低位产生溢出
    if ((*C1) < Ret_XY0)
        (*C2) += 1;
    //7.本次计算是否高位产生溢出
    if (((*C2)  < Ret_XY1) ||
        ((*C2) == Ret_XY1 && (*C1) < Ret_XY0))
         (*Ow) += 1;
#endif
}

/* 方运算(计算位乘方器) */
static void Cflint_Square1(Cflint_Type D1, Cflint_Type D2, Cflint_Type *C1, Cflint_Type *C2, Cflint_Type *Ow)
{
#ifdef Cflint_Type2
//利用关键字优化策略
    
    //1.计算乘数结果
    //2.计算上一位的进位
    Cflint_Type2 Ret_XY = (Cflint_Type2)D1 * (Cflint_Type2)D2;
    Cflint_Type2 Ret_01 = (Cflint_Type2)(*C1);
    Ret_01 |= (Cflint_Type2)(*C2) << Cflint_Bits;
    //3.计算进位
    (*Ow) += (Ret_XY >> (Cflint_Bits * 2 - 1));
    //4.计算最终结果
    Ret_XY *= 2;
    Ret_01 += Ret_XY;
    //4.本次计算是否高位产生溢出
    if (Ret_01 <  Ret_XY) (*Ow) += 1;
    if (Ret_01 >= Ret_XY) (*Ow) += 0;
    //5.保存本次计算的低位和高位
    (*C1) = (Cflint_Type)(Ret_01 >> 0);
    (*C2) = (Cflint_Type)(Ret_01 >> Cflint_Bits);
#else
//不利用关键字优化策略
    Cflint_Type Ret_XY0 = 0;
    Cflint_Type Ret_XY1 = 0;
    //1.获得乘数的高位与低位
    Cflint_Type X0 = D1 & CFLINT_ByteLH;
    Cflint_Type X1 = D1 >> (Cflint_Bits / 2);
    Cflint_Type Y0 = D2 & CFLINT_ByteLH;
    Cflint_Type Y1 = D2 >> (Cflint_Bits / 2);
    //2.分别计算高位与低位的交乘
    Cflint_Type I0 = X0 * Y0;
    Cflint_Type I1 = X0 * Y1;
    Cflint_Type I2 = X1 * Y0;
    Cflint_Type I3 = X1 * Y1;
    //3.计算溢出
    Cflint_Type Ow0 = I0 >> (Cflint_Bits / 2); //低位的进位
    Cflint_Type Ret_0  = I1 + I2 + Ow0;        //低位的值
    Cflint_Type Ret_1  = I3;                         //高位的值
    if (Ret_0 < I1)                                   //低位溢出
        Ret_1 += ((Cflint_Type)1 << (Cflint_Bits / 2));
    //4.计算终点值
    Ret_XY0 |= (I0 & CFLINT_ByteLH);
    Ret_XY0 |= (Ret_0 << (Cflint_Bits / 2));
    Ret_XY1 |= (Ret_1);
    Ret_XY1 += (Ret_0 >> (Cflint_Bits / 2));
    //5.计算进位
    (*Ow) += (Ret_XY1 >> (Cflint_Bits - 1));
    //5.保存本次计算的低位和高位
    Ret_XY1 <<= 1;
    Ret_XY1  |= (Ret_XY0 >> (Cflint_Bits - 1));
    Ret_XY0 <<= 1;
    
    (*C1) += Ret_XY0;
    (*C2) += Ret_XY1;
    //6.本次计算是否低位产生溢出
    if ((*C1) < Ret_XY0)
        (*C2) += 1;
    //7.本次计算是否高位产生溢出
    if (((*C2)  < Ret_XY1) ||
        ((*C2) == Ret_XY1 && (*C1) < Ret_XY0))
         (*Ow) += 1;
#endif
}

/* 乘运算 */
void Cflint_Mult(Cflint_Type *Ret, Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len)
{
    Cflint_Type C1 = 0;
    Cflint_Type C2 = 0;
    Cflint_Type Ow = 0;
    
    for (uint32_t K = 0; K < Len; K++) {
        /* 使用位乘法器 */
        for (uint32_t I = 0; I <= K; I++)
            Cflint_Mult1(Op_1[I], Op_2[K - I], &C1, &C2, &Ow);
        /* 保留低位, 高位变低位, 进位变高位, 进位清0 */
        Ret[K] = C1; C1 = C2; C2 = Ow; Ow = 0;
    }
    for (uint32_t K = Len; K < Len * 2 - 1; K++) {
        /* 使用位乘法器 */
        for (uint32_t I = (K + 1) - Len; I < Len; I++)
            Cflint_Mult1(Op_1[I], Op_2[K - I], &C1, &C2, &Ow);
        /* 保留低位, 高位变低位, 进位变高位, 进位清0 */
        Ret[K] = C1; C1 = C2; C2 = Ow; Ow = 0;
    }
    
    Ret[Len * 2 - 1] = C1;
}

/* 方运算 */
void Cflint_Square(Cflint_Type *Ret, Cflint_Type *Op, uint32_t Len)
{
    Cflint_Type C1 = 0;
    Cflint_Type C2 = 0;
    Cflint_Type Ow = 0;

    for (uint32_t K = 0; K < Len * 2 - 1; K++) {
         uint32_t Min = (K < Len ? 0 : ((K + 1) - Len));
        /* 使用位乘法器 */
        for (uint32_t I = Min; I <= K && I <= K - I; I++) {
            if (I <  K - I) Cflint_Square1(Op[I], Op[K - I], &C1, &C2, &Ow);
            if (I >= K - I) Cflint_Mult1(Op[I], Op[K - I], &C1, &C2, &Ow);
        }
        /* 保留低位, 高位变低位, 进位变高位, 进位清0 */
        Ret[K] = C1; C1 = C2; C2 = Ow; Ow = 0;
    }
    
    Ret[Len * 2 - 1] = C1;
}

/* 带余除运算 */
void Cflint_Devide(Cflint_Type *Quo, Cflint_Type *Mod, Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len)
{
    /* 初始化时模为被除数,商为0 */
    Cflint_Copy(Mod, Op_0, Len);
    Cflint_ValSet(Quo, Len, 0);
    Cflint_Type *Op = Op_1;
    /* 特例:除数为0检查 */
    if (Cflint_IsZero(Op, Len))
        return;
    /* 除数移位记录 */
    int64_t MovedBits2 = 0;
    /* 主迭代运行 */
    do {
        /* 比较被除数和除数 */
        int8_t CmpRet = Cflint_Cmp(Mod, Op, Len);
        /* 被除数等于除数,恰好被整除时 */
        if (CmpRet == 0) {
            /* 商累加1个单元 */
            Cflint_AddBit(Quo, Len, 1);
            if (MovedBits2 != 0) {
                /* 商进所有位,除数回退所有位 */
                Cflint_ShiftL2(Quo, Len, MovedBits2);
                Cflint_ShiftR2(Op, Len, MovedBits2);
            }
            Cflint_ValSet(Mod, Len, 0);
            return;
        }
        /* 被除数小于除数 */
        if (CmpRet == -1) {
            if (MovedBits2 != 0) {
                int64_t Nums0 = Cflint_Nums2(Mod, Len);
                int64_t Nums1 = Cflint_Nums2(Op, Len);
                /* 在同一水位线时,,商进1个位,除数回退1个位 */
                if (Nums0 == Nums1) {
                    Cflint_ShiftL2(Quo, Len, 1);
                    Cflint_ShiftR2(Op, Len, 1);
                    MovedBits2 -= 1;
                }
                /* 不在同一水位线时,缺少退位时,商进N个位,除数回退N个位 */
                if (Nums0 != Nums1 && Nums1 - Nums0 > MovedBits2) {
                    Cflint_ShiftL2(Quo, Len, MovedBits2);
                    Cflint_ShiftR2(Op, Len, MovedBits2);
                    MovedBits2 = 0;
                }
                /* 不在同一水位线时,冗余退位时,商进N个位,除数回退N个位 */
                if (Nums0 != Nums1 && Nums1 - Nums0 <= MovedBits2) {
                    Cflint_ShiftL2(Quo, Len, Nums1 - Nums0);
                    Cflint_ShiftR2(Op, Len, Nums1 - Nums0);
                    MovedBits2 -= Nums1 - Nums0;
                }
                continue;
            }
            return;
        }
        /* 被除数大于除数 */
        if (CmpRet == 1) {
            /* 计算被除数和除数有效2进制位数 */
            int64_t Nums0 = Cflint_Nums2(Mod, Len);
            int64_t Nums1 = Cflint_Nums2(Op, Len);
            /* 如果它们未处在同一级别,除数放大且小于被除数 */
            if (Nums0 > Nums1 + 1) {
                Cflint_ShiftL2(Op, Len, Nums0 - Nums1);
                MovedBits2 += Nums0 - Nums1;
                continue;
            }
            /* 现在被除数和除数至少处于同一级别, 执行约减 */
            Cflint_Sub(Mod, Mod, Op, Len);
            /* 商累加1个单元 */
            Cflint_AddBit(Quo, Len, 1);
            continue;
        }
    } while (1);
}

/* 基础模运算 */
void Cflint_Mod(Cflint_Type *Mod, Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len)
{
    /* 初始化时模为被除数,商为0 */
    if (Mod != Op_0)
        Cflint_Copy(Mod, Op_0, Len);
    Cflint_Type *Op = Op_1;
    /* 特例:除数为0检查 */
    if (Cflint_IsZero(Op, Len))
        return;
    /* 除数移位记录 */
    int64_t MovedBits2 = 0;
    /* 主迭代运行 */
    do {
        /* 比较被除数和除数 */
        int8_t CmpRet = Cflint_Cmp(Mod, Op, Len);
        /* 被除数等于除数,恰好被整除时 */
        if (CmpRet == 0) {
            if (MovedBits2 != 0) {
                /* 除数回退所有位 */
                Cflint_ShiftR2(Op, Len, MovedBits2);
            }
            Cflint_ValSet(Mod, Len, 0);
            return;
        }
        /* 被除数小于除数 */
        if (CmpRet == -1) {
            if (MovedBits2 != 0) {
                int64_t Nums0 = Cflint_Nums2(Mod, Len);
                int64_t Nums1 = Cflint_Nums2(Op, Len);
                /* 在同一水位线时,后退一个单元 */
                if (Nums0 == Nums1) {
                    Cflint_ShiftR2(Op, Len, 1);
                    MovedBits2 -= 1;
                }
                /* 不在同一水位线时,缺少退位时,除数回退N个位 */
                if (Nums0 != Nums1 && Nums1 - Nums0 > MovedBits2) {
                    Cflint_ShiftR2(Op, Len, MovedBits2);
                    MovedBits2 = 0;
                }
                /* 不在同一水位线时,冗余退位时,除数回退N个位 */
                if (Nums0 != Nums1 && Nums1 - Nums0 <= MovedBits2) {
                    Cflint_ShiftR2(Op, Len, Nums1 - Nums0);
                    MovedBits2 -= Nums1 - Nums0;
                }
                continue;
            }
            return;
        }
        /* 被除数大于除数 */
        if (CmpRet == 1) {
            /* 计算被除数和除数有效2进制位数 */
            int64_t Nums0 = Cflint_Nums2(Mod, Len);
            int64_t Nums1 = Cflint_Nums2(Op, Len);
            /* 如果它们未处在同一级别,除数放大且小于被除数 */
            if (Nums0 > Nums1 + 1) {
                Cflint_ShiftL2(Op, Len, Nums0 - Nums1);
                MovedBits2 += Nums0 - Nums1;
                continue;
            }
            /* 现在被除数和除数至少处于同一级别, 执行约减 */
            Cflint_Sub(Mod, Mod, Op, Len);
            continue;
        }
    } while (1);
}

/* 模和运算 */
void Cflint_ModAdd(Cflint_Type *Ret, Cflint_Type *Mod, Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len)
{
    Cflint_Type *T_0 = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T_1 = Cflint_Alloc(Cflint_Byte * Len);
    
    Cflint_Mod(T_0, Op_0, Mod, Len);
    Cflint_Mod(T_1, Op_1, Mod, Len);
    
    if (Cflint_Add(T_0, T_0, T_1, Len) != 0 || Cflint_Cmp(T_0, Mod, Len) != -1)
        Cflint_Sub(T_0, T_0, Mod, Len);
    
    Cflint_Copy(Ret, T_0, Len);
    
    Cflint_Free(T_0);
    Cflint_Free(T_1);
}

/* 模差运算 */
void Cflint_ModSub(Cflint_Type *Ret, Cflint_Type *Mod, Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len)
{
    Cflint_Type *T_0 = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T_1 = Cflint_Alloc(Cflint_Byte * Len);
    
    Cflint_Mod(T_0, Op_0, Mod, Len);
    Cflint_Mod(T_1, Op_1, Mod, Len);
    
    if (Cflint_Sub(T_0, T_0, T_1, Len) != 0)
        Cflint_Add(T_0, T_0, Mod, Len);
    
    Cflint_Copy(Ret, T_0, Len);
    
    Cflint_Free(T_0);
    Cflint_Free(T_1);
}

/* 模乘运算 */
void Cflint_ModMult(Cflint_Type *Ret, Cflint_Type *Mod, Cflint_Type *Op_0, Cflint_Type *Op_1, uint32_t Len)
{
    Cflint_Type *T_0 = Cflint_Alloc(Cflint_Byte * (Len * 2));
    Cflint_Type *T_1 = Cflint_Alloc(Cflint_Byte * (Len * 2));
    
    Cflint_Mult(T_0, Op_0, Op_1, Len);
    Cflint_ValSet(T_1, Len * 2, 0);
    Cflint_Copy(T_1, Mod, Len);
    Cflint_Mod(T_0, T_0, T_1, Len * 2);
    Cflint_Copy(Ret, T_0, Len);
    
    Cflint_Free(T_0);
    Cflint_Free(T_1);
}

/* 模方运算 */
void Cflint_ModSquare(Cflint_Type *Ret, Cflint_Type *Mod, Cflint_Type *Op, uint32_t Len)
{
    Cflint_Type *T_0 = Cflint_Alloc(Cflint_Byte * (Len * 2));
    Cflint_Type *T_1 = Cflint_Alloc(Cflint_Byte * (Len * 2));
    
    Cflint_Square(T_0, Op, Len);
    Cflint_ValSet(T_1, Len * 2, 0);
    Cflint_Copy(T_1, Mod, Len);
    Cflint_Mod(T_0, T_0, T_1, Len * 2);
    Cflint_Copy(Ret, T_0, Len);
    
    Cflint_Free(T_0);
    Cflint_Free(T_1);
}

/* 模幂运算 */
void Cflint_ModExp(Cflint_Type *Ret, Cflint_Type *Mod, Cflint_Type *Op, Cflint_Type *Exp, uint32_t Len)
{
    /* 固有开销 */
    Cflint_Type *X = Cflint_Alloc(Cflint_Byte * (Len * 2));  //保留Ret
    Cflint_Type *Y = Cflint_Alloc(Cflint_Byte * (Len * 2));  //保留Mod
    Cflint_Type *A = Cflint_Alloc(Cflint_Byte * (Len * 2));  //保留Op
    Cflint_Type *B = Cflint_Alloc(Cflint_Byte * (Len * 2));  //保留Op
    
    /* 特例:除数为0检查 */
    if (Cflint_IsZero(Mod, Len))
        goto over;
    /* 初始化:X == 1, Y == Mod */
    Cflint_ValSet(X, Len * 2, 0);
    Cflint_AddBit(X, Len * 2, 1);
    Cflint_ValSet(Y, Len * 2, 0);
    Cflint_Copy(Y, Mod, Len);
    /* 初始化:A == Op % Mod, B == 0 */
    Cflint_ValSet(A, Len * 2, 0);
    Cflint_Mod(A, Op, Mod, Len);
    Cflint_ValSet(B, Len * 2, 0);
    /* 计算:2**Nums <= Exp < 2**(Nums + 1) */
    int64_t Nums = Cflint_Nums2(Exp, Len);
    /* 检查:2**K == Exp */
    bool IsExp2 = Cflint_IsExp2(Exp, Len);
    /* 特殊优化场景:指数为2的幂 */
    if ( IsExp2) {
        /* 一个数的0次幂为1 */
        if (Nums == -1) {
            Cflint_Copy(Ret, X, Len);
            goto over;
        }
        /* 一个数的1次幂为本身 */
        if (Nums == 0) {
            Cflint_Mod(X, A, Y, Len * 2);
            Cflint_Copy(Ret, X, Len);
            goto over;
        }
        /* 顺向迭代 */
        for (int64_t Bits2 = 1; Bits2 <= Nums; Bits2++) {
            /* 计算:X = A**2 % Y */
            Cflint_Square(B, A, Len);
            Cflint_Copy(A, B, Len);
            Cflint_Mod(X, A, Y, Len * 2);
        }
        Cflint_Copy(Ret, X, Len);
        goto over;
    }
    /* 普通场景 */
    if (!IsExp2) {
        for (int64_t Bits2 = 0; Bits2 <= Nums; Bits2++) {
            /* 索引计算 */
            uint32_t BitN = Bits2 / Cflint_Bits;
            uint32_t Bit2 = Bits2 % Cflint_Bits;
            /* 逆向迭代 */
            if ((Exp[BitN] & (1 << Bit2)) != 0) {
                /* 计算:X = X * A % Y */
                Cflint_Mult(B, X, A, Len);
                Cflint_Mod(X, B, Y, Len * 2);
            }
            /* 计算:A = A**2 % Y */
            Cflint_Square(B, A, Len);
            Cflint_Mod(A, B, Y, Len * 2);
        }
        Cflint_Copy(Ret, X, Len);
        goto over;
    }
    
    over:
    Cflint_Free(X);
    Cflint_Free(Y);
    Cflint_Free(A);
    Cflint_Free(B);
}

/* 模逆运算 */
void Cflint_ModInv(Cflint_Type *Ret, Cflint_Type *Op, Cflint_Type *Mod, uint32_t Len)
{
    /* 固有额外空间开销 */
    Cflint_Type *A = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *B = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *U = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *V = Cflint_Alloc(Cflint_Byte * Len);
    
    /* 1.除数为0检查 */
    if (Cflint_IsZero(Op, Len)) {
        Cflint_ValSet(Ret, Len, 0);
        goto over;
    }
    /* 2.初始化:A = Op,B = Mod */
    Cflint_Copy(A, Op, Len);
    Cflint_Copy(B, Mod,  Len);
    /* 3.初始化:V = 0,U = 1 */
    Cflint_ValSet(V, Len, 0);
    Cflint_ValSet(U, Len, 0);
    Cflint_AddBit(U, Len, 1);
    /* 4.准备状态量 */
    int8_t CmpRet = 0;
    /* 5.开始主流程 */
    while ((CmpRet = Cflint_Cmp(A, B, Len)) != 0) {
        if ((A[0] & 1) == 0 || (B[0] & 1) == 0) {
            /* 场景1:A最低位为0,或,B最低位为0 */
            Cflint_Type *AB = NULL, *UV = NULL;
            /* 合并场景,这里先查B后查A */
            if ((B[0] & 1) == 0) { AB = B; UV = V;}
            if ((A[0] & 1) == 0) { AB = A; UV = U;}
            /* AB >>= 1 */
            Cflint_ShiftR2(AB, Len, 1);
            /* UV最低位不为0:UV += Mod */
            Cflint_Type Ow = 0;
            if ((UV[0] & 1) != 0)
                Ow = Cflint_Add(UV, UV, Mod, Len);
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
            if (CmpRet == 1) {
                AB_Max = A; UV_Max = U;
                AB_Min = B; UV_Min = V;
            }
            if (CmpRet != 1) {
                AB_Max = B; UV_Max = V;
                AB_Min = A; UV_Min = U;
            }
            /* AB_Max -= AB_Min, AB_Max >>= 1 */
            Cflint_Sub(AB_Max, AB_Max, AB_Min, Len);
            Cflint_ShiftR2(AB_Max, Len, 1);
            /* 对UV进行蒙哥马利约减:(UV_Max - UV_Min) % Mod */
            if (Cflint_Cmp(UV_Max, UV_Min, Len) == -1)
                Cflint_Add(UV_Max, UV_Max, Mod, Len);
            Cflint_Sub(UV_Max, UV_Max, UV_Min, Len);
            /* Max1最低位不为0:UV_Max += Mod */
            Cflint_Type Ow = 0;
            if ((UV_Max[0] & 1) != 0)
                Ow = Cflint_Add(UV_Max, UV_Max, Mod, Len);
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
    
    over:
    Cflint_Free(A);
    Cflint_Free(B);
    Cflint_Free(U);
    Cflint_Free(V);
}
