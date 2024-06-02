#ifndef SCUI_STRING_H
#define SCUI_STRING_H

typedef struct {
    scui_color_mix_t    color;          // 字符串颜色
    scui_color_mix_t    filter;         // 去底色
    scui_coord_t        margin;         // 边距
    scui_coord_t        gap_line;       // 间距
    scui_coord_t        gap_none;       // 间距:空字符
    uint64_t            mode:2;         // 绘制模式(0:回退滚动;1:循环滚动)
    uint64_t            align_hor:2;    // 书写对齐(0:左对齐;1:右对齐;2:中心对齐)
    uint64_t            align_ver:2;    // 书写对齐(0:上对齐;1:下对齐;2:中心对齐)
    uint64_t            line_way:1;     // 书写方向(0:水平书写;1:垂直书写)
    uint64_t            line_multi:1;   // 多行模式(非默认)
    uint64_t            line_stand:1;   // 标准字符(非默认)
    uint64_t            line_ltr:1;     // 绘制方向
    uint64_t            cover:1;        // 保留底色
    /* 内部域: */
    uint32_t            number;         // 字符数量
    uint32_t           *unicode;        // 字符串(unicode)
    void               *layout;         // 布局资源
    scui_coord_t        offset;
    uint64_t            update:1;       // 更新标记
} scui_string_args_t;

typedef struct {
    scui_widget_t       widget;
    scui_handle_t       name;           // 字库名字
    scui_handle_t       text;           // 字符串
    scui_string_args_t  args;           // 字符串绘制参数
    /* 内部域: */
    uint8_t *str_utf8;  // 字符串(utf8)
} scui_string_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t widget;
    scui_handle_t       name;           // 字库名字
    scui_handle_t       text;           // 字符串
    scui_string_args_t  args;           // 字符串绘制参数
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
 *@param handle   字符串控件句柄
 *@param str_utf8 字符串(utf8)
 */
void scui_string_update_str(scui_handle_t handle, uint8_t *str_utf8);

/*@brief 字符串控件事件处理回调
 *@param event 事件
 */
void scui_string_event(scui_event_t *event);

#endif
