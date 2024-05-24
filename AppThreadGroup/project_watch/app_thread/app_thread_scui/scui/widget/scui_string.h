#ifndef SCUI_STRING_H
#define SCUI_STRING_H

typedef enum {
    scui_string_mode_dot,       // 画不完打点
    scui_string_mode_rewind,    // 回退滚动
    scui_string_mode_circular,  // 循环滚动
} scui_string_mode_t;

typedef enum {
    scui_string_align_cc,       // [center, center]
    scui_string_align_lc,       // [left,   center]
    scui_string_align_lu,       // [left,   up    ]
    scui_string_align_ld,       // [left,   down  ]
    scui_string_align_rc,       // [right,  center]
    scui_string_align_ru,       // [right,  up    ]
    scui_string_align_rd,       // [right,  down  ]
} scui_string_align_t;

typedef struct {
    scui_widget_t       widget;
    scui_string_align_t align;      // 字符串书写对齐
    scui_string_mode_t  mode;       // 字符串模式
    scui_event_dir_t    dir;        // 字符串书写方向(暂支持水平书写)
    scui_handle_t       font;       // 字库
    scui_handle_t       text;       // 字符串
    /* 内部域: */
    uint8_t *str;   // 字符串
} scui_string_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t widget;
    scui_string_align_t align;      // 字符串书写对齐
    scui_string_mode_t  mode;       // 字符串模式
    scui_event_dir_t    dir;        // 字符串书写方向(暂支持水平书写)
    scui_handle_t       font;       // 字库
    scui_handle_t       text;       // 字符串
} scui_string_maker_t;
#pragma pack(pop)

/*@brief 字符串控件创建
 *@param maker  字符串控件创建参数
 *@param handle 字符串控件句柄
 *@param layout 通过布局创建
 */
void scui_string_create(scui_string_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 字符串控件销毁
 *@param handle 字符串控件句柄
 */
void scui_string_destroy(scui_handle_t handle);

/*@brief 字符串控件更新文本
 *@param handle 字符串控件句柄
 *@param text   字符串句柄
 */
void scui_string_update_text(scui_handle_t handle, scui_handle_t text);

/*@brief 字符串控件更新文本
 *@param handle 字符串控件句柄
 *@param text   字符串句柄
 */
void scui_string_update_str(scui_handle_t handle, char *str);

/*@brief 字符串控件事件处理回调
 *@param event 事件
 */
void scui_string_event(scui_event_t *event);

#endif
