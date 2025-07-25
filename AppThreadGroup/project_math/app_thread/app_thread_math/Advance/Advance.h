#ifndef ADVANCE_H
#define ADVANCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* 计算定积分 */
double Cal_DI(double (*F_X)(double X), double Left, double Right, uint32_t Precision, uint32_t Flag);

/* 牛顿切线法求取方程近似解 */
double Cal_NT(double (*F0_X)(double X), double (*F1_X)(double X), double Start, double Precision);

#endif
