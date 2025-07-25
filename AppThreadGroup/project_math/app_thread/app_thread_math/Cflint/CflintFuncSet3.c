
#include "Cflint.h"

/* 最大公因数为1检查(互素检查) */
bool Cflint_GCDCheck(Cflint_Type *A, Cflint_Type *B, Cflint_Type *Temp[3], uint32_t Len)
{
    /* 固有开销 */
    Cflint_Type *Module   = Temp[0];
    Cflint_Type *Dividend = Temp[1];
    Cflint_Type *Divisor  = Temp[2];
    /* 将A,B拷贝到缓冲区,数据计算在缓冲区内 */
    Cflint_Copy(Dividend, A, Len);
    Cflint_Copy(Divisor,  B, Len);
    /* 当B为0时 */
    if (Cflint_IsZero(B, Len))
        return false;
    /* 进行向下递归 */
    do {
        /* Module = Dividend % Divisor */
        Cflint_Mod(Module, Dividend, Divisor, Len);
        /* 更新被除数和除数 */
        Cflint_Copy(Dividend, Divisor, Len);
        Cflint_Copy(Divisor,  Module,  Len);
        /* 当模为0的时候结束,用于回溯递归 */
        if (Cflint_IsZero(Divisor, Len)) {
            /* 处理这个模为0的时候的结果 */
            Cflint_AddBit(Divisor, Len, 1);
            /* 检查除数是否为1 */
            return Cflint_Equal(Dividend, Divisor, Len);
        }
    } while (1);
}

/* 最大公因数 */
void Cflint_GCD(Cflint_Type *Ret, Cflint_Type *A, Cflint_Type *B,
                Cflint_Type *Temp[3],   uint32_t  Len)
{
    /* 固有开销 */
    Cflint_Type *Module   = Temp[0];
    Cflint_Type *Dividend = Temp[1];
    Cflint_Type *Divisor  = Temp[2];
    /* 将A,B拷贝到缓冲区,数据计算在缓冲区内 */
    Cflint_Copy(Dividend, A, Len);
    Cflint_Copy(Divisor,  B, Len);
    /* 当B为0时 */
    if (Cflint_IsZero(B, Len))
        return;
    /* 进行向下递归 */
    do {
        /* Module = Dividend % Divisor */
        Cflint_Mod(Module, Dividend, Divisor, Len);
        /* 更新被除数和除数 */
        Cflint_Copy(Dividend, Divisor, Len);
        Cflint_Copy(Divisor,  Module,  Len);
        /* 当模为0的时候结束,用于回溯递归 */
        if (Cflint_IsZero(Divisor, Len)) {
            /* 拷贝被除数 */
            Cflint_Copy(Ret, Dividend, Len);
            return;
        }
    } while (1);
}

/* 最小公倍数 */
void Cflint_LCM(Cflint_Type *Ret, Cflint_Type *A, Cflint_Type *B,
                Cflint_Type *Temp[5],   uint32_t  Len)
{
    /* 1.计算A*B */
    Cflint_Mult(Temp[3], A, B, Len);
    Cflint_SetValue(Temp[4], Len * 2, 0);
    /* 2.计算GCD(A,B); */
    Cflint_GCD(Temp[4], A, B, Temp, Len);
    /* 3.计算A * B / GCD(A,B) */
    Cflint_Devide(Temp[0], Temp[1], Temp[3], Temp[4], Len * 2);
    /* 4.得出结果 */
    Cflint_Copy(Ret, Temp[0], Len * 2);
}

/* 扩展欧几里得算法 */
void Cflint_GCDExtend(Cflint_Type *A,    Cflint_Type *B, Cflint_Type *GCD,
                      Cflint_Type *X,    Cflint_Type *X_Flag,
                      Cflint_Type *Y,    Cflint_Type *Y_Flag,
                      Cflint_Type *Temp[7], uint32_t  Len)
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
    Cflint_Type *Dividend = GCD;
    Cflint_Type *Divisor  = Temp[0];
    Cflint_Type *Quotient = Temp[1];
    Cflint_Type *Module   = Temp[2];
    Cflint_Type *VV = Temp[3]; Cflint_Type VV_Flag = 0;
    Cflint_Type *TT = Temp[4]; Cflint_Type TT_Flag = 0;
    Cflint_Type *T1 = Temp[5];
    Cflint_Type *T2 = Temp[6];
    /* 1.初始化:X=1,Y=0 */
    *X_Flag = 0; *Y_Flag = 0;
    Cflint_SetValue(Y, (Len + 1) * 2, 0);
    Cflint_SetValue(X, (Len + 1) * 2, 0);
    Cflint_AddBit(X, (Len + 1) * 2, 1);
    /* 2.初始化:Dividend = A, Divisor = B, VV = 0 */
    Cflint_SetValue(VV, (Len + 1) * 2, 0);
    Cflint_Copy(Dividend, A, Len);
    Cflint_Copy(Divisor,  B, Len);
    /* 3.初始化除数为0检查 */
    if (Cflint_IsZero(Divisor, Len))
        return;
    /* 开始主循环直到除数为0 */
    while (!Cflint_IsZero(Divisor, Len)) {
        /* Quotient = Dividend / Divisor */
        /* Module   = Dividend % Divisor */
        Cflint_Devide(Quotient, Module, Dividend, Divisor, Len);
        /* TT = X - Quotient * VV */
        Cflint_Mult(T1, Quotient, VV, Len);
        /* TT = X * (*X_Flag) - T1(Quotient * VV) * (VV_Flag) */
        Cflint_FlagSum(TT, &TT_Flag, X, *X_Flag,
                       T1, VV_Flag == 1 ? 0 : 1, (Len + 1) * 2);
        /* X = VV, *X_Flag = VV_Flag */
        Cflint_Copy(X, VV, (Len + 1) * 2);
        *X_Flag = VV_Flag;
        /* VV = TT, VV_Flag = TT_Flag */
        Cflint_Copy(VV, TT, (Len + 1) * 2);
        VV_Flag = TT_Flag;
        /* Dividend = Divisor */
        /* Divisor  = Module  */
        Cflint_Copy(Dividend, Divisor, Len);
        Cflint_Copy(Divisor,  Module,  Len);
    }
    /* Y = (Dividend - A * X) / B */
    Cflint_SetValue(T1, (Len + 1) * 2, 0);
    Cflint_Copy(T1, Dividend, Len);
    Cflint_Mult(T2, A, X, Len);
    /* T1 = T1(Dividend) - T2(A * X) * (*X_Flag) */
    Cflint_FlagSum(T1, Y_Flag, T1, 0,
                   T2, *X_Flag == 1 ? 0 : 1, (Len + 1) * 2);
    /* T2 = B */
    Cflint_SetValue(T2, (Len + 1) * 2, 0);
    Cflint_Copy(T2, B, Len);
    /* Y = T1 / T2 */
    Cflint_Devide(VV, TT, T1, T2, (Len + 1) * 2);
    Cflint_Copy(Y, VV, (Len + 1) * 2);
}

/* 欧几里得乘法逆 */
void Cflint_GCDInverse(Cflint_Type *A,       Cflint_Type *N,
                       Cflint_Type *Inverse, Cflint_Type *GCD,    
                       Cflint_Type *Temp[8],     uint32_t Len)
{
    /* 固有开销 */
    Cflint_Type *Quotient = Temp[0];
    Cflint_Type *Module   = Temp[1];
    Cflint_Type *Dividend = Temp[2];
    Cflint_Type *Divisor  = Temp[3];
    Cflint_Type *VV = Temp[4];
    Cflint_Type *TT = Temp[5];
    Cflint_Type *T1 = Temp[6];
    Cflint_Type *T2 = Temp[7];
    /* 1.初始化Inverse=1 */
    Cflint_SetValue(Inverse, Len, 0);
    Cflint_AddBit(Inverse, Len, 1);
    /* 2.初始化Devidend=A,Divisor=N,VV=0 */
    Cflint_SetValue(VV, Len, 0);
    Cflint_Copy(Dividend, A, Len);
    Cflint_Copy(Divisor,  N, Len);
    /* 3.T1===N */
    Cflint_SetValue(T1, Len * 2, 0);
    Cflint_Copy(T1, N, Len);
    /* 4.初始化除数为0检查 */
    if (Cflint_IsZero(Divisor, Len))
        return;
    /* 开始主循环直到除数为0 */
    while (!Cflint_IsZero(Divisor, Len)) {
        /* Quotient = Dividend / Divisor */
        /* Module   = Dividend % Divisor */
        Cflint_Devide(Quotient, Module, Dividend, Divisor, Len);
        /* TT = (Inverse - Quotient * VV)%N */
        Cflint_Mult(T2, Quotient, VV, Len);
        Cflint_Mod(T2, T2, T1, Len * 2);
        /* 蒙哥马利模减 */
        Cflint_Type Ow = Cflint_Sub(TT, Inverse, T2, Len);
        if (Ow != 0)
            Cflint_Add(TT, TT, T1, Len);
        /* Inverse = VV */
        Cflint_Copy(Inverse, VV, Len);
        /* VV = TT */
        Cflint_Copy(VV, TT, Len);
        /* Dividend = Divisor */
        /* Divisor  = Module  */
        Cflint_Copy(Dividend, Divisor, Len);
        Cflint_Copy(Divisor,  Module,  Len);
    }
    /* GCD=Dividend */
    Cflint_Copy(GCD, Dividend, Len);
}

