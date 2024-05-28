#ifndef SCUI_STRING_LAYOUT_H
#define SCUI_STRING_LAYOUT_H

typedef struct {
    scui_handle_t name;
    scui_coord_t  line_num;
    scui_coord_t  line_height;
    scui_coord_t  line_tail  [SCUI_STRING_LIMIT_LINE];   // 布局行尾位置索引
    scui_coord_t  layout_x   [SCUI_STRING_LIMIT_LINE];   // 布局行水平偏移量
    bool          hyphen     [SCUI_STRING_LIMIT_LINE];   // 布局行尾连字符
    scui_coord_t  offset_x;         // 初始行水平偏移量
    scui_coord_t  offset_y;         // 起始行垂直偏移量
    scui_coord_t  complete_num;     // 完成排版的字符
    scui_coord_t  width_render;     // 渲染区域宽
    scui_coord_t  width_line_f;     // 布局排版后宽度(首行)
    scui_coord_t  width_line_l;     // 布局排版后宽度(尾行剩余)
    bool          standard;         // 标准布局模式
} scui_string_layout_t;

#endif
