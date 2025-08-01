#ifndef SCUI_MATH_H
#define SCUI_MATH_H

// 快除 0xFF
#define SCUI_DIV_U255(x)    (((x) * 0x8081U) >> 0x17)
#define SCUI_DIV_0xFF(x)    (((x) * 0x8081U) >> 0x17)

// 弧度角度转换
#define SCUI_RAD_BY_A(angle)    ((angle) * 3.141592654f / 180.0f)
#define SCUI_RAD_TO_A(radian)   ((radian) * 180.0f / 3.141592654f)

/*@brief 指数函数
 *@param x 数字
 *@param e 指数
 *@retval 返回值(x^e)
 */
int64_t scui_pow(int64_t x, int8_t e);

/*@brief 随机数
 *@param limit 数字
 *@retval 返回值
 */
uint32_t scui_rand(uint32_t limit);

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

/*@brief 计算一个向量的atan2
 *@param x 向量值x(范围:(-1456, +1456))
 *@param y 向量值y(范围:(-1456, +1456))
 *@retval 根据给定参数在[0..360]范围内计算出的角度
 */
uint16_t scui_atan2(int32_t x, int32_t y);

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

/*@brief 十进制数字位数
 *@param val 数字
 *@retval 位数
 */
uint8_t scui_dec_bits(uint32_t val);

#endif
