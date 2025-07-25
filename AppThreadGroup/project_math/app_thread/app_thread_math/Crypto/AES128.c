
#include "AES128.h"

/* 辅助功能1:密钥转矩阵 */
static inline void AES128_ChangeData0(uint8_t Matrix[4][4], uint8_t Key[16])
{   //秘钥输入状态矩阵
    for (uint8_t I = 0; I <= 3; I++)
        for (uint8_t J = 0; J <= 3; J++)
            Matrix[J][I] = Key[I * 4 + J];
}

/* 辅助功能2:矩阵转密钥 */
static inline void AES128_ChangeData1(uint8_t Matrix[4][4], uint8_t Key[16])
{   //状态矩阵输入秘钥
    for (uint8_t I = 0; I <= 3; I++)
        for (uint8_t J = 0; J <= 3; J++)
            Key[I * 4 + J] = Matrix[J][I];
}

#define AES128_KEY_TO_MATRIX(Matrix, Key)   AES128_ChangeData0(Matrix, Key)
#define AES128_MATRIX_TO_KEY(Matrix, Key)   AES128_ChangeData1(Matrix, Key)

/* AES128_S1 -->正S盒 */
static const uint8_t AES128_S1[16][16] = {
    {0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5
    ,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76},
    {0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0
    ,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0},
    {0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc
    ,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15},
    {0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a
    ,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75},
    {0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0
    ,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84},
    {0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b
    ,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf},
    {0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85
    ,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8},
    {0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5
    ,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2},
    {0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17
    ,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73},
    {0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88
    ,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb},
    {0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c
    ,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79},
    {0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9
    ,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08},
    {0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6
    ,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a},
    {0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e
    ,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e},
    {0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94
    ,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf},
    {0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68
    ,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16}
};

/* AES128_S2 -->逆S盒 */
static const uint8_t AES128_S2[16][16] = {
    {0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38
    ,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb},
    {0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87
    ,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb},
    {0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d
    ,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e},
    {0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2
    ,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25},
    {0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16
    ,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92},
    {0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda
    ,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84},
    {0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a
    ,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06},
    {0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02
    ,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b},
    {0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea
    ,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73},
    {0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85
    ,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e},
    {0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89
    ,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b},
    {0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20
    ,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4},
    {0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31
    ,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f},
    {0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d
    ,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef},
    {0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0
    ,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61},
    {0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26
    ,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d}
};

/* 正S盒字节代换 */
static inline uint8_t AES128_ChangeByte0(uint8_t Byte)
{
    return (AES128_S1[(Byte & 0xF0) >> 4][(Byte & 0x0F)]);
}

/* 逆S盒字节代换 */
static inline uint8_t AES128_ChangeByte1(uint8_t Byte)
{
    return (AES128_S2[(Byte & 0xF0) >> 4][(Byte & 0x0F)]);
}

/* 工步流1:字节代换 */
static void AES128_ChangeByte(uint8_t Matrix[4][4], uint8_t Flag)
{
    if (Flag == 0) {
        for (uint8_t I = 0; I <= 3; I++)
            for (uint8_t J = 0; J <= 3; J++)
                Matrix[I][J] = AES128_ChangeByte0(Matrix[I][J]);
    }
    if (Flag == 1) {
        for (uint8_t I = 0; I <= 3; I++)
            for (uint8_t J = 0; J <= 3; J++)
                Matrix[I][J] = AES128_ChangeByte1(Matrix[I][J]);
    }
}

/* 工步流2:行位移(状态矩阵按行位移:0移位(左移)1逆移位(右移)) */
static void AES128_LineShift(uint8_t Matrix[4][4], uint8_t Flag)
{   //不要用循环,实际循环的效率应低于批量赋值
    //(且批量赋值不增加太多代码量,可接受范围内)
    uint8_t Byte[4] = {0};
    //第0行不移位
    if (Flag == 0) {//第1行左移一位
        Byte[0]      = Matrix[1][0];
        Matrix[1][0] = Matrix[1][1];
        Matrix[1][1] = Matrix[1][2];
        Matrix[1][2] = Matrix[1][3];
        Matrix[1][3] = Byte[0];
    }
    if (Flag == 1) {//第1行右移一位
        Byte[0]      = Matrix[1][3];
        Matrix[1][3] = Matrix[1][2];
        Matrix[1][2] = Matrix[1][1];
        Matrix[1][1] = Matrix[1][0];
        Matrix[1][0] = Byte[0];
    }
    {   //第2行左右移俩位(右左移俩位)
        Byte[0]      = Matrix[2][0];
        Byte[1]      = Matrix[2][1];
        Matrix[2][0] = Matrix[2][2];
        Matrix[2][1] = Matrix[2][3];
        Matrix[2][2] = Byte[0];
        Matrix[2][3] = Byte[1];
    }
    if (Flag == 0) {//第3行左移三位(右移一位)
        Byte[3]      = Matrix[3][3];
        Matrix[3][3] = Matrix[3][2];
        Matrix[3][2] = Matrix[3][1];
        Matrix[3][1] = Matrix[3][0];
        Matrix[3][0] = Byte[3];
    }
    if (Flag == 1) {//第3行右移三位(左移一位)
        Byte[3]      = Matrix[3][0];
        Matrix[3][0] = Matrix[3][1];
        Matrix[3][1] = Matrix[3][2];
        Matrix[3][2] = Matrix[3][3];
        Matrix[3][3] = Byte[3];
    }
}

/* 伽罗瓦域加运算:GF(2^8) */
static inline uint8_t AES128_GF_Addition(uint8_t Data1, uint8_t Data2)
{
    return (Data1 ^ Data2);
}

/* 伽罗瓦域乘运算:GF(2^8) */
static inline uint8_t AES128_GF_Multiply2(uint8_t Data)
{
    return (((Data & 0x80) == 0 ? 0x00 : 0x1b) ^ (Data << 1));
}

/* 工步流3:列混合(状态矩阵按列混合:0混合1逆混合),基于GF(2^8) */
static void AES128_ColumnMixture(uint8_t Matrix[4][4], uint8_t Flag)
{
    uint8_t Byte[4][4] = {0};

    if (Flag == 0) {
        //针对列混合矩阵进行定向优化
        for (uint8_t I = 0; I <= 3; I++)
        for (uint8_t J = 0; J <= 3; J++) {//第0列:
             uint8_t Result = 0;
            Result ^= AES128_GF_Multiply2(Matrix[(0 + I) % 4][J]);  //0x02
            Result ^= AES128_GF_Multiply2(Matrix[(1 + I) % 4][J]);  //0x03
            Result ^= Matrix[(1 + I) % 4][J];
            Result ^= Matrix[(2 + I) % 4][J];                       //0x01
            Result ^= Matrix[(3 + I) % 4][J];                       //0x01
            Byte[I][J] = Result;
        }
        for (uint8_t I = 0; I <= 3; I++)
        for (uint8_t J = 0; J <= 3; J++)
             Matrix[I][J] = Byte[I][J];
    }
    if (Flag == 1) {
        uint8_t Temp = 0;
        //针对列混合矩阵进行定向优化
        for (uint8_t I = 0; I <= 3; I++)
        for (uint8_t J = 0; J <= 3; J++) {//第0列:
             uint8_t Result = 0;
            Temp = Matrix[(0 + I) % 4][J];          //0x0e
            Result ^= (Temp = AES128_GF_Multiply2(Temp));
            Result ^= (Temp = AES128_GF_Multiply2(Temp));
            Result ^= (Temp = AES128_GF_Multiply2(Temp));
            Temp = Matrix[(1 + I) % 4][J];          //0x0b
            Result ^= (Temp);
            Result ^= (Temp = AES128_GF_Multiply2(Temp));
                      (Temp = AES128_GF_Multiply2(Temp));
            Result ^= (Temp = AES128_GF_Multiply2(Temp));
            Temp = Matrix[(2 + I) % 4][J];          //0x0d
            Result ^= (Temp);
                      (Temp = AES128_GF_Multiply2(Temp));
            Result ^= (Temp = AES128_GF_Multiply2(Temp));
            Result ^= (Temp = AES128_GF_Multiply2(Temp));
            Temp = Matrix[(3 + I) % 4][J];          //0x09
            Result ^= (Temp);
                      (Temp = AES128_GF_Multiply2(Temp));
                      (Temp = AES128_GF_Multiply2(Temp));
            Result ^= (Temp = AES128_GF_Multiply2(Temp));
            Byte[I][J] = Result;
        }
        for (uint8_t I = 0; I <= 3; I++)
        for (uint8_t J = 0; J <= 3; J++)
             Matrix[I][J] = Byte[I][J];
    }
}

/* 工步流3:列混合,不使用 */
#if 0

/* 列混合矩阵 */
static const uint8_t AES128_C1[4][4] = {
    {0x2,0x3,0x1,0x1},
    {0x1,0x2,0x3,0x1},
    {0x1,0x1,0x2,0x3},
    {0x3,0x1,0x1,0x2}
};

/* 列混合逆矩阵 */
static const uint8_t AES128_C2[4][4] = {
    {0xe,0xb,0xd,0x9},
    {0x9,0xe,0xb,0xd},
    {0xd,0x9,0xe,0xb},
    {0xb,0xd,0x9,0xe}
};

/* 伽罗瓦域乘运算:GF(2^8) */
static uint8_t inline AES128_GF_Multiply(uint8_t Data1, uint8_t Data2)
{
    uint8_t Result = 0;

    if ((Data2 & 0x01) != 0)
        Result ^= Data1;
    for (uint8_t I = 1; I < 8; I++) {
         uint8_t Temp = AES128_GF_Multiply2(Temp);
        if ((Data2 & (1 << I)) != 0)
            Result ^= Temp;
    }
    return Result;
}

/* 工步流3:列混合(状态矩阵按列混合:0混合1逆混合),基于GF(2^8) */
static void AES128_ColumnMixture(uint8_t Matrix[4][4], uint8_t Flag)
{
    uint8_t Byte[4][4] = {0};

    uint8_t Temp = 0;
    if (Flag == 0) {
        for (uint8_t I = 0; I <= 3; I++)
        for (uint8_t J = 0; J <= 3; J++) {
             uint8_t Result = 0;
            for (uint8_t K = 0; K <= 3; K++) {
                Temp   = AES128_GF_Multiply(Matrix[K][J],AES128_C1[I][K]);
                Result = AES128_GF_Addition(Result, Temp);
            }
            Byte[I][J] = Result;
        }
        for (uint8_t I = 0; I <= 3; I++)
        for (uint8_t J = 0; J <= 3; J++)
             Matrix[I][J] = Byte[I][J];
    }
    if (Flag == 1) {
        for (uint8_t I = 0; I <= 3; I++)
        for (uint8_t J = 0; J <= 3; J++) {
             uint8_t Result = 0;
            for (uint8_t K = 0; K <= 3; K++) {
                Temp   = AES128_GF_Multiply(Matrix[K][J],AES128_C2[I][K]);
                Result = AES128_GF_Addition(Result, Temp);
            }
            Byte[I][J] = Result;
        }
        for (uint8_t I = 0; I <= 3; I++)
        for (uint8_t J = 0; J <= 3; J++)
             Matrix[I][J] = Byte[I][J];
    }
}

#endif

/* 工步流4:秘钥轮加(状态矩阵从上往下,从左往右:轮加,逆轮加一致) */
static void AES128_KeyWheel(uint8_t Matrix0[4][4], uint8_t Matrix1[4][4])
{
    for (uint8_t I = 0; I <= 3; I++)
    for (uint8_t J = 0; J <= 3; J++)
        Matrix0[I][J] ^= Matrix1[I][J];
}

/* 轮常量,不使用 */
#if 0
static const uint8_t AES128_Round[10][4] = {
    {0x01,0x00,0x00,0x00},
    {0x02,0x00,0x00,0x00},
    {0x04,0x00,0x00,0x00},
    {0x08,0x00,0x00,0x00},
    {0x10,0x00,0x00,0x00},
    {0x20,0x00,0x00,0x00},
    {0x40,0x00,0x00,0x00},
    {0x80,0x00,0x00,0x00},
    {0x1b,0x00,0x00,0x00},
    {0x36,0x00,0x00,0x00}
};
#endif

/* 状态函数T函数 */
static void AES128_Function_T(uint8_t Key[4], uint8_t round)
{
    uint8_t Temp = 0;
    //1.字循环
    Temp   = Key[0];
    Key[0] = Key[1];
    Key[1] = Key[2];
    Key[2] = Key[3];
    Key[3] = Temp;
    //2.字节代换
    Key[0] = AES128_ChangeByte0(Key[0]);
    Key[1] = AES128_ChangeByte0(Key[1]);
    Key[2] = AES128_ChangeByte0(Key[2]);
    Key[3] = AES128_ChangeByte0(Key[3]);
    //3.轮常量异或
    if (round < 9)
        Key[0] ^= (1 << (round - 1));
    if (round == 9)
        Key[0] ^= 0x1b;
    if (round == 10)
        Key[0] ^= 0x36;
}

/* 工步流5:秘钥扩展 */
static void AES128_KeyExtern(uint8_t Matrix[11][4][4], uint8_t Key[16])
{
    uint8_t  Temp[2][16] = {0};
    uint8_t *Key_T = (void *)0;
    uint8_t *Key_0 = Temp[0];    //前一轮
    uint8_t *Key_1 = Temp[1];    //当前轮

    //1.初始状态矩阵
    AES128_KEY_TO_MATRIX(Matrix[0], Key);

    //2.初始上一轮数据
    for (uint8_t K = 0; K < 16; K++)
        Key_0[K] = Key[K];

    //3.轮运算
    for (uint8_t K = 1; K <= 10; K++) {
        for (uint8_t T = 0; T < 16; T++)
            Key_1[T] = Key_0[T];
        //1.求轮函数
        AES128_Function_T(&(Key_0[12]), K);
        //2.求记录
        for (uint8_t T = 0; T < 4; T++)
            Key_1[T] ^= Key_0[T + 12];
        for (uint8_t T = 4; T < 16; T++)
            Key_1[T] ^= Key_1[T - 4];
        //3.保存当前轮记录
        AES128_KEY_TO_MATRIX(Matrix[1], Key_1);
        //交换
        Key_T = Key_0;
        Key_0 = Key_1;
        Key_1 = Key_T;
    }
}

/* AES128加解密算法 */
void AES128_Run(uint8_t *Text, uint32_t Length, uint8_t Key[16], uint8_t Flag)
{
    //1.建立状态矩阵
    uint8_t AES128_Matrix[11][4][4] = {0};
    uint8_t Matrix[4][4] = {0};
    //2.扩展秘钥
    AES128_KeyExtern(AES128_Matrix, Key);
    //3.迭代加密或解密
    if (Flag == 0) {
        for (uint32_t I = 0; I < Length; I += 16) {
            AES128_KEY_TO_MATRIX(Matrix, &(Text[I]));
            //1.起始秘钥轮加
            {
                AES128_KeyWheel(Matrix, AES128_Matrix[0]);
            }
            //2.前九轮
            for (uint32_t K = 1; K <= 9; K++) {
                //1.字节代换
                AES128_ChangeByte(Matrix, 0);
                //2.行移位
                AES128_LineShift(Matrix, 0);
                //3.列混合
                AES128_ColumnMixture(Matrix, 0);
                //4.秘钥轮加
                AES128_KeyWheel(Matrix, AES128_Matrix[K]);
            }
            //3.第十轮
            {
                //1.字节代换
                AES128_ChangeByte(Matrix, 0);
                //2.行移位
                AES128_LineShift(Matrix, 0);
                //4.秘钥轮加
                AES128_KeyWheel(Matrix, AES128_Matrix[10]);
            }
            AES128_MATRIX_TO_KEY(Matrix, &(Text[I]));
        }
    }
    if (Flag == 1) {
        //额外动作:解密时部分扩展秘钥逆列混合
        for (uint32_t K = 9; K >= 1; K--)
            AES128_ColumnMixture(AES128_Matrix[K], 1);

        for (uint32_t I = 0; I < Length; I += 16) {
            AES128_KEY_TO_MATRIX(Matrix, &(Text[I]));
            //1.起始秘钥轮加
            {
                AES128_KeyWheel(Matrix, AES128_Matrix[10]);
            }
            //2.前九轮
            for (uint32_t K = 9; K >= 1; K--) {
                //1.逆字节代换
                AES128_ChangeByte(Matrix, 1);
                //2.逆行移位
                AES128_LineShift(Matrix, 1);
                //3.逆列混合
                AES128_ColumnMixture(Matrix, 1);
                //4.秘钥轮加
                AES128_KeyWheel(Matrix, AES128_Matrix[K]);
            }
            //3.第十轮
            {
                //1.逆字节代换
                AES128_ChangeByte(Matrix, 1);
                //2.逆行移位
                AES128_LineShift(Matrix, 1);
                //4.秘钥轮加
                AES128_KeyWheel(Matrix, AES128_Matrix[0]);
            }
            AES128_MATRIX_TO_KEY(Matrix, &(Text[I]));
        }
    }
}

