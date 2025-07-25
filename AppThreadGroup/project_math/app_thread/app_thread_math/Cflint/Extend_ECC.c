
#include "Cflint.h"

#define Ecc_NativeToBytes(Bytes, Natives, Len) \
        Cflint_Native4ToBytes((uint8_t *)(Bytes), (Natives), (Len))
#define Ecc_BytesToNative(Bytes, Natives, Len) \
        Cflint_Native4ToBytes((uint8_t *)(Bytes), (Natives), (Len))

/* 椭圆曲线坐标: */
typedef struct EccPointBody {
    Cflint_Type X[Ecc_Curve_Size1];
    Cflint_Type Y[Ecc_Curve_Size1];
} EccPoint;

static const    EccPoint CURVE_G                  = Ecc_Curve_G;
static const Cflint_Type CURVE_P[Ecc_Curve_Size1] = Ecc_Curve_P;
static const Cflint_Type CURVE_B[Ecc_Curve_Size1] = Ecc_Curve_B;
static const Cflint_Type CURVE_N[Ecc_Curve_Size2] = Ecc_Curve_N;

static    EccPoint *Curve_G = (   EccPoint *)(&CURVE_G);
static Cflint_Type *Curve_P = (Cflint_Type *)( CURVE_P);
static Cflint_Type *Curve_B = (Cflint_Type *)( CURVE_B);
static Cflint_Type *Curve_N = (Cflint_Type *)( CURVE_N);

static bool (*Ecc_RNG)(Cflint_Type *Data, uint32_t Size) = NULL;

void Ecc_RNG_Function(bool (*Rng)(Cflint_Type *Data, uint32_t Size))
{
    Ecc_RNG = Rng;
}

static inline bool EccPoint_IsZero(EccPoint *Point)
{
    return Cflint_IsZero(Point->X, Ecc_Curve_Size1) &&
           Cflint_IsZero(Point->Y, Ecc_Curve_Size1);
}

/*仿射坐标:(X, Y) --- Jacobian加重射影坐标:(X, Y, Z)
 *转化机制:射影坐标(X, Y, Z) ===> 仿射坐标(X / (Z**2), Y / (Z**3))
 *椭圆曲线方程(仿射坐标):
 *(Y**2) = (X**3) + A(系数) * X + B(系数)
 *椭圆曲线方程(Jacobian加重射影坐标):
 *(Y**2) = (X**3) + A(系数) * X * (Z**4) + B(系数) * (Z**6)
 *!!!下面椭圆曲线计算是
 *!!!在Jacobian加重射影坐标下运算
 */

/* R = P + P: */
static void EccPoint_Double(Cflint_Type *X, Cflint_Type *Y, Cflint_Type *Z)
{
    Cflint_Type *CP = Curve_P;
    /* T1 = X, T2 = Y, T3 = Z */
    Cflint_Type T4[Ecc_Curve_Size1] = {0};
    Cflint_Type T5[Ecc_Curve_Size1] = {0};
    /* 额外缓冲开销 */
    Cflint_Type  Temp1[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type  Temp2[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type *TT[] = {Temp1, Temp2};
    /* 特殊检查:Z == 0 */
    if (Cflint_IsZero(Z, Ecc_Curve_Size1) == true)
        return;
    /* 下面是针对secp256k1有的Jacobian加重射影优化 */
#if (Ecc_SCEP == Ecc_secp256k1)
    /* T5 = (Y**2) */
    /* T4 = (Y**2 * X) = A */
    /* T1 = (X**2) */
    /* T5 = (Y**4) */
    /* T3 = (Y * Z) = Z3 */
    Cflint_ModSquare(T5,   CP, Y,      TT, Ecc_Curve_Size1);
    Cflint_ModMult(T4, CP, X,  T5, TT, Ecc_Curve_Size1);
    Cflint_ModSquare(X,    CP, X,      TT, Ecc_Curve_Size1);
    Cflint_ModSquare(T5,   CP, T5,     TT, Ecc_Curve_Size1);
    Cflint_ModMult(Z,  CP, Y,  Z,  TT, Ecc_Curve_Size1);
    /* T2 = 2 * (X**2) */
    /* T2 = 3 * (X**2) */
    Cflint_ModAdd(Y, CP, X, X, TT, Ecc_Curve_Size1);
    Cflint_ModAdd(Y, CP, Y, X, TT, Ecc_Curve_Size1);
    /* T2 = 3 * (X**2) / 2 = B */
    Cflint_Type Ow = 0;
    if (Cflint_IsEven(Y, Ecc_Curve_Size1) == false)
        Ow = Cflint_Add(Y, Y, CP, Ecc_Curve_Size1);
    Cflint_ShiftR2(Y, Ecc_Curve_Size1, 1);
    if (Ow != 0)
        Y[Ecc_Curve_Size1 - 1] |= CFLINT_ByteH1;
    /* T1 = (B**2) */
    /* T1 = (B**2) - A */
    /* T1 = (B**2) - A * 2 = X3 */
    Cflint_ModSquare(X,      CP, Y,     TT, Ecc_Curve_Size1);
    Cflint_ModSub(X, CP, X, T4, TT, Ecc_Curve_Size1);
    Cflint_ModSub(X, CP, X, T4, TT, Ecc_Curve_Size1);
    /* T4 = A - X3 */
    /* T2 = B * (A - X3) */
    /* T2 = B * (A - X3) - (Y**4) = y3 */
    Cflint_ModSub(T4, CP, T4, X,  TT, Ecc_Curve_Size1);
    Cflint_ModMult(Y,     CP, Y,  T4, TT, Ecc_Curve_Size1);
    Cflint_ModSub(Y,  CP, Y,  T5, TT, Ecc_Curve_Size1);
#else
    /* T4 = (Y**2) */
    /* T5 = (Y**2 * X) = A */
    /* T4 = (Y**4) = (T4**2) */
    /* T2 = (Y * Z) = Z3 */
    /* T3 = (Z**2) */
    Cflint_ModSquare(T4,   CP, Y,      TT, Ecc_Curve_Size1);
    Cflint_ModMult(T5, CP, X,  T4, TT, Ecc_Curve_Size1);
    Cflint_ModSquare(T4,   CP, T4,     TT, Ecc_Curve_Size1);
    Cflint_ModMult(Y,  CP, Y,  Z,  TT, Ecc_Curve_Size1);
    Cflint_ModSquare(Z,    CP, Z,      TT, Ecc_Curve_Size1);
    /* T1 = X + Z**2 */
    /* T3 = 2 * Z**2 */
    /* T3 = X - Z**2 */
    /* T1 = X**2 - Z**4 = (X + Z**2)(X - Z**2) */
    Cflint_ModAdd(X,    CP, X, Z, TT, Ecc_Curve_Size1);
    Cflint_ModAdd(Z,    CP, Z, Z, TT, Ecc_Curve_Size1);
    Cflint_ModSub(Z, CP, X, Z, TT, Ecc_Curve_Size1);
    Cflint_ModMult(X,    CP, X, Z, TT, Ecc_Curve_Size1);
    /* T3 = 2 * (X**2 - Z**4) */
    /* T1 = 3 * (X**2 - Z**4) */
    Cflint_ModAdd(Z,    CP, X, X, TT, Ecc_Curve_Size1);
    Cflint_ModAdd(X,    CP, X, Z, TT, Ecc_Curve_Size1);
    /* T1 = 3 * (X**2 - Z**4) / 2 = B */
    Cflint_Type Ow = 0;
    if (Cflint_IsEven(X, Ecc_Curve_Size1) == false)
        Ow = Cflint_Add(X, X, CP, Ecc_Curve_Size1);
    Cflint_ShiftR2(X, Ecc_Curve_Size1, 1);
    if (Ow != 0)
        X[Ecc_Curve_Size1 - 1] |= CFLINT_ByteH1;
    /* T3 = B**2 */
    /* T3 = B**2 - A */
    /* T3 = B**2 - A * 2 = X3 */
    /* T5 = A - X3 */
    /* T1 = B * (A - X3) */
    /* T4 = B * (A - X3) - Y**4 = Y3  */
    Cflint_ModSquare(Z,       CP, X,      TT, Ecc_Curve_Size1);
    Cflint_ModSub(Z,  CP, Z,  T5, TT, Ecc_Curve_Size1);
    Cflint_ModSub(Z,  CP, Z,  T5, TT, Ecc_Curve_Size1);
    Cflint_ModSub(T5, CP, T5, Z,  TT, Ecc_Curve_Size1);
    Cflint_ModMult(X,     CP, X,  T5, TT, Ecc_Curve_Size1);
    Cflint_ModSub(T4, CP, X,  T4, TT, Ecc_Curve_Size1);
    /* Z  == X3 */
    /* Y  == Z3 */
    /* T4 == Y3 */
    Cflint_Copy(X, Z,  Ecc_Curve_Size1);
    Cflint_Copy(Z, Y,  Ecc_Curve_Size1);
    Cflint_Copy(Y, T4, Ecc_Curve_Size1);
#endif
}

/* P(X, Y) ==> (X * Z**2, Y * Z**3) */
static void EccPoint_ApplyZ(Cflint_Type *X, Cflint_Type *Y, Cflint_Type *Z)
{
    Cflint_Type *CP = Curve_P;
    /*  */
    Cflint_Type T1[Ecc_Curve_Size1] = {0};
    /* 额外缓冲开销 */
    Cflint_Type  Temp1[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type  Temp2[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type *TT[] = {Temp1, Temp2};
    /* T1 = (Z**2) */
    /* X  = (Z**2) * X */
    /* T1 = (Z**3) */
    /* X  = (Z**3) * Y */
    Cflint_ModSquare(T1,   CP, Z,     TT, Ecc_Curve_Size1);
    Cflint_ModMult(X,  CP, X, T1, TT, Ecc_Curve_Size1);
    Cflint_ModMult(T1, CP, Z, T1, TT, Ecc_Curve_Size1);
    Cflint_ModMult(Y,  CP, Y, T1, TT, Ecc_Curve_Size1);
}

/* P(X1, Y1) => 2 * P, (X2, Y2) =>P' */
static void EccPoint_DoubleInit(Cflint_Type *X1, Cflint_Type *Y1,
                                Cflint_Type *X2, Cflint_Type *Y2,
                                Cflint_Type *Zi)
{
    Cflint_Type Z[Ecc_Curve_Size1] = {0};
    if (Zi != NULL)
        Cflint_Copy(Z, Zi, Ecc_Curve_Size1);
    if (Zi == NULL) {
        Cflint_SetValue(Z, Ecc_Curve_Size1, 0);
        Cflint_AddBit(Z, Ecc_Curve_Size1, 1);
    }
    /* (X2, Y2) = (X1, Y1) */
    Cflint_Copy(X2, X1, Ecc_Curve_Size1);
    Cflint_Copy(Y2, Y1, Ecc_Curve_Size1);
    /* (X1, Y1) ==>(X1, Y1, Z) */
    EccPoint_ApplyZ(X1, Y1, Z);
    /* (X1, Y1) ==> P * 2 */
    EccPoint_Double(X1, Y1, Z);
    /* (X2, Y2) ==> P' */
    EccPoint_ApplyZ(X2, Y2, Z);
}

/* Input:  P(X1, Y1, Z), Q(X2, Y2, Z) */
/* Output: P => (P')(X1', Y1', Z3), Q => (P + Q)(X3, Y3, Z3) */
static void EccPoint_Addition1(Cflint_Type *X1, Cflint_Type *Y1,
                               Cflint_Type *X2, Cflint_Type *Y2)
{
    Cflint_Type *CP = Curve_P;
    /* T1 = X1, T2 = Y1, T3 = X2, T4 = Y2 */
    Cflint_Type T5[Ecc_Curve_Size1] = {0};
    /* 额外缓冲开销 */
    Cflint_Type  Temp1[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type  Temp2[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type *TT[] = {Temp1, Temp2};
    /* T5 = (X2 - X1) */
    /* T5 = (X2 - X1)**2 = A */
    /* T1 = (X1 * A) = B */
    /* T3 = (X2 * A) = C */
    /* T4 = (Y2 - Y1) */
    /* T5 = (Y2 - Y1)**2 = D */
    Cflint_ModSub(T5, CP, X2, X1, TT, Ecc_Curve_Size1);
    Cflint_ModSquare(T5,      CP, T5,     TT, Ecc_Curve_Size1);
    Cflint_ModMult(X1,    CP, X1, T5, TT, Ecc_Curve_Size1);
    Cflint_ModMult(X2,    CP, X2, T5, TT, Ecc_Curve_Size1);
    Cflint_ModSub(Y2, CP, Y2, Y1, TT, Ecc_Curve_Size1);
    Cflint_ModSquare(T5,      CP, Y2,     TT, Ecc_Curve_Size1);
    /* T5 = (D - B) */
    /* T5 = (D - B - C) = X3 */
    /* T3 = (C - B) */
    /* T2 = (C - B) * Y1 */
    /* T3 = (B - X3) */
    /* T4 = (B - X3) * (Y2 - Y1) */
    /* T4 = (B - X3) * (Y2 - Y1) - (C - B) * Y1 = Y3 */
    Cflint_ModSub(T5, CP, T5, X1, TT, Ecc_Curve_Size1);
    Cflint_ModSub(T5, CP, T5, X2, TT, Ecc_Curve_Size1);
    Cflint_ModSub(X2, CP, X2, X1, TT, Ecc_Curve_Size1);
    Cflint_ModMult(Y1,    CP, Y1, X2, TT, Ecc_Curve_Size1);
    Cflint_ModSub(X2, CP, X1, T5, TT, Ecc_Curve_Size1);
    Cflint_ModMult(Y2,    CP, X2, Y2, TT, Ecc_Curve_Size1);
    Cflint_ModSub(Y2, CP, Y2, Y1, TT, Ecc_Curve_Size1);
    /* T3 = T5 */
    Cflint_Copy(X2, T5, Ecc_Curve_Size1);
}

/* Input:  P(X1, Y1, Z), Q(X2, Y2, Z) */
/* Output: P => (P - Q)(X3', Y3', Z3), Q => (P + Q)(X3, Y3, Z3) */
static void EccPoint_Addition2(Cflint_Type *X1, Cflint_Type *Y1,
                               Cflint_Type *X2, Cflint_Type *Y2)
{
    Cflint_Type *CP = Curve_P;
    /* T1 = X1, T2 = Y1, T3 = X2, T4 = Y2 */
    Cflint_Type T5[Ecc_Curve_Size1] = {0};
    Cflint_Type T6[Ecc_Curve_Size1] = {0};
    Cflint_Type T7[Ecc_Curve_Size1] = {0};
    /* 额外缓冲开销 */
    Cflint_Type  Temp1[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type  Temp2[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type *TT[] = {Temp1, Temp2};
    /* T5 = (X2 - X1) */
    /* T5 = (X2 - X1)**2 = A */
    /* T1 = (X1 * A) = B */
    /* T3 = (X2 * A) = C */
    /* T5 = (Y2 + Y1) */
    /* T4 = (Y2 - Y1) */
    Cflint_ModSub(T5, CP, X2, X1, TT, Ecc_Curve_Size1);
    Cflint_ModSquare(T5,      CP, T5,     TT, Ecc_Curve_Size1);
    Cflint_ModMult(X1,    CP, X1, T5, TT, Ecc_Curve_Size1);
    Cflint_ModMult(X2,    CP, X2, T5, TT, Ecc_Curve_Size1);
    Cflint_ModAdd(T5,    CP, Y2, Y1, TT, Ecc_Curve_Size1);
    Cflint_ModSub(Y2, CP, Y2, Y1, TT, Ecc_Curve_Size1);
    /* T6 = (C - B) */
    /* T2 = (C - B) * Y1 = E */
    /* T6 = (B + C) */
    /* T3 = (Y2 - Y1)**2 = D */
    /* T3 = (D - (B + C)) = X3 */
    Cflint_ModSub(T6, CP, X2, X1, TT, Ecc_Curve_Size1);
    Cflint_ModMult(Y1,    CP, Y1, T6, TT, Ecc_Curve_Size1);
    Cflint_ModAdd(T6,    CP, X1, X2, TT, Ecc_Curve_Size1);
    Cflint_ModSquare(X2,      CP, Y2,     TT, Ecc_Curve_Size1);
    Cflint_ModSub(X2, CP, X2, T6, TT, Ecc_Curve_Size1);
    /* T7 = (B  - X3) */
    /* T4 = (Y2 - Y1)*(B - X3) */
    /* T4 = (Y2 - Y1)*(B - X3) - E = Y3 */
    Cflint_ModSub(T7, CP, X1, X2, TT, Ecc_Curve_Size1);
    Cflint_ModMult(Y2,    CP, Y2, T7, TT, Ecc_Curve_Size1);
    Cflint_ModMult(Y2,    CP, Y2, Y1, TT, Ecc_Curve_Size1);
    /* T7 = (Y2 + Y1)**2 = F */
    /* T7 = (F - (B + C)) = X3' */
    /* T6 = (X3' - B) */
    /* T6 = (Y2 + Y1)*(X3' - B) */
    /* T2 = (Y2 + Y1)*(X3' - B) - E = Y3' */
    Cflint_ModSquare(T7,      CP, T5,     TT, Ecc_Curve_Size1);
    Cflint_ModSub(T7, CP, T7, T6, TT, Ecc_Curve_Size1);
    Cflint_ModSub(T6, CP, T7, X1, TT, Ecc_Curve_Size1);
    Cflint_ModMult(T6,    CP, T6, T5, TT, Ecc_Curve_Size1);
    Cflint_ModSub(Y1, CP, T6, Y1, TT, Ecc_Curve_Size1);
    /* T1 = T7 */
    Cflint_Copy(X1, T7, Ecc_Curve_Size1);
}

/* Ret = X**3 + A * X + B */
static void EccPoint_CurveX(Cflint_Type *Ret, Cflint_Type *X)
{
    Cflint_Type *R  = Ret;
    Cflint_Type *CP = Curve_P;
    Cflint_Type *CB = Curve_B;
    /* 额外缓冲开销 */
    Cflint_Type  Temp1[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type  Temp2[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type *TT[] = {Temp1, Temp2};
#if (Ecc_CURVE == Ecc_secp256k1)
    /* R = (X**2) */
    /* R = (X**3) */
    /* R = (X**3 + B) */
    Cflint_ModSquare(R,   CP, X,     TT, Ecc_Curve_Size1);
    Cflint_ModMult(R, CP, X,  R, TT, Ecc_Curve_Size1);
    Cflint_ModAdd(R, CP, CB, R, TT, Ecc_Curve_Size1);
#else
    /* -A = 3 */
    Cflint_Type N3[Ecc_Curve_Size1] = {3};
    /* R = (X**2) */
    /* R = (X**2) - 3 */
    /* R = (X**3) - 3 * X */
    /* R = (X**3) - 3 * X + B */
    Cflint_ModSquare(R,      CP, X,     TT, Ecc_Curve_Size1);
    Cflint_ModSub(R, CP, R, N3, TT, Ecc_Curve_Size1);
    Cflint_ModMult(R,    CP, R, X,  TT, Ecc_Curve_Size1);
    Cflint_ModAdd(R,    CP, R, CB, TT, Ecc_Curve_Size1);
#endif
}

static void EccPoint_Multiply(EccPoint *Ret, Cflint_Type *Scalar,
                              EccPoint *Point,  Cflint_Type *InitZ,
                               int64_t  Bits2)
{
    Cflint_Type *CP = Curve_P;
    /* R0, R1 */
    Cflint_Type RX[2][Ecc_Curve_Size1] = {0};
    Cflint_Type RY[2][Ecc_Curve_Size1] = {0};
    Cflint_Type Z[Ecc_Curve_Size1] = {0};
    bool Bit2 = false;
    int64_t Idx = 0;
    /* 额外缓冲开销 */
    Cflint_Type  Temp1[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type  Temp2[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type  Temp3[Ecc_Curve_Size1] = {0};
    Cflint_Type  Temp4[Ecc_Curve_Size1] = {0};
    Cflint_Type *TT[] = {Temp1, Temp2, Temp3, Temp4};
    /* (Rx[1], Ry[1]) <=== (Point->X, Point->Y) */
    Cflint_Copy(RX[1], Point->X, Ecc_Curve_Size1);
    Cflint_Copy(RY[1], Point->Y, Ecc_Curve_Size1);
    /* 初始化倍乘 */
    EccPoint_DoubleInit(RX[1], RY[1], RX[0], RY[0], InitZ);
    /* 逆序叠加 */
    for (Idx = Bits2 - 2; Idx > 0; Idx--) {
        Bit2 = !Cflint_Bit2Check(Scalar, Ecc_Curve_Size1, Idx);
        EccPoint_Addition2(RX[1 - Bit2], RY[1 - Bit2], RX[Bit2], RY[Bit2]);
        EccPoint_Addition1(RX[Bit2], RY[Bit2], RX[1 - Bit2], RY[1 - Bit2]);
    }
    {
        Bit2 = !Cflint_Bit2Check(Scalar, Ecc_Curve_Size1, Idx);
        EccPoint_Addition2(RX[1 - Bit2], RY[1 - Bit2], RX[Bit2], RY[Bit2]);
    }
    /* 找到最后(1 / Z)值 */
    /* (X1 - X0) */
    /* (X1 - X0) * Yb */
    /* (X1 - X0) * Yb * PX */
    /* 1 / ((X1 - X0) * Yb * PX) */
    /* 1 / ((X1 - X0) * Yb * PX) * PY */
    /* 1 / ((X1 - X0) * Yb * PX) * PY * Xb */
    Cflint_ModSub(Z, CP, RX[1], RX[0],        TT, Ecc_Curve_Size1);
    Cflint_ModMult(Z,    CP, Z,     RY[1 - Bit2], TT, Ecc_Curve_Size1);
    Cflint_ModMult(Z,    CP, Z,     Point->X,     TT, Ecc_Curve_Size1);
    Cflint_ModInv(Z,  Z, CP,                      TT, Ecc_Curve_Size1);
    Cflint_ModMult(Z,    CP, Z,     Point->Y,     TT, Ecc_Curve_Size1);
    Cflint_ModMult(Z,    CP, Z,     RX[1 - Bit2], TT, Ecc_Curve_Size1);
    /* 结束(1 / Z)计算 */
    EccPoint_Addition1(RX[Bit2], RY[Bit2], RX[1 - Bit2], RY[1 - Bit2]);
    EccPoint_ApplyZ(RX[0], RY[0], Z);
    /* (Ret->X, Ret->Y) <=== (Rx[0], Ry[0]) */
    Cflint_Copy(Ret->X, RX[0], Ecc_Curve_Size1);
    Cflint_Copy(Ret->Y, RY[0], Ecc_Curve_Size1);
}

#define ECC_MAX_TRIES   64  /* 加密流程尝试最大迭代次数限制 */

static bool Ecc_ComputeKey(EccPoint *Public, Cflint_Type *Private)
{
    if (Cflint_IsZero(Private, Ecc_Curve_Size1) == true)
        return false;
#if (ECC_SCEP == ECC_secp160r1)
    int64_t Bits2 = Cflint_Nums2(Private, Ecc_Curve_Size1) + 1;
    EccPoint_Multiply(Public, Curve_G, Private, NULL, Bits2);
    return (EccPoint_IsZero(Public) == true) ? false : true;
#else
    Cflint_Type  Temp1[Ecc_Curve_Size1] = {0};
    Cflint_Type  Temp2[Ecc_Curve_Size1] = {0};
    Cflint_Type *TT[] = {Temp1, Temp2};
    Cflint_Type  Ow = 0;
    uint32_t     Len = Cflint_Bits * Ecc_Curve_Size1 + 1;
    
    if (Cflint_Compare(Curve_N, Private, Ecc_Curve_Size1) != 1)
        return false;
    
    Ow = 
    Cflint_Add(Temp1, Private, Curve_N, Ecc_Curve_Size1);
    Cflint_Add(Temp2, Temp1,   Curve_N, Ecc_Curve_Size1);
    Ow = (Ow == 0) ? 1 : 0;
    EccPoint_Multiply(Public, TT[Ow], Curve_G, NULL, Len);
    return (EccPoint_IsZero(Public) == true) ? false : true;
#endif
}

bool Ecc_MakeKey(Cflint_Type PublicKey[Ecc_Curve_Size1 * 2],
                 Cflint_Type PrivateKey[Ecc_Curve_Size1])
{
    EccPoint    Public = {0};
    Cflint_Type Private[Ecc_Curve_Size1] = {0};
    
    for (uint32_t tries = 0; tries < ECC_MAX_TRIES; tries++) {
        /* 生成随机私钥 */
        if (Ecc_RNG(Private, Ecc_Curve_Size1) == false)
            continue;
        /* 尝试运算点乘,生成公钥 */
        if (Ecc_ComputeKey(&Public, Private) == false)
            continue;
        Ecc_NativeToBytes(PrivateKey, Private, Ecc_Curve_Size1);
        Ecc_NativeToBytes(PublicKey + Ecc_Curve_Size1 * 0, Public.X, Ecc_Curve_Size1);
        Ecc_NativeToBytes(PublicKey + Ecc_Curve_Size1 * 1, Public.Y, Ecc_Curve_Size1);
        return true;
    }
    return false;
}

bool Ecc_ShareSecret(Cflint_Type PublicKey[Ecc_Curve_Size1 * 2],
                     Cflint_Type PrivateKey[Ecc_Curve_Size1],
                     Cflint_Type SecretKey[Ecc_Curve_Size1])
{
    EccPoint     Product;
    EccPoint     Public = {0};
    Cflint_Type  Private[Ecc_Curve_Size1] = {0};
    Cflint_Type  Random[Ecc_Curve_Size1] = {0};
    Cflint_Type *InitZ = NULL;
    /* 生成随机Z */
    for (uint32_t tries = 0; tries < ECC_MAX_TRIES; tries++) {
        if (Ecc_RNG(Random, Ecc_Curve_Size1) == false)
            continue;
        if (Cflint_IsZero(Random, Ecc_Curve_Size1) == true)
            continue;
        InitZ = Random;
        break;
    }
    Ecc_BytesToNative(PrivateKey, Private, Ecc_Curve_Size1);
    Ecc_BytesToNative(PublicKey + Ecc_Curve_Size1 * 0, Public.X, Ecc_Curve_Size1);
    Ecc_BytesToNative(PublicKey + Ecc_Curve_Size1 * 1, Public.Y, Ecc_Curve_Size1);
#if (ECC_SCEP == ECC_secp160r1)
    int64_t Bits2 = Cflint_Nums2(Private, Ecc_Curve_Size1) + 1;
    EccPoint_Multiply(&Product, Private, &Public, InitZ, Bits2);
#else
    Cflint_Type  Temp[Ecc_Curve_Size1] = {0};
    Cflint_Type *TT[] = {Private, Temp};
    Cflint_Type  Ow = 0;
    uint32_t     Len = Cflint_Bits * Ecc_Curve_Size1 + 1;
    
    Ow = 
    Cflint_Add(Private, Private, Curve_N, Ecc_Curve_Size1);
    Cflint_Add(Temp,    Private, Curve_N, Ecc_Curve_Size1);
    Ow = (Ow == 0) ? 1 : 0;
    EccPoint_Multiply(&Public, TT[Ow], Curve_G, InitZ, Len);
#endif
    Ecc_BytesToNative(SecretKey, Product.X, Ecc_Curve_Size1);
    return (EccPoint_IsZero(&Product) == true) ? false : true;
}

bool Ecc_ValidKey(Cflint_Type PublicKey[Ecc_Curve_Size1 * 2])
{
    EccPoint     Public = {0};
    Cflint_Type  Temp_1[Ecc_Curve_Size1] = {0};
    Cflint_Type  Temp_2[Ecc_Curve_Size1] = {0};
    Cflint_Type  Temp1[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type  Temp2[Ecc_Curve_Size1 * 2] = {0};
    Cflint_Type *TT[] = {Temp1, Temp2};
    
    Ecc_BytesToNative(PublicKey + Ecc_Curve_Size1 * 0, Public.X, Ecc_Curve_Size1);
    Ecc_BytesToNative(PublicKey + Ecc_Curve_Size1 * 1, Public.Y, Ecc_Curve_Size1);
    
    if (EccPoint_IsZero(&Public) == true)
        return false;
    if (Cflint_Compare(Curve_P, Public.X, Ecc_Curve_Size1) != 1)
        return false;
    if (Cflint_Compare(Curve_P, Public.Y, Ecc_Curve_Size1) != 1)
        return false;
    
    Cflint_ModSquare(Temp_1, Curve_P, Public.Y, TT, Ecc_Curve_Size1);
    EccPoint_CurveX(Temp_2, Temp_1);
    
    return Cflint_Equal(Temp_1, Temp2, Ecc_Curve_Size1);
}

