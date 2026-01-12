#ifndef SCUI_LINEAR_H
#define SCUI_LINEAR_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_scroll_t scroll;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    /* 内部域: */
    scui_multi_t        clear_num;
    scui_multi_t        clear_byte;
    scui_multi_t        remain_num;
    scui_multi_t        remain_byte;
} scui_linear_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_scroll_maker_t scroll;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
} scui_linear_maker_t;
#pragma pack(pop)

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    /* 内部域: */
    scui_handle_t       handle_s;       // 从控件树(slave)
    scui_sbitfd_t       draw:1;         // 绘制标记
    scui_sbitfd_t       refr:1;         // 刷新标记
} scui_linear_m_t;

// 子控件:需要重定向事件响应
#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
} scui_linear_m_maker_t;
#pragma pack(pop)

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    /* 内部域: */
    scui_surface_t     *surface_s;      // 子控件树画布
    scui_handle_t       handle_m;       // 主控件(master)
} scui_linear_s_t;

// 子控件:需要重定向事件响应
#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
} scui_linear_s_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_linear_make(void *inst, void *inst_maker, scui_handle_t *handle);
void scui_linear_m_make(void *inst, void *inst_maker, scui_handle_t *handle);
void scui_linear_s_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_linear_burn(scui_handle_t handle);
void scui_linear_m_burn(scui_handle_t handle);
void scui_linear_s_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_linear_invoke(scui_event_t *event);
void scui_linear_m_invoke(scui_event_t *event);
void scui_linear_s_invoke(scui_event_t *event);

#endif
