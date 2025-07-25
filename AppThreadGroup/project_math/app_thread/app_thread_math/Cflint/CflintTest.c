
#include <stdio.h>
#include "Cflint.h"

# if 0
/* Windows */
#include <windows.h>
#include <wincrypt.h>
bool Calculate_RNG(Cflint_Type *dest, uint32_t size)
{
    size *= CFLINT_BYTE;
    
    HCRYPTPROV prov;
    
    if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL,
                                                CRYPT_VERIFYCONTEXT))
        return false;

    CryptGenRandom(prov, size, (BYTE *)dest);
    CryptReleaseContext(prov, 0);
    return true;
}
#else
bool Calculate_RNG(Cflint_Type *dest, uint32_t size)
{
    static uint32_t RandomCount = 0;
    
    /* 在每~~~次后更新一次随机数种子 */
    if ((RandomCount += 4) % rand() % 10000) {
        RandomCount = 0;
        srand((unsigned)time(NULL));
    }
    
    for (uint32_t Idx = 0; Idx < size; Idx++) {
    
        uint8_t Data[4] = {rand(),rand(),rand(),rand(),};
        uint32_t Data4  = Data[0] << 24 |
                          Data[1] << 16 |
                          Data[2] <<  8 |
                          Data[3] <<  0;
        
        dest[Idx] = Data4;
    }
}
#endif

Cflint_Type Test_MakeRandom(void)
{
    static uint32_t RandomCount = 0;
    
    /* 在每~~~次后更新一次随机数种子 */
    if ((RandomCount += 4) % rand() % 10000) {
        RandomCount = 0;
        srand((unsigned)time(NULL));
    }
    
    uint8_t Data[4] = {rand(),rand(),rand(),rand(),};
    uint32_t Data4  = Data[0] << 24 |
                      Data[1] << 16 |
                      Data[2] <<  8 |
                      Data[3] <<  0;
    
    return Data4;
}

void Test_CflintFuncSet1(void)
{
    uint32_t Idx = 0;
    #define TEST_FUNCTIONSET1_LENGTH    10
    Cflint_Type Ret[TEST_FUNCTIONSET1_LENGTH * 2] = {0};
    
    /* 左移位运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET1_LENGTH; Idx++)
        Ret[Idx] = ~0;
    Cflint_ShiftL2(Ret, TEST_FUNCTIONSET1_LENGTH, 1);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_ShiftL2:::Ret:\n");
    for (Idx = 0; Idx < TEST_FUNCTIONSET1_LENGTH; Idx++)
        printf("%llx ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* 右移位运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET1_LENGTH; Idx++)
        Ret[Idx] = ~0;
    Cflint_ShiftR2(Ret, TEST_FUNCTIONSET1_LENGTH, 1);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_ShiftR2:::Ret:\n");
    for (Idx = 0; Idx < TEST_FUNCTIONSET1_LENGTH; Idx++)
        printf("%llx ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* 掩码运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET1_LENGTH * 2; Idx++)
        Ret[Idx] = (Cflint_Type)(~0);
    Cflint_Mask2(Ret, TEST_FUNCTIONSET1_LENGTH * 2, Cflint_Bits * 4 + 3);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_Mask2:::Ret:\n");
    for (Idx = 0; Idx < TEST_FUNCTIONSET1_LENGTH * 2; Idx++)
        printf("%x ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* Native互转运算 */
    /*************************************************************************/
    uint8_t  Native1[2] = {110, 111};
    uint16_t Native2[2] = {11100, 11111};
    uint32_t Native4[2] = {111110000, 111111111};
    uint64_t Native8[2] = {1111111000000, 1111111111111};
    uint8_t  Bytes1[2]  = {0};
    uint8_t  Bytes2[4]  = {0};
    uint8_t  Bytes4[8]  = {0};
    uint8_t  Bytes8[16] = {0};
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    printf("Cflint_NativeToBytes:\n");
    printf("Cflint_BytesToNative:\n");
    printf("\nNative1:%llu %llu", Native1[0], Native1[1]);
    printf("\nNative2:%llu %llu", Native2[0], Native2[1]);
    printf("\nNative4:%llu %llu", Native4[0], Native4[1]);
    printf("\nNative8:%llu %llu", Native8[0], Native8[1]);
    Cflint_Native1ToBytes(Bytes1, Native1, 2);
    Cflint_Native2ToBytes(Bytes2, Native2, 2);
    Cflint_Native4ToBytes(Bytes4, Native4, 2);
    Cflint_Native8ToBytes(Bytes8, Native8, 2);
    printf("\nBytes1:");
    for (Idx = 0; Idx < 2 * 1; Idx++)
        printf("%llu ", Bytes1[Idx]);
    printf("\nBytes2:");
    for (Idx = 0; Idx < 2 * 2; Idx++)
        printf("%llu ", Bytes2[Idx]);
    printf("\nBytes4:");
    for (Idx = 0; Idx < 2 * 4; Idx++)
        printf("%llu ", Bytes4[Idx]);
    printf("\nBytes8:");
    for (Idx = 0; Idx < 2 * 8; Idx++)
        printf("%llu ", Bytes8[Idx]);
    Cflint_BytesToNative1(Bytes1, Native1, 2);
    Cflint_BytesToNative2(Bytes2, Native2, 2);
    Cflint_BytesToNative4(Bytes4, Native4, 2);
    Cflint_BytesToNative8(Bytes8, Native8, 2);
    printf("\nNative1:%llu %llu", Native1[0], Native1[1]);
    printf("\nNative2:%llu %llu", Native2[0], Native2[1]);
    printf("\nNative4:%llu %llu", Native4[0], Native4[1]);
    printf("\nNative8:%llu %llu", Native8[0], Native8[1]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* Native与操作数互转运算 */
    /*************************************************************************/
    uint8_t  Native_1[4] = {110, 111};
    uint16_t Native_2[4] = {11100, 11111};
    uint32_t Native_4[4] = {111110000, 111111111};
    uint64_t Native_8[4] = {1111111000000, 1111111111111};
    Cflint_Type Op_1[1] = {0};
    Cflint_Type Op_2[2] = {0};
    Cflint_Type Op_4[4] = {0};
    Cflint_Type Op_8[8] = {0};
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    printf("Cflint_NativeToOp:\n");
    printf("Cflint_OpToNative:\n");
    Cflint_Copy(Op_1, (Cflint_Type *)Native_1, 1);
    Cflint_Copy(Op_2, (Cflint_Type *)Native_2, 2);
    Cflint_Copy(Op_4, (Cflint_Type *)Native_4, 4);
    Cflint_Copy(Op_8, (Cflint_Type *)Native_8, 8);
    Cflint_NativeToOp(Op_1, 1, 1);
    Cflint_NativeToOp(Op_2, 2, 2);
    Cflint_NativeToOp(Op_4, 4, 4);
    Cflint_NativeToOp(Op_8, 8, 8);
    printf("\nNative_1:");
    for (Idx = 0; Idx < 4; Idx++)
        printf("%llu ", Native_1[Idx]);
    printf("\nNative_2:");
    for (Idx = 0; Idx < 4; Idx++)
        printf("%llu ", Native_2[Idx]);
    printf("\nNative_4:");
    for (Idx = 0; Idx < 4; Idx++)
        printf("%llu ", Native_4[Idx]);
    printf("\nNative_8:");
    for (Idx = 0; Idx < 4; Idx++)
        printf("%llu ", Native_8[Idx]);
    printf("\nOp_1:");
    for (Idx = 0; Idx < 1; Idx++)
        printf("%llu ", Op_1[Idx]);
    printf("\nOp_2:");
    for (Idx = 0; Idx < 2; Idx++)
        printf("%llu ", Op_2[Idx]);
    printf("\nOp_4:");
    for (Idx = 0; Idx < 4; Idx++)
        printf("%llu ", Op_4[Idx]);
    printf("\nOp_8:");
    for (Idx = 0; Idx < 8; Idx++)
        printf("%llu ", Op_8[Idx]);
    Cflint_OpToNative(Op_1, 1, 1);
    Cflint_OpToNative(Op_2, 2, 2);
    Cflint_OpToNative(Op_4, 4, 4);
    Cflint_OpToNative(Op_8, 8, 8);
    Cflint_Copy((Cflint_Type *)Native_1, Op_1, 1);
    Cflint_Copy((Cflint_Type *)Native_2, Op_2, 2);
    Cflint_Copy((Cflint_Type *)Native_4, Op_4, 4);
    Cflint_Copy((Cflint_Type *)Native_8, Op_8, 8);
    printf("\nNative_1:");
    for (Idx = 0; Idx < 4; Idx++)
        printf("%llu ", Native_1[Idx]);
    printf("\nNative_2:");
    for (Idx = 0; Idx < 4; Idx++)
        printf("%llu ", Native_2[Idx]);
    printf("\nNative_4:");
    for (Idx = 0; Idx < 4; Idx++)
        printf("%llu ", Native_4[Idx]);
    printf("\nNative_8:");
    for (Idx = 0; Idx < 4; Idx++)
        printf("%llu ", Native_8[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    
    /*************************************************************************/
    uint8_t Len = 20;
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    printf("Cflint_NumToHex:\n");
    printf("Cflint_HexToNum:\n");
    {
        uint8_t  Temp0[20] = {0};
        uint8_t  Temp1[20] = {0};
        uint8_t  Temp2[20] = {0};
        uint8_t *Temp[3] = {Temp0, Temp1, Temp2};
        uint8_t  Hex[20] = {0};
        uint8_t  Num[20] = "1910019100";
        Cflint_NumToHex(Hex, Num, Temp, Len);
        printf("\nHex:");
        for (Idx = 0; Idx < 20; Idx++)
            printf("%x ", Hex[Idx]);
        
        Cflint_HexToNum(Hex, Num, Temp, Len);
        printf("\nNum:");
        for (Idx = 0; Idx < 20; Idx++)
            printf("%c", Num[Idx]);
    }
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
}

void Test_CflintFuncSet2(void)
{
    uint32_t Idx = 0;
    #define TEST_FUNCTIONSET2_LENGTH    10
    Cflint_Type Ret  [TEST_FUNCTIONSET2_LENGTH * 2] = {0};
    Cflint_Type Op_1[TEST_FUNCTIONSET2_LENGTH] = {0};
    Cflint_Type Op_2[TEST_FUNCTIONSET2_LENGTH] = {0};
    Cflint_Type Ow = 0;
    
    /* 和运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET2_LENGTH; Idx++) {
        Op_1[Idx] = (~0) - Idx;
        Op_2[Idx] = (~0) - Idx;
    }
    Ow =
    Cflint_Add(Ret, Op_1, Op_2, TEST_FUNCTIONSET2_LENGTH);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_Add:::Ow:%d, Ret:\n", Ow);
    for (Idx = 0; Idx < TEST_FUNCTIONSET2_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* 差运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET2_LENGTH; Idx++) {
        Op_1[Idx] = TEST_FUNCTIONSET2_LENGTH - Idx;
        Op_2[Idx] = Idx;
    }
    Ow =
    Cflint_Sub(Ret, Op_1, Op_2, TEST_FUNCTIONSET2_LENGTH);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_Sub:::Ow:%d, Ret:\n", Ow);
    for (Idx = 0; Idx < TEST_FUNCTIONSET2_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/

    /* 乘运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET2_LENGTH; Idx++) {
        Op_1[Idx] = Idx;
        Op_2[Idx] = Idx;
    }
    Cflint_Mult(Ret, Op_1, Op_2, TEST_FUNCTIONSET2_LENGTH);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_Mult:::Ret:\n");
    for (Idx = 0; Idx < TEST_FUNCTIONSET2_LENGTH * 2; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/

    /* 方运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET2_LENGTH; Idx++) {
        Op_1[Idx] = Idx;
        Op_2[Idx] = Idx;
    }
    Cflint_Square(Ret, Op_1, TEST_FUNCTIONSET2_LENGTH);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_Square:::Ret:\n");
    for (Idx = 0; Idx < TEST_FUNCTIONSET2_LENGTH * 2; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
}

void Test_CflintFuncSet3(void)
{
    uint32_t Idx = 0;
    #define TEST_FUNCTIONSET3_LENGTH    10
    Cflint_Type Ret  [TEST_FUNCTIONSET3_LENGTH * 2] = {0};
    Cflint_Type Quotient[TEST_FUNCTIONSET3_LENGTH * 2] = {0};
    Cflint_Type Module  [TEST_FUNCTIONSET3_LENGTH * 2] = {0};
    Cflint_Type Op_1[TEST_FUNCTIONSET3_LENGTH * 2] = {0};
    Cflint_Type Op_2[TEST_FUNCTIONSET3_LENGTH * 2] = {0};
    Cflint_Type Ow = 0;
    
    /* 除运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET3_LENGTH * 2; Idx++) {
        if (Idx < TEST_FUNCTIONSET3_LENGTH) {
            Op_1[Idx] = Idx;
            Op_2[Idx] = TEST_FUNCTIONSET3_LENGTH - Idx;
            Module[Idx]   = TEST_FUNCTIONSET3_LENGTH - Idx - 1;
        } else {
            Op_1[Idx] = 0;
            Op_2[Idx] = 0;
            Module  [Idx] = 0;
        }
    }
    /* 先用乘法运算计算一下值 */
    Cflint_Mult(Ret, Op_1, Op_2, TEST_FUNCTIONSET3_LENGTH);
    /* 为其添加一个余数(这里不让其溢出) */
    Cflint_Add(Ret, Ret, Module, TEST_FUNCTIONSET3_LENGTH * 2);
    /* 再计算除数结果 */
    Cflint_Devide(Quotient, Module, Ret, Op_1,
                  TEST_FUNCTIONSET3_LENGTH * 2);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_Devide:::");
    printf("\n---------------------------------------------------------------");
    printf("\nRet:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET3_LENGTH * 2; Idx++)
        printf("%llx ", Ret[Idx]);
    printf("\nOp1:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET3_LENGTH; Idx++)
        printf("%llx ", Op_1[Idx]);
    printf("\nOp2:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET3_LENGTH; Idx++)
        printf("%llx ", Op_2[Idx]);
    printf("\nQuotient:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET3_LENGTH * 2; Idx++)
        printf("%llx ", Quotient[Idx]);
    printf("\nModule:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET3_LENGTH * 2; Idx++)
        printf("%llx ", Module[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* 模运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET3_LENGTH * 2; Idx++) {
        if (Idx < TEST_FUNCTIONSET3_LENGTH) {
            Op_1[Idx] = Idx;
            Op_2[Idx] = TEST_FUNCTIONSET3_LENGTH - Idx;
            Module  [Idx] = TEST_FUNCTIONSET3_LENGTH - Idx - 1;
        } else {
            Op_1[Idx] = 0;
            Op_2[Idx] = 0;
            Module  [Idx] = 0;
        }
    }
    /* 先用乘法运算计算一下值 */
    Cflint_Mult(Ret, Op_1, Op_2, TEST_FUNCTIONSET3_LENGTH);
    /* 为其添加一个余数(这里不让其溢出) */
    Cflint_Add(Ret, Ret, Module, TEST_FUNCTIONSET3_LENGTH * 2);
    /* 再计算模结果 */
    Cflint_Mod(Module, Ret, Op_1, TEST_FUNCTIONSET3_LENGTH * 2);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_Mod:::");
    printf("\n---------------------------------------------------------------");
    printf("\nRet:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET3_LENGTH * 2; Idx++)
        printf("%llx ", Ret[Idx]);
    printf("\nOp1:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET3_LENGTH; Idx++)
        printf("%llx ", Op_1[Idx]);
    printf("\nOp2:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET3_LENGTH; Idx++)
        printf("%llx ", Op_2[Idx]);
    printf("\nModule:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET3_LENGTH * 2; Idx++)
        printf("%llx ", Module[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
}

void Test_CflintFuncSet4(void)
{
    uint32_t Idx = 0;
    #define TEST_FUNCTIONSET4_LENGTH    3
    Cflint_Type Ret  [TEST_FUNCTIONSET4_LENGTH] = {0};
    Cflint_Type Op [TEST_FUNCTIONSET4_LENGTH] = {0};
    Cflint_Type Exponent[TEST_FUNCTIONSET4_LENGTH] = {0};
    Cflint_Type Module  [TEST_FUNCTIONSET4_LENGTH * 2] = {0};
    /* 模幂运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET4_LENGTH; Idx++) {
        Op[Idx] = TEST_FUNCTIONSET4_LENGTH - Idx;
        Module[Idx]  = TEST_FUNCTIONSET4_LENGTH - Idx;
    }
    Module[0]   = 0; 
    Exponent[0] = 5;
    /* 模幂运算 */
    {
        Cflint_Type  Temp1 [TEST_FUNCTIONSET4_LENGTH * 2] = {0};
        Cflint_Type  Temp2 [TEST_FUNCTIONSET4_LENGTH * 2] = {0};
        Cflint_Type  Temp3 [TEST_FUNCTIONSET4_LENGTH * 2] = {0};
        Cflint_Type  Temp4 [TEST_FUNCTIONSET4_LENGTH * 2] = {0};
        Cflint_Type *Temp[4] = {Temp1, Temp2, Temp3, Temp4};
        /* 模幂运算 */
        Cflint_ModExp(Ret, Module, Op, Exponent,
                              Temp, TEST_FUNCTIONSET4_LENGTH);
    }
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_ModExp:::");
    printf("\n---------------------------------------------------------------");
    printf("\nOp:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET4_LENGTH; Idx++)
        printf("%u ", Op[Idx]);
    printf("\nModule:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET4_LENGTH; Idx++)
        printf("%u ", Module[Idx]);
    printf("\nRet:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET4_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* 模逆运算 */
    /*************************************************************************/
    Cflint_SetValue(Module, TEST_FUNCTIONSET4_LENGTH * 2, 0);
    for (Idx = 0; Idx < TEST_FUNCTIONSET4_LENGTH; Idx++) {
        Op[Idx] = TEST_FUNCTIONSET4_LENGTH - Idx;
        Module[Idx]  = TEST_FUNCTIONSET4_LENGTH - Idx;
    }
    Module[0] = 1;
    /* 模逆运算 */
    {
        Cflint_Type  Temp1 [TEST_FUNCTIONSET4_LENGTH] = {0};
        Cflint_Type  Temp2 [TEST_FUNCTIONSET4_LENGTH] = {0};
        Cflint_Type  Temp3 [TEST_FUNCTIONSET4_LENGTH] = {0};
        Cflint_Type  Temp4 [TEST_FUNCTIONSET4_LENGTH] = {0};
        Cflint_Type *Temp[4] = {Temp1, Temp2, Temp3, Temp4};
        Cflint_ModInv(Ret, Op, Module,
                             Temp, TEST_FUNCTIONSET4_LENGTH);
    }
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_ModInv:::");
    printf("\n---------------------------------------------------------------");
    printf("\nOp:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET4_LENGTH; Idx++)
        printf("%u ", Op[Idx]);
    printf("\nModule:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET4_LENGTH; Idx++)
        printf("%u ", Module[Idx]);
    printf("\nRet:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET4_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /* 注:Module和Op如果不互素,模逆不存在 */
    printf("(Ret * Op) % Module:::Ret:\n");
    Cflint_Type Temp1 [TEST_FUNCTIONSET4_LENGTH * 2] = {0};
    Cflint_Mult(Temp1, Ret, Op, TEST_FUNCTIONSET4_LENGTH);
    Cflint_Mod(Temp1, Temp1, Module, TEST_FUNCTIONSET4_LENGTH * 2);
    Cflint_Copy(Ret, Temp1, TEST_FUNCTIONSET4_LENGTH);
    for (Idx = 0; Idx < TEST_FUNCTIONSET4_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
}

void Test_CflintFuncSet5(void)
{
    uint32_t Idx = 0;
    #define TEST_FUNCTIONSET5_LENGTH     5
    Cflint_Type Ret[TEST_FUNCTIONSET5_LENGTH]  = {0};
    Cflint_Type Ret1[TEST_FUNCTIONSET5_LENGTH] = {0};
    Cflint_Type A[TEST_FUNCTIONSET5_LENGTH] = {0};
    Cflint_Type B[TEST_FUNCTIONSET5_LENGTH] = {0};
    /* 因为运算中涉及到乘和差运算,所以这是理论最大空间 */
    Cflint_Type X[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
    Cflint_Type Y[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
    /* 开辟出来的用于解算结果正确性的空间,本身无用 */
    Cflint_Type Temp5[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
    Cflint_Type Temp6[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
    Cflint_Type Temp7[(TEST_FUNCTIONSET5_LENGTH + 1) * 4] = {0};
    Cflint_Type Temp8[(TEST_FUNCTIONSET5_LENGTH + 1) * 4] = {0};
    Cflint_Type X_Flag = 0, Y_Flag = 0;
    bool Check = 0;
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++) {
        A[Idx] = TEST_FUNCTIONSET5_LENGTH - Idx;
        B[Idx] = TEST_FUNCTIONSET5_LENGTH - Idx;
    }
    B[0] = 1;
    /*************************************************************************/
    
    /*************************************************************************/
    /* 素数判别检查 */
    {
        Cflint_Type Ret[TEST_FUNCTIONSET5_LENGTH] = {1, 0, 1};
        Cflint_Type  Temp1[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp2[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp3[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type *Temp[3] = {Temp1, Temp2, Temp3};
        Check = Cflint_IsPrime1(Ret, Temp, TEST_FUNCTIONSET5_LENGTH);
    }
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_IsPrime1:::%d", Check);
    printf("\n---------------------------------------------------------------");
    /*************************************************************************/
    /* 素数判别检查 */
    {
        Cflint_Type Ret[TEST_FUNCTIONSET5_LENGTH] = {1, 1, 1};
        Cflint_Type  Temp1[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp2[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp3[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type *Temp[3] = {Temp1, Temp2, Temp3};
        Check = Cflint_IsPrime1(Ret, Temp, TEST_FUNCTIONSET5_LENGTH);
    }
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_IsPrime1:::%d", Check);
    printf("\n---------------------------------------------------------------");
    /*************************************************************************/
    
    /* 欧几里得互素检查运算 */
    /*************************************************************************/
    {
        Cflint_Type  Temp1[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp2[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp3[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type *Temp[3] = {Temp1, Temp2, Temp3};
        Check = Cflint_GCDCheck(A, B, Temp, TEST_FUNCTIONSET5_LENGTH);
    }
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_GCDCheck:::");
    printf("\n---------------------------------------------------------------");
    printf("\nA:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++)
        printf("%u ", A[Idx]);
    printf("\nB:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++)
        printf("%u ", B[Idx]);
    printf("\nRet:%u", Check);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* 扩展欧几里得运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++) {
        A[Idx] = TEST_FUNCTIONSET5_LENGTH - Idx;
        B[Idx] = TEST_FUNCTIONSET5_LENGTH - Idx;
    }
    B[0] = 1;
    /*************************************************************************/
    /* 扩展欧几里得运算 */
    {
        Cflint_Type  Temp1[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp2[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp3[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp4[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp5[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp6[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp7[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
        Cflint_Type *Temp[7] = {Temp1, Temp2, Temp3, Temp4,
                                Temp5, Temp6, Temp7};
        Cflint_GCDExtend(A, B, Ret, X, &X_Flag, Y, &Y_Flag,
                         Temp, TEST_FUNCTIONSET5_LENGTH);
    }
    /*************************************************************************/
    Cflint_SetValue(Temp5, (TEST_FUNCTIONSET5_LENGTH + 1) * 2, 0);
    Cflint_SetValue(Temp6, (TEST_FUNCTIONSET5_LENGTH + 1) * 2, 0);
    Cflint_SetValue(Temp7, (TEST_FUNCTIONSET5_LENGTH + 1) * 4, 0);
    Cflint_SetValue(Temp8, (TEST_FUNCTIONSET5_LENGTH + 1) * 4, 0);
    Cflint_Copy(Temp5, A, TEST_FUNCTIONSET5_LENGTH);
    Cflint_Copy(Temp6, B, TEST_FUNCTIONSET5_LENGTH);
    Cflint_Mult(Temp7, Temp5, X, (TEST_FUNCTIONSET5_LENGTH + 1) * 2);
    Cflint_Mult(Temp8, Temp6, Y, (TEST_FUNCTIONSET5_LENGTH + 1) * 2);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_GCDExtend:::");
    printf("\n---------------------------------------------------------------");
    printf("\nA:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++)
        printf("%u ", A[Idx]);
    printf("\nB:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++)
        printf("%u ", B[Idx]);
    printf("\nRet:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\nX_Flag:%d, X:", X_Flag);
    for (Idx = 0; Idx < (TEST_FUNCTIONSET5_LENGTH + 1) * 2; Idx++)
        printf("%u ", X[Idx]);
    printf("\nY_Flag:%d, Y:", Y_Flag);
    for (Idx = 0; Idx < (TEST_FUNCTIONSET5_LENGTH + 1) * 2; Idx++)
        printf("%u ", Y[Idx]);
    printf("\nA*X:");
    for (Idx = 0; Idx < (TEST_FUNCTIONSET5_LENGTH + 1) * 2; Idx++)
        printf("%u ", Temp7[Idx]);
    printf("\nB*Y:");
    for (Idx = 0; Idx < (TEST_FUNCTIONSET5_LENGTH + 1) * 2; Idx++)
        printf("%u ", Temp8[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* 欧几里得乘法逆 */
    /*************************************************************************/
    {
        Cflint_Type  Temp1[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp2[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp3[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp4[TEST_FUNCTIONSET5_LENGTH] = {0};
        Cflint_Type  Temp5[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp6[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp7[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp8[(TEST_FUNCTIONSET5_LENGTH + 1) * 2] = {0};
        Cflint_Type *Temp[8] = {Temp1, Temp2, Temp3, Temp4,
                                Temp5, Temp6, Temp7, Temp8};
        Cflint_GCDInverse(A, B, Ret, Ret1,
                          Temp, TEST_FUNCTIONSET5_LENGTH);
    }
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_GCDInverse:::");
    printf("\n---------------------------------------------------------------");
    printf("\nA:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++)
        printf("%u ", A[Idx]);
    printf("\nB:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++)
        printf("%u ", B[Idx]);
    printf("\nGCD:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++)
        printf("%u ", Ret1[Idx]);
    printf("\nInverse:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /* 注:Module和Op如果不互素,模逆不存在 */
    printf("(Ret * A) % B:::Ret:\n");
    Cflint_Mult(Temp5, Ret, A, TEST_FUNCTIONSET5_LENGTH);
    Cflint_SetValue(Temp6, TEST_FUNCTIONSET5_LENGTH * 2, 0);
    Cflint_Copy(Temp6, B, TEST_FUNCTIONSET5_LENGTH);
    Cflint_Mod(Temp5, Temp5, Temp6, TEST_FUNCTIONSET5_LENGTH * 2);
    Cflint_Copy(Ret, Temp5, TEST_FUNCTIONSET5_LENGTH);
    for (Idx = 0; Idx < TEST_FUNCTIONSET5_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
}

void Test_CflintFuncSet6(void)
{
    uint32_t Idx = 0;
    #define TEST_FUNCTIONSET6_LENGTH     5
    Cflint_Type Ret[TEST_FUNCTIONSET6_LENGTH] = {0};
    Cflint_Type Op[TEST_FUNCTIONSET6_LENGTH] = {0};
    int8_t Ret1 = 0;
    
    /* 2次方根整数部运算 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET6_LENGTH; Idx++)
        Op[Idx] = TEST_FUNCTIONSET6_LENGTH - Idx;
    /*************************************************************************/
    /* 2次方根整数部运算 */
    {
        Cflint_Type  Temp1[TEST_FUNCTIONSET6_LENGTH] = {0};
        Cflint_Type  Temp2[TEST_FUNCTIONSET6_LENGTH] = {0};
        Cflint_Type  Temp3[TEST_FUNCTIONSET6_LENGTH] = {0};
        Cflint_Type  Temp4[TEST_FUNCTIONSET6_LENGTH] = {0};
        Cflint_Type *Temp[4] = {Temp1, Temp2, Temp3, Temp4};
        Cflint_Root2Integer(Ret, Op, Temp, TEST_FUNCTIONSET6_LENGTH);
    }
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_Root2Integer:::");
    printf("\n---------------------------------------------------------------");
    printf("\nRet:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET6_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\nOp:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET6_LENGTH; Idx++)
        printf("%u ", Op[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* 2次方数判别检查 */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET6_LENGTH; Idx++)
        Op[Idx] = TEST_FUNCTIONSET6_LENGTH - Idx;
    Op[0] = 1;
    Op[1] = 2;
    /*************************************************************************/
    {
        Cflint_Type  Temp1[TEST_FUNCTIONSET6_LENGTH * 2] = {0};
        Cflint_Type  Temp2[TEST_FUNCTIONSET6_LENGTH] = {0};
        Cflint_Type  Temp3[TEST_FUNCTIONSET6_LENGTH] = {0};
        Cflint_Type  Temp4[TEST_FUNCTIONSET6_LENGTH] = {0};
        Cflint_Type *Temp[4] = {Temp1, Temp2, Temp3, Temp4};
        Ret1 = Cflint_Root2Check(Ret, Op, Temp,
                                    TEST_FUNCTIONSET6_LENGTH);
    }
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_Root2Check:::");
    printf("\n---------------------------------------------------------------");
    printf("\nRet:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET6_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\nOp:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET6_LENGTH; Idx++)
        printf("%u ", Op[Idx]);
    printf("\nRet:%d", Ret1);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* 符号Jacobi(Op_1/Op_2)计算 */
    /*************************************************************************/
    Ret[0]  = 2;
    Op[0] = 7;
    /*************************************************************************/
    /* 符号Jacobi(Op_1/Op_2)计算 */
    {
        Cflint_Type  Temp1[TEST_FUNCTIONSET6_LENGTH] = {0};
        Cflint_Type  Temp2[TEST_FUNCTIONSET6_LENGTH] = {0};
        Cflint_Type  Temp3[TEST_FUNCTIONSET6_LENGTH] = {0};
        Cflint_Type *Temp[3] = {Temp1, Temp2, Temp3};
        Ret1 = Cflint_JacobiFlag(Ret, Op, Temp, 1);
    }
    printf("\n-------------------------------------------------------------\n");
    printf("Jacobi(%d/%d):Ret:%d",Ret[0], Op[0], Ret1);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
}

void Test_CflintFuncSet7(void)
{
    uint32_t Idx = 0;
    #define TEST_FUNCTIONSET7_LENGTH     3
    Cflint_Type Ret[TEST_FUNCTIONSET7_LENGTH] = {0};
    Cflint_Type Module[TEST_FUNCTIONSET7_LENGTH] = {0};
    Cflint_Type Op[TEST_FUNCTIONSET7_LENGTH] = {0};
    /* 开辟出来的用于解算结果正确性的空间,本身无用 */
    Cflint_Type Temp1[(TEST_FUNCTIONSET7_LENGTH + 1) * 2] = {0};
    Cflint_Type Temp2[(TEST_FUNCTIONSET7_LENGTH + 1) * 2] = {0};
    int8_t Ret1 = 0;
    
    /* 二次剩余计算:((Ret**2) % Op_2 == Op_1 % Op_2) */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
        Module[Idx] = 1;
    for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
        Op[Idx] = 0;
    Module[1] = 0;
    /*************************************************************************/
    while (1) {
        Cflint_AddBit(Op, TEST_FUNCTIONSET7_LENGTH, 1);
        /*********************************************************************/
        if (Cflint_Compare(Op, Module, TEST_FUNCTIONSET7_LENGTH) == 1)
            break;
        /*********************************************************************/
        /* 二次剩余计算:((Ret**2) % Op_2 == Op_1 % Op_2) */
        {
            Cflint_Type  Temp1[TEST_FUNCTIONSET7_LENGTH] = {0};
            Cflint_Type  Temp2[TEST_FUNCTIONSET7_LENGTH] = {0};
            Cflint_Type  Temp3[TEST_FUNCTIONSET7_LENGTH] = {0};
            Cflint_Type  Temp4[TEST_FUNCTIONSET7_LENGTH] = {0};
            Cflint_Type  Temp5[TEST_FUNCTIONSET7_LENGTH] = {0};
            Cflint_Type  Temp6[TEST_FUNCTIONSET7_LENGTH * 2] = {0};
            Cflint_Type  Temp7[TEST_FUNCTIONSET7_LENGTH * 2] = {0};
            Cflint_Type  Temp8[TEST_FUNCTIONSET7_LENGTH * 2] = {0};
            Cflint_Type  Temp9[TEST_FUNCTIONSET7_LENGTH * 2] = {0};
            Cflint_Type  Temp10[TEST_FUNCTIONSET7_LENGTH * 2] = {0};
            Cflint_Type *Temp[10] = {Temp1, Temp2, Temp3, Temp4, Temp5,
                                     Temp6, Temp7, Temp8, Temp9, Temp10};
            /* 二次剩余计算:((Ret**2) % Op_2 == Op_1 % Op_2) */
            Ret1 = Cflint_ModuloP1Root2(Op, Module, Ret,
                                           Temp, TEST_FUNCTIONSET7_LENGTH);
            if (Ret1 == 0)
                continue;
        }
        printf("\n---------------------------------------------------------\n");
        printf("Cflint_ModuloP1Root2:::");
        printf("\n-----------------------------------------------------------");
        printf("\nModule:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
            printf("%u ", Module[Idx]);
        printf("\nRet:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
            printf("%u ", Ret[Idx]);
        printf("\nOp:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
            printf("%u ", Op[Idx]);
        /*********************************************************************/
        Cflint_Square(Temp1, Ret, TEST_FUNCTIONSET7_LENGTH);
        Cflint_SetValue(Temp2, TEST_FUNCTIONSET7_LENGTH * 2, 0);
        Cflint_Copy(Temp2, Module, TEST_FUNCTIONSET7_LENGTH);
        Cflint_Mod(Temp1, Temp1, Temp2, TEST_FUNCTIONSET7_LENGTH * 2);
        printf("\n(Ret**2)(Mod)Module:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
            printf("%u ", Temp1[Idx]);
        /*********************************************************************/
        Cflint_Mod(Temp1, Op, Module, TEST_FUNCTIONSET7_LENGTH);
        printf("\n(Op)(Mod)Module:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
            printf("%u ", Temp1[Idx]);
        printf("\n---------------------------------------------------------\n");
        /*********************************************************************/
    }
    /*************************************************************************/
    
    /* 二次剩余计算:((Ret**2) % Op_2 == Op_1 % Op_2) */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
        Module[Idx] = 1;
    for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
        Op[Idx] = 0;
    // Module[0] = 7;
    /*************************************************************************/
    while (1) {
        Cflint_AddBit(Op, TEST_FUNCTIONSET7_LENGTH, 1);
        /*********************************************************************/
        if (Cflint_Compare(Op, Module, TEST_FUNCTIONSET7_LENGTH) == 1)
            break;
        /*********************************************************************/
        /* 二次剩余计算:((Ret**2) % Op_2 == Op_1 % Op_2) */
        {
            Cflint_Type  Temp1[TEST_FUNCTIONSET7_LENGTH] = {0};
            Cflint_Type  Temp2[TEST_FUNCTIONSET7_LENGTH] = {0};
            Cflint_Type  Temp3[TEST_FUNCTIONSET7_LENGTH] = {0};
            Cflint_Type  Temp4[TEST_FUNCTIONSET7_LENGTH] = {0};
            Cflint_Type  Temp5[TEST_FUNCTIONSET7_LENGTH] = {0};
            Cflint_Type  Temp6[TEST_FUNCTIONSET7_LENGTH * 2] = {0};
            Cflint_Type  Temp7[TEST_FUNCTIONSET7_LENGTH * 2] = {0};
            Cflint_Type  Temp8[TEST_FUNCTIONSET7_LENGTH * 2] = {0};
            Cflint_Type  Temp9[TEST_FUNCTIONSET7_LENGTH * 2] = {0};
            Cflint_Type  Temp10[TEST_FUNCTIONSET7_LENGTH * 2] = {0};
            Cflint_Type *Temp[10] = {Temp1, Temp2, Temp3, Temp4, Temp5,
                                     Temp6, Temp7, Temp8, Temp9, Temp10};
            /* 二次剩余计算:((Ret**2) % Op_2 == Op_1 % Op_2) */
            Ret1 = Cflint_ModuloP1Root2(Op, Module, Ret,
                                           Temp, TEST_FUNCTIONSET7_LENGTH);
            if (Ret1 == 0)
                continue;
        }
        printf("\n---------------------------------------------------------\n");
        printf("Cflint_ModuloP1Root2:::");
        printf("\n-----------------------------------------------------------");
        printf("\nModule:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
            printf("%u ", Module[Idx]);
        printf("\nRet:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
            printf("%u ", Ret[Idx]);
        printf("\nOp:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
            printf("%u ", Op[Idx]);
        /*********************************************************************/
        Cflint_Square(Temp1, Ret, TEST_FUNCTIONSET7_LENGTH);
        Cflint_SetValue(Temp2, TEST_FUNCTIONSET7_LENGTH * 2, 0);
        Cflint_Copy(Temp2, Module, TEST_FUNCTIONSET7_LENGTH);
        Cflint_Mod(Temp1, Temp1, Temp2, TEST_FUNCTIONSET7_LENGTH * 2);
        printf("\n(Ret**2)(Mod)Module:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
            printf("%u ", Temp1[Idx]);
        /*********************************************************************/
        Cflint_Mod(Temp1, Op, Module, TEST_FUNCTIONSET7_LENGTH);
        printf("\n(Op)(Mod)Module:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET7_LENGTH; Idx++)
            printf("%u ", Temp1[Idx]);
        printf("\n---------------------------------------------------------\n");
        /*********************************************************************/
    }
    /*************************************************************************/
}

void Test_CflintFuncSet8(void)
{
    uint32_t Idx = 0;
    #define TEST_FUNCTIONSET8_LENGTH     2
    Cflint_Type Op[TEST_FUNCTIONSET8_LENGTH * 2] = {0};
    Cflint_Type Ret[TEST_FUNCTIONSET8_LENGTH * 2] = {0};
    Cflint_Type Module1[TEST_FUNCTIONSET8_LENGTH] = {0};
    Cflint_Type Module2[TEST_FUNCTIONSET8_LENGTH] = {0};
    /* 开辟出来的用于解算结果正确性的空间,本身无用 */
    Cflint_Type Temp0[TEST_FUNCTIONSET8_LENGTH * 4] = {0};
    Cflint_Type Temp1[TEST_FUNCTIONSET8_LENGTH * 4] = {0};
    Cflint_Type Temp2[TEST_FUNCTIONSET8_LENGTH * 4] = {0};
    int8_t Ret1 = 0;
    
    /* 意外备注:当Length为3时,使用Set7中的俩个数找不到一个满足要求的值 */
    
    /* 二次剩余计算:((Ret**2) % Op_2 == Op_1 % Op_2) */
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH; Idx++)
        Module1[Idx] = 1;
    //Module1[1] = 0;
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH; Idx++)
        Module2[Idx] = 1;
    Module2[0] = 7;
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
        Op[Idx] = 0;
    /*************************************************************************/
    Cflint_Mult(Temp0, Module1, Module2, TEST_FUNCTIONSET8_LENGTH);
    /*************************************************************************/
    {
        Cflint_Type  Temp1[TEST_FUNCTIONSET8_LENGTH] = {0};
        Cflint_Type  Temp2[TEST_FUNCTIONSET8_LENGTH] = {0};
        Cflint_Type  Temp3[TEST_FUNCTIONSET8_LENGTH] = {0};
        Cflint_Type *Temp[3] = {Temp1, Temp2, Temp3};
        bool Ret = Cflint_GCDCheck(Module1, Module2, Temp,
                                      TEST_FUNCTIONSET8_LENGTH);
        printf("\n---------------------------------------------------------\n");
        printf("Cflint_GCDCheck:::%d", Ret);
        printf("\n-----------------------------------------------------------");
    }
    /*************************************************************************/
#if 0
    /*************************************************************************/
    {   /* 这是通过下面的低效查找查到的一个同时可以通过俩个二次剩余的数 */
        Op[0] = 1;Op[1] = 1;Op[2] = 1;Op[3] = 1;
    }
    /*************************************************************************/
    /* 二次剩余计算:((Ret**2) % Op_2 == Op_1 % Op_2) */
    {
        Cflint_Type  Temp1[TEST_FUNCTIONSET8_LENGTH] = {0};
        Cflint_Type  Temp2[TEST_FUNCTIONSET8_LENGTH] = {0};
        Cflint_Type  Temp3[TEST_FUNCTIONSET8_LENGTH] = {0};
        Cflint_Type  Temp4[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp5[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp6[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp7[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp8[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp9[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp10[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp11[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
        Cflint_Type  Temp12[TEST_FUNCTIONSET8_LENGTH * 4] = {0};
        Cflint_Type  Temp13[TEST_FUNCTIONSET8_LENGTH * 4] = {0};
        Cflint_Type *Temp[13] = {Temp1,  Temp2,  Temp3, Temp4, Temp5,
                                 Temp6,  Temp7,  Temp8, Temp9, Temp10,
                                 Temp11, Temp12, Temp13};
        /* 二次剩余计算:((Ret**2) % (Op_2 * Op3) ==  */
        /*               (Op_1)  % (Op_2 * Op3))    */
        Ret1 = Cflint_Modulo1Root2(Op, Module1, Module2, Ret,
                                      Temp, TEST_FUNCTIONSET8_LENGTH);
    }
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_Modulo1Root2:::");
    printf("\n---------------------------------------------------------------");
    printf("\nRet:%d", Ret1);
    printf("\nModule1:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH; Idx++)
        printf("%u ", Module1[Idx]);
    printf("\nModule2:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH; Idx++)
        printf("%u ", Module2[Idx]);
    printf("\nModule1*Module2:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
        printf("%u ", Temp0[Idx]);
    printf("\nRet:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\nOp:");
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
        printf("%u ", Op[Idx]);
    /*************************************************************************/
    Cflint_SetValue(Temp2, TEST_FUNCTIONSET8_LENGTH * 2, 0);
    Cflint_Copy(Temp2, Module1, TEST_FUNCTIONSET8_LENGTH);
    Cflint_Mod(Temp1, Op, Temp2, TEST_FUNCTIONSET8_LENGTH * 2);
    printf("\n(Op)(Mod)(Module1):");
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
        printf("%u ", Temp1[Idx]);
    /*************************************************************************/
    Cflint_SetValue(Temp2, TEST_FUNCTIONSET8_LENGTH * 2, 0);
    Cflint_Copy(Temp2, Module2, TEST_FUNCTIONSET8_LENGTH);
    Cflint_Mod(Temp1, Op, Temp2, TEST_FUNCTIONSET8_LENGTH * 2);
    printf("\n(Op)(Mod)(Module2):");
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
        printf("%u ", Temp1[Idx]);
    /*************************************************************************/
    Cflint_Square(Temp1, Ret, TEST_FUNCTIONSET8_LENGTH * 2);
    Cflint_SetValue(Temp2, TEST_FUNCTIONSET8_LENGTH * 4, 0);
    Cflint_Copy(Temp2, Temp0, TEST_FUNCTIONSET8_LENGTH * 2);
    Cflint_Mod(Temp1, Temp1, Temp2, TEST_FUNCTIONSET8_LENGTH * 2);
    printf("\n(Ret**2)(Mod)(Module1 * Module2):");
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
        printf("%u ", Temp1[Idx]);
    /*************************************************************************/
    Cflint_SetValue(Temp2, TEST_FUNCTIONSET8_LENGTH * 4, 0);
    Cflint_Copy(Temp2, Op, TEST_FUNCTIONSET8_LENGTH);
    Cflint_Mod(Temp1, Temp2, Temp0, TEST_FUNCTIONSET8_LENGTH * 2);
    printf("\n(Op)(Mod)(Module1 * Module2):");
    for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
        printf("%u ", Temp1[Idx]);
    printf("\n---------------------------------------------------------\n");
    /*************************************************************************/
#else
    /*************************************************************************/
    while (1) {
        Cflint_AddBit(Op, TEST_FUNCTIONSET8_LENGTH * 2, 1);
        /*********************************************************************/
        if (Cflint_Compare(Op, Temp0, TEST_FUNCTIONSET8_LENGTH * 2) == 1)
            break;
        /*********************************************************************/
        /* 二次剩余计算:((Ret**2) % Op_2 == Op_1 % Op_2) */
        {
            Cflint_Type  Temp1[TEST_FUNCTIONSET8_LENGTH] = {0};
            Cflint_Type  Temp2[TEST_FUNCTIONSET8_LENGTH] = {0};
            Cflint_Type  Temp3[TEST_FUNCTIONSET8_LENGTH] = {0};
            Cflint_Type  Temp4[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
            Cflint_Type  Temp5[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
            Cflint_Type  Temp6[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
            Cflint_Type  Temp7[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
            Cflint_Type  Temp8[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
            Cflint_Type  Temp9[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
            Cflint_Type  Temp10[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
            Cflint_Type  Temp11[(TEST_FUNCTIONSET8_LENGTH + 1) * 2] = {0};
            Cflint_Type  Temp12[TEST_FUNCTIONSET8_LENGTH * 4] = {0};
            Cflint_Type  Temp13[TEST_FUNCTIONSET8_LENGTH * 4] = {0};
            Cflint_Type *Temp[13] = {Temp1,  Temp2,  Temp3, Temp4, Temp5,
                                     Temp6,  Temp7,  Temp8, Temp9, Temp10,
                                     Temp11, Temp12, Temp13};
            /* 二次剩余计算:((Ret**2) % (Op_2 * Op3) ==  */
            /*               (Op_1)  % (Op_2 * Op3))    */
            Ret1 = Cflint_Modulo1Root2(Op, Module1, Module2, Ret,
                                          Temp, TEST_FUNCTIONSET8_LENGTH);
            {
                static uint32_t count = 0;
                /* 事实上很难找 */
                if (count++ > 1000000) {
                    count = 0;
                printf("\nOp:");
                for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
                    printf("%u ", Op[Idx]);
                }
            }
            if (Ret1 == 0)
                continue;
            if (Cflint_IsZero(Ret, TEST_FUNCTIONSET8_LENGTH) == true)
                continue;
        }
        printf("\n---------------------------------------------------------\n");
        printf("Cflint_Modulo1Root2:::");
        printf("\n-----------------------------------------------------------");
        printf("\nModule1:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH; Idx++)
            printf("%u ", Module1[Idx]);
        printf("\nModule2:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH; Idx++)
            printf("%u ", Module2[Idx]);
        printf("\nModule1*Module2:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
            printf("%u ", Temp0[Idx]);
        printf("\nRet:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
            printf("%u ", Ret[Idx]);
        printf("\nOp:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
            printf("%u ", Op[Idx]);
        /*********************************************************************/
        Cflint_SetValue(Temp2, TEST_FUNCTIONSET8_LENGTH * 2, 0);
        Cflint_Copy(Temp2, Module1, TEST_FUNCTIONSET8_LENGTH);
        Cflint_Mod(Temp1, Op, Temp2, TEST_FUNCTIONSET8_LENGTH * 2);
        printf("\n(Op)(Mod)(Module1):");
        for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
            printf("%u ", Temp1[Idx]);
        /*********************************************************************/
        Cflint_SetValue(Temp2, TEST_FUNCTIONSET8_LENGTH * 2, 0);
        Cflint_Copy(Temp2, Module2, TEST_FUNCTIONSET8_LENGTH);
        Cflint_Mod(Temp1, Op, Temp2, TEST_FUNCTIONSET8_LENGTH * 2);
        printf("\n(Op)(Mod)(Module2):");
        for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
            printf("%u ", Temp1[Idx]);
        /*********************************************************************/
        Cflint_Square(Temp1, Ret, TEST_FUNCTIONSET8_LENGTH * 2);
        Cflint_SetValue(Temp2, TEST_FUNCTIONSET8_LENGTH * 4, 0);
        Cflint_Copy(Temp2, Temp0, TEST_FUNCTIONSET8_LENGTH * 2);
        Cflint_Mod(Temp1, Temp1, Temp2, TEST_FUNCTIONSET8_LENGTH * 2);
        printf("\n(Ret**2)(Mod)(Module1 * Module2):");
        for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
            printf("%u ", Temp1[Idx]);
        /*********************************************************************/
        Cflint_SetValue(Temp2, TEST_FUNCTIONSET8_LENGTH * 4, 0);
        Cflint_Copy(Temp2, Op, TEST_FUNCTIONSET8_LENGTH * 2);
        Cflint_Mod(Temp1, Temp2, Temp0, TEST_FUNCTIONSET8_LENGTH * 2);
        printf("\n(Op)(Mod)(Module1 * Module2):");
        for (Idx = 0; Idx < TEST_FUNCTIONSET8_LENGTH * 2; Idx++)
            printf("%u ", Temp1[Idx]);
        printf("\n---------------------------------------------------------\n");
        /*********************************************************************/
    }
#endif
}

void Test_CflintFuncSet9(void)
{
    uint32_t Idx = 0;
    #define TEST_FUNCTIONSET9_LENGTH     (1024 / 8)
    Cflint_Type Ret[TEST_FUNCTIONSET9_LENGTH] = {0};
    Cflint_Type Min[TEST_FUNCTIONSET9_LENGTH] = {0};
    Cflint_Type Max[TEST_FUNCTIONSET9_LENGTH] = {0};
    Min[TEST_FUNCTIONSET9_LENGTH - 1] = 0x60000000;
    Max[TEST_FUNCTIONSET9_LENGTH - 1] = 0xa0000000;
    /*************************************************************************/
    {
        Cflint_Type Ret1[3] = {1, 0, 1};
        Cflint_Type Ret2[3] = {7, 1, 1};
        Cflint_Type  Temp1[3] = {0};
        Cflint_Type  Temp2[3] = {0};
        Cflint_Type  Temp3[3] = {0};
        Cflint_Type *Temp[3] = {Temp1, Temp2, Temp3};
        bool Check1 = Cflint_IsPrime1(Ret1, Temp, 3);
        bool Check2 = Cflint_IsPrime1(Ret2, Temp, 3);
        printf("\n---------------------------------------------------------\n");
        printf("Cflint_IsPrime1:::%d\n", Check1);
        printf("Cflint_IsPrime1:::%d\n", Check2);
        printf("\n-----------------------------------------------------------");
    }
    /*************************************************************************/
    /* 素数查找 */
    {
        //!当前素数查找算法效率低下
        //!问题1:随机数不够随机,不适用素数生成
        //!问题2:内部基础算法速度过慢
        Cflint_Type  Temp1[TEST_FUNCTIONSET9_LENGTH] = {0};
        Cflint_Type  Temp2[TEST_FUNCTIONSET9_LENGTH] = {0};
        Cflint_Type  Temp3[TEST_FUNCTIONSET9_LENGTH] = {0};
        Cflint_Type  Temp4[TEST_FUNCTIONSET9_LENGTH * 2] = {0};
        Cflint_Type  Temp5[TEST_FUNCTIONSET9_LENGTH * 2] = {0};
        Cflint_Type  Temp6[TEST_FUNCTIONSET9_LENGTH * 2] = {0};
        Cflint_Type  Temp7[TEST_FUNCTIONSET9_LENGTH * 2] = {0};
        Cflint_Type *Temp[7] = {Temp1, Temp2, Temp3, Temp4,
                                Temp5, Temp6, Temp7,};
        uint32_t Check = 0;
        Check = Cflint_RandomPrime(Ret, Temp, Min, Max,
                                   TEST_FUNCTIONSET9_LENGTH,
                                   Test_MakeRandom, ~0);
        printf("\n---------------------------------------------------------\n");
        printf("Cflint_RandomPrime:::%u\n", Check);
        printf("\n-----------------------------------------------------------");
        printf("\nRet:");
        for (Idx = 0; Idx < TEST_FUNCTIONSET9_LENGTH; Idx++)
            printf("%llx ", Ret[Idx]);
        printf("\n-----------------------------------------------------------");
        
    }
}

/* 测试蒙哥马利模(未完成,测试未通过) */
void Test_Mentgomery(void)
{
#if 0
    uint32_t Idx = 0;
    #define TEST_MENTGOMERY_LENGTH    3
    Cflint_Type Ret [TEST_MENTGOMERY_LENGTH] = {0};
    Cflint_Type X [TEST_MENTGOMERY_LENGTH] = {0};
    Cflint_Type Y [TEST_MENTGOMERY_LENGTH] = {0};
    Cflint_Type N [TEST_MENTGOMERY_LENGTH * 2] = {0};
    Cflint_Type R [TEST_MENTGOMERY_LENGTH * 2] = {0};
    Cflint_Type RR_N [TEST_MENTGOMERY_LENGTH] = {0};
    Cflint_Type RP [(TEST_MENTGOMERY_LENGTH + 1) * 2] = {0};
    Cflint_Type NP [(TEST_MENTGOMERY_LENGTH + 1) * 2] = {0};
    Cflint_Type RP_Flag = 0, NP_Flag = 0;
    Cflint_Type Temp1 [(TEST_MENTGOMERY_LENGTH + 1) * 4] = {0};
    Cflint_Type Temp2 [(TEST_MENTGOMERY_LENGTH + 1) * 4] = {0};
    Cflint_Type Temp3 [(TEST_MENTGOMERY_LENGTH + 1) * 4] = {0};
    Cflint_Type Temp4 [(TEST_MENTGOMERY_LENGTH + 1) * 4] = {0};
    Cflint_Type Temp5 [(TEST_MENTGOMERY_LENGTH + 1) * 4] = {0};
    /*************************************************************************/
    for (Idx = 0; Idx < TEST_MENTGOMERY_LENGTH; Idx++) {
        X[Idx] = TEST_MENTGOMERY_LENGTH - Idx;
        Y[Idx] = TEST_MENTGOMERY_LENGTH - Idx;
        N[Idx] = TEST_MENTGOMERY_LENGTH - Idx;
    }
    Y[0] = 1;
    N[0] = 1;
    /*************************************************************************/
    
    /* 预计算及其验证 */
    /*************************************************************************/
    /* 特例:除数为0检查 */
    if (Cflint_IsZero(N, TEST_MENTGOMERY_LENGTH) == true) {
        Cflint_SetValue(Ret, TEST_MENTGOMERY_LENGTH, 0);
        return;
    }
    /* 计算R_Index */
    int64_t R_Index = Cflint_Nums2(N, TEST_MENTGOMERY_LENGTH) + 1;
    /* 一个数的0次幂为1 */
    if (R_Index == 0) {
        Cflint_SetValue(Ret, TEST_MENTGOMERY_LENGTH, 0);
        Cflint_AddBit(Ret, TEST_MENTGOMERY_LENGTH, 1);
        return;
    }
    /* 生成R */
    Cflint_SetValue(R, TEST_MENTGOMERY_LENGTH * 2, 0);
    Cflint_AddBit(R, TEST_MENTGOMERY_LENGTH * 2, 1);
    Cflint_ShiftL2(R, TEST_MENTGOMERY_LENGTH * 2, R_Index);
    /* 检查GCD(R,N) == 1 */
    if (Cflint_GCDCheck(R, N, Temp1, Temp2, Temp3,
                        TEST_MENTGOMERY_LENGTH * 2) == false) {
        Cflint_SetValue(Ret, TEST_MENTGOMERY_LENGTH, 0);
        return;
    }
    /* 4.计算RR' + NN' = GCD(R,N) == 1 */
    // Cflint_ModuleInverse(RP, R, N, Temp1, Temp2, Temp3, Temp4,
    //                      TEST_MENTGOMERY_LENGTH);
    // Cflint_ModuleInverse(NP, N, R, Temp1, Temp2, Temp3, Temp4,
    //                      TEST_MENTGOMERY_LENGTH);
    Cflint_ExtendGCD(R, N, RP, NP, Ret, &RP_Flag, &NP_Flag,
                     Temp1, Temp2, Temp3, Temp4, Temp5,
                     TEST_MENTGOMERY_LENGTH);
    /* 5.计算RR%N = ((R%N)*(R%N))%N = ((R-N)*(R-N))%N */
    Cflint_Sub(Temp1, R, N, TEST_MENTGOMERY_LENGTH * 2);
    Cflint_Square(Temp2, Temp1, TEST_MENTGOMERY_LENGTH);
    Cflint_Mod(RR_N, Temp2, N, TEST_MENTGOMERY_LENGTH * 2);
    /* 验算预先量 */
    /*************************************************************************/
    Cflint_SetValue(Temp1, (TEST_MENTGOMERY_LENGTH + 1) * 2, 0);
    Cflint_Copy(Temp1, R, TEST_MENTGOMERY_LENGTH);
    Cflint_Mult(Temp2, Temp1, RP, (TEST_MENTGOMERY_LENGTH + 1) * 2);
    Cflint_Copy(Temp1, N, TEST_MENTGOMERY_LENGTH);
    Cflint_Mult(Temp3, Temp1, NP, (TEST_MENTGOMERY_LENGTH + 1) * 2);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_M_Pretractment:::Check:");
    printf("\n---------------------------------------------------------------");
    printf("\nR:");
    for (Idx = 0; Idx < TEST_MENTGOMERY_LENGTH; Idx++)
        printf("%u ", R[Idx]);
    printf("\nN:");
    for (Idx = 0; Idx < TEST_MENTGOMERY_LENGTH; Idx++)
        printf("%u ", N[Idx]);
    printf("\nRet:");
    for (Idx = 0; Idx < TEST_MENTGOMERY_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\nRP_Flag:%d, RP:", RP_Flag);
    for (Idx = 0; Idx < (TEST_MENTGOMERY_LENGTH + 1) * 2; Idx++)
        printf("%u ", RP[Idx]);
    printf("\nNP_Flag:%d, NP:", NP_Flag);
    for (Idx = 0; Idx < (TEST_MENTGOMERY_LENGTH + 1) * 2; Idx++)
        printf("%u ", NP[Idx]);
    printf("\nR*RP:");
    for (Idx = 0; Idx < (TEST_MENTGOMERY_LENGTH + 1) * 4; Idx++)
        printf("%u ", Temp2[Idx]);
    printf("\nN*NP:");
    for (Idx = 0; Idx < (TEST_MENTGOMERY_LENGTH + 1) * 4; Idx++)
        printf("%u ", Temp3[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* 蒙哥马利约减X%N = REDC((X*R)%N) */
    /*************************************************************************/
    Cflint_Mult(Temp1, X, R, TEST_MENTGOMERY_LENGTH);
    Cflint_SetValue(Temp2, TEST_MENTGOMERY_LENGTH * 2, 0);
    Cflint_Mod(Temp1, Temp1, N, TEST_MENTGOMERY_LENGTH * 2);
    Cflint_M_ModuloReduction(Ret, TEST_MENTGOMERY_LENGTH, R_Index,
                             Temp1, N, NP, Temp2, Temp3);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_M_ModuloReduction:::");
    printf("\n---------------------------------------------------------------");
    printf("\nX:");
    for (Idx = 0; Idx < TEST_MENTGOMERY_LENGTH; Idx++)
        printf("%u ", X[Idx]);
    printf("\nN:");
    for (Idx = 0; Idx < TEST_MENTGOMERY_LENGTH; Idx++)
        printf("%u ", N[Idx]);
    printf("\nRet:");
    for (Idx = 0; Idx < TEST_MENTGOMERY_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    
    /* 蒙哥马利模乘 */
    /*************************************************************************/
    Cflint_M_ModuloMultiply(Ret, X, Y, N, NP, RR_N,
                            Temp1, Temp2, Temp3, Temp4,
                            R_Index, TEST_MENTGOMERY_LENGTH);
    printf("\n-------------------------------------------------------------\n");
    printf("Cflint_M_ModuloMultiply:::");
    printf("\n---------------------------------------------------------------");
    printf("\nX:");
    for (Idx = 0; Idx < TEST_MENTGOMERY_LENGTH; Idx++)
        printf("%u ", X[Idx]);
    printf("\nY:");
    for (Idx = 0; Idx < TEST_MENTGOMERY_LENGTH; Idx++)
        printf("%u ", Y[Idx]);
    printf("\nN:");
    for (Idx = 0; Idx < TEST_MENTGOMERY_LENGTH; Idx++)
        printf("%u ", N[Idx]);
    printf("\nRet:");
    for (Idx = 0; Idx < TEST_MENTGOMERY_LENGTH; Idx++)
        printf("%u ", Ret[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
#endif
}

/* 测试椭圆曲线(未完成,测试未通过) */
void Test_ECC(void)
{
    Cflint_Type RNGTest[Ecc_Curve_Size1];
    Cflint_Type PublicKey[Ecc_Curve_Size1 * 2];
    Cflint_Type PrivateKey[Ecc_Curve_Size1];
    Cflint_Type SecretKey[Ecc_Curve_Size1];
    bool Ret1 = false;
    bool Ret2 = false;
    bool Ret3 = false;
    
    Ecc_RNG_Function(Calculate_RNG);
    Calculate_RNG(RNGTest, Ecc_Curve_Size1);
    
    /*************************************************************************/
    printf("\n-------------------------------------------------------------\n");
    printf("Calculate_RNG:::");
    for (uint32_t Idx = 0; Idx < Ecc_Curve_Size1; Idx++)
        printf("%llx ", RNGTest[Idx]);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
    printf("\n-------------------------------------------------------------\n");
    Ret1 = Ecc_MakeKey(PublicKey, PrivateKey);
    Ret2 = Ecc_ShareSecret(PublicKey, PrivateKey, SecretKey);
    Ret3 = Ecc_ValidKey(PublicKey);
    /* 公钥校验不通过,有待审查 */
    printf("Ecc_MakeKey:%d\n",      Ret1);
    printf("Ecc_ShareSecret:%d\n",  Ret2);
    printf("Ecc_ValidKey:%d\n",     Ret3);
    printf("\n-------------------------------------------------------------\n");
    /*************************************************************************/
}

void Test_CflintFunc(void)
{
    Test_CflintFuncSet1();
    Test_CflintFuncSet2();
    Test_CflintFuncSet3();
    Test_CflintFuncSet4();
    Test_CflintFuncSet5();
    Test_CflintFuncSet6();
    Test_CflintFuncSet7();
    Test_CflintFuncSet8();
    // 未完成,测试不通过
    // Test_CflintFunctionSet9();
    // 未完成,测试不通过
    // Test_Mentgomery();
    // 未完成,测试不通过
    // Test_ECC();
}
