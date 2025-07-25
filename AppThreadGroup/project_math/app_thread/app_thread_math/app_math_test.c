/*实现目标:
 *    数学测试文件
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_math_test.h"

// 测试用数学函数f(x):
static double AppMathTest_Fx(double X)
{
    return (1 / (1 + X * X));
}

// 测试用数学函数f(x): 0阶
static double AppMathTest_F0x(double X)
{
    return X * X * X - 2 * X * X - 4 * X - 7;
}

// 测试用数学函数f(x): 1阶导数
static double AppMathTest_F1x(double X)
{
    return 3 * X * X - 4 * X - 4;
}

static void AppMathTest_Advance(void)
{
    APP_SYS_LOG_INFO("%lf",Cal_DI(AppMathTest_Fx, 0.0, 1.0, 10, 1));
    APP_SYS_LOG_INFO("%lf",Cal_DI(AppMathTest_Fx, 0.0, 1.0, 10, 2));
    APP_SYS_LOG_INFO("%lf",Cal_NT(AppMathTest_F0x, AppMathTest_F1x, 4, 0.0001));
}

static void AppMathTest_FiniteSet(uint32_t *Set, uint32_t Size)
{
    FiniteFullSet FullSet = {0};
    FiniteSubSet  Sub1    = {0};
    FiniteSubSet  Sub2    = {0};
    FiniteSubSet  Sub3    = {0};
    
    uint32_t SetFull[1024] = {0};
    uint8_t  BitMap1[1024 / FINITE_SUBSET_BITS + 1] = {0};
    uint8_t  BitMap2[1024 / FINITE_SUBSET_BITS + 1] = {0};
    uint8_t  BitMap3[1024 / FINITE_SUBSET_BITS + 1] = {0};
    memcpy(SetFull, Set, sizeof(uint32_t) * Size);
        
    FullSet.Set    = SetFull;
    FullSet.Length = Size;
    FullSet.Number = 10;
    
    Sub1.BitMap = BitMap1;
    Sub2.BitMap = BitMap2;
    Sub3.BitMap = BitMap3;
    Sub1.Length = Size / FINITE_SUBSET_BITS + 1;
    Sub2.Length = Size / FINITE_SUBSET_BITS + 1;
    Sub3.Length = Size / FINITE_SUBSET_BITS + 1;
    Sub1.Full = &FullSet;
    Sub2.Full = &FullSet;
    Sub3.Full = &FullSet;
    
    {
        for (uint32_t Index = 1; Index <= Size; Index++)
            if (FiniteSubSet_ElementSet(&Sub1, Index) == false)
                APP_SYS_LOG_INFO("Set Fail");
        for (uint32_t Index = 1; Index <= Size; Index++)
            if (FiniteSubSet_ElementCheck(&Sub1, Index) == false)
                APP_SYS_LOG_INFO("Check Fail");
        for (uint32_t Index = 1; Index <= Size; Index++)
            if (FiniteSubSet_ElementClear(&Sub1, Index) == false)
                APP_SYS_LOG_INFO("Clear Fail");
        for (uint32_t Index = 1; Index <= Size; Index++)
            if (FiniteSubSet_ElementCheck(&Sub1, Index) == true)
                APP_SYS_LOG_INFO("Check Fail");
    }
    
    {
        uint32_t Length = 0;
        uint32_t Temp[1024] = {0};
        
        for (uint32_t Index = 1; Index <= Size; Index++)
            if (FiniteSubSet_ElementClear(&Sub1, Index) == false)
                APP_SYS_LOG_INFO("Clear Fail");
        for (uint32_t Index = 1; Index <= Size; Index++)
            if (FiniteSubSet_ElementClear(&Sub2, Index) == false)
                APP_SYS_LOG_INFO("Clear Fail");
        
        FiniteSubSet_ElementSet(&Sub2, 1);
        FiniteSubSet_ElementSet(&Sub2, 3);
        FiniteSubSet_ElementSet(&Sub2, 5);
        FiniteSubSet_ElementSet(&Sub2, 7);
        FiniteSubSet_ElementSet(&Sub2, 9);
        
        FiniteSubSet_Not(&Sub1, &Sub2);

        APP_SYS_LOG_INFO("Sub1 = ~Sub2:");
        Length = FiniteSubSet_ElementGets(&Sub2, Temp);
        APP_SYS_LOG_INFO("Sub2:");
        for (uint32_t Index = 0; Index < Length; Index++)
            APP_SYS_LOG_INFO("%d ", Temp[Index]);
        Length = FiniteSubSet_ElementGets(&Sub1, Temp);
        APP_SYS_LOG_INFO("Sub1:");
        for (uint32_t Index = 0; Index < Length; Index++)
            APP_SYS_LOG_INFO("%d ", Temp[Index]);
    }
    
    {
        uint32_t Length = 0;
        uint32_t Temp[1024] = {0};
        
        for (uint32_t Index = 1; Index <= Size; Index++)
            if(FiniteSubSet_ElementClear(&Sub1, Index) == false)
                APP_SYS_LOG_INFO("Clear Fail");
        for (uint32_t Index = 1; Index <= Size; Index++)
            if(FiniteSubSet_ElementClear(&Sub2, Index) == false)
                APP_SYS_LOG_INFO("Clear Fail");
        for (uint32_t Index = 1; Index <= Size; Index++)
            if(FiniteSubSet_ElementClear(&Sub3, Index) == false)
                APP_SYS_LOG_INFO("Clear Fail");
        
        FiniteSubSet_ElementSet(&Sub2, 1);
        FiniteSubSet_ElementSet(&Sub2, 3);
        FiniteSubSet_ElementSet(&Sub2, 5);
        FiniteSubSet_ElementSet(&Sub2, 7);
        FiniteSubSet_ElementSet(&Sub2, 9);
        FiniteSubSet_ElementSet(&Sub3, 1);
        FiniteSubSet_ElementSet(&Sub3, 4);
        FiniteSubSet_ElementSet(&Sub3, 7);
        FiniteSubSet_ElementSet(&Sub3, 10);
        
        
        APP_SYS_LOG_INFO("Sub2:");
        Length = FiniteSubSet_ElementGets(&Sub2, Temp);
        for (uint32_t Index = 0; Index < Length; Index++)
            APP_SYS_LOG_INFO("%d ", Temp[Index]);
        APP_SYS_LOG_INFO("Sub3:");
        Length = FiniteSubSet_ElementGets(&Sub3, Temp);
        for (uint32_t Index = 0; Index < Length; Index++)
            APP_SYS_LOG_INFO("%d ", Temp[Index]);
        
        
        APP_SYS_LOG_INFO("Sub1 = Sub2 & Sub3:");
        FiniteSubSet_And(&Sub1, &Sub2, &Sub3);
        Length = FiniteSubSet_ElementGets(&Sub1, Temp);
        APP_SYS_LOG_INFO("Sub1:");
        for (uint32_t Index = 0; Index < Length; Index++)
            APP_SYS_LOG_INFO("%d ", Temp[Index]);
        
        APP_SYS_LOG_INFO("Sub1 = Sub2 | Sub3:");
        FiniteSubSet_Or(&Sub1, &Sub2, &Sub3);
        Length = FiniteSubSet_ElementGets(&Sub1, Temp);
        APP_SYS_LOG_INFO("Sub1:");
        for (uint32_t Index = 0; Index < Length; Index++)
            APP_SYS_LOG_INFO("%d ", Temp[Index]);
        
        APP_SYS_LOG_INFO("Sub1 = Sub2 - Sub3:");
        FiniteSubSet_Sub(&Sub1, &Sub2, &Sub3);
        Length = FiniteSubSet_ElementGets(&Sub1, Temp);
        APP_SYS_LOG_INFO("Sub1:");
        for (uint32_t Index = 0; Index < Length; Index++)
            APP_SYS_LOG_INFO("%d ", Temp[Index]);
    }
    
}

static void AppMathTest_Cordix(void)
{
    for (int i = 0; i < 10; i++) {//测试arctan,sin,cos,tan
        double t = (double)i * 1.1;
        double Sin  = CordixSin(t);
        double Cos  = CordixCos(t);
        double Tan  = CordixTan(t);
        double ATan = CordixArctan(Sin, Cos);
        APP_SYS_LOG_INFO("");
        APP_SYS_LOG_INFO("CordixSin(%lf) = %.10lf, sin(%lf) = %.10lf", t, Sin, t, sin(t));
        APP_SYS_LOG_INFO("CordixCos(%lf) = %.10lf, cos(%lf) = %.10lf", t, Cos, t, cos(t));
        APP_SYS_LOG_INFO("CordixTan(%lf) = %.10lf, tan(%lf) = %.10lf", t, Tan, t, tan(t));
        APP_SYS_LOG_INFO("CordixArctan(x) = %.9lf, atan(x) = %.9lf", ATan, atan2(sin(t), cos(t)));
        APP_SYS_LOG_INFO("");
    }
}

/* AES128加解密测试 */
void AppMathTest_AES128(uint8_t *Text, uint32_t Size)
{
    uint8_t Key[16]  = {
        0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
        0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x00
    };
    
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    APP_SYS_LOG_INFO_RAW("AES128:Source Code:");
    for (int32_t Index = 0; Index < Size; Index)
        APP_SYS_LOG_INFO_RAW("%c", Text[Index++]);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    AES128_Run(Text, Size, Key, 0);
    APP_SYS_LOG_INFO_RAW("AES128:Encrypt Code:");
    for (int32_t Index = 0; Index < Size; Index++)
        APP_SYS_LOG_INFO_RAW("%2x ", Text[Index]);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    AES128_Run(Text, Size, Key, 1);
    APP_SYS_LOG_INFO_RAW("AES128:Decrypt Code:");
    for (int32_t Index = 0; Index < Size; Index++)
        APP_SYS_LOG_INFO_RAW("%c", Text[Index]);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
}

/* RC4加解密测试 */
void AppMathTest_RC4(uint8_t *Text, uint32_t Size)
{
    uint8_t Key[256] = {0};
    uint8_t RC4Key[256] = {0};
    uint8_t RC4Key1[256] = {0};
    uint8_t RC4Key2[256] = {0};
    /* 初始化秘钥状态矩阵 */
    for (int32_t Index = 0; Index < 256; Index++)
        Key[Index] = Index;
    RC4Set(RC4Key, Key, 256);
    for (uint32_t Index = 0; Index < 256; Index++)
        RC4Key1[Index] = RC4Key[Index];
    for (uint32_t Index = 0; Index < 256; Index++)
        RC4Key2[Index] = RC4Key[Index];
    
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    APP_SYS_LOG_INFO_RAW("RC4:Source Code:");
    for (int32_t Index = 0; Index < Size; Index++)
        APP_SYS_LOG_INFO_RAW("%c", Text[Index]);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    APP_SYS_LOG_INFO_RAW("RC4:Encrypt Code:");
    RC4Run(RC4Key1, Text, Size); /* 加密解密使用相同的密钥 */
    for (int32_t Index = 0; Index < Size; Index++)
        APP_SYS_LOG_INFO_RAW("%2x ", Text[Index]);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    APP_SYS_LOG_INFO_RAW("RC4:Decrypt Code:");
    RC4Run(RC4Key2, Text, Size); /* 加密解密使用相同的密钥 */
    for (int32_t Index = 0; Index < Size; Index++)
        APP_SYS_LOG_INFO_RAW("%c", Text[Index]);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
}

/* SHA256信息摘要测试 */
void AppMathTest_SHA256(uint8_t *Text, uint32_t Size)
{
    uint8_t Digest[32] = {0};
    
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    APP_SYS_LOG_INFO_RAW("SHA256:Source Code:");
    for (int32_t Index = 0; Index < Size; Index++)
        APP_SYS_LOG_INFO_RAW("%c", Text[Index]);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    APP_SYS_LOG_INFO_RAW("SHA256:Digest:");
    SHA256_Run(Text, Size, Digest);
    for (int32_t Index = 0; Index < 32; Index++)
        APP_SYS_LOG_INFO_RAW("%02x", Digest[Index]);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
}

/* MD5信息摘要测试 */
void AppMathTest_MD5(uint8_t *Text, uint32_t Size)
{
    uint8_t Digest[16] = {0};
    
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    APP_SYS_LOG_INFO_RAW("MD5:Source Code:");
    for (int32_t Index = 0; Index < Size; Index++)
        APP_SYS_LOG_INFO_RAW("%c", Text[Index]);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    APP_SYS_LOG_INFO_RAW("MD5:Digest:");
    MD5_Run(Text, Size, Digest);
    for (int32_t Index = 0; Index < 16; Index++)
        APP_SYS_LOG_INFO_RAW("%02x", Digest[Index]);
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
}

void AppMathTest_Cflint(void)
{
    Test_CflintFunc();
}

void AppMathTest(void)
{
    APP_SYS_LOG_INFO("");
    APP_SYS_LOG_INFO("");
    APP_SYS_LOG_INFO("");
    
    
    
    // AppMathTest_Advance();
    
    #if 0
    uint32_t Set[10] = {1,2,3,4,5,6,7,8,9,10};
    AppMathTest_FiniteSet(Set, 10);
    #endif
    
    // AppMathTest_Cordix();
    
    #if 0
    uint8_t TestStr[] = "AppThreadGroup-ProjectMath";
    AppMathTest_AES128(TestStr, Sizeof(TestStr) - 1);
    AppMathTest_RC4(TestStr, Sizeof(TestStr) - 1);
    AppMathTest_SHA256(TestStr, Sizeof(TestStr) - 1);
    AppMathTest_MD5(TestStr, Sizeof(TestStr) - 1);
    // Sha256:  f4f84be5a8046c52e47b52aced6f8744a4a787fec6f058964d05ec7a00cdfd86
    // MD5:     073D95EEB81C057BE515CE4AC4F62A7B    32位大
    #endif
    
    AppMathTest_Cflint();
    
}
