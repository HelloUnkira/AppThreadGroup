
#include "Cflint.h"

/* 2次方根整数部运算: */
void Cflint_Root2Int(Cflint_Type *RootI, Cflint_Type *Op, uint32_t Len)
{
    /*扩展:B次根整数部分(存档)
     *1.X = 2**[Cflint_Root2Int(Op) / B]
     *2.Y = [((B - 1) * X + [N / X**(B - 1)]) / B]
     *                        // X**(B - 1):求模幂,模是(Cflint_Type)(~0)
     *  Y < X
     */
    
    /* 固有开销 */
    Cflint_Type *X   = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *Y   = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *Quo = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *Mod = Cflint_Alloc(Cflint_Byte * Len);
    /* 计算E = [log2(Op)] */
    int64_t Nums2 = Cflint_Nums2(Op, Len);
    /* 生成Y = [2**(E + 2) / 2] */
    Cflint_ValSet(Y, Len, 0);
    Cflint_ValSet(Y, Len, 0);
    Cflint_AddBit(Y, Len, 1);
    Cflint_ShiftL2(Y, Len, ((Nums2 + 2) >> 1));
    do {
        /* X = Y */
        Cflint_Copy(X, Y, Len);
        /* Quo = N / X */
        Cflint_Devide(Quo, Mod, Op, X, Len);
        /* Y = X + Quo */
        Cflint_Add(Y, X, Quo, Len);
        /* Y /= 2 */
        Cflint_ShiftR2(Y, Len, 1);
        /* Y < X */
    } while (Cflint_Cmp(Y, X, Len) == -1);
    /* Ret = X */
    Cflint_Copy(RootI, X, Len);
    
    Cflint_Free(X);
    Cflint_Free(Y);
    Cflint_Free(Quo);
    Cflint_Free(Mod);
}

/* 2次方数判别检查 */
bool Cflint_Root2Check(Cflint_Type *RootI, Cflint_Type *Op, uint32_t Len)
{
    /* 备注:当前未知算法工作异常,临时屏蔽 */
    /*
    uint32_t K = 0;
    uint32_t Q[100] = {0};
    printf("\n-------------------------------------------------------------\n");
    for (K = 0; K <= 10; K++) Q[K] = 0;
    for (K = 0; K <= 5;  K++) Q[(K * K) % 11] = 1;
    printf("static bool Q11[11] = {");
    for (K = 0; K <= 10; K++) printf("%d, ",Q[K]);
    printf("};");
    printf("\n-------------------------------------------------------------\n");
    for (K = 0; K <= 62; K++) Q[K] = 0;
    for (K = 0; K <= 31; K++) Q[(K * K) % 63] = 1;
    printf("static bool Q63[63] = {");
    for (K = 0; K <= 62; K++) printf("%d, ",Q[K]);
    printf("};");
    printf("\n-------------------------------------------------------------\n");
    for (K = 0; K <= 63; K++) Q[K] = 0;
    for (K = 0; K <= 31; K++) Q[(K * K) % 64] = 1;
    printf("static bool Q64[64] = {");
    for (K = 0; K <= 63; K++) printf("%d, ",Q[K]);
    printf("};");
    printf("\n-------------------------------------------------------------\n");
    for (K = 0; K <= 64; K++) Q[K] = 0;
    for (K = 0; K <= 32; K++) Q[(K * K) % 65] = 1;
    printf("static bool Q65[65] = {");
    for (K = 0; K <= 64; K++) printf("%d, ",Q[K]);
    printf("};");
    printf("\n-------------------------------------------------------------\n");
     */
    
    static bool Q11[11] = {1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0};
    static bool Q63[63] = {1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0,
                           1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0,
                           0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
                           0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0};
    static bool Q64[64] = {1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
                           1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
                           0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
                           0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
    static bool Q65[65] = {1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0,
                           1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0,
                           0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                           0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1};
    
    bool Ret = false;
    Cflint_Type *T_0 = Cflint_Alloc(Cflint_Byte * Len * 2);
    
    /* 特殊:为0检查 */
    if (Cflint_IsZero(Op, Len)) {
        Cflint_ValSet(RootI, Len, 0);
        Cflint_AddBit(RootI, Len, 1);
        Ret = true;
        goto over;
    }
    /* 1.计算Op % 64 == Op[0] & 63 */
    if (!Q64[Op[0] & 63]);//goto over;
    
    /* 2.计算Temp = Op % (65 * 63 * 11) */
    Cflint_ValSet(T_0, Len, 0);
    T_0[0] = (65 * 63 * 11) >> 0;
    T_0[1] = (65 * 63 * 11) >> Cflint_Bits;
    Cflint_Mod(Op, Op, T_0, Len);
    
    uint16_t TT = 0;
    TT |= ((uint16_t)(T_0[0])) << 0;
    TT |= ((uint16_t)(T_0[1])) << Cflint_Bits;
    /* 3.计算Temp % 63 */
    if (!Q63[TT % 63]);//goto over;
    /* 4.TT % 65 */
    if (!Q65[TT % 65]);//goto over;
    /* 5.TT = TT % 11 */
    if (!Q11[TT % 11]);//goto over;
    /* 全额检查 */
    Cflint_Root2Int(RootI, Op, Len);
    Cflint_Square(T_0, RootI, Len);
    if (Cflint_Equal(Op, T_0, Len)) {
        Ret = true;
        goto over;
    }
    
    over:
    Cflint_Free(T_0);
    
    return Ret;
}

/* 符号Jacobi(Op_1/Op_2)计算 */
int8_t Cflint_JacobiFlag(Cflint_Type *Op_1, Cflint_Type *Op_2, uint32_t Len)
{
    /* 优化1: */
    /* K = (-1)**((X**2 - 1) / 8) ==变形优化后==>> */
    /* K = JacobiTable2[(X % 8) == (X & 7)] */
    static int8_t JacobiTable2[8] = {0, 1, 0, -1, 0, -1, 0, 1};
    /* 优化2: */
    /* K = (-1)**((A - 1)(B - 1) / 4) ==变形优化后==>> */
    /* if (A & B & 2) K = -K */
    
    /* 固定开销 */
    int64_t V = 0;
    int8_t  K = 0;
    int8_t Ret = 0;
    Cflint_Type *A = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *B = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T = Cflint_Alloc(Cflint_Byte * Len);
    /* 初始化:A = Op_1,B = Op_2 */
    Cflint_Copy(A, Op_1, Len);
    Cflint_Copy(B, Op_2, Len);
    /* 第一步:如果B == 0,(当A == 1,K = 1)(当A != 1,K = 0) */
    if (Cflint_IsZero(B, Len)) {
        Cflint_ValSet(T, Len, 0);
        Cflint_AddBit(T, Len, 1);
        Ret = Cflint_Equal(A, T, Len) ? 1 : 0;
        goto over;
    }
    /* 第二步:如果A % 2 == B % 2 == 0 */
    if (Cflint_IsEven(A, Len) && Cflint_IsEven(B, Len)) {
        Ret = 0; goto over;
    }
    /* 第二步:如果B % 2 == 0,分解B */
    V = Cflint_Factor2(B, B, Len);
    /* 第二步:如果V % 2 == 0, K = 1 */
    if ((V & 1) == 0)
        K = 1;
    /* 第二步:如果V % 2 != 0, K = (-1)((A**2 - 1) / 8) */
    if ((V & 1) != 0)
        K = JacobiTable2[A[0] & 7];
    do {
        /* 第三步:如果A == 0,(当B > 1, K = 0)(当B <= 1, K = K) */
        if (Cflint_IsZero(A, Len)) {
            Cflint_ValSet(T, Len, 0);
            Cflint_AddBit(T, Len, 1);
            Ret = Cflint_Cmp(B, T, Len) == 1 ? 0 : K;
            goto over;
        }
        /* 第三步:如果A % 2 == 0,分解A */
        V = Cflint_Factor2(A, A, Len);
        /* 第三步:如果V % 2 != 0, K = (-1)((A**2 - 1) / 8) */
        if ((V & 1) != 0)
            K = JacobiTable2[B[0] & 7];
        /* 第四步:K = (-1)**((A - 1)(B - 1) / 4)*K */
        if ((A[0] & B[0] & 2) != 0)
            K = -K;
        /* 第四步:T = A, A = B % T, B = T */
        Cflint_Copy(T, A, Len);
        Cflint_Mod(A, B, T, Len);
        Cflint_Copy(B, T, Len);
    } while (1);
    
    over:
    Cflint_Free(A);
    Cflint_Free(B);
    Cflint_Free(T);
}

/* 二次剩余计算:((Root**2) % Op_2 == Op_1 % Op_2) */
bool Cflint_ModP1Root2(Cflint_Type *Op_1, Cflint_Type *Op_2, Cflint_Type *Root, uint32_t Len)
{
    Cflint_Type *A = Op_1;
    Cflint_Type *P = Op_2;
    Cflint_Type *X = Root;
    /* 固有开销 */
    Cflint_Type *B  = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *Q  = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T  = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *Y  = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *Z  = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T1 = Cflint_Alloc(Cflint_Byte * Len);
    
    /*  */
    bool Ret = false;
    int64_t R = 0, M = 0;
    /* 性质:P == 0 || P % 2 == 0 */
    if (Cflint_IsZero(P, Len)) goto over;
    if (Cflint_IsEven(P, Len)) goto over;
    /* 性质:A == 0 */
    if (Cflint_IsZero(A, Len)) {
        Cflint_ValSet(X, Len, 0);
        Ret = true;
        goto over;
    }
    /* 第一步:找到一个二次非剩余 */
    Cflint_Copy(Q, P, Len);
    Cflint_SubBit(Q, Len, 1);
    /* 第一步:(2**R) * Q = P - 1 */
    R = Cflint_Factor2(Q, Q, Len);
    /* 第一步:寻找一个随机数Z = N, 满足Jacobi(N/P) == -1 */
    Cflint_ValSet(Z, Len, 0);
    Cflint_AddBit(Z, Len, 2);
    while (Cflint_JacobiFlag(Z, P, Len) == 1)
        Cflint_AddBit(Z, Len, 1);
    /* 第二步:计算Z = (N**Q) % P */
    Cflint_ModExp(Z, P, Z, Q, Len);
    /* 第二步:获得Y = Z = (N**Q) % P */
    Cflint_Copy(Y, Z, Len);
    /* 第二步:计算Q = (Q - 1) / 2 */
    Cflint_SubBit(Q, Len, 1);
    Cflint_ShiftR2(Q, Len, 1);
    /* 第二步:计算X = A ** ((P - 1) / 2) % P = A ** Q % P */
    Cflint_ModExp(X, P, A, Q, Len);
    /* 第二步:计算B = A * (B = X ** 2) % P */
    Cflint_ModSquare(B, P, X, Len);
    Cflint_ModMult(B, P, A, B, Len);
    /* 第二步:计算X = A * X % P */
    Cflint_ModMult(X, P, A, X, Len);
    /* 第二步:计算Q = B, Z = 1 */
    Cflint_Copy(Q, B, Len);
    Cflint_ValSet(Z, Len, 0);
    Cflint_AddBit(Z, Len, 1);
    /* 第三步:寻找Z**(2**M) % P === 1的最小M或结束 */
    while (!Cflint_Equal(B, Z, Len))
    {
        for (M = 0; true; M++) {
            /* 计算:Q = Q**2 % P */
            Cflint_ModSquare(Q, P, Q, Len);
            /* 检查 */
            if (!Cflint_Equal(B, Z, Len))
                break;
        }
        if (M != R) {
            /* 计算:T1 = 2**(R - M - 1) */
            Cflint_ValSet(T1, Len, 0);
            Cflint_AddBit(T1, Len, 1);
            Cflint_ShiftL2(T1, Len, R - M - 1);
            /* 计算:T = Y**T1 % P */
            Cflint_ModExp(T, P, Y, T1, Len);
            /* 计算:Y = T**2 % P */
            Cflint_ModSquare(Y, P, T, Len);
            /* 计算:X = X * T % P */
            Cflint_ModMult(X, P, X, T, Len);
            /* 计算:B = B * Y % P */
            Cflint_ModMult(B, P, B, Y, Len);
            /* 计算:Q = B, R = M */
            Cflint_Copy(Q, B, Len);
            R = M;
            continue;
        }
        goto over;
    }
    Ret = true;
    
    over:
    Cflint_Free(B);
    Cflint_Free(Q);
    Cflint_Free(T);
    Cflint_Free(Y);
    Cflint_Free(Z);
    Cflint_Free(T1);
    
    return Ret;
}

/* 扩展二次剩余计算:(X(K)**2) % P == A % (P**K) */
bool Cflint_ModPkRoot2(Cflint_Type *Op_1, Cflint_Type *Op_2, Cflint_Type *Root, int64_t Exp, uint32_t Len)
{
    /* 递推公式如下:
     * (X(K) * 2 * X(K - 1)) + (X(K - 1)**2 - A) / P == 0 % P
     * ============>
     * X(K) == ((A % P - (X(K - 1)**2) % P) % P) / ((2 * P * X(K - 1)) % P)
     * X(K) == ((A % P - (X(K - 1)**2) % P) % P) / (X(K - 1) % P)
     */
    
    Cflint_Type *A  = Op_1;
    Cflint_Type *P  = Op_2;
    Cflint_Type *X  = Root;
    Cflint_Type *T1 = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T2 = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T3 = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T4 = Cflint_Alloc(Cflint_Byte * Len);
    
    bool Ret = false;
    /* 解算:X(K - 1)**2 = A % P**(K - 1) */
    if (!Cflint_ModP1Root2(A, P, X, Len))
        goto over;
    
    /* 逆向递归 */
    for (uint32_t K = 0; K < Exp; K++) {
        /* 计算:T1 = (X(K - 1)**2) % P */
        Cflint_ModSquare(T1, X, P, Len);
        /* 计算:T1 = ((A - (X(K - 1)**2)) % P) */
        Cflint_ModSub(T1, P, A, T1, Len);
        /* 计算:T2 = X(K - 1) % P */
        Cflint_Mod(T2, X, P ,Len);
        /* 计算:Quo = T1 / T2 */
        Cflint_Devide(T3, T4, T1, T2, Len);
        Cflint_Mod(X, T3, P, Len);
    }
    
    over:
    Cflint_Free(T1);
    Cflint_Free(T2);
    Cflint_Free(T3);
    Cflint_Free(T4);
    
    Ret = true;
    return Ret;
}

/* 二次剩余计算:((Root**2) % (Op_2 * Op3) == (Op_1) % (Op_2 * Op3)) */
bool Cflint_Mod1Root2(Cflint_Type *Op_1, Cflint_Type *Op_2, Cflint_Type *Op3, Cflint_Type *Root, uint32_t Len)
{
    Cflint_Type *A  = Op_1;
    Cflint_Type *P  = Op_2;
    Cflint_Type *Q  = Op3;
    Cflint_Type *X  = Root;
    Cflint_Type *T  = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *XP = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *XQ = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *U  = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2); //(Len+1)*2
    Cflint_Type *V  = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2); //(Len+1)*2
    Cflint_Type *N  = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2); //(Len+1)*2
    Cflint_Type *X0 = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2); //(Len+1)*2
    Cflint_Type *X1 = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2); //(Len+1)*2
    Cflint_Type *X2 = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2); //(Len+1)*2
    Cflint_Type *X3 = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2); //(Len+1)*2
    Cflint_Type *T0 = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2); //(Len+1)*2
    Cflint_Type *T1 = Cflint_Alloc(Cflint_Byte * Len * 4); //Len * 4
    Cflint_Type *T2 = Cflint_Alloc(Cflint_Byte * Len * 4); //Len * 4
    
    Cflint_Type  U_Flag = 0; Cflint_Type  V_Flag = 0;
    Cflint_Type X0_Flag = 0; Cflint_Type X2_Flag = 0;
    Cflint_Type X1_Flag = 0; Cflint_Type X3_Flag = 0;
    bool Ret = false;
    
    /* 查验:A == 0 */
    if (Cflint_IsZero(A, Len * 2)) {
        Cflint_ValSet(X, Len * 2, 0);
        Ret = true;
        goto over;
    }
    /* 计算:T = A % P */
    Cflint_ValSet(T0, Len * 2, 0);
    Cflint_Copy(T0, P, Len);
    Cflint_Mod(T1, A, T0, Len * 2);
    Cflint_Copy(T, T1, Len);
    /* 解算:((XP**2) % P == A % P) */
    if (!Cflint_ModP1Root2(T, P, XP, Len))
        goto over;
    /* 计算:T = A % Q */
    Cflint_ValSet(T0, Len * 2, 0);
    Cflint_Copy(T0, Q, Len);
    Cflint_Mod(T1, A, T0, Len * 2);
    Cflint_Copy(T, T1, Len);
    /* 解算:((XQ**2) % Q == A % Q) */
    if (!Cflint_ModP1Root2(T, Q, XQ, Len))
        goto over;
    /* 解算:P * U + Q * V = GCD(P, Q) */
    Cflint_ExtGCD(P, Q, N, U, &U_Flag, V, &V_Flag, Len);
    /* 计算:N = P * Q */
    N[Len * 2 + 0] = 0;
    N[Len * 2 + 1] = 0;
    Cflint_Mult(N, P, Q, Len);
    Cflint_ValSet(T2, Len * 4, 0);
    Cflint_Copy(T2, N, Len * 2);
    /* 计算:U = P * XQ * U % N */
    Cflint_Mod(U, U, N, (Len + 1) * 2);
    Cflint_Mult(T0, P, XQ, Len);
    Cflint_Mult(T1, T0, U, Len * 2);
    Cflint_Mod(T1, T1, T2, Len * 4);
    Cflint_Copy(U, T1, Len * 2);
    /* 计算:V = Q * XP * V % N */
    Cflint_Mod(V, V, N, (Len + 1) * 2);
    Cflint_Mult(T0, Q, XP, Len);
    Cflint_Mult(T1, T0, V, Len * 2);
    Cflint_Mod(T1, T1, T2, Len * 4);
    Cflint_Copy(V, T1, Len * 2);
    /* 计算4个X: */
    /* 计算:X0 = (U + V) % N, X1 = -X0 % N = N - X0 */
    Cflint_Type Ow1 = Cflint_FlagSum(X0, &X0_Flag, U, U_Flag, V, V_Flag, Len * 2);
    Cflint_FlagMod(X0, X0, N, X0_Flag, Len * 2);
    Cflint_Sub(X1, N, X0, Len * 2);
    /* 计算:U_Flag~~ */
    U_Flag = Cflint_FInv(U_Flag);
    /* 计算:X2 = (V - U) % N, X3 = -X2 % N = N - X2 */
    Cflint_Type Ow2 = Cflint_FlagSum(X2, &X2_Flag, U, U_Flag, V, V_Flag, Len * 2);
    Cflint_FlagMod(X2, X2, N, X2_Flag, Len * 2);
    Cflint_Sub(X3, N, X2, Len * 2);
    /* X = Min(X0, X1, X2, X3) */
    Cflint_Type *R1 = (Cflint_Cmp(X0, X1, Len * 2) == -1) ? X0 : X1;
    Cflint_Type *R2 = (Cflint_Cmp(X2, X3, Len * 2) == -1) ? X2 : X3;
    Cflint_Type *R0 = (Cflint_Cmp(R1, R2, Len * 2) == -1) ? R1 : R2;
    /* 返回最小的根 */
    Cflint_Copy(X, R0, Len * 2);
    Ret = true;
    
    over:
    Cflint_Free(T);
    Cflint_Free(XP);
    Cflint_Free(XQ);
    Cflint_Free(U);
    Cflint_Free(V);
    Cflint_Free(N);
    Cflint_Free(X0);
    Cflint_Free(X1);
    Cflint_Free(X2);
    Cflint_Free(X3);
    Cflint_Free(T0);
    Cflint_Free(T1);
    Cflint_Free(T2);
    
    goto over;
}

/* 线性同余方程组计算:X == Ai % Mi,当i != j时, GCD(Mi, Mj) == 1 */
int8_t Cflint_LCE(Cflint_Type **Ops, Cflint_Type *X, uint64_t Num, uint32_t Len, uint32_t LenMax)
{
    uint32_t L_MX = 0;
    uint32_t L_UV = 0;
    Cflint_Type  *M   = Cflint_Alloc(Cflint_Byte * LenMax);
    Cflint_Type  *UL  = Cflint_Alloc(Cflint_Byte * LenMax);
    Cflint_Type  *VL  = Cflint_Alloc(Cflint_Byte * LenMax);
    Cflint_Type  *TL0 = Cflint_Alloc(Cflint_Byte * LenMax);
    Cflint_Type  *TL1 = Cflint_Alloc(Cflint_Byte * LenMax);
    /* 固有开销1:定长开销 */
    Cflint_Type  *GCD = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type  *U   = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2); //(Len+1)*2
    Cflint_Type  *V   = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2); //(Len+1)*2
    Cflint_Type  *T0  = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type  *T1  = Cflint_Alloc(Cflint_Byte * Len);
    
    int8_t Ret = 0;
    Cflint_Type X_Flag = 0;
    Cflint_Type U_Flag = 0; Cflint_Type V_Flag = 0;
    
    /* 无同余方程容错 */
    if (Num == 0) {
        Ret = 0;
        goto over;
    }
    /* 第一步:载入第一个同余方程 */
    L_MX = Len;
    Cflint_Copy(X, Ops[0 * 2 + 0], Len);
    Cflint_Copy(M, Ops[0 * 2 + 1], Len);
    /* 第二步:循环解算同余方程 */
    for (uint32_t Idx = 1; Idx < Num; Idx++) {
        /* 载入其余同余方程 */
        Cflint_Type *Ai = Ops[Idx * 2 + 0];
        Cflint_Type *Mi = Ops[Idx * 2 + 1];
        /* GCD(M, Mi) = GCD(M % Mi, Mi),这里将M化为Length长度 */
        if (LenMax < L_MX * 2) {
            Ret = -2;
            goto over;
        }
        Cflint_ValSet(TL1, L_MX, 0);
        Cflint_Copy(TL1, Mi, Len);
        Cflint_Mod(TL0, M, TL1, L_MX);
        /* 解算扩展欧几里得方程 */
        Cflint_ExtGCD(TL0, Mi, GCD, U, &U_Flag, V, &V_Flag, Len);
        /* 非素检查:GCD == 1 */
        if (Cflint_SubBit(GCD, Len, 1) != 0) {
            Ret = -1;
            goto over;
        }
        if (!Cflint_IsZero(GCD, Len)) {
            Ret = -1;
            goto over;
        }
        /* 计算U = U * Ai, U的长度为理论最大值(Len + 1) * 4 */
        Cflint_Copy(T0, U, (Len + 1) * 2);
        Cflint_ValSet(T1, (Len + 1) * 2, 0);
        Cflint_Copy(T1, Ai, Len);
        Cflint_Mult(U, T0, T1, (Len + 1) * 2);
        /* 计算V = V * Mi, V的长度为理论最大值(Len + 1) * 4 */
        Cflint_Copy(T0, V, (Len + 1) * 2);
        Cflint_ValSet(T1, (Len + 1) * 2, 0);
        Cflint_Copy(T1, Mi, Len);
        Cflint_Mult(V, T0, T1, (Len + 1) * 2);
        /* 计算最大范围: */
        L_UV = (L_MX > (Len + 1) * 4) ? L_MX : (Len + 1) * 4;
        if (LenMax <= L_UV + 1) {
            Ret = -2;
            goto over;
        }
        /* 计算变长乘:UL = U * M */
        Cflint_ValSet(T0, L_UV, 0);
        Cflint_ValSet(T1, L_UV, 0);
        Cflint_Copy(T0, U, (Len + 1) * 4);
        Cflint_Copy(T0, M, L_MX);
        Cflint_Mult(UL, T0, T1, L_MX);
        /* 计算变长乘:VL = V * X */
        Cflint_ValSet(T0, L_UV, 0);
        Cflint_ValSet(T1, L_UV, 0);
        Cflint_Copy(T0, V, (Len + 1) * 4);
        Cflint_Copy(T0, X, L_MX);
        Cflint_Mult(VL, T0, T1, L_MX);
        /* 计算和:X * X_Flag = U * Ai * M * U_Flag + V * Mi * X * V_Flag */
        X[L_UV] = Cflint_FlagSum(X, &X_Flag, UL, U_Flag, VL, V_Flag, L_UV);
        /* 计算乘:M *= Mi,这里需要对其归化 */
        Cflint_ValSet(T1, L_MX, 0);
        Cflint_Copy(T1, Mi, Len);
        Cflint_Mult(T0, M, T1, L_MX);
        L_MX = (Cflint_Nums2(T0, L_MX * 2) + 1) / Cflint_Bits;
        /* 计算模:X*X_Flag %= M */
        uint32_t L_MXUV = L_MX > (L_UV + 1) ? L_MX : (L_UV + 1);
        Cflint_ValSet(T0, L_MXUV, 0);
        Cflint_ValSet(T1, L_MXUV, 0);
        Cflint_Copy(T0, X, L_UV + 1);
        Cflint_Copy(T1, M, L_MX);
        Cflint_FlagMod(X, T0, T1, X_Flag, L_MXUV);
    }
    
    over:
    Cflint_Free(M);
    Cflint_Free(UL);
    Cflint_Free(VL);
    Cflint_Free(TL0);
    Cflint_Free(TL1);
    Cflint_Free(GCD);
    Cflint_Free(U);
    Cflint_Free(V);
    Cflint_Free(T0);
    Cflint_Free(T1);
    
    return Ret;
}

