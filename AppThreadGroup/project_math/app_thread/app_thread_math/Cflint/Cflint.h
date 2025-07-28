#ifndef CFLINT_H
#define CFLINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*Cflint(Abbr):
 *C Functions For Large Integers In Number Theory And Cryptography
 *大数理论与密码学的C函数集
 */

/* 大数数组数据布局: */
/* [Low][......][High] */

typedef enum {
    Cflint_FPos = 0,
    Cflint_FNeg,
} Cflint_Flag;

#define Cflint_FInv(Flag)   ((Flag) == Cflint_FNeg ? Cflint_FPos : Cflint_FNeg)

/* 数据类型相关的部分 */
#define CFLINT_ByteLH       0x0000FFFF      /* 平台字:低半字0,高半字1 */
#define CFLINT_ByteH1       0x80000000      /* 平台字:最高位为1 */
#define CFLINT_ByteA1       0xFFFFFFFF      /* 平台字:全字为1 */
#define Cflint_Bits         32              /* 二进制位数 */
#define Cflint_Byte         4               /* 字节数 */
#define Cflint_Type         uint32_t        /* 字节类型 */
#define Cflint_Type2        uint64_t        /* 关键字优化 */

/* 资源申请释放:malloc和free */
#define Cflint_Alloc        malloc
#define Cflint_Free         free

/* 运算相关接口 */
#include "CflintFuncSet1.h"         /* 简介:辅助功能,位运算功能 */
#include "CflintFuncSet2.h"         /* 简介:基础运算功能 */
#include "CflintFuncSet3.h"         /* 简介:大数欧几里得运算功能 */
#include "CflintFuncSet4.h"         /* 简介:求根及其二次剩余 */
#include "CflintFuncSet5.h"         /* 简介:大随机数,素性相关,未完善 */

/* 扩展相关接口 */
#include "Extend_RSA.h"              /* 这里只提供流程,未提供完全实现 */
#include "Extend_ECC.h"
#include "Extend_Mentgomery.h"       /* 蒙哥马利运算相关接口 */

#endif
