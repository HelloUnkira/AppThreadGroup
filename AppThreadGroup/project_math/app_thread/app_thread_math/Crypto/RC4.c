
#include "RC4.h"

/* 备注:该部分摘要于其他 */

/* KSA */
void RC4Set(uint8_t RC4Key[256], uint8_t *Key, uint32_t Length)
{
    for (uint32_t I = 0; I < 256; I++)
        RC4Key[I] = I;
    /* 初始化状态量 */
    for (uint32_t I = 0, J = 0; I < 256; I++) {
        /* */
        J = (J + RC4Key[I] + Key[I % Length]) & 0xFF;
        /* */
        RC4Key[I] = RC4Key[I] ^ RC4Key[J];
        RC4Key[J] = RC4Key[I] ^ RC4Key[J];
        RC4Key[I] = RC4Key[I] ^ RC4Key[J];
    }
}

/* PRGA */
void RC4Run(uint8_t *RC4Key, uint8_t *Data, uint32_t Length)
{
    uint32_t I = 0;
    uint32_t J = 0;
    for (uint32_t K = 0; K < Length; K++) {
        /* 更新状态量 */
        I = (I + 1) & 0xFF;
        J = (J + RC4Key[I]) & 0xFF;
        /* 交换状态量 */
        RC4Key[I] = RC4Key[I] ^ RC4Key[J];
        RC4Key[J] = RC4Key[I] ^ RC4Key[J];
        RC4Key[I] = RC4Key[I] ^ RC4Key[J];
        /* 计算输出索引 */
        Data[K] ^= RC4Key[(RC4Key[I] + RC4Key[J]) & 0xFF];
    }
}

