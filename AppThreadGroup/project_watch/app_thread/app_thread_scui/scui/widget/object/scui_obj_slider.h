#ifndef SCUI_OBJ_SLIDER_H
#define SCUI_OBJ_SLIDER_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_object_t object;
    scui_obj_bar_t obj_bar;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    /* 内部域: */
    scui_coord3_t value_base;   /* 按下基准值 */
    scui_coord_t  point_base;   /* 按下基准点(沿轴) */
} scui_obj_slider_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_object_maker_t object;
    scui_obj_bar_maker_t obj_bar;
    SCUI_EXTEND_FIELD_E
} scui_obj_slider_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_obj_slider_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_slider_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_slider_invoke(scui_event_t *event);

#endif
