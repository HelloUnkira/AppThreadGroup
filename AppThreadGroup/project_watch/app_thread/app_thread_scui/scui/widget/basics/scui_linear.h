#ifndef SCUI_LINEAR_H
#define SCUI_LINEAR_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_scroll_t scroll;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t       list_num;           // 列表数量
    /* 内部域: */
    scui_event_cb_t    *event_cb_list_m;    // 列表子控件响应回调(主)
    scui_event_cb_t    *event_cb_list_s;    // 列表子控件树响应回调(从)
    scui_surface_t    **list_surface_s;     // 从控件树画布列表(slave)
    scui_handle_t      *list_widget_s;      // 从控件树列表(slave)
    scui_handle_t      *list_widget_m;      // 主控件列表(master)
    scui_area_t        *list_draw_clip;     // 绘制目标剪切域
    scui_handle_t       list_draw_idx;      // 绘制目标
    bool               *list_draw;          // 绘制标记
    bool               *list_refr;          // 刷新标记
    /* 内部域: */
    uintptr_t           clear_num;
    uintptr_t           clear_byte;
    uintptr_t           remain_num;
    uintptr_t           remain_byte;
} scui_linear_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_scroll_maker_t scroll;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t       list_num;           // 列表数量
} scui_linear_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_linear_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_linear_burn(scui_handle_t handle);

/*@brief 列表控件缓存资源回收
 *@param handle 列表控件句柄
 *@param any    回收所有
 */
void scui_linear_recycle(scui_handle_t handle, bool any);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_linear_invoke(scui_event_t *event);

#endif
