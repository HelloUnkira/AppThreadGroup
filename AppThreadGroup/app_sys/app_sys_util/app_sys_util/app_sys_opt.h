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

/*@brief asm:寄存器读写
 */
#define app_sys_asm_reg_r(r)     ({uintptr_t v; asm volatile("mrs %0, "##r : "=r" (v)); v;})
#define app_sys_asm_reg_w(v, r)  ({asm volatile("msr " ##r## ", %x0"::"rZ"((uintptr_t)v));})

/*@brief asm...
 */
#define app_sys_asm_bar()       asm volatile("":::"memory");
#define app_sys_asm_wfe()        asm volatile("wfe" : : : "memory")
#define app_sys_asm_wfi()        asm volatile("wfi" : : : "memory")
#define app_sys_asm_isb()        asm volatile("isb" : : : "memory")
#define app_sys_asm_dmb(opt)    asm volatile("dmb " #opt : : : "memory")
#define app_sys_asm_dsb(opt)    asm volatile("dsb " #opt : : : "memory")

/* 可选择使用的错误码 */
typedef enum {
    app_sys_err_code_success,             ///< success
    /*  */
    //operation
    app_sys_err_code_internal,            ///< internal error
    app_sys_err_code_forbidden,           ///< operation forbidden
    app_sys_err_code_unsupported,         ///< operation unsupported
    app_sys_err_code_timeout,             ///< operation timed out
    app_sys_err_code_busy,                ///< operation busy
    app_sys_err_code_resources_leak,      ///< operation resources leak
    app_sys_err_code_not_found,           ///< operation not found
    app_sys_err_code_not_space,           ///< operation not space
    app_sys_err_code_not_mem,             ///< operation not memory
    //operation argument
    app_sys_err_code_null,                ///< null pointer
    app_sys_err_code_invalid_args,        ///< invalid argumenter
    app_sys_err_code_invalid_addr,        ///< bad memory address
    app_sys_err_code_invalid_stat,        ///< invalid state
    app_sys_err_code_invalid_flag,        ///< invalid flag
    app_sys_err_code_invalid_data,        ///< invalid data
    app_sys_err_code_invalid_size,        ///< invalid size
    app_sys_err_code_invalid_file_name,   ///< invalid file name
    //system status
    app_sys_err_code_disabled_soft_dev,   ///< soft device has been disabled
    app_sys_err_code_svc_handler_miss,    ///< svc handler is miss
    app_sys_err_code_connect_count,       ///< maximum connection count exceeded.
    app_sys_err_code_low_power,           ///< battery percent is too low
    app_sys_err_code_bt_ota,              ///< not enough resources for operation
    /*  */
    app_sys_err_code_num,
} app_sys_err_code_type_t;

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
