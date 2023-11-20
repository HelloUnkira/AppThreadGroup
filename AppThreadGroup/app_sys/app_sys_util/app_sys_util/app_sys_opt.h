#ifndef APP_SYS_OPT_H
#define APP_SYS_OPT_H

/*扩展简化接口
 *    可以考虑使用
 */

/*@brief 标号字符串化
 */
#define app_sys_stringify_(lab) #lab
#define app_sys_stringify(lab)  app_sys_stringify_(lab)

/*@brief 标号连接
 */
#define app_sys_concat_(lab1, lab2) lab1 ## lab2
#define app_sys_concat(lab1, lab2)  app_sys_concat_(lab1, lab2)

/*@brief 固定到elf指定seg
 */
#define app_sys_section_(t1, t2, t3)    \
    __attribute__(section("." app_sys_stringify(t1) "." app_sys_stringify(t2) "." app_sys_stringify(t3)))
#define app_sys_section(t1, t2, t3) app_sys_section_(t1, t2, t3)
#define app_sys_section_unique(seg) app_sys_section_(seg, __FILE__, __COUNTER__)

#define APP_EXT_ASM_USE     1
#if     APP_EXT_ASM_USE

/*@brief asm:寄存器读写
 */
#define app_sys_asm_reg_r(r)     ({uintptr_t v; asm volatile("mrs %0, "##r : "=r" (v)); v;})
#define app_sys_asm_reg_w(v, r)  ({asm volatile("msr " ##r## ", %x0"::"rZ"((uintptr_t)v));})

/*@brief asm...
 */
#define app_sys_asm_bar()       asm volatile("":::"memory");
#define app_sys_asm_wfe()		asm volatile("wfe" : : : "memory")
#define app_sys_asm_wfi()		asm volatile("wfi" : : : "memory")
#define app_sys_asm_isb()		asm volatile("isb" : : : "memory")
#define app_sys_asm_dmb(opt)    asm volatile("dmb " #opt : : : "memory")
#define app_sys_asm_dsb(opt)	asm volatile("dsb " #opt : : : "memory")

#endif

/*@brief 内存读写接口
 */
#define app_sys_mem_r(addr, type)       ((*(volatile type *)(addr)))
#define app_sys_mem_w(addr, data, type) ((*(volatile type *)(addr)) = (data))

/*@brief 泛化封装:内存字节逆向
 *       例:uint32_t data = 0x11223344; app_sys_mem_rev_b4(data); // data == 0x44332211;
 */
static inline void app_sys_mem_rev_b1(void *addr)
{
}
static inline void app_sys_mem_rev_b2(void *addr)
{
    for (int8_t idx1 = 0, idx2 = 1; idx1 < idx2; idx1++, idx2--) {
        uint8_t half_1 = app_sys_mem_r((uint8_t *)addr + idx1, uint8_t);
        uint8_t half_2 = app_sys_mem_r((uint8_t *)addr + idx2, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx2, half_1, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx1, half_2, uint8_t);
    }
}
static inline void app_sys_mem_rev_b4(void *addr)
{
    for (int8_t idx1 = 0, idx2 = 3; idx1 < idx2; idx1++, idx2--) {
        uint8_t half_1 = app_sys_mem_r((uint8_t *)addr + idx1, uint8_t);
        uint8_t half_2 = app_sys_mem_r((uint8_t *)addr + idx2, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx2, half_1, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx1, half_2, uint8_t);
    }
}
static inline void app_sys_mem_rev_b8(void *addr)
{
    for (int8_t idx1 = 0, idx2 = 7; idx1 < idx2; idx1++, idx2--) {
        uint8_t half_1 = app_sys_mem_r((uint8_t *)addr + idx1, uint8_t);
        uint8_t half_2 = app_sys_mem_r((uint8_t *)addr + idx2, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx2, half_1, uint8_t);
        app_sys_mem_w((uint8_t *)addr + idx1, half_2, uint8_t);
    }
}

/*@brief bit位操作(获取,设置,清除)
 *       泛化封装:(不建议直接使用)
 */
#define app_sys_bit_get(addr, pos, type) (app_sys_mem_r(addr, uint##type##_t) & ((1 << (pos)) % type) != 0)
#define app_sys_bit_set(addr, pos, type) (app_sys_mem_w(addr, app_sys_mem_r(addr, uint##type##_t) | ( (1 << (pos % type))), uint##type##_t))
#define app_sys_bit_rst(addr, pos, type) (app_sys_mem_w(addr, app_sys_mem_r(addr, uint##type##_t) & (~(1 << (pos % type))), uint##type##_t))

/*@brief bit位操作(获取,设置,清除)(扩展到任意类型数据流)
 *       例: uint32_t arr[6] = {0};             // 数据流
 *           app_sys_bit_ext_get(arr, 53, 32);  // 获取第53个bit位(操作arr[1]的53-32位)
 *           app_sys_bit_ext_set(arr, 53, 32);  // 设置第53个bit位(操作arr[1]的53-32位)
 *           app_sys_bit_ext_rst(arr, 53, 32);  // 清除第53个bit位(操作arr[1]的53-32位)
 */
#define app_sys_bit_ext_get(addr, pos, type) app_sys_bit_get((&(addr)[(pos) / type]), (pos) % type, type)
#define app_sys_bit_ext_set(addr, pos, type) app_sys_bit_set((&(addr)[(pos) / type]), (pos) % type, type)
#define app_sys_bit_ext_rst(addr, pos, type) app_sys_bit_rst((&(addr)[(pos) / type]), (pos) % type, type)

/*@brief 大小端序(接口使用需与平台字节对齐)(默认小端序)
 *       一般好的业务流程并不需要使用大小端序通配接口
 */
#ifndef APP_EXT_ARCH_IS_BE
static inline void app_sys_to_le_b1(void *addr) {app_sys_mem_rev_b1(addr);}
static inline void app_sys_to_le_b2(void *addr) {app_sys_mem_rev_b2(addr);}
static inline void app_sys_to_le_b4(void *addr) {app_sys_mem_rev_b4(addr);}
static inline void app_sys_to_le_b8(void *addr) {app_sys_mem_rev_b8(addr);}
static inline void app_sys_to_be_b1(void *addr) {}
static inline void app_sys_to_be_b2(void *addr) {}
static inline void app_sys_to_be_b4(void *addr) {}
static inline void app_sys_to_be_b8(void *addr) {}
#else
static inline void app_sys_to_le_b1(void *addr) {}
static inline void app_sys_to_le_b2(void *addr) {}
static inline void app_sys_to_le_b4(void *addr) {}
static inline void app_sys_to_le_b8(void *addr) {}
static inline void app_sys_to_be_b1(void *addr) {app_sys_mem_rev_b1(addr);}
static inline void app_sys_to_be_b2(void *addr) {app_sys_mem_rev_b2(addr);}
static inline void app_sys_to_be_b4(void *addr) {app_sys_mem_rev_b4(addr);}
static inline void app_sys_to_be_b8(void *addr) {app_sys_mem_rev_b8(addr);}
#endif

/*@brief 字节对齐
 */
static inline void * app_sys_align_low(  void *addr, uintptr_t align) {return (void *)(((uintptr_t)addr + (        0)) & ~(align - 1));}
static inline void * app_sys_align_high( void *addr, uintptr_t align) {return (void *)(((uintptr_t)addr + (align - 1)) & ~(align - 1));}
static inline bool   app_sys_align_check(void *addr, uintptr_t align) {return (bool  )(((uintptr_t)addr % (align)) == 0);}

/*@brief 字符字节简化操作
 */
static inline bool app_sys_char_is_upper(char C) {return ('A' <= C && C <= 'Z');}
static inline bool app_sys_char_is_lower(char C) {return ('a' <= C && C <= 'z');}
static inline bool app_sys_char_is_digit(char C) {return ('0' <= C && C <= '9');}
static inline bool app_sys_char_is_uphex(char C) {return ('A' <= C && C <= 'F');}
static inline bool app_sys_char_is_lohex(char C) {return ('a' <= C && C <= 'f');}
static inline bool app_sys_char_is_print(char C) {return (' ' <= C && C <= '~');}  /* Printable */
static inline bool app_sys_char_is_graph(char C) {return ('!' <= C && C <= '~');}  /* Printable(Except Space) */
static inline bool app_sys_char_is_blank(char C) {return (' ' == C || '\t' == C || '\v' == C);}
static inline bool app_sys_char_is_space(char C) {return (' ' == C || C - '\t' <= 4);} /* Spaceable */
static inline bool app_sys_char_is_alpha(char C) {return (app_sys_char_is_upper(C) || app_sys_char_is_lower(C));}
static inline bool app_sys_char_is_alnum(char C) {return (app_sys_char_is_upper(C) || app_sys_char_is_lower(C) || app_sys_char_is_digit(C));}
static inline bool app_sys_char_is_adhex(char C) {return (app_sys_char_is_uphex(C) || app_sys_char_is_lohex(C) || app_sys_char_is_digit(C));}
static inline bool app_sys_char_is_punct(char C) {return (app_sys_char_is_print(C) && !app_sys_char_is_alnum(C) && ! app_sys_char_is_space(C));} /* Punctuation */
static inline char app_sys_char_to_upper(char C) {return (app_sys_char_is_lower(C) ? C - 'a' + 'A' : C);}
static inline char app_sys_char_to_lower(char C) {return (app_sys_char_is_upper(C) ? C - 'A' + 'a' : C);}
static inline bool app_sys_char_is_ascii(char C) {return ((C & 0x80) == 0);}
static inline char app_sys_char_to_ascii(char C) {return ((C & 0x7F));}

static inline char app_sys_char_hex_to_alnum(uint8_t C) {return app_sys_char_is_adhex(C) ? C < 10 ? C + '0' : C - 10 + 'A' : 0xFF;}
static inline uint8_t app_sys_char_alnum_to_hex(char C) {return app_sys_char_is_digit(C) ? C - '0' : app_sys_char_is_upper(C) ? C - 'A' + 10 :
                                                                                             app_sys_char_is_lower(C) ? C - 'a' + 10 : 0xFF;}

/*@brief 未使用参数
 */
#define app_sys_expr_unused(expr)  do {(volatile void *)(expr);} while (0)

/*@brief 静态断言
 */
#define app_sys_expr_assert_(expr, func, line)  \
    uint8_t expr_assert##file##line##[expr ? 1 : -1];
#define app_sys_expr_assert(expr)   app_sys_expr_assert_(expr, __func__, __LINE__)

/*@brief 条件检查
 */
#define app_sys_expr_cond(cond, expr)  if (cond) do {expr;} while (0)

/*@brief 从成员地址(编译解析)中计算其(结构)所有者地址
 */
#define app_sys_own_ofs(type, name, addr)    ((type *)((uintptr_t)(addr) - ((uintptr_t)&((type *)0)->name)))

/*@brief 静态数组成员数量
 */
#define app_sys_arr_len(arr)    (sizeof(arr) / sizeof(arr[0]))

/*@brief 数组拷贝
 */
#define app_sys_arr_cpy(arr1, arr2, len)    \
    for (uint32_t idx = 0; idx < len; arr1[idx] = arr2[idx], idx++)

/*@brief 数组拷贝
 *       在C中的数组是二等公民
 *       通过结构体包装就可以获得一等公民的效果
 */
#define app_sys_arr_bcpy(type, len, arr1, arr2)     \
    do {typedef struct {type arr[len];} arr; *(arr *)(arr1) = *(arr *)(arr2);} while (0)

/*@brief 数组逆序
 */
#define app_sys_arr_rev(type, arr, len)                     \
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
#define app_sys_arr_lsft(arr, len, cnt, val)                \
    do {                                                    \
        for (int32_t idx = 0; idx + cnt < len; idx++)       \
            arr[idx] = arr[idx + cnt];                      \
        for (int32_t idx = 0; idx < cnt; idx++)             \
            arr[idx] = val;                                 \
    } while (0)                                             \

/*@brief 数组右移位(左边填充指定值)
 */
#define app_sys_arr_rsft(arr, len, cnt, val)                \
    do {                                                    \
        for (int32_t idx = len - 1; idx - cnt >= 0; idx--)  \
            arr[idx] = arr[idx - cnt];                      \
        for (int32_t idx = cnt - 1; idx >= 0; idx--)        \
            arr[idx] = val;                                 \
    } while (0)                                             \

/*@brief 大小比较,绝对值,绝对距离
 *       泛化封装:(不建议直接使用)
 */
#define app_sys_abs(x)          ((x) <  0 ? -(x) : (x))
#define app_sys_min(x, y)       ((x) < (y) ? (x) : (y))
#define app_sys_max(x, y)       ((x) > (y) ? (x) : (y))
#define app_sys_dist(x, y)      ((x) > (y) ? (x) - (y) : (y) - (x))
#define app_sys_mod_abs(x, m)   (((x) % (m) + (m)) % (m))

/*@brief 区间映射
 */
#define app_sys_map(x, l_i, r_i, l_o, r_o)  (((x) - (l_i)) * ((r_o) - (l_o)) / ((r_i) - (l_i)) + (l_o))

/*@brief 前导bit0数量
 */
#define app_sys_clz(type, data)                             \
({                                                          \
    uint8_t retval = sizeof(type) * 8;                      \
    for (uint8_t idx = 0; idx < sizeof(type) * 8; idx++)    \
        if ((1 << (sizeof(type) * 8 - 1 - idx)) & data) {   \
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
        if ((1 << (idx)) & data) {                          \
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
        if (((1 << (idx)) & data) == 0) cnt++;              \
    cnt;                                                    \
})                                                          \

/*@brief bit1的数量
 */
#define app_sys_cnt_o(type, data)   (sizeof(type) - app_sys_cnt_z(type, data))

/*@brief 判断x是否等于2**x
 */
#define app_sys_pow2check(x)    ((x) != 0 && ((x) & ((x) - 1)) == 0)

/*@brief 求size最近2**k
 */
static inline uintptr_t app_sys_pow2near(uintptr_t size)
{
    /* 求size最近2**x */
    uint64_t pow2 = (uint64_t)size - 1;
    pow2 |= pow2 >> 1;
    pow2 |= pow2 >> 2;
    pow2 |= pow2 >> 4;
    pow2 |= pow2 >> 8;
    pow2 |= pow2 >> 16;
    pow2 |= pow2 >> 32;
    pow2  = pow2 < (uint64_t)size ? pow2 << 1 : pow2;
    return (uintptr_t)pow2;
}

/*@brief keep add...
 */

#endif
