#ifndef SCUI_ROLLER_H
#define SCUI_ROLLER_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_scroll_t scroll;
    scui_linear_t linear;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    /* 内部域: */
    scui_handle_t center;
} scui_roller_t;

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_scroll_maker_t scroll;
    scui_linear_maker_t linear;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
} scui_roller_maker_t;

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 *@param layout     通过布局创建
 */
void scui_roller_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_roller_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_roller_event(scui_event_t *event);

#endif
