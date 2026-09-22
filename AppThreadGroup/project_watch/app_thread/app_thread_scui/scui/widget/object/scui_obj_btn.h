#ifndef SCUI_OBJ_BTN_H
#define SCUI_OBJ_BTN_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_object_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_sbitfd_t  fixed:1;     /* 固定标记(无缩放) */
    scui_sbitfd_t  check:1;     /* 选中标记 */
    scui_sbitfd_t  click:1;     /* 点击标记 */
    /* 内部域: */
} scui_obj_btn_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_object_maker_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_sbitfd_t  fixed:1;     /* 固定标记(无缩放) */
    scui_sbitfd_t  check:1;     /* 选中标记 */
} scui_obj_btn_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_obj_btn_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_btn_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_btn_invoke(scui_event_t *event);

#endif
