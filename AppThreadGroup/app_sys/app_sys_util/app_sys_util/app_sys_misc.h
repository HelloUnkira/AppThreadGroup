#ifndef APP_SYS_MISC_H
#define APP_SYS_MISC_H

/*@brief     计算定积分
 *@param[in] fun 函数
 *@param[in] l 左区间
 *@param[in] r 右区间
 *@param[in] p 精度
 *@param[in] f 模式(1:梯形法;2:抛物线法)
 */
double app_sys_cal_di(double (*fun)(double x), double l, double r, uint32_t p, uint32_t f);

/*@brief     牛顿切线法求取方程近似解
 *@param[in] fun0 0阶函数(原函数)
 *@param[in] fun1 1阶函数(原函数的导数)
 *@param[in] s 起始点
 *@param[in] p 精度
 */
double app_sys_cal_nt(double (*fun0)(double x), double (*fun1)(double x), double s, double p);

#endif
