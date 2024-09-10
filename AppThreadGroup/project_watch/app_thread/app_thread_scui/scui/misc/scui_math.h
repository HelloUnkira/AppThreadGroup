#ifndef SCUI_MATH_H
#define SCUI_MATH_H

/*@brief 弧度角度转换
 *@param angle 角度
 *@retval 弧度
 */
double scui_radian_by_angle(double angle);

/*@brief 弧度角度转换
 *@param radian 弧度
 *@retval 角度
 */
double scui_radian_to_angle(double radian);

/*@brief 三角函数(sin)
 *@param angle 弧度
 *@retval 三角函数值
 */
double scui_sin(double radian);

/*@brief 三角函数(cos)
 *@param angle 弧度
 *@retval 三角函数值
 */
double scui_cos(double radian);

/*@brief 三角函数(tan)
 *@param angle 弧度
 *@retval 三角函数值
 */
double scui_tan(double radian);

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

/*@brief 三角函数(sin),放大4096倍
 *@param angle 角度
 *@retval 三角函数值
 */
int32_t scui_sin4096(int32_t angle);

/*@brief 三角函数(cos),放大4096倍
 *@param angle 角度
 *@retval 三角函数值
 */
int32_t scui_cos4096(int32_t angle);

/*@brief 三角函数(tan),放大4096倍
 *@param angle 角度
 *@retval 三角函数值
 */
int32_t scui_tan4096(int32_t angle);

/*@brief 平方根
 *@param x 数字
 *@param q_i 平方根整数部
 *@param q_f 平方根小数部
 *@param mask: (root,mask):<16,0x80>;<256,0x800>;<xxx,0x8000>;
 */
void scui_sqrt(int32_t x, int32_t *q_i, int32_t *q_f, int32_t mask);

/*@brief 计算一个三次Bezier函数的值。
* @param t  参数t时间范围[0..SCUI_SCALE_COF]
* @param u0 起始值0值范围[0..SCUI_SCALE_COF]
* @param u1 控制值1值范围[0..SCUI_SCALE_COF]
* @param u2 控制值2值范围[0..SCUI_SCALE_COF]
* @param u3 结束值3值范围[0..SCUI_SCALE_COF]
* @retval 给定参数在范围内计算的值
*/
int32_t scui_bezier3(int32_t t, int32_t u0, int32_t u1, int32_t u2, int32_t u3);

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
typedef int32_t (*scui_map_cb_t)(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o);

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_step(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o);

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_linear(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o);

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_ease_in(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o);

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_ease_out(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o);

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_ease_in_out(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o);

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_overshoot(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o);

/*@brief 路径映射(scui_map)多态(参数值,返回值)
 */
int32_t scui_map_bounce(int32_t x, int32_t l_i, int32_t r_i, int32_t l_o, int32_t r_o);

/*@brief 有序序列二分搜索
 *@param arr  元素数组
 *@param len  元素个数
 *@param size 元素大小
 *@param key  关键字
 *@param cmp  比较语义回调(似memcmp语义)
 *@retval 返回目标或空
 */
void * scui_bsearch(void *arr, uint32_t len, uint32_t size, void *key, int32_t (*cmp)(void *key, void *tar));

#endif
