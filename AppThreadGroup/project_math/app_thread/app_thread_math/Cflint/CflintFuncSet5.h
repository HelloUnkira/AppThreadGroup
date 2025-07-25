#ifndef CFLINT_FUNC_SET5_H
#define CFLINT_FUNC_SET5_H

/* 大随机数生成(注意初始化回调的随机状态情况) */
void Cflint_Random(Cflint_Type  *Op, uint32_t Len,
                   Cflint_Type (*Random)(void));

/* 强素数判别检查(Miller_Rabin测试) */
/* 要求:Temp[3~6]的长度为Length*2 */
bool Cflint_IsPrime(Cflint_Type *X, Cflint_Type *Temp[7], uint32_t Len,
                    Cflint_Type (*Random)(void), bool Weak);

/* 指定查找次数,尝试找到一个强素数(Min和Max同时为空则取值范围在全域内) */
/* 要求:Temp[4~7]的长度为Length*2 */
//!当前素数查找算法效率低下
//!问题1:随机数不够随机,不适用素数生成
//!问题2:内部基础算法速度过慢
uint32_t Cflint_RandomPrime(Cflint_Type *X, Cflint_Type *Temp[7],
                            Cflint_Type *Min, Cflint_Type *Max, uint32_t Len,
                            Cflint_Type (*Random)(void), uint32_t CountMax);

/* 孪生素数判别检查 */
bool Cflint_IsPrime1(Cflint_Type *X, Cflint_Type *Temp[3], uint32_t Len);

/* 素数生成器:不实现,给出流程
 * uint64_t B = (1 + [sqrt[N]]) >> 1;
 * uint64_t L = (N >> 1);
 *          L = (L - ((N > 0) && ((N & 1) == 0)) ? 1 : 0);
 * bool F[L + 1] = {1};
 * uint64_t P = 3;
 * uint64_t S = 4;
 * for (uint64_t I = 1; I <= B; I++) {
 *   if (F[I] == true)
 *       for (uint64_t K = S; K <= L; F[K] = 0, K += P);
 *   S += P + P + 2;
 *   P += 2;
 * }
 * 现在if (F[I] == 1)的(I * 2 + 1)就是一个素数,素数2额外计算
 */

#endif
