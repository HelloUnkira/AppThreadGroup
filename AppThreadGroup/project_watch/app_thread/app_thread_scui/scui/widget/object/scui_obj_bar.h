#ifndef SCUI_OBJ_BAR_H
#define SCUI_OBJ_BAR_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_object_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_sbitfd_t way:1;            /* 方向(水平:0;垂直:1) */
    scui_coord3_t value_lim;        /* 进度限制(默认:100) */
    scui_coord_t  value_cur;        /* 当前进度 */
    scui_sbitfd_t value_int:1;      /* 进度单元 */
    /* 内部域: */
} scui_obj_bar_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_object_maker_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_sbitfd_t way:1;            /* 方向(水平:0;垂直:1) */
    scui_coord3_t value_lim;        /* 进度限制(默认:100) */
    scui_coord_t  value_cur;        /* 当前进度 */
    scui_sbitfd_t value_int:1;      /* 进度单元 */
} scui_obj_bar_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_obj_bar_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_bar_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_bar_invoke(scui_event_t *event);

#endif
