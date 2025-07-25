
#include "MD5.h"

/* 初始状态量(小端序) */
static inline void MD5_Ready(uint32_t Buffer[4])
{
    const uint32_t MD5_Const[4] = {
        0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476,
    };
    for (uint32_t Index = 0; Index < 4; Index++)
        Buffer[Index] = MD5_Const[Index];
}

/* 需要填充字节数 */
static inline uint32_t MD5_FillBytes(uint32_t Length)
{
    uint32_t FillBytes = Length % 64;
    FillBytes = FillBytes >= 56 ? 64 - FillBytes + 56 : 56 - FillBytes;
    return FillBytes;
}

/* 分组切块(小端序块) */
static inline bool MD5_Block(uint8_t *Text,      uint32_t  Length,
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
        /* 3.最后要填充数据位的长度,填充Length*8 */
        else {
            uint64_t LengthBits = Length * 8;
            for (uint32_t Nonius = 0; Nonius < 8; Nonius++)
                Block[64 - 8 + Nonius] = LengthBits >> (Nonius * 8);
            RunToEnd = true;
            break;
        }
    *Offset += 64;
    return RunToEnd;
}

/* 加密轮(小端序块) */
static inline void MD5_Wheel(uint32_t Block[16], uint32_t Buffer[4])
{
    /* 常量ti = (uint32_t)(abs(sin(i+1))*(2pow32)) */
    const uint32_t MD5_Ti[] = {
        0xD76AA478,0xE8C7B756,0x242070DB,0xC1BDCEEE,
        0xF57C0FAF,0x4787C62A,0xA8304613,0xFD469501,
        0x698098D8,0x8B44F7AF,0xFFFF5BB1,0x895CD7BE,
        0x6B901122,0xFD987193,0xA679438E,0x49B40821,
        0xF61E2562,0xC040B340,0x265E5A51,0xE9B6C7AA,
        0xD62F105D,0x02441453,0xD8A1E681,0xE7D3FBC8,
        0x21E1CDE6,0xC33707D6,0xF4D50D87,0x455A14ED,
        0xA9E3E905,0xFCEFA3F8,0x676F02D9,0x8D2A4C8A,
        0xFFFA3942,0x8771F681,0x6D9D6122,0xFDE5380C,
        0xA4BEEA44,0x4BDECFA9,0xF6BB4B60,0xBEBFBC70,
        0x289B7EC6,0xEAA127FA,0xD4EF3085,0x04881D05,
        0xD9D4D039,0xE6DB99E5,0x1FA27CF8,0xC4AC5665,
        0xF4292244,0x432AFF97,0xAB9423A7,0xFC93A039,
        0x655B59C3,0x8F0CCC92,0xFFEFF47D,0x85845DD1,
        0x6FA87E4F,0xFE2CE6E0,0xA3014314,0x4E0811A1,
        0xF7537E82,0xBD3AF235,0x2AD7D2BB,0xEB86D391,
    };
    /* 移位数 */
    const uint32_t MD5_Si[] = {
         7,12,17,22, 7,12,17,22,
         7,12,17,22, 7,12,17,22,
         5, 9,14,20, 5, 9,14,20,
         5, 9,14,20, 5, 9,14,20,
         4,11,16,23, 4,11,16,23,
         4,11,16,23, 4,11,16,23,
         6,10,15,21, 6,10,15,21,
         6,10,15,21, 6,10,15,21,
    };
    /* 循环移位宏(右移N位,左移32-N位) */
    #define MD5_Shift(X, N) (((X) << (N)) | ((X) >> (32-(N))))
    /* 四个轮函数 */
    #define MD5_FuncF(X, Y, Z)  (((X) & (Y)) | ((~(X)) & (Z)))
    #define MD5_FuncG(X, Y, Z)  (((X) & (Z)) | ((~(Z)) & (Y)))
    #define MD5_FuncH(X, Y, Z)  ((X) ^ (Y) ^ (Z))
    #define MD5_FuncI(X, Y, Z)  ((Y) ^ ((~(Z)) | (X)))
    /* 暂存数 */
    uint32_t A = Buffer[0];
    uint32_t B = Buffer[1];
    uint32_t C = Buffer[2];
    uint32_t D = Buffer[3];
    uint32_t F = 0;
    uint32_t G = 0;
    
    for (uint32_t Index = 0; Index < 64; Index++) {
        
        if (Index < 16) {
            F = MD5_FuncF(B, C, D);
            G = (Index) % 16;
        } else
        if (Index < 32) {
            F = MD5_FuncG(B, C, D);
            G = (Index * 5 + 1) % 16;
        } else
        if (Index < 48) {
            F = MD5_FuncH(B, C, D);
            G = (Index * 3 + 5) % 16;
        } else
        if (Index < 64) {
            F = MD5_FuncI(B, C, D);
            G = (Index * 7) % 16;
        }
        
        uint32_t Temp = D; D = C; C = B;
        B = B + MD5_Shift(A + F + MD5_Ti[Index] + Block[G], MD5_Si[Index]);
        A = Temp;
    }
    
    Buffer[0] += A;
    Buffer[1] += B;
    Buffer[2] += C;
    Buffer[3] += D;
}

void MD5_Run(uint8_t *Text, uint32_t Length, uint8_t Digest[16])
{
    /* 初始化MD5缓存器,用于保存中间变量和最终结果(字节流小端序) */
    uint32_t *Result = (uint32_t *)Digest;
    MD5_Ready(Result);
    /* 计算最终需要填充的字节数 */
    uint32_t FillBytes = MD5_FillBytes(Length);
    /* 循环摘要流程 */
    bool RunToEnd = false;
    do {
        /* 1.填充分组Block */
        uint32_t Offset = 0;
        uint8_t  Block[64] = {0};
        RunToEnd = MD5_Block(Text, Length, Block, &Offset, FillBytes);
        /* 2.处理数据段轮函数 */
        MD5_Wheel((uint32_t *)Block, Result);
        /*  */
    } while (RunToEnd == false);
}

