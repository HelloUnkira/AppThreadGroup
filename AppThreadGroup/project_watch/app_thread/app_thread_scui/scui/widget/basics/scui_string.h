#ifndef SCUI_STRING_H
#define SCUI_STRING_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t       text;           // 字符串
    scui_handle_t       font_idx;       // 字库索引
    scui_handle_t       lang_type;      // 语言类型
    scui_string_args_t  args;           // 字符串绘制参数
    scui_coord_t        unit_ms;        // 滚动单元时间
    scui_coord_t        unit_dx;        // 滚动单元距离
    uint64_t            unit_s:1;       // 滚动模式(单次滚动,结束后不再滚动)
    uint64_t            draw_cache:1;   // 绘制缓存块
    scui_surface_t     *draw_surface;   // 绘制缓存块
    /* 内部域: */
    uint64_t            unit_anima:1;   // 滚动动画标记
    uint64_t            unit_abort:1;   // 滚动中止标记
    uint64_t            unit_over:1;    // 滚动结束
    scui_coord_t        unit_way;       // 滚动单元方向
    scui_coord_t        rcd_ms;
    uint8_t            *str_utf8;       // 字符串(utf8)
} scui_string_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t       text;           // 字符串
    scui_handle_t       font_idx;       // 字库索引
    scui_handle_t       lang_type;      // 语言类型
    scui_string_args_t  args;           // 字符串绘制参数
    scui_coord_t        unit_ms;        // 滚动单元时间
    scui_coord_t        unit_dx;        // 滚动单元距离
    uint64_t            unit_s:1;       // 滚动模式(单次滚动,结束后不再滚动)
    uint64_t            draw_cache:1;   // 绘制缓存块
} scui_string_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 *@param layout     通过布局创建
 */
void scui_string_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_string_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_string_event(scui_event_t *event);

#endif
