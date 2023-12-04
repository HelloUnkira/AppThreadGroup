/*实现目标:
 *    杂项的零碎小功能集合
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 计算定积分
 *@param fun 函数
 *@param l 左区间
 *@param r 右区间
 *@param p 精度
 *@param f 模式(1:梯形法;2:抛物线法)
 */
double app_sys_cal_di(double (*fun)(double x), double l, double r, uint32_t p, uint32_t f)
{
    double cur = 0.0, ret = 0.0, ret1 = 0.0, ret2 = 0.0;
    double del_1 = (r - l) / (double)p;
    double del_2 = (r - l) / (double)p * 2.0;
    
    if (fun == NULL || l > r || p == 0)
        return 0.0;
    
    //梯形法公式:
    //(b-a)/n(y(1)+...+y(n-1)+y(0)/2+y(n)/2)
    //此时:--> p = n
    if (f == 1) {
        
        cur = l;
        for (uint32_t idx = 1; idx < p; idx++) {
            cur += del_1;
            ret += fun(cur);
        }
        
        ret += (fun(l) + fun(r)) / 2.0;
        ret *= del_1;
    }
    
    //抛物线法公式(辛普森公式):
    //(b-a)/6n(y(0)+y(2n)+4(y1+...+y(2n-1))+2(y2+...+y(2n-2)))
    //此时:--> p = 2n
    if (f == 2) {
        if (p % 2 != 0)
            p += 1;
        
        //求奇数部分
        cur = l + del_1;
        for (uint32_t idx = 1; idx < p; idx += 2) {
            ret1 += fun(cur);
            cur  += del_2;
        }
        //求偶数部分
        cur = l + del_2;
        for (uint32_t idx = 2; idx < p; idx += 2) {
            ret2 += fun(cur);
            cur  += del_2;
        }
        
        ret += (fun(l) + fun(r));
        ret += ret1 * 4.0 + ret2 * 2.0;
        ret *= del_1 / 3.0;
    }
    
    return ret;
}

/*@brief 牛顿切线法求取方程近似解
 *@param fun0 0阶函数(原函数)
 *@param fun1 1阶函数(原函数的导数)
 *@param s 起始点
 *@param p 精度
 */
double app_sys_cal_nt(double (*fun0)(double x), double (*fun1)(double x), double s, double p)
{
    p = app_sys_abs(p);
    if (fun0 == NULL || fun1 == NULL || p <= 1e-5)
        return 0;
    
    double x = s, x_0, x_1, t;
    
    while (1) {
        // 计算多项式的值与一阶导数的值
        x_0 = fun0(x);
        x_1 = fun1(x);
        //
        if (app_sys_abs(x_1) <= 1e-5)
            break;
        // 计算切点值
        t = x_0 / x_1;
        //
        if (app_sys_abs(t) <= p)
            break;
        //迭代
        x = x - t;
    }
    return x;
}
