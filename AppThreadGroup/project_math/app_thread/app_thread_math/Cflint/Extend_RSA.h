#ifndef EXTEND_RSA_H
#define EXTEND_RSA_H

/* 备注:这里并没有提供完整的方案,只是表现一些流程 */
/*RSA流程:
 *1.根据找寻素数对(P, Q),使用大随机奇素数接口
 *2.计算公共模数:N = P * Q
 *3.根据欧拉公式计算欧拉数:R = (P - 1) * (Q - 1)
 *4.找寻一个小于R的整数:E满足E与R互质
 *5.计算乘法逆:D = (1 / E)(Mod R)
 *6.销毁P和Q,生成公钥(N, E), 生成私钥(N, D)
 *RSA加解密:
 *密文 = (明文**E) % N
 *明文 = (密文**D) % N
 */

/* 通过目标素数对(P, Q)构造公钥(N, E)和私钥(N, D) */
/* 要求Temp[0~7]为Length*2 */
/* 要求Temp[7~9]为Length*4 */
void RSA_Make(Cflint_Type *P, Cflint_Type *Q, Cflint_Type *N,
              Cflint_Type *E, Cflint_Type *D, Cflint_Type *R,
              Cflint_Type *Temp[9], uint32_t Len);

void RSA_Encrypt(Cflint_Type *N, Cflint_Type *E, Cflint_Type *I, Cflint_Type *O,
                 Cflint_Type *Temp[4], uint32_t Len);

void RSA_Decrypt(Cflint_Type *N, Cflint_Type *D, Cflint_Type *I, Cflint_Type *O,
                 Cflint_Type *Temp[4], uint32_t Len);

#endif