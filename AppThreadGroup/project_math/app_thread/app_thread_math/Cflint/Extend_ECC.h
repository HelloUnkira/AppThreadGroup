#ifndef EXTEND_ECC_H
#define EXTEND_ECC_H

/* 本扩展从ThirdPort/uECC_II源码中提取后,经过整理形成 */

#if (Cflint_Byte != 4)
/* 当前解析的Ecc只使用四字节对齐 */
#error "Current Ecc Curve Only Support Type 4"
#endif

/* 导入指定的椭圆曲线 */
#include "Extend_ECC_Curve.h"

/* 配置ECC使用的随机数发生器,用于抵抗侧通道攻击 */
void Ecc_RNG_Function(bool (*Rng)(Cflint_Type *Data, uint32_t Size));

bool Ecc_MakeKey(Cflint_Type PublicKey[Ecc_Curve_Size1 * 2],
                 Cflint_Type PrivateKey[Ecc_Curve_Size1]);

bool Ecc_ShareSecret(Cflint_Type PublicKey[Ecc_Curve_Size1 * 2],
                     Cflint_Type PrivateKey[Ecc_Curve_Size1],
                     Cflint_Type SecretKey[Ecc_Curve_Size1]);

bool Ecc_ValidKey(Cflint_Type PublicKey[Ecc_Curve_Size1 * 2]);

#endif