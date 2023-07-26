#ifndef APP_EXT_SIMPLIFY_H
#define APP_EXT_SIMPLIFY_H

/*扩展简化接口
 *    可以考虑使用
 */

/*@brief 标号字符串化
 */
#define app_stringify_(lab) #lab
#define app_stringify(lab)  app_stringify_(lab)

/*@brief 标号连接
 */
#define app_concat_(lab1, lab2) lab1 ## lab2
#define app_concat(lab1, lab2)  app_concat_(lab1, lab2)

/*@brief 固定到elf指定seg
 */
#define app_section_(t1, t2, t3)    \
    __attribute__(section("." app_stringify(t1) "." app_stringify(t2) "." app_stringify(t3)))
#define app_section(t1, t2, t3) app_section_(t1, t2, t3)
#define app_section_unique(seg) app_section_(seg, __FILE__, __COUNTER__)

#define APP_ASM_USE     1
#if     APP_ASM_USE

/*@brief asm:寄存器读写
 */
#define app_asm_reg_r(r)     ({uintptr_t v; asm volatile("mrs %0, "##r : "=r" (v)); v;})
#define app_asm_reg_w(v, r)  ({asm volatile("msr " ##r## ", %x0"::"rZ"((uintptr_t)v));})

/*@brief asm...
 */
#define app_asm_bar()       asm volatile("":::"memory");
#define app_asm_wfe()		asm volatile("wfe" : : : "memory")
#define app_asm_wfi()		asm volatile("wfi" : : : "memory")
#define app_asm_isb()		asm volatile("isb" : : : "memory")
#define app_asm_dmb(opt)    asm volatile("dmb " #opt : : : "memory")
#define app_asm_dsb(opt)	asm volatile("dsb " #opt : : : "memory")

#endif

/*@brief 内存读写接口
 *       泛化封装:(不建议直接使用)
 */
#define app_mem_r(addr, type)       ((*(volatile type *)(addr)))
#define app_mem_w(addr, data, type) ((*(volatile type *)(addr)) = (data))

/*@brief 内存读接口
 *       根据数据类型选择接口,用于流程规范化使用
 */
static inline    float app_mem_r_f32(void *addr) {return app_mem_r(addr,    float);}
static inline   double app_mem_r_f64(void *addr) {return app_mem_r(addr,   double);}
static inline   int8_t app_mem_r_i8( void *addr) {return app_mem_r(addr,   int8_t);}
static inline  int16_t app_mem_r_i16(void *addr) {return app_mem_r(addr,  int16_t);}
static inline  int32_t app_mem_r_i32(void *addr) {return app_mem_r(addr,  int32_t);}
static inline  int64_t app_mem_r_i64(void *addr) {return app_mem_r(addr,  int64_t);}
static inline  uint8_t app_mem_r_u8( void *addr) {return app_mem_r(addr,  uint8_t);}
static inline uint16_t app_mem_r_u16(void *addr) {return app_mem_r(addr, uint16_t);}
static inline uint32_t app_mem_r_u32(void *addr) {return app_mem_r(addr, uint32_t);}
static inline uint64_t app_mem_r_u64(void *addr) {return app_mem_r(addr, uint64_t);}

/*@brief 内存写接口
 *       根据数据类型选择接口,用于流程规范化使用
 */
static inline void app_mem_w_f32(void *addr,    float data) {app_mem_w(addr, data,    float);}
static inline void app_mem_w_f64(void *addr,   double data) {app_mem_w(addr, data,   double);}
static inline void app_mem_w_i8( void *addr,   int8_t data) {app_mem_w(addr, data,   int8_t);}
static inline void app_mem_w_i16(void *addr,  int16_t data) {app_mem_w(addr, data,  int16_t);}
static inline void app_mem_w_i32(void *addr,  int32_t data) {app_mem_w(addr, data,  int32_t);}
static inline void app_mem_w_i64(void *addr,  int64_t data) {app_mem_w(addr, data,  int64_t);}
static inline void app_mem_w_u8( void *addr,  uint8_t data) {app_mem_w(addr, data,  uint8_t);}
static inline void app_mem_w_u16(void *addr, uint16_t data) {app_mem_w(addr, data, uint16_t);}
static inline void app_mem_w_u32(void *addr, uint32_t data) {app_mem_w(addr, data, uint32_t);}
static inline void app_mem_w_u64(void *addr, uint64_t data) {app_mem_w(addr, data, uint64_t);}

/*@brief 泛化封装:内存字节逆向
 *       例:uint32_t data = 0x11223344; app_mem_rev_b4(data); // data == 0x44332211;
 */
static inline void app_mem_rev_b1(void *addr)
{
}
static inline void app_mem_rev_b2(void *addr)
{
    app_mem_rev_b1((void *)((uint8_t *)addr + 0));
    app_mem_rev_b1((void *)((uint8_t *)addr + 1));
    
    app_mem_w_u16(addr, ((uint16_t)app_mem_r_u8((void *)((uint8_t *)addr + 0)) << 8) |
                        ((uint16_t)app_mem_r_u8((void *)((uint8_t *)addr + 1)) >> 8));
}
static inline void app_mem_rev_b4(void *addr)
{
    app_mem_rev_b2((void *)((uint16_t *)addr + 0));
    app_mem_rev_b2((void *)((uint16_t *)addr + 1));
    
    app_mem_w_u32(addr, ((uint32_t)app_mem_r_u16((void *)((uint16_t *)addr + 0)) << 16) |
                        ((uint32_t)app_mem_r_u16((void *)((uint16_t *)addr + 1)) >> 16));
}
static inline void app_mem_rev_b8(void *addr)
{
    app_mem_rev_b4((void *)((uint32_t *)addr + 0));
    app_mem_rev_b4((void *)((uint32_t *)addr + 1));
    
    app_mem_w_u64(addr, ((uint64_t)app_mem_r_u32((void *)((uint32_t *)addr + 0)) << 32) |
                        ((uint64_t)app_mem_r_u32((void *)((uint32_t *)addr + 1)) >> 32));
}

/*@brief bit位操作(获取,设置,清除)
 *       泛化封装:(不建议直接使用)
 */
#define app_bit_get(addr, pos, type) (app_mem_r_u##type(addr) & ((1 << (pos)) % type)) != 0
#define app_bit_set(addr, pos, type) (app_mem_w_u##type(addr, app_mem_r_u##type(addr) | ( (1 << (pos % type)))))
#define app_bit_rst(addr, pos, type) (app_mem_w_u##type(addr, app_mem_r_u##type(addr) & (~(1 << (pos % type)))))

/*@brief bit位操作(获取,设置,清除)(扩展到任意类型数据流)
 *       泛化封装:(不建议直接使用)
 */
#define app_bit_ext_get(addr, pos, type) app_bit_get(&((addr)[(pos) / type]), (pos) % type, type)
#define app_bit_ext_set(addr, pos, type) app_bit_set(&((addr)[(pos) / type]), (pos) % type, type)
#define app_bit_ext_rst(addr, pos, type) app_bit_rst(&((addr)[(pos) / type]), (pos) % type, type)

/*@brief bit位操作(获取,设置,清除)(扩展到任意类型数据流)
 *       例: uint32_t arr[6] = {0}; //数据流
 *           app_bit_ext_get_u32(arr, 53);  //获取第53个bit位(操作arr[1]的53-32位)
 *           app_bit_ext_set_u32(arr, 53);  //设置第53个bit位(操作arr[1]的53-32位)
 *           app_bit_ext_rst_u32(arr, 53);  //清除第53个bit位(操作arr[1]的53-32位)
 */
static inline bool app_bit_ext_get_u8(uint8_t   *addr, uintptr_t pos) {return app_bit_ext_get(addr, pos,  8);}
static inline bool app_bit_ext_get_u16(uint16_t *addr, uintptr_t pos) {return app_bit_ext_get(addr, pos, 16);}
static inline bool app_bit_ext_get_u32(uint32_t *addr, uintptr_t pos) {return app_bit_ext_get(addr, pos, 32);}
static inline bool app_bit_ext_get_u64(uint64_t *addr, uintptr_t pos) {return app_bit_ext_get(addr, pos, 64);}
static inline bool app_bit_ext_set_u8(uint8_t   *addr, uintptr_t pos) {return app_bit_ext_set(addr, pos,  8);}
static inline bool app_bit_ext_set_u16(uint16_t *addr, uintptr_t pos) {return app_bit_ext_set(addr, pos, 16);}
static inline bool app_bit_ext_set_u32(uint32_t *addr, uintptr_t pos) {return app_bit_ext_set(addr, pos, 32);}
static inline bool app_bit_ext_set_u64(uint64_t *addr, uintptr_t pos) {return app_bit_ext_set(addr, pos, 64);}
static inline bool app_bit_ext_rst_u8(uint8_t   *addr, uintptr_t pos) {return app_bit_ext_rst(addr, pos,  8);}
static inline bool app_bit_ext_rst_u16(uint16_t *addr, uintptr_t pos) {return app_bit_ext_rst(addr, pos, 16);}
static inline bool app_bit_ext_rst_u32(uint32_t *addr, uintptr_t pos) {return app_bit_ext_rst(addr, pos, 32);}
static inline bool app_bit_ext_rst_u64(uint64_t *addr, uintptr_t pos) {return app_bit_ext_rst(addr, pos, 64);}

/*@brief 大小端序通配
 *       一般好的业务流程并不需要使用大小端序通配接口
 */
//#define APP_EXT_ARCH_IS_LE
#define APP_EXT_ARCH_IS_BE

#ifndef APP_EXT_ARCH_IS_LE
#ifndef APP_EXT_ARCH_IS_BE
#error  "unknown app arch endian"
#endif
#endif

/*@brief 小端序通配
 *       一般好的业务流程并不需要使用大小端序通配接口
 */
static inline void app_to_le_u8( void *addr)
{
}
static inline void app_to_le_u16(void *addr)
{
#ifndef APP_EXT_ARCH_IS_BE
    uint16_t data_16 = app_mem_r_u16(addr);
    app_mem_w_u8((void *)((uintptr_t)addr + 0), (data_16 >> 0) && 0xFF);
    app_mem_w_u8((void *)((uintptr_t)addr + 1), (data_16 >> 8) && 0xFF);
    app_to_le_u8((void *)((uintptr_t)addr + 0));
    app_to_le_u8((void *)((uintptr_t)addr + 1));
#endif
}
static inline void app_to_le_u32(void *addr)
{
#ifndef APP_EXT_ARCH_IS_BE
    uint32_t data_32 = app_mem_r_u32(addr);
    app_mem_w_u16((void *)((uintptr_t)addr + 0), (data_32 >>  0) && 0xFFFF);
    app_mem_w_u16((void *)((uintptr_t)addr + 2), (data_32 >> 16) && 0xFFFF);
    app_to_le_u16((void *)((uintptr_t)addr + 0));
    app_to_le_u16((void *)((uintptr_t)addr + 2));
#endif
}
static inline void app_to_le_u64(void *addr)
{
#ifndef APP_EXT_ARCH_IS_BE
    uint64_t data_64 = app_mem_r_u64(addr);
    app_mem_w_u32((void *)((uintptr_t)addr + 0), (data_64 >>  0) && 0xFFFFFFFF);
    app_mem_w_u32((void *)((uintptr_t)addr + 4), (data_64 >> 32) && 0xFFFFFFFF);
    app_to_le_u32((void *)((uintptr_t)addr + 0));
    app_to_le_u32((void *)((uintptr_t)addr + 4));
#endif
}

/*@brief 大端序通配
 *       一般好的业务流程并不需要使用大小端序通配接口
 */
static inline void app_to_be_u8( void *addr)
{
}
static inline void app_to_be_u16(void *addr)
{
#ifndef APP_EXT_ARCH_IS_LE
    uint16_t data_16 = app_mem_r_u16(addr);
    app_mem_w_u8((void *)((uintptr_t)addr + 1), (data_16 >> 0) && 0xFF);
    app_mem_w_u8((void *)((uintptr_t)addr + 0), (data_16 >> 8) && 0xFF);
    app_to_be_u8((void *)((uintptr_t)addr + 1));
    app_to_be_u8((void *)((uintptr_t)addr + 0));
#endif
}
static inline void app_to_be_u32(void *addr)
{
#ifndef APP_EXT_ARCH_IS_LE
    uint32_t data_32 = app_mem_r_u32(addr);
    app_mem_w_u16((void *)((uintptr_t)addr + 2), (data_32 >>  0) && 0xFFFF);
    app_mem_w_u16((void *)((uintptr_t)addr + 0), (data_32 >> 16) && 0xFFFF);
    app_to_be_u16((void *)((uintptr_t)addr + 2));
    app_to_be_u16((void *)((uintptr_t)addr + 0));
#endif
}
static inline void app_to_be_u64(void *addr)
{
#ifndef APP_EXT_ARCH_IS_LE
    uint64_t data_64 = app_mem_r_u64(addr);
    app_mem_w_u32((void *)((uintptr_t)addr + 4), (data_64 >>  0) && 0xFFFFFFFF);
    app_mem_w_u32((void *)((uintptr_t)addr + 0), (data_64 >> 32) && 0xFFFFFFFF);
    app_to_be_u32((void *)((uintptr_t)addr + 4));
    app_to_be_u32((void *)((uintptr_t)addr + 0));
#endif
}

/*@brief 字节对齐
 */
static inline uintptr_t app_align_val(void)         {return sizeof(uintptr_t);}
static inline uintptr_t app_align_none(void *addr)  {return (uintptr_t)(addr);}
static inline uintptr_t app_align_off(void *addr)   {return app_align_none(addr) % app_align_val();}
static inline uintptr_t app_align_base(void *addr)  {return app_align_none(addr) - app_align_off(addr);}
static inline uintptr_t app_align_low(void *addr)   {return app_align_base(addr);}
static inline uintptr_t app_align_high(void *addr)  {return app_align_base(addr) + app_align_val();}
static inline bool      app_align_check(void *addr) {return app_align_off(addr) == 0;}

/*@brief 字符字节简化操作
 */
static inline bool app_char_is_upper(char C) {return ('A' <= C && C <= 'Z');}
static inline bool app_char_is_lower(char C) {return ('a' <= C && C <= 'z');}
static inline bool app_char_is_digit(char C) {return ('0' <= C && C <= '9');}
static inline bool app_char_is_uphex(char C) {return ('A' <= C && C <= 'F');}
static inline bool app_char_is_lohex(char C) {return ('a' <= C && C <= 'f');}
static inline bool app_char_is_print(char C) {return (' ' <= C && C <= '~');}  /* Printable */
static inline bool app_char_is_graph(char C) {return ('!' <= C && C <= '~');}  /* Printable(Except Space) */
static inline bool app_char_is_blank(char C) {return (' ' == C || '\t' == C || '\v' == C);}
static inline bool app_char_is_space(char C) {return (' ' == C || C - '\t' <= 4);} /* Spaceable */
static inline bool app_char_is_alpha(char C) {return (app_char_is_upper(C) || app_char_is_lower(C));}
static inline bool app_char_is_alnum(char C) {return (app_char_is_upper(C) || app_char_is_lower(C) || app_char_is_digit(C));}
static inline bool app_char_is_adhex(char C) {return (app_char_is_uphex(C) || app_char_is_lohex(C) || app_char_is_digit(C));}
static inline bool app_char_is_punct(char C) {return (app_char_is_print(C) && !app_char_is_alnum(C) && ! app_char_is_space(C));} /* Punctuation */
static inline char app_char_to_upper(char C) {return (app_char_is_lower(C) ? C - 'a' + 'A' : C);}
static inline char app_char_to_lower(char C) {return (app_char_is_upper(C) ? C - 'A' + 'a' : C);}
static inline bool app_char_is_ascii(char C) {return ((C & 0x80) == 0);}
static inline char app_char_to_ascii(char C) {return ((C & 0x7F));}

static inline char app_char_hex_to_alnum(uint8_t C) {return app_char_is_adhex(C) ? C < 10 ? C + '0' : C - 10 + 'A' : 0xFF;}
static inline uint8_t app_char_alnum_to_hex(char C) {return app_char_is_digit(C) ? C - '0' : app_char_is_upper(C) ? C - 'A' + 10 :
                                                                                             app_char_is_lower(C) ? C - 'a' + 10 : 0xFF;}

/*@brief 未使用参数
 */
#define app_expr_unused(expr)  do {(volatile void *)(expr);} while (0)

/*@brief 条件检查
 */
#define app_expr_cond(cond, expr)  if (cond) do {expr;} while (0)

/*@brief 从成员地址(编译解析)中计算其(结构)所有者地址
 */
#define app_owner_calc(type, name, addr)    ((type *)((uintptr_t)(addr) - ((uintptr_t)&((type *)0)->name)))

/*@brief 静态数组成员数量
 */
#define app_arr_len(arr)    (sizeof(arr) / sizeof(arr[0]))

/*@brief 数组拷贝
 */
#define app_arr_cpy(arr1, arr2, len)    \
    for (uint32_t idx = 0; idx < len; arr1[idx] = arr2[idx], idx++)

/*@brief 数组拷贝
 *       在C中的数组是二等公民
 *       通过结构体包装就可以获得一等公民的效果
 */
#define app_arr_bcpy(type, len, arr1, arr2)     \
    do {typedef struct {type arr[len];} arr; *(arr *)(arr1) = *(arr *)(arr2);} while (0)

/*@brief 数组逆序
 */
#define app_arr_rev(type, arr, len)                         \
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
#define app_arr_lshift(arr, len, cnt, val)                  \
    do {                                                    \
        for (int32_t idx = 0; idx + cnt < len; idx++)       \
            arr[idx] = arr[idx + cnt];                      \
        for (int32_t idx = 0; idx < cnt; idx++)             \
            arr[idx] = val;                                 \
    } while (0)                                             \

/*@brief 数组右移位(左边填充指定值)
 */
#define app_arr_rshift(arr, len, cnt, val)                  \
    do {                                                    \
        for (int32_t idx = len - 1; idx - cnt >= 0; idx--)  \
            arr[idx] = arr[idx - cnt];                      \
        for (int32_t idx = cnt - 1; idx >= 0; idx--)        \
            arr[idx] = val;                                 \
    } while (0)                                             \

/*@brief 大小比较,绝对值
 *       泛化封装:(不建议直接使用)
 */
#define app_min(x, y)   ((x) < (y) ? (x) : (y))
#define app_max(x, y)   ((x) > (y) ? (x) : (y))
#define app_abs(x)      ((x) <  0 ? -(x) : (x))

/*@brief 最小值
 */
static inline    float app_min_f32(   float x,    float y) {return app_min(x, y);}
static inline   double app_min_f64(  double x,   double y) {return app_min(x, y);}
static inline   int8_t app_min_i8(   int8_t x,   int8_t y) {return app_min(x, y);}
static inline  int16_t app_min_i16( int16_t x,  int16_t y) {return app_min(x, y);}
static inline  int32_t app_min_i32( int32_t x,  int32_t y) {return app_min(x, y);}
static inline  int64_t app_min_i64( int64_t x,  int64_t y) {return app_min(x, y);}
static inline  uint8_t app_min_u8(  uint8_t x,  uint8_t y) {return app_min(x, y);}
static inline uint16_t app_min_u16(uint16_t x, uint16_t y) {return app_min(x, y);}
static inline uint32_t app_min_u32(uint32_t x, uint32_t y) {return app_min(x, y);}
static inline uint64_t app_min_u64(uint64_t x, uint64_t y) {return app_min(x, y);}

/*@brief 最大值
 */
static inline    float app_max_f32(   float x,    float y) {return app_max(x, y);}
static inline   double app_max_f64(  double x,   double y) {return app_max(x, y);}
static inline   int8_t app_max_i8(   int8_t x,   int8_t y) {return app_max(x, y);}
static inline  int16_t app_max_i16( int16_t x,  int16_t y) {return app_max(x, y);}
static inline  int32_t app_max_i32( int32_t x,  int32_t y) {return app_max(x, y);}
static inline  int64_t app_max_i64( int64_t x,  int64_t y) {return app_max(x, y);}
static inline  uint8_t app_max_u8(  uint8_t x,  uint8_t y) {return app_max(x, y);}
static inline uint16_t app_max_u16(uint16_t x, uint16_t y) {return app_max(x, y);}
static inline uint32_t app_max_u32(uint32_t x, uint32_t y) {return app_max(x, y);}
static inline uint64_t app_max_u64(uint64_t x, uint64_t y) {return app_max(x, y);}

/*@brief 绝对值
 */
static inline    float app_abs_f32(  float x) {return app_abs(x);}
static inline   double app_abs_f64( double x) {return app_abs(x);}
static inline   int8_t app_abs_i8(  int8_t x) {return app_abs(x);}
static inline  int16_t app_abs_i16(int16_t x) {return app_abs(x);}
static inline  int32_t app_abs_i32(int32_t x) {return app_abs(x);}
static inline  int64_t app_abs_i64(int64_t x) {return app_abs(x);}
static inline  uint8_t app_abs_u8(  int8_t x) {return app_abs(x);}
static inline uint16_t app_abs_u16(int16_t x) {return app_abs(x);}
static inline uint32_t app_abs_u32(int32_t x) {return app_abs(x);}
static inline uint64_t app_abs_u64(int64_t x) {return app_abs(x);}

/*@brief 区间映射
 *       泛化封装:(不建议直接使用)
 */
#define app_map(x, l_i, r_i, l_o, r_o)  ((((r_o) - (l_o)) / ((r_i) - (l_i))) * ((x) - (l_i)) + (l_o))

/*@brief 区间映射
 */
static inline  int8_t app_map_i8(  int8_t x,  int8_t l_i,  int8_t l_o,  int8_t r_i,  int8_t r_o) {return app_map(x, l_i, r_i, l_o, r_o);}
static inline int16_t app_map_i16(int16_t x, int16_t l_i, int16_t l_o, int16_t r_i, int16_t r_o) {return app_map(x, l_i, r_i, l_o, r_o);}
static inline int32_t app_map_i32(int32_t x, int32_t l_i, int32_t l_o, int32_t r_i, int32_t r_o) {return app_map(x, l_i, r_i, l_o, r_o);}
static inline int64_t app_map_i64(int64_t x, int64_t l_i, int64_t l_o, int64_t r_i, int64_t r_o) {return app_map(x, l_i, r_i, l_o, r_o);}

/*@brief 前导bit0数量
 *       泛化封装:(不直接使用)
 */
#define app_clz_func(type, data)                            \
{                                                           \
    for (uint8_t idx = 0; idx < sizeof(type) * 8; idx++)    \
        if ((1 << (sizeof(type) * 8 - 1 - idx)) & data)     \
            return idx;                                     \
            return sizeof(type) * 8;                        \
}                                                           \

/*@brief 前导bit0数量
 */
static inline  uint8_t app_clz_b1( uint8_t data) app_clz_func( uint8_t, data)
static inline uint16_t app_clz_b2(uint16_t data) app_clz_func(uint16_t, data)
static inline uint32_t app_clz_b4(uint32_t data) app_clz_func(uint32_t, data)
static inline uint64_t app_clz_b8(uint64_t data) app_clz_func(uint64_t, data)

/*@brief 后缀bit0数量
 *       泛化封装:(不直接使用)
 */
#define app_ctz_func(type, data)                            \
{                                                           \
    for (uint8_t idx = 0; idx < sizeof(type) * 8; idx++)    \
        if ((1 << (idx)) & data)                            \
            return idx;                                     \
            return sizeof(type) * 8;                        \
}                                                           \

/*@brief 后缀bit0数量
 */
static inline  uint8_t app_ctz_b1( uint8_t data) app_ctz_func( uint8_t, data)
static inline uint16_t app_ctz_b2(uint16_t data) app_ctz_func(uint16_t, data)
static inline uint32_t app_ctz_b4(uint32_t data) app_ctz_func(uint32_t, data)
static inline uint64_t app_ctz_b8(uint64_t data) app_ctz_func(uint64_t, data)

/*@brief bit0的数量
 *       泛化封装:(不直接使用)
 */
#define app_cnt_z_func(type, data)                          \
{                                                           \
    uint8_t cnt = 0;                                        \
    for (uint8_t idx = 0; idx < sizeof(type) * 8; idx++)    \
        if (((1 << (idx)) & data) == 0) cnt++;              \
    return cnt;                                             \
}                                                           \

/*@brief bit0的数量
 *       bit1的数量
 */
static inline uint8_t app_cnt_z_b1( uint8_t data) app_cnt_z_func( uint8_t, data)
static inline uint8_t app_cnt_z_b2(uint16_t data) app_cnt_z_func(uint16_t, data)
static inline uint8_t app_cnt_z_b4(uint32_t data) app_cnt_z_func(uint32_t, data)
static inline uint8_t app_cnt_z_b8(uint64_t data) app_cnt_z_func(uint64_t, data)
static inline uint8_t app_cnt_o_b1( uint8_t data) {return sizeof( uint8_t) - app_cnt_z_b1(data);}
static inline uint8_t app_cnt_o_b2(uint16_t data) {return sizeof(uint16_t) - app_cnt_z_b2(data);}
static inline uint8_t app_cnt_o_b4(uint32_t data) {return sizeof(uint32_t) - app_cnt_z_b4(data);}
static inline uint8_t app_cnt_o_b8(uint64_t data) {return sizeof(uint64_t) - app_cnt_z_b8(data);}

/*@brief keep add...
 */

#endif
