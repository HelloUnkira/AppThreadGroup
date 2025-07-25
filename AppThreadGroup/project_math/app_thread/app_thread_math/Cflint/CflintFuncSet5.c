
#include "Cflint.h"

/* 大随机数生成 */
void Cflint_Random(Cflint_Type  *Op, uint32_t Len,
                   Cflint_Type (*Random)(void))
{
    /* 这里直接以最简单的方式生成即可,但要逆向生成 */
    /* 因为随机数的目标是每一个bit位都随机 */
    /* 那么大小随机数均可完成该功能 */
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        Op[Len - Idx - 1] = Random();
}

/* 强素数判别检查(Miller_Rabin测试) */
bool Cflint_IsPrime(Cflint_Type *X, Cflint_Type *Temp[7], uint32_t Len,
                    Cflint_Type (*Random)(void), bool Weak)
{
    Cflint_Type  *A  = Temp[0];
    Cflint_Type  *R  = Temp[1];
    Cflint_Type  *E  = Temp[2];
    Cflint_Type **TT = Temp + 3;   //Temp[3~6]:Len * 2
    /* 小素数筛查 */
    if (Weak) {
        /* 数据量决定了当次小筛选的强度 */
        uint32_t SizerLength  = 50;
        uint32_t SizerTable[] = {
              3,   5,   7,  11,  13,  17,  19,  23,  29,  31,
             37,  41,  43,  47,  53,  59,  61,  67,  71,  73,
             79,  83,  89,  97, 101, 103, 107, 109, 113, 127,
            131, 137, 139, 149, 151, 157, 163, 167, 173, 179,
            181, 191, 193, 197, 199, 211, 223, 227, 229, 233,
        };
        for (uint32_t Idx = 0; Idx < SizerLength; Idx++) {
            Cflint_SetValue(A, Len, 0);
            /* 设置除数 */
#if 0
#elif Cflint_Byte == 1
            A[0] = SizerTable[Idx] >>  0;
            A[1] = SizerTable[Idx] >>  8;
            A[2] = SizerTable[Idx] >> 16;
            A[3] = SizerTable[Idx] >> 24;
#elif Cflint_Byte == 2
            A[0] = SizerTable[Idx] >>  0;
            A[1] = SizerTable[Idx] >> 16;
#else
            Cflint_AddBit(A, Len, SizerTable[Idx]);
#endif
            /* 计算模,如果能被整除则未通过当次素性测试 */
            Cflint_Mod(R, X, A, Len);
            if (Cflint_IsZero(R, Len))
                return false;
        }
    }
    /* 素数筛查(Miller_Rabin测试) */
    //位长度: 512;  轮数: 7
    //位长度: 1024; 轮数: 4
    //位长度: 1536; 轮数: 3
    uint32_t Wheel = 10;
    uint32_t Bits2 = Len * Cflint_Bits;
    if (Bits2 >= 1536) Wheel = 3; else
    if (Bits2 >= 1024) Wheel = 4; else
    if (Bits2 >=  512) Wheel = 7; else Wheel = 10;
    for (uint32_t Count = 0; Count < Wheel; Count++) {
        /* 随机生成一个数并满足它小于: P - 1 */
        Cflint_Random(A, Len, Random);
        Cflint_SubBit(X, Len, 1);
        Cflint_Mod(A, A, X, Len);
        Cflint_AddBit(X, Len, 1);
        /* 设置初始指数为:D(奇数) = (P - 1) / (2 ** K) */
        uint64_t Bit2_K = 0;
        Cflint_Copy(E, X, Len);
        Cflint_SubBit(E, Len, 1);
        while (Cflint_IsEven(E, Len)) {
            Cflint_ShiftR2(E, Len, 1);
            Bit2_K++;
        }
        /* 初始测试:R = A ^ D Mod X */
        Cflint_ModExp(R, X, A, E, TT, Len);
        /* 检查模是否为 X - 1 */
        Cflint_AddBit(R, Len, 1);
        if (Cflint_Equal(R, X, Len))
            continue;
        /* 检查模是否为 1 */
        Cflint_SubBit(R, Len, 2);
        if (Cflint_IsZero(R, Len))
            continue;
        Cflint_AddBit(R, Len, 1);
        bool BreakLoop = false;
        /* 循环测试:R = R ^ 2 Mod X */
        for (uint32_t Idx1 = 0; Idx1 < Bit2_K; Idx1++) {
            Cflint_Copy(A, R, Len);
            Cflint_ModSquare(R, X, A, TT, Len);
            /* 检查模是否为 X - 1 */
            Cflint_AddBit(R, Len, 1);
            if (Cflint_Equal(R, X, Len)) {
                BreakLoop = true;
                break;
            }
            /* 检查模是否为 1 */
            Cflint_SubBit(R, Len, 2);
            if (Cflint_IsZero(R, Len))
                return false;
            Cflint_AddBit(R, Len, 1);
        }
        if (BreakLoop)
            continue;
        return false;
    }
    return true;
}

/* 指定查找次数,尝试找到一个强素数(Min和Max同时为空则取值范围在全域内) */
uint32_t Cflint_RandomPrime(Cflint_Type *X, Cflint_Type *Temp[7],
                            Cflint_Type *Min, Cflint_Type *Max, uint32_t Len,
                            Cflint_Type (*Random)(void), uint32_t CountMax)
{
    for (uint32_t Count = 0; Count < CountMax; Count++) {
        /* 生成随机数 */
        Cflint_Random(X, Len, Random);
        if (Cflint_IsEven(X, Len))
            Cflint_AddBit(X, Len, 1);
        if (Min != NULL && Max != NULL) {
            Cflint_Sub(Temp[0], Max, Min, Len);
            Cflint_Mod(Temp[1], X, Temp[0], Len);
            Cflint_Add(X, Min, Temp[1], Len);
            if (Cflint_IsEven(X, Len))
                Cflint_AddBit(X, Len, 1);
            if (Cflint_Compare(X, Max, Len) == +1)
                Cflint_SubBit(X, Len, 2);
            if (Cflint_Compare(X, Min, Len) == -1)
                return 0;
        }
        
        if (Cflint_IsPrime(X, Temp, Len, Random, false))
            return Count;
    }
    return CountMax;
}

/* 孪生素数判别检查 */
bool Cflint_IsPrime1(Cflint_Type *X, Cflint_Type *Temp[3], uint32_t Len)
{
    /* 固有开销 */
    Cflint_Type *Module   = Temp[1];
    Cflint_Type *Divisor  = Temp[2];
    /* 单独检查:2, 3 */
    Cflint_SetValue(Module, Len, 0);
    /* 单独检查:生成2,匹配2 */
    Module[0] = 2;
    if(Cflint_Equal(X, Module, Len))
        return true;
    /* 单独检查:生成3,匹配3 */
    Module[0] = 3;
    if(Cflint_Equal(X, Module, Len))
        return true;
    /* 解算:Module = X % 6 */
    Cflint_SetValue(Divisor, Len, 0);
    Divisor[0] = 6;
    Cflint_Mod(Module, X, Divisor, Len);
    /* 验证是否为孪生素数1:(6 * N - 1) % 6 = 5 */
    Divisor[0] = 5;
    if(Cflint_Equal(Divisor, Module, Len))
        return true;
    /* 验证是否为孪生素数2:(6 * N + 1) % 6 = 1 */
    Divisor[0] = 1;
    if(Cflint_Equal(Divisor, Module, Len))
        return true;
    return false;
}
