#ifndef SCUI_OBJ_ARC_H
#define SCUI_OBJ_ARC_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_object_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord3_t  angle_c;       /* 当前角度 */
    scui_sbitfd_t  anti:1;        /* 反方向 */
    scui_sbitfd_t  ext_touch:1;   /* 跟手修改 */
    scui_sbitfd_t  ext_spinner:1; /* 加载圆环 */
    scui_coord_t   angle_down;    /* 跟手:基准角(按下=落点,拖动中=上一次采样) */
    /* 内部域: */
} scui_obj_arc_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_object_maker_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord3_t  angle_c;       /* 当前角度 */
    scui_sbitfd_t  anti:1;        /* 反方向 */
    scui_sbitfd_t  ext_touch:1;   /* 跟手修改 */
    scui_sbitfd_t  ext_spinner:1; /* 加载圆环 */
    scui_coord_t   angle_down;    /* 跟手:基准角(按下=落点,拖动中=上一次采样) */
} scui_obj_arc_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_obj_arc_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_arc_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_arc_invoke(scui_event_t *event);

#endif
