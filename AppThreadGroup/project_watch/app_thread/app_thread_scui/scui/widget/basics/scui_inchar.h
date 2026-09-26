#ifndef SCUI_INCHAR_H
#define SCUI_INCHAR_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_string_t string;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_area_t     area;       /* 光标区域 */
    scui_coord_t    limit;      /* 最大字符数量 */
    scui_coord_t    index;      /* 光标字符索引 */
    scui_color_t    color;      /* 光标颜色 */
    scui_sbitfd_t   hide:1;     /* 光标隐藏 */
    /* 内部域: */
    scui_sbitfd_t   prev:1;     /* 方向标记(前向:真;后向:假) */
    scui_coord_t    bre_time;   /* 光标呼吸计时 */
    scui_coord_t    str_num;    /* 当前字符数量 */
    scui_coord_t   *str_width;  /* 字符宽度 */
    scui_coord_t   *ofs_utf8;   /* 字符偏移(utf8) */
    uint8_t        *str_utf8;   /* 字符串(utf8) */
    uint32_t       *str_uni;    /* 字符串(unicode)同步 */
} scui_inchar_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_string_maker_t string;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord_t    limit;      /* 最大字符数量 */
    scui_coord_t    index;      /* 光标字符索引 */
    scui_color_t    color;      /* 光标颜色 */
    scui_sbitfd_t   hide:1;     /* 光标隐藏 */
} scui_inchar_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_inchar_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_inchar_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_inchar_invoke(scui_event_t *event);

#endif
