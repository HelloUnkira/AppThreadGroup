#ifndef SCUI_MATH_H
#define SCUI_MATH_H

/*@brief 三角函数(sin),放大4096倍
 *@param angle 角度
 *@retval 三级函数值
 */
int32_t scui_sin4096(int32_t angle);

/*@brief 三角函数(cos),放大4096倍
 *@param angle 角度
 *@retval 三级函数值
 */
int32_t scui_cos4096(int32_t angle);

/*@brief 三角函数(tan),放大4096倍
 *@param angle 角度
 *@retval 三级函数值
 */
int32_t scui_tan4096(int32_t angle);

/*@brief 平方根
 *@param x 数字
 *@param q_i 平方根整数部
 *@param q_f 平方根小数部
 *@param mask: (root,mask):<16,0x80>;<256,0x800>;<xxx,0x8000>;
 */
void scui_sqrt(int32_t x, int32_t *q_i, int32_t *q_f, int32_t mask);

#define SCUI_BEZIER_VAL_MAX   1024
#define SCUI_BEZIER_VAL_SHIFT   10

/*@brief 计算一个三次Bezier函数的值。
* @param t  参数t时间范围[0..SCUI_BEZIER_VAL_MAX]
* @param u0 起始值0值范围[0..SCUI_BEZIER_VAL_MAX]
* @param u1 控制值1值范围[0..SCUI_BEZIER_VAL_MAX]
* @param u2 控制值2值范围[0..SCUI_BEZIER_VAL_MAX]
* @param u3 结束值3值范围[0..SCUI_BEZIER_VAL_MAX]
* @retval 给定参数在范围内计算的值
*/
int32_t scui_bezier3(int32_t t, int32_t u0, int32_t u1, int32_t u2, int32_t u3);

/*@brief 指数函数
 *@param x 数字
 *@param e 指数
 *@retval 返回值(x^e)
 */
int64_t scui_pow(int64_t x, int8_t e);

/*@brief 随机数
 *@param min 数字
 *@param max 数字
 *@retval 返回值
 */
int64_t scui_rand(int64_t min, int64_t max);

#endif
