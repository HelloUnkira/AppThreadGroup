#ifndef APP_SYS_MISC_H
#define APP_SYS_MISC_H

/*@brief 内存读写接口
 */
#define app_sys_mem_r(addr, type)       ((*(volatile type *)(addr)))
#define app_sys_mem_w(addr, data, type) ((*(volatile type *)(addr)) = (data))

/*@brief 泛化封装:内存字节逆向
 *       例:uint32_t data = 0;
 *          data  = 0x11223344;
 *          app_sys_mem_rev_b4(data);
 *          data == 0x44332211;
 */
void app_sys_mem_rev_b1(void *addr);
void app_sys_mem_rev_b2(void *addr);
void app_sys_mem_rev_b4(void *addr);
void app_sys_mem_rev_b8(void *addr);

/*@brief bit位操作(获取,设置,清除)
 *       泛化封装:(不建议直接使用)
 */
#define app_sys_bit_get(addr, pos, type) ((app_sys_mem_r(addr, uint##type##_t) & ((1 << (pos)) % type)) != 0)
#define app_sys_bit_set(addr, pos, type) ((app_sys_mem_w(addr, app_sys_mem_r(addr, uint##type##_t) | ( (1 << (pos % type))), uint##type##_t)))
#define app_sys_bit_rst(addr, pos, type) ((app_sys_mem_w(addr, app_sys_mem_r(addr, uint##type##_t) & (~(1 << (pos % type))), uint##type##_t)))

/*@brief bit位操作(获取,设置,清除)(扩展到任意类型数据流)
 *       例: uint32_t arr[6] = {0};              // 数据流
 *           app_sys_bit_ext_get(arr, 53, 32);  // 获取第53个bit位(操作arr[1]的53-32位)
 *           app_sys_bit_ext_set(arr, 53, 32);  // 设置第53个bit位(操作arr[1]的53-32位)
 *           app_sys_bit_ext_rst(arr, 53, 32);  // 清除第53个bit位(操作arr[1]的53-32位)
 */
#define app_sys_bit_ext_get(addr, pos, type) app_sys_bit_get((&(addr)[(pos) / type]), (pos) % type, type)
#define app_sys_bit_ext_set(addr, pos, type) app_sys_bit_set((&(addr)[(pos) / type]), (pos) % type, type)
#define app_sys_bit_ext_rst(addr, pos, type) app_sys_bit_rst((&(addr)[(pos) / type]), (pos) % type, type)

/*@brief 字节对齐
 */
void * app_sys_align_low(void *addr,   uintptr_t align);
void * app_sys_align_high(void *addr,  uintptr_t align);
bool   app_sys_align_check(void *addr, uintptr_t align);

/*@brief 条件检查,未使用参数
 */
#define app_sys_expr_cond(cond, expr)   do {if (cond) {expr;}} while (0)
#define app_sys_expr_unused(expr)       do {(void)(expr);} while (0)

/*@brief 静态断言
 */
#define app_sys_expr_assert_(expr, func, line)  \
    uint8_t expr_assert##file##line##[expr ? 1 : -1];
#define app_sys_expr_assert(expr)   app_sys_expr_assert_(expr, __func__, __LINE__)

/*@brief 从成员地址(编译解析)中计算其(结构)所有者地址
 */
#define app_sys_own_ofs(type, name, addr)    ((type *)((uintptr_t)(addr) - ((uintptr_t)&((type *)0)->name)))

/*@brief 静态数组成员数量
 */
#define app_sys_arr_len(arr)    (sizeof(arr) / sizeof((arr)[0]))

/*@brief 数组成员最小值
 */
#define app_sys_arr_min(arr, len, min)          \
do {                                            \
    min = arr[0];                               \
    for (uint32_t idx = 0; idx < len; idx++)    \
        if (min > arr[idx])                     \
            min = arr[idx];                     \
} while (0)                                     \

/*@brief 数组成员最大值
 */
#define app_sys_arr_max(arr, len, max)          \
do {                                            \
    max = arr[0];                               \
    for (uint32_t idx = 0; idx < len; idx++)    \
        if (max < arr[idx])                     \
            max = arr[idx];                     \
} while (0)                                     \

/*@brief 数组拷贝
 */
#define app_sys_arr_cpy(arr1, arr2, len)    \
    for (uint32_t idx = 0; idx < len; arr1[idx] = arr2[idx], idx++)

/*@brief 数组拷贝
 *       在C中的数组是二等公民
 *       通过结构体包装就可以获得一等公民的效果
 */
#define app_sys_arr_cpy_s(type, len, arr1, arr2)        \
do {                                                    \
    typedef struct {type arr[len];} arr_t;              \
    *(arr_t *)(arr1) = *(arr_t *)(arr2);                \
} while (0)                                             \

/*@brief 数组逆序
 */
#define app_sys_arr_rev(type, arr, len)                 \
do {                                                    \
    for (uint32_t idx0 = 0; idx0 < len / 2; idx0++) {   \
         uint32_t idx1 = len - 1 - idx0;                \
         type tmp = arr[idx0];                          \
        arr[idx0] = arr[idx1];                          \
        arr[idx1] = tmp;                                \
    }                                                   \
} while (0)                                             \

/*@brief 数组左移位(右边填充指定值)
 */
#define app_sys_arr_lsft(arr, len, cnt, val)            \
do {                                                    \
    for (int32_t idx = 0; idx + cnt < len; idx++)       \
        arr[idx] = arr[idx + cnt];                      \
    for (int32_t idx = 0; idx < cnt; idx++)             \
        arr[idx] = val;                                 \
} while (0)                                             \

/*@brief 数组右移位(左边填充指定值)
 */
#define app_sys_arr_rsft(arr, len, cnt, val)            \
do {                                                    \
    for (int32_t idx = len - 1; idx - cnt >= 0; idx--)  \
        arr[idx] = arr[idx - cnt];                      \
    for (int32_t idx = cnt - 1; idx >= 0; idx--)        \
        arr[idx] = val;                                 \
} while (0)                                             \

/*@brief 大小比较,绝对值,绝对距离
 *       泛化封装:(不建议直接使用)
 */
#define app_sys_mabs(x, m)          (((x) % (m) + (m)) % (m))
#define app_sys_abs(x)              ((x) <  0 ? -(x) : (x))
#define app_sys_min(x, y)           ((x) < (y) ? (x) : (y))
#define app_sys_max(x, y)           ((x) > (y) ? (x) : (y))
#define app_sys_dist(x, y)          ((x) > (y) ? (x) - (y) : (y) - (x))

/*@brief 区间剪切
 */
#define app_sys_clamp(x, min, max)  (app_sys_max(min, app_sys_min(max, x)))

/*@brief 包含关系
 */
#define app_sys_betw_xx(x, l, r)    ((x) >  (l) && (x) <  (r))
#define app_sys_betw_lx(x, l, r)    ((x) >= (l) && (x) <  (r))
#define app_sys_betw_xr(x, l, r)    ((x) >  (l) && (x) <= (r))
#define app_sys_betw_lr(x, l, r)    ((x) >= (l) && (x) <= (r))

/*@brief 区间映射
 */
#define app_sys_map(x, l_i, r_i, l_o, r_o)                          \
    ((l_i) <= (r_i) && (x) <= (l_i) ? (l_o) :                       \
     (l_i) <= (r_i) && (x) >= (r_i) ? (r_o) :                       \
     (l_i) >= (r_i) && (x) >= (l_i) ? (l_o) :                       \
     (l_i) >= (r_i) && (x) <= (r_i) ? (r_o) :                       \
    ((x) - (l_i)) * ((r_o) - (l_o)) / ((r_i) - (l_i)) + (l_o))      \

/*@brief 前导bit0数量
 */
#define app_sys_clz(type, data)                             \
({                                                          \
    uint8_t retval = sizeof(type) * 8;                      \
    for (uint8_t idx = 0; idx < sizeof(type) * 8; idx++)    \
        if ((1 << (sizeof(type) * 8 - 1 - idx)) & (data)) { \
            retval = idx;                                   \
            break;                                          \
        }                                                   \
    retval;                                                 \
})                                                          \

/*@brief 后缀bit0数量
 *       泛化封装:(不直接使用)
 */
#define app_sys_ctz(type, data)                             \
({                                                          \
    uint8_t retval = sizeof(type) * 8;                      \
    for (uint8_t idx = 0; idx < sizeof(type) * 8; idx++)    \
        if ((1 << (idx)) & (data)) {                        \
            retval = idx;                                   \
            break;                                          \
        }                                                   \
    retval;                                                 \
})                                                          \

/*@brief bit0的数量
 */
#define app_sys_cnt_z(type, data)                           \
({                                                          \
    uint8_t cnt = 0;                                        \
    for (uint8_t idx = 0; idx < sizeof(type) * 8; idx++)    \
        if (((1 << (idx)) & (data)) == 0) cnt++;            \
    cnt;                                                    \
})                                                          \

/*@brief bit1的数量
 */
#define app_sys_cnt_o(type, data)   (sizeof(type) - app_sys_cnt_z(type, data))

/*@brief 判断x是否等于2**x
 */
#define app_sys_pow2_check(x)   ((x) != 0 && ((x) & ((x) - 1)) == 0)

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 移动指定字符到尾部
 *@param str 字符串
 *@param c   指定字符
 *@param rev rev时移动到头部
 */
void app_sys_str_move(char *str, char c, bool rev);

/*@brief 循环左旋转字符串
 *@param str 字符串
 *@param ofs 旋转点
 *@param rev rev时循环右旋转
 */
void app_sys_str_rotate(char *str, uint32_t ofs, bool rev);

/*@brief 指定字符为分割点逆序字符串
 *@param str 字符串
 *@param c   指定字符
 */
void app_sys_str_reverse(char *str, char c);

/*@brief 将一个索引下标数组随机打乱
 *@param arr 索引下标数组
 *@param len 索引下标数组长度
 */
void app_sys_idx_shuffle(uint32_t *arr, uint32_t len);

/*@brief 计算数据流的crc32校验
 *@param data 数据流
 *@param size 数据大小
 */
uint32_t app_sys_crc32(uint8_t *data, uint32_t size);

/*@brief 计算数据流的crc8校验
 *@param data 数据流
 *@param size 数据大小
 */
uint8_t app_sys_crc8(uint8_t *data, uint32_t size);

/*@brief 计算数据流的checksum32校验
 *@param data 数据流
 *@param size 数据大小
 */
uint32_t app_sys_checksum32(void *data, uint32_t size);

/*@brief 计算定积分
 *@param fun 函数
 *@param l 左区间
 *@param r 右区间
 *@param p 精度
 *@param f 模式(1:梯形法;2:抛物线法)
 */
double app_sys_cal_di(double (*fun)(double x), double l, double r, uint32_t p, uint32_t f);

/*@brief 牛顿切线法求取方程近似解
 *@param fun0 0阶函数(原函数)
 *@param fun1 1阶函数(原函数的导数)
 *@param s 起始点
 *@param p 精度
 */
double app_sys_cal_nt(double (*fun0)(double x), double (*fun1)(double x), double s, double p);

#endif
