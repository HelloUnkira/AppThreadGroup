#ifndef SCUI_KEYBOARD_H
#define SCUI_KEYBOARD_H

/* 键值
 * 0x20以下的取值保留给功能键
 * 可打印键取值即ASCII字符本身
 */
typedef enum {
    scui_keyboard_key_none = 0,     /* 无效键值 */
    scui_keyboard_key_del,          /* 退格 */
    scui_keyboard_key_space,        /* 空格 */
    scui_keyboard_key_enter,        /* 确认 */
    scui_keyboard_key_case,         /* 大小写切换 */
    scui_keyboard_key_sym,          /* 符号页切换 */
    scui_keyboard_key_lang,         /* 语言切换 */
} scui_keyboard_key_t;

/* 键盘布局类型 */
typedef enum {
    scui_keyboard_type_num = 0,     /* 数字键盘 */
    scui_keyboard_type_word,        /* 字母键盘 */
    scui_keyboard_type_sym,         /* 符号键盘 */
    scui_keyboard_type_num_t,
} scui_keyboard_type_t;

/* 按键条目(布局与辅助信息)
 * 键面文本不属于本模组职责, 由调用方依code取本地化字库或符号
 */
typedef struct {
    uint32_t     code;              /* 键值(ASCII字符, 或scui_keyboard_key_t) */
    scui_coord_t unit;              /* 宽度单位数(<=0当1) */
    scui_coord_t row;               /* 轨道号(<row_num且升序, 同轨连续) */
} scui_keyboard_item_t;

/* 键盘布局
 * 按键表由本模组静态维护, 调用方只读不释放
 * unit与row可直接驱动layout(flex)或与bmat等价的等比分配
 */
typedef struct {
    scui_coord_t               num;     /* 按键数量 */
    scui_coord_t               row_num; /* 轨道数量 */
    const scui_keyboard_item_t *item;   /* 按键表 */
} scui_keyboard_layout_t;

/*@brief 键盘布局查询
 *@param type   布局类型
 *@param upper  字母大写(word类型有效)
 *@param layout 布局输出
 */
void scui_keyboard_layout(scui_keyboard_type_t type, bool upper, scui_keyboard_layout_t *layout);

/*@brief 键盘按键查询
 *@param type 布局类型
 *@param idx  按键号
 *@retval 键值(0:越界)
 */
uint32_t scui_keyboard_code(scui_keyboard_type_t type, scui_coord_t idx);

/*@brief 键盘单位总数查询(单轨)
 *@param type 布局类型
 *@param row  轨道号
 *@retval 单位总数(0:轨道为空)
 */
scui_coord_t scui_keyboard_unit(scui_keyboard_type_t type, scui_coord_t row);

#endif
