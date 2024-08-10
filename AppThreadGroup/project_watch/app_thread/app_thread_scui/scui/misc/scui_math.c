/*实现目标:
 *    常用函数
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 弧度角度转换
 *@param angle 角度
 *@retval 弧度
 */
double scui_radian_by_angle(double angle)
{
    return angle * 3.141592654f / 180.0f;
}

/*@brief 弧度角度转换
 *@param radian 弧度
 *@retval 角度
 */
double scui_radian_to_angle(double radian)
{
    return radian * 180.0f / 3.141592654f;
}

/*@brief 三角函数(sin)
 *@param angle 弧度
 *@retval 三角函数值
 */
double scui_sin(double radian)
{
    return sin(radian);
}

/*@brief 三角函数(cos)
 *@param angle 弧度
 *@retval 三角函数值
 */
double scui_cos(double radian)
{
    return cos(radian);
}

/*@brief 三角函数(tan)
 *@param angle 弧度
 *@retval 三角函数值
 */
double scui_tan(double radian)
{
    return tan(radian);
}

/*@brief 指数函数
 *@param x 数字
 *@param e 指数
 *@retval 返回值(x^e)
 */
int64_t scui_pow(int64_t x, int8_t e)
{
    int64_t ret = 1;
    for (e = e; e != 0; e >>= 1) {
        if ((e & 0x1) != 0)
            ret *= x;
        x *= x;
    }
    return ret;
}

/*@brief 随机数
 *@param min 数字
 *@param max 数字
 *@retval 返回值
 */
int64_t scui_rand(int64_t min, int64_t max)
{
    static int64_t a = 0x1234ABCD; /*Seed*/
    
    /*Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"*/
    int64_t x = a;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    a = x;
    
    return (a % (max - min + 1)) + min;
}

/*@brief 三角函数(sin),放大4096倍
 *@param angle 角度
 *@retval 三角函数值
 */
int32_t scui_sin4096(int32_t angle)
{
    /* 0~90 */
    static const int16_t scui_sin_table_0_90[] =
    {
           0,   71,  143,  214,  286,  357,  428,  499,  570,  641,
         711,  782,  852,  921,  991, 1060, 1129, 1198, 1266, 1334,
        1401, 1468, 1534, 1600, 1666, 1731, 1796, 1860, 1923, 1986,
        2048, 2110, 2171, 2231, 2290, 2349, 2408, 2465, 2522, 2578,
        2633, 2687, 2741, 2793, 2845, 2896, 2946, 2996, 3044, 3091,
        3138, 3183, 3228, 3271, 3314, 3355, 3396, 3435, 3474, 3511,
        3547, 3582, 3617, 3650, 3681, 3712, 3742, 3770, 3798, 3824,
        3849, 3873, 3896, 3917, 3937, 3956, 3974, 3991, 4006, 4021,
        4034, 4046, 4056, 4065, 4074, 4080, 4086, 4090, 4094, 4095, 4096
    };
    
    int32_t a = ((angle % 360) + 360) % 360;
    
    if (a >= 0 && a <= 90)
        return +scui_sin_table_0_90[a];
    if (a > 90 && a <= 180)
        return +scui_sin_table_0_90[180 - a];
    if (a > 180 && a <= 270)
        return -scui_sin_table_0_90[a - 180];
        return -scui_sin_table_0_90[360 - a];
}

/*@brief 三角函数(cos),放大4096倍
 *@param angle 角度
 *@retval 三角函数值
 */
int32_t scui_cos4096(int32_t angle)
{
    return scui_sin4096(angle + 90);
}

/*@brief 三角函数(tan),放大4096倍
 *@param angle 角度
 *@retval 三角函数值
 */
int32_t scui_tan4096(int32_t angle)
{
    return (scui_sin4096(angle) << 12) / scui_cos4096(angle);
}

/*@brief 平方根
 *@param x 数字
 *@param q_i 平方根整数部
 *@param q_f 平方根小数部
 *@param mask: (root,mask):<16,0x80>;<256,0x800>;<xxx,0x8000>;
 */
void scui_sqrt(int32_t x, int32_t *q_i, int32_t *q_f, int32_t mask)
{
    /*To get 4 bit precision. (sqrt(256) = 16 = 4 bit)*/
    x = x << 8;
    
    int32_t root = 0;
    int32_t trial;
    // http://ww1.microchip.com/...en/AppNotes/91040a.pdf
    
    do {
        trial = root + mask;
        if (trial * trial <= x)
            root  = trial;
        mask = mask >> 1;
    } while (mask != 0);

    *q_i = (root >> 4);
    *q_f = (root & 0xf) << 4;
}

/*@brief 计算一个三次Bezier函数的值。
* @param t  参数t时间范围[0..SCUI_SCALE_COF]
* @param u0 起始值0值范围[0..SCUI_SCALE_COF]
* @param u1 控制值1值范围[0..SCUI_SCALE_COF]
* @param u2 控制值2值范围[0..SCUI_SCALE_COF]
* @param u3 结束值3值范围[0..SCUI_SCALE_COF]
* @retval 给定参数在范围内计算的值
*/
int32_t scui_bezier3(int32_t t, int32_t u0, int32_t u1, int32_t u2, int32_t u3)
{
    int32_t t_rem  = SCUI_SCALE_COF - t;
    int32_t t_rem2 = (t_rem  * t_rem) >> SCUI_SCALE_OFS;
    int32_t t_rem3 = (t_rem2 * t_rem) >> SCUI_SCALE_OFS;
    int32_t t2     = (t  * t) >> SCUI_SCALE_OFS;
    int32_t t3     = (t2 * t) >> SCUI_SCALE_OFS;
    
    int32_t v1 = (1 * t_rem3 * 1  * u0) >> (SCUI_SCALE_OFS * 1);
    int32_t v2 = (3 * t_rem2 * t  * u1) >> (SCUI_SCALE_OFS * 2);
    int32_t v3 = (3 * t_rem  * t2 * u2) >> (SCUI_SCALE_OFS * 2);
    int32_t v4 = (t3 * u3) >> SCUI_SCALE_OFS;
    
    return v1 + v2 + v3 + v4;
}

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_step(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o)
{
    return x < (r_i - l_i) / 2 ? l_o : r_o;
}

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_linear(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o)
{
    int32_t step = scui_map(x, l_i, r_i, 0, SCUI_SCALE_COF);
    return ((step * (r_o - l_o)) >> SCUI_SCALE_OFS) + l_o;
}

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_ease_in(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o)
{
    int32_t xval = scui_map(x, l_i, r_i, 0, SCUI_SCALE_COF);
    int32_t step = scui_bezier3(xval, 0, 50, 100, SCUI_SCALE_COF);
    return ((step * (r_o - l_o)) >> SCUI_SCALE_OFS) + l_o;
}

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_ease_out(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o)
{
    int32_t xval = scui_map(x, l_i, r_i, 0, SCUI_SCALE_COF);
    int32_t step = scui_bezier3(xval, 0, 900, 950, SCUI_SCALE_COF);
    return ((step * (r_o - l_o)) >> SCUI_SCALE_OFS) + l_o;
}

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_ease_in_out(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o)
{
    int32_t xval = scui_map(x, l_i, r_i, 0, SCUI_SCALE_COF);
    int32_t step = scui_bezier3(xval, 0, 50, 950, SCUI_SCALE_COF);
    return ((step * (r_o - l_o)) >> SCUI_SCALE_OFS) + l_o;
}

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_overshoot(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o)
{
    int32_t xval = scui_map(x, l_i, r_i, 0, SCUI_SCALE_COF);
    int32_t step = scui_bezier3(xval, 0, 1000, 1300, SCUI_SCALE_COF);
    return ((step * (r_o - l_o)) >> SCUI_SCALE_OFS) + l_o;
}

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_bounce(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o)
{
    int32_t xval = scui_map(x, l_i, r_i, 0, SCUI_SCALE_COF);
    int32_t diff = r_o - l_o;
    
    /* 3反弹有5个部分:3个向下和2个向上,一部分是t/5长 */
    
    if (xval < 408) {   /* 下降 */
        xval *= 2500;
        xval  = xval >> SCUI_SCALE_OFS;
    } else if (xval < 614) {    /* 第一次回弹 */
        xval -= 408;
        xval *= 5;
        xval  = SCUI_SCALE_COF - xval;
        diff /= 20;
    } else if (xval < 819) {    /* 后退 */
        xval -= 614;
        xval *= 5;
        diff /= 20;
    } else if (xval < 921) {    /* 第二次回弹 */
        xval -= 819;
        xval *= 10;
        xval  = SCUI_SCALE_COF - xval;
        diff /= 40;
    } else if (xval < SCUI_SCALE_COF) {    /* 后退 */
        xval -= 921;
        xval *= 10;
        diff /= 40;
    }
    
    xval = scui_clamp(xval, 0, SCUI_SCALE_COF);
    int32_t step = scui_bezier3(xval, SCUI_SCALE_COF, 800, 500, 0);
    return r_o - ((step * diff) >> SCUI_SCALE_OFS);
}

/*@brief 有序序列二分搜索
 *@param arr  元素数组
 *@param len  元素个数
 *@param size 元素大小
 *@param key  关键字
 *@param cmp  比较语义回调(似memcmp语义)
 *@retval 返回目标或空
 */
void *scui_bsearch(void *arr, uint32_t len, uint32_t size, void *key, int32_t (*cmp)(void *key, void *tar))
{
    uint8_t *mid = arr;
    
    while(len != 0) {
        
        mid += (len / 2) * size;
        int32_t ret = cmp(key, mid);
        
        if (ret > 0) {
            len = (len / 2) - ((len & 1) == 0);
            arr = (mid += size);
            continue;
        }
        if (ret < 0) {
            len /= 2;
            mid  = arr;
            continue;
        }
        
        return mid;
    }
    return NULL;
}
