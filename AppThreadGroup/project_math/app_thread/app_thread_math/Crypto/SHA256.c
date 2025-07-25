
#include "SHA256.h"

/* 初始状态量(大端序) */
static inline void SHA256_Ready(uint32_t Buffer[8])
{
    /* 自然数中前8个质数平方根的小数部分取前32bit */
    const uint32_t SHA256_Const[8] = {
        0x6A09E667,0xBB67AE85,0x3C6EF372,0xA54FF53A,
        0x510E527F,0x9B05688C,0x1F83D9AB,0x5BE0CD19,
    };
    for (uint32_t Index = 0; Index < 8; Index++)
        Buffer[Index] = SHA256_Const[Index];
}

/* 需要填充字节数 */
static inline uint32_t SHA256_FillBytes(uint32_t Length)
{
    uint32_t FillBytes = Length % 64;
    FillBytes = FillBytes >= 56 ? 64 - FillBytes + 56 : 56 - FillBytes;
    return FillBytes;
}

/* 分组切块(小端序块) */
static inline bool SHA256_Block(uint8_t *Text,      uint32_t  Length,
                                uint8_t  Block[64], uint32_t *Offset,
                                uint32_t FillBytes)
{
    bool RunToEnd = false;
    /* 处理后的数据流需满足补位到bits==448(mod512),bytes==56(mod64) */
    /* 填充规则为追加1其他补0,也即补一个0x80后补0 */
    /* 最后的bits==64(mod512),bytes==8(mod64) */
    for (uint32_t Index = 0; Index < 64; Index++)
        /* 正常的数据填充时 */
        if (*Offset + Index < Length)
            Block[Index] = Text[*Offset + Index];
        /* 补充的数据填充时 */
        /* 1.第一个要填充的字节,填充0x80 */
        else if (*Offset + Index == Length)
            Block[Index] = 0x80;
        /* 2.中间要填充的字节,填充0x00 */
        else if (*Offset + Index  < Length + FillBytes)
            Block[Index] = 0x00;
        /* 3.最后要填充数据位的长度(指定大端序),填充Length*8 */
        else {
            uint64_t LengthBits = Length * 8;
            for (uint32_t Nonius = 0; Nonius < 8; Nonius++)
                Block[64 - 8 + Nonius] = LengthBits >> ((7 - Nonius) * 8);
            RunToEnd = true;
            break;
        }
    *Offset += 64;
    return RunToEnd;
}

/* 加密轮(大端序块) */
static inline void SHA256_Wheel(uint32_t Block[16], uint32_t Buffer[8])
{
    /* 这些常量是对自然数中前64个质数立方根的小数部分取前32bit */
    const uint32_t SHA256_Ki[64] = {
        0x428A2F98,0x71374491,0xB5C0FBCF,0xE9B5DBA5,
        0x3956C25B,0x59F111F1,0x923F82A4,0xAB1C5ED5,
        0xD807AA98,0x12835B01,0x243185BE,0x550C7DC3,
        0x72BE5D74,0x80DEB1FE,0x9BDC06A7,0xC19BF174,
        0xE49B69C1,0xEFBE4786,0x0FC19DC6,0x240CA1CC,
        0x2DE92C6F,0x4A7484AA,0x5CB0A9DC,0x76F988DA,
        0x983E5152,0xA831C66D,0xB00327C8,0xBF597FC7,
        0xC6E00BF3,0xD5A79147,0x06CA6351,0x14292967,
        0x27B70A85,0x2E1B2138,0x4D2C6DFC,0x53380D13,
        0x650A7354,0x766A0ABB,0x81C2C92E,0x92722C85,
        0xA2BFE8A1,0xA81A664B,0xC24B8B70,0xC76C51A3,
        0xD192E819,0xD6990624,0xF40E3585,0x106AA070,
        0x19A4C116,0x1E376C08,0x2748774C,0x34B0BCB5,
        0x391C0CB3,0x4ED8AA4A,0x5B9CCA4F,0x682E6FF3,
        0x748F82EE,0x78A5636F,0x84C87814,0x8CC70208,
        0x90BEFFFA,0xA4506CEB,0xBEF9A3F7,0xC67178F2,
    };
    /* 循环移位宏(右移N位,左移32-N位) */
    #define SHA256_S(X, N) (((X) >> (N)) | ((X) << (32-(N))))
    #define SHA256_R(X, N) (((X) >> (N)))
    /* 轮函数 */
    #define SHA256_Ch(X, Y, Z)  (((X) & (Y)) ^ ((~(X)) & (Z)))
    #define SHA256_Ma(X, Y, Z)  (((X) & (Y)) ^ ((X) & (Z)) ^ ((Y) & (Z)))
    #define SHA256_EP0(X)       (SHA256_S(X,  2) ^ SHA256_S(X, 13) ^ SHA256_S(X, 22))
    #define SHA256_EP1(X)       (SHA256_S(X,  6) ^ SHA256_S(X, 11) ^ SHA256_S(X, 25))
    #define SHA256_SIG0(X)      (SHA256_S(X,  7) ^ SHA256_S(X, 18) ^ SHA256_R(X, 3))
    #define SHA256_SIG1(X)      (SHA256_S(X, 17) ^ SHA256_S(X, 19) ^ SHA256_R(X, 10))
    /* 初始准备 */
    uint32_t M[64] = {0};
    for (uint32_t Index =  0; Index < 16; Index++)
        M[Index] = Block[Index];
    for (uint32_t Index = 16; Index < 64; Index++)
        M[Index] = SHA256_SIG1(M[Index -  2]) + M[Index -  7] +
                   SHA256_SIG0(M[Index - 15]) + M[Index - 16];
    /* 设置初始值 */
    uint32_t A = Buffer[0];
    uint32_t B = Buffer[1];
    uint32_t C = Buffer[2];
    uint32_t D = Buffer[3];
    uint32_t E = Buffer[4];
    uint32_t F = Buffer[5];
    uint32_t G = Buffer[6];
    uint32_t H = Buffer[7];
    /*原数据块为ABCDEFGH，则一次循环为：
     *A'=Σ1(E)+Ch(E,F,G)+M(t)+K(t)+H+Ma(A,B,C)+Σ0(A)
     *B'=A
     *C'=B
     *D'=C
     *E'=Σ1(E)+Ch(E,F,G)+M(t)+K(t)+H+D
     *F'=E
     *G'=F
     *H'=G
     */
	//64次加密循环
	for (uint32_t Index = 0; Index < 64; Index++) {
		uint32_t T1 = SHA256_EP1(E) + SHA256_Ch(E, F, G) + SHA256_Ki[Index] + M[Index] + H;
		uint32_t T2 = SHA256_EP0(A) + SHA256_Ma(A, B, C);
        
		H = G; G = F; F = E; E = T1 + D;
		D = C; C = B; B = A; A = T1 + T2;
	}
    Buffer[0] += A;
    Buffer[1] += B;
    Buffer[2] += C;
    Buffer[3] += D;
    Buffer[4] += E;
    Buffer[5] += F;
    Buffer[6] += G;
    Buffer[7] += H;
}

void SHA256_Run(uint8_t *Text, uint32_t Length, uint8_t Digest[32])
{
    /* 初始化SHA256缓存器,用于保存中间变量和最终结果(字节流小端序) */
    uint32_t *Result = (uint32_t *)Digest;
    SHA256_Ready(Result);
    /* 计算最终需要填充的字节数 */
    uint32_t FillBytes = SHA256_FillBytes(Length);
    /* 循环摘要流程 */
    bool RunToEnd = false;
    do {
        /* 1.填充分组Block */
        uint32_t Offset = 0;
        uint8_t  Block[64] = {0};
        RunToEnd = SHA256_Block(Text, Length, Block, &Offset, FillBytes);
        /* 转为大端序流 */
        for (uint32_t Index = 0; Index < 16; Index++) {
            uint8_t Temp1 = Block[Index * 4 + 0];
            Block[Index * 4 + 0] = Block[Index * 4 + 3];
            Block[Index * 4 + 3] = Temp1;
            uint8_t Temp2 = Block[Index * 4 + 1];
            Block[Index * 4 + 1] = Block[Index * 4 + 2];
            Block[Index * 4 + 2] = Temp2;
        }
        /* 2.处理数据段轮函数 */
        SHA256_Wheel((uint32_t *)Block, Result);
    } while (RunToEnd == false);
    /* 结果转为大端序流 */
    for (uint32_t Index = 0; Index < 8; Index++) {
        uint8_t Temp1 = Digest[Index * 4 + 0];
        Digest[Index * 4 + 0] = Digest[Index * 4 + 3];
        Digest[Index * 4 + 3] = Temp1;
        uint8_t Temp2 = Digest[Index * 4 + 1];
        Digest[Index * 4 + 1] = Digest[Index * 4 + 2];
        Digest[Index * 4 + 2] = Temp2;
    }
}

