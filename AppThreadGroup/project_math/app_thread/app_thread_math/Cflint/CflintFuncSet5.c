
#include "Cflint.h"

/* 大随机数生成(注意初始化回调的随机状态情况) */
void Cflint_Random(Cflint_Type *Op, uint32_t Len, Cflint_Type (*Random)(void))
{
    /* 这里直接以最简单的方式生成即可,但要逆向生成 */
    /* 因为随机数的目标是每一个bit位都随机 */
    /* 那么大小随机数均可完成该功能 */
    for (uint32_t Idx = 0; Idx < Len; Idx++)
        Op[Len - Idx - 1] = Random();
}

/* 强素数判别检查(Miller_Rabin测试) */
bool Cflint_IsPrime(Cflint_Type *X, uint32_t Len, Cflint_Type (*Random)(void), bool Weak)
{
    Cflint_Type *A = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *R = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *E = Cflint_Alloc(Cflint_Byte * Len);
    
    bool Ret = true;
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
            Cflint_ValSet(A, Len, 0);
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
            if (Cflint_IsZero(R, Len)) {
                Ret = false;
                goto over;
            }
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
        Cflint_ModExp(R, X, A, E, Len);
        /* 检查模是否为 X - 1 */
        Cflint_AddBit(R, Len, 1);
        if (Cflint_Equal(R, X, Len))
            continue;
        /* 检查模是否为 1 */
        Cflint_SubBit(R, Len, 2);
        if (Cflint_IsZero(R, Len))
            continue;
        Cflint_AddBit(R, Len, 1);
        bool Loop = false;
        /* 循环测试:R = R ^ 2 Mod X */
        for (uint32_t Idx1 = 0; Idx1 < Bit2_K; Idx1++) {
            Cflint_Copy(A, R, Len);
            Cflint_ModSquare(R, X, A, Len);
            /* 检查模是否为 X - 1 */
            Cflint_AddBit(R, Len, 1);
            if (Cflint_Equal(R, X, Len)) {
                Loop = true;
                break;
            }
            /* 检查模是否为 1 */
            Cflint_SubBit(R, Len, 2);
            if (Cflint_IsZero(R, Len)) {
                Ret = false;
                goto over;
            }
            Cflint_AddBit(R, Len, 1);
        }
        if (Loop)
            continue;
        
        Ret = false;
        goto over;
    }
    
    over:
    Cflint_Free(A);
    Cflint_Free(R);
    Cflint_Free(E);
    
    return Ret;
}

/* 孪生素数判别检查 */
bool Cflint_IsPrime1(Cflint_Type *X, uint32_t Len)
{
    /* 固有开销 */
    Cflint_Type *Mod = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *Y   = Cflint_Alloc(Cflint_Byte * Len);
    bool Ret = false;
    /* 单独检查:2, 3 */
    Cflint_ValSet(Mod, Len, 0);
    /* 单独检查:生成2,匹配2 */
    Mod[0] = 2;
    if (Cflint_Equal(X, Mod, Len)) {
        Ret = true; goto over;
    }
    /* 单独检查:生成3,匹配3 */
    Mod[0] = 3;
    if (Cflint_Equal(X, Mod, Len)) {
        Ret = true; goto over;
    }
    /* 解算:Mod = X % 6 */
    Cflint_ValSet(Y, Len, 0);
    Y[0] = 6;
    Cflint_Mod(Mod, X, Y, Len);
    /* 验证是否为孪生素数1:(6 * N - 1) % 6 = 5 */
    Y[0] = 5;
    if (Cflint_Equal(Y, Mod, Len)) {
        Ret = true; goto over;
    }
    /* 验证是否为孪生素数2:(6 * N + 1) % 6 = 1 */
    Y[0] = 1;
    if (Cflint_Equal(Y, Mod, Len)) {
        Ret = true; goto over;
    }
    
    over:
    Cflint_Free(Mod);
    Cflint_Free(Y);
    
    return Ret;
}

/* 指定查找次数,尝试找到一个强素数(Min和Max同时为空则取值范围在全域内) */
uint32_t Cflint_RandomPrime(Cflint_Type *X, Cflint_Type *Min, Cflint_Type *Max, uint32_t Len,
                            Cflint_Type (*Random)(void), uint32_t CntMax)
{
    Cflint_Type *T_0 = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T_1 = Cflint_Alloc(Cflint_Byte * Len);
    
    uint32_t CntAll = CntMax;
    for (uint32_t Cnt = 0; Cnt < CntMax; Cnt++) {
        /* 生成随机数 */
        Cflint_Random(X, Len, Random);
        if (Cflint_IsEven(X, Len))
            Cflint_AddBit(X, Len, 1);
        if (Min != NULL && Max != NULL) {
            Cflint_Sub(T_0, Max, Min, Len);
            Cflint_Mod(T_1, X, T_0, Len);
            Cflint_Add(X, Min, T_1, Len);
            if (Cflint_IsEven(X, Len))
                Cflint_AddBit(X, Len, 1);
            if (Cflint_Cmp(X, Max, Len) == +1)
                Cflint_SubBit(X, Len, 2);
            if (Cflint_Cmp(X, Min, Len) == -1) {
                CntAll = 0; goto over;
            }
        }
        
        if (Cflint_IsPrime(X, Len, Random, false)) {
            CntAll = Cnt; goto over;
        }
    }
    
    over:
    Cflint_Free(T_0);
    Cflint_Free(T_1);
    
    return CntAll;
}
