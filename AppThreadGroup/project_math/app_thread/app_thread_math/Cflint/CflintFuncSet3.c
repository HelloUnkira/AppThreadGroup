
#include "Cflint.h"

/* 带符号与运算:Ret * (*RetFlag) = Op_1 * (Op1Flag) + Op_2 * (Op2Flag) */
/* 备注:大数运算本身是无符号语义运算,这里是为某些数学运算额外扩展 */
Cflint_Type Cflint_FlagSum(Cflint_Type *Ret,  Cflint_Flag *RetFlag,
                           Cflint_Type *Op_1, Cflint_Flag  Op1Flag,
                           Cflint_Type *Op_2, Cflint_Flag  Op2Flag,
                              uint32_t  Len)
{
    if (Op1Flag == Cflint_FPos && Op2Flag == Cflint_FPos) {
        *RetFlag = Cflint_FPos;
        return Cflint_Add(Ret, Op_1, Op_2, Len);
    }
    if (Op1Flag == Cflint_FNeg && Op2Flag == Cflint_FNeg) {
        *RetFlag = Cflint_FNeg;
        return Cflint_Add(Ret, Op_1, Op_2, Len);
    }
    if (Op1Flag == Cflint_FPos && Op2Flag == Cflint_FNeg) {
        int8_t CmpRet = Cflint_Cmp(Op_1, Op_2, Len);
        if (CmpRet != -1) {
            *RetFlag = Cflint_FPos;
            return Cflint_Sub(Ret, Op_1, Op_2, Len);
        }
        if (CmpRet == -1) {
            *RetFlag = Cflint_FNeg;
            return Cflint_Sub(Ret, Op_2, Op_1, Len);
        }
    }
    if (Op1Flag == 1 && Op2Flag == Cflint_FPos) {
        int8_t CmpRet = Cflint_Cmp(Op_2, Op_1, Len);
        if (CmpRet != -1) {
            *RetFlag = Cflint_FPos;
            return Cflint_Sub(Ret, Op_2, Op_1, Len);
        }
        if (CmpRet == -1) {
            *RetFlag = Cflint_FNeg;
            return Cflint_Sub(Ret, Op_1, Op_2, Len);
        }
    }
}

/* 带符号模运算:Ret = Op * (OpFlag) % Mod */
/* 备注:大数运算本身是无符号语义运算,这里是为某些数学运算额外扩展 */
void Cflint_FlagMod(Cflint_Type *Ret, Cflint_Type *Op,
                    Cflint_Type *Mod, Cflint_Flag  OpFlag,
                       uint32_t  Len)
{
    if (OpFlag == Cflint_FPos) {
        Cflint_Mod(Ret, Op, Mod, Len);
        return;
    }
    if (OpFlag == Cflint_FNeg) {
        Cflint_Mod(Ret, Op, Mod, Len);
        Cflint_Sub(Ret, Op, Ret, Len);
        return;
    }
}

/* 最大公因数为1检查(互素检查) */
bool Cflint_GCDCheck(Cflint_Type *A, Cflint_Type *B, uint32_t Len)
{
    bool Ret = false;
    /* 固有开销 */
    Cflint_Type *Mod = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *OpX = Cflint_Alloc(Cflint_Byte * Len); // 被除数
    Cflint_Type *OpY = Cflint_Alloc(Cflint_Byte * Len); // 除数
    /* 将A,B拷贝到缓冲区,数据计算在缓冲区内 */
    Cflint_Copy(OpX, A, Len);
    Cflint_Copy(OpY, B, Len);
    /* 当B为0时 */
    if (Cflint_IsZero(B, Len))
        goto over;
    /* 进行向下递归 */
    do {
        /* Mod = OpX % OpY */
        Cflint_Mod(Mod, OpX, OpY, Len);
        /* 更新被除数和除数 */
        Cflint_Copy(OpX, OpY, Len);
        Cflint_Copy(OpY, Mod, Len);
        /* 当模为0的时候结束,用于回溯递归 */
        if (Cflint_IsZero(OpY, Len)) {
            /* 处理这个模为0的时候的结果 */
            Cflint_AddBit(OpY, Len, 1);
            /* 检查除数是否为1 */
            Ret = Cflint_Equal(OpX, OpY, Len);
            goto over;
        }
    } while (1);
    
    over:
    Cflint_Free(Mod);
    Cflint_Free(OpX);
    Cflint_Free(OpY);
    
    return Ret;
}

/* 最大公因数 */
void Cflint_GCD(Cflint_Type *Ret, Cflint_Type *A, Cflint_Type *B, uint32_t Len)
{
    /* 固有开销 */
    Cflint_Type *Mod = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *OpX = Cflint_Alloc(Cflint_Byte * Len); // 被除数
    Cflint_Type *OpY = Cflint_Alloc(Cflint_Byte * Len); // 除数
    /* 将A,B拷贝到缓冲区,数据计算在缓冲区内 */
    Cflint_Copy(OpX, A, Len);
    Cflint_Copy(OpY, B, Len);
    /* 当B为0时 */
    if (Cflint_IsZero(B, Len))
        goto over;
    /* 进行向下递归 */
    do {
        /* Mod = OpX % OpY */
        Cflint_Mod(Mod, OpX, OpY, Len);
        /* 更新被除数和除数 */
        Cflint_Copy(OpX, OpY, Len);
        Cflint_Copy(OpY, Mod, Len);
        /* 当模为0的时候结束,用于回溯递归 */
        if (Cflint_IsZero(OpY, Len)) {
            /* 拷贝被除数 */
            Cflint_Copy(Ret, OpX, Len);
            goto over;
        }
    } while (1);
    
    over:
    Cflint_Free(Mod);
    Cflint_Free(OpX);
    Cflint_Free(OpY);
}

/* 最小公倍数 */
void Cflint_LCM(Cflint_Type *Ret, Cflint_Type *A, Cflint_Type *B, uint32_t Len)
{
    Cflint_Type *T_0 = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T_1 = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T_2 = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T_3 = Cflint_Alloc(Cflint_Byte * Len * 2);
    Cflint_Type *T_4 = Cflint_Alloc(Cflint_Byte * Len * 2);
    
    /* 1.计算A*B */
    Cflint_Mult(T_3, A, B, Len);
    Cflint_ValSet(T_4, Len * 2, 0);
    /* 2.计算GCD(A,B); */
    Cflint_GCD(T_4, A, B, Len);
    /* 3.计算A*B/GCD(A,B) */
    Cflint_Devide(T_0, T_1, T_3, T_4, Len * 2);
    /* 4.得出结果 */
    Cflint_Copy(Ret, T_0, Len * 2);
    
    Cflint_Free(T_0);
    Cflint_Free(T_1);
    Cflint_Free(T_2);
    Cflint_Free(T_3);
    Cflint_Free(T_4);
}

/* 扩展欧几里得算法 */
void Cflint_ExtGCD(Cflint_Type *A, Cflint_Type *B, Cflint_Type *GCD,
                   Cflint_Type *X, Cflint_Flag *X_Flag,
                   Cflint_Type *Y, Cflint_Flag *Y_Flag,
                   uint32_t Len)
{
    /*案例代码:
     *uint64_t GCDExtend(uint64_t A, uint64_t B, uint64_t &X, uint64_t &Y)
     *{
     *    if (B == 0) {
     *        X = 1;
     *        Y = 0;
     *        return A;
     *    } else {
     *        uint64_t Q   = A / B;
     *        uint64_t M   = A % B;
     *        uint64_t GCD = GCDExtend(B, M, X, Y);
     *        uint64_t T = X; X = Y; Y = T; Y = Y - Q * X;
     *        return GCD;
     *    }
     *}
     */
    
    /* 固有开销 */
    Cflint_Type *OpX = GCD;
    Cflint_Type *OpY = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *Quo = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *Mod = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *VV  = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2);
    Cflint_Type *TT  = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2);
    Cflint_Type *T1  = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2);
    Cflint_Type *T2  = Cflint_Alloc(Cflint_Byte * (Len + 1) * 2);
    Cflint_Flag VV_Flag = 0;
    Cflint_Flag TT_Flag = 0;
    /* 1.初始化:X=1,Y=0 */
    *X_Flag = Cflint_FPos;
    *Y_Flag = Cflint_FPos;
    Cflint_ValSet(Y, (Len + 1) * 2, 0);
    Cflint_ValSet(X, (Len + 1) * 2, 0);
    Cflint_AddBit(X, (Len + 1) * 2, 1);
    /* 2.初始化:OpX = A, OpY = B, VV = 0 */
    Cflint_ValSet(VV, (Len + 1) * 2, 0);
    Cflint_Copy(OpX, A, Len);
    Cflint_Copy(OpY,  B, Len);
    /* 3.初始化除数为0检查 */
    if (Cflint_IsZero(OpY, Len))
        goto over;
    /* 开始主循环直到除数为0 */
    while (!Cflint_IsZero(OpY, Len)) {
        /* Quo = OpX / OpY */
        /* Mod   = OpX % OpY */
        Cflint_Devide(Quo, Mod, OpX, OpY, Len);
        /* TT = X - Quo * VV */
        Cflint_Mult(T1, Quo, VV, Len);
        /* TT = X * (*X_Flag) - T1(Quo * VV) * (VV_Flag) */
        Cflint_FlagSum(TT, &TT_Flag, X, *X_Flag, T1, Cflint_FInv(VV_Flag), (Len + 1) * 2);
        /* X = VV, *X_Flag = VV_Flag */
        Cflint_Copy(X, VV, (Len + 1) * 2);
        *X_Flag = VV_Flag;
        /* VV = TT, VV_Flag = TT_Flag */
        Cflint_Copy(VV, TT, (Len + 1) * 2);
        VV_Flag = TT_Flag;
        /* OpX = OpY */
        /* OpY = Mod */
        Cflint_Copy(OpX, OpY, Len);
        Cflint_Copy(OpY, Mod, Len);
    }
    /* Y = (OpX - A * X) / B */
    Cflint_ValSet(T1, (Len + 1) * 2, 0);
    Cflint_Copy(T1, OpX, Len);
    Cflint_Mult(T2, A, X, Len);
    /* T1 = T1(OpX) - T2(A * X) * (*X_Flag) */
    Cflint_FlagSum(T1, Y_Flag, T1, 0, T2, Cflint_FInv(*X_Flag), (Len + 1) * 2);
    /* T2 = B */
    Cflint_ValSet(T2, (Len + 1) * 2, 0);
    Cflint_Copy(T2, B, Len);
    /* Y = T1 / T2 */
    Cflint_Devide(VV, TT, T1, T2, (Len + 1) * 2);
    Cflint_Copy(Y, VV, (Len + 1) * 2);
    
    over:
    Cflint_Free(OpY);
    Cflint_Free(Quo);
    Cflint_Free(Mod);
    Cflint_Free(VV);
    Cflint_Free(TT);
    Cflint_Free(T1);
    Cflint_Free(T2);
}

/* 欧几里得乘法逆: Inv = (1 / A) % N, GCD = GCD(A, N) */
void Cflint_InvGCD(Cflint_Type *A, Cflint_Type *N, Cflint_Type *Inv, Cflint_Type *GCD, uint32_t Len)
{
    /* 固有开销 */
    Cflint_Type *Quo = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *Mod = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *OpX = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *OpY = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *VV  = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *TT  = Cflint_Alloc(Cflint_Byte * Len);
    Cflint_Type *T1  = Cflint_Alloc(Cflint_Byte * Len * 2);
    Cflint_Type *T2  = Cflint_Alloc(Cflint_Byte * Len * 2);
    /* 1.初始化Inv=1 */
    Cflint_ValSet(Inv, Len, 0);
    Cflint_AddBit(Inv, Len, 1);
    /* 2.初始化OpX=A,OpY=N,VV=0 */
    Cflint_ValSet(VV, Len, 0);
    Cflint_Copy(OpX, A, Len);
    Cflint_Copy(OpY, N, Len);
    /* 3.T1===N */
    Cflint_ValSet(T1, Len * 2, 0);
    Cflint_Copy(T1, N, Len);
    /* 4.初始化除数为0检查 */
    if (Cflint_IsZero(OpY, Len))
        goto over;
    /* 开始主循环直到除数为0 */
    while (!Cflint_IsZero(OpY, Len)) {
        /* Quo = OpX / OpY */
        /* Mod = OpX % OpY */
        Cflint_Devide(Quo, Mod, OpX, OpY, Len);
        /* TT = (Inv - Quo * VV)%N */
        Cflint_Mult(T2, Quo, VV, Len);
        Cflint_Mod(T2, T2, T1, Len * 2);
        /* 蒙哥马利模减 */
        Cflint_Type Ow = Cflint_Sub(TT, Inv, T2, Len);
        if (Ow != 0) Cflint_Add(TT, TT, T1, Len);
        /* Inv = VV */
        Cflint_Copy(Inv, VV, Len);
        /* VV = TT */
        Cflint_Copy(VV, TT, Len);
        /* OpX = OpY */
        /* OpY  = Mod  */
        Cflint_Copy(OpX, OpY, Len);
        Cflint_Copy(OpY,  Mod,  Len);
    }
    /* GCD=OpX */
    Cflint_Copy(GCD, OpX, Len);
    
    over:
    Cflint_Free(Quo);
    Cflint_Free(Mod);
    Cflint_Free(OpX);
    Cflint_Free(OpY);
    Cflint_Free(VV);
    Cflint_Free(TT);
    Cflint_Free(T1);
    Cflint_Free(T2);
}
