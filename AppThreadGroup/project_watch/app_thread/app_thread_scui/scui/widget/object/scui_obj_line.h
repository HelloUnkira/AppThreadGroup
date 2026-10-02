#ifndef SCUI_OBJ_LINE_H
#define SCUI_OBJ_LINE_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_object_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord_t  mode;         /* 模式(0:坐标;1:极坐标) */
    scui_coord_t  seg_num;      /* 线段数量 */
    scui_coord_t  dot_num;      /* 端点数量 */
    /* 内部域: */
    scui_coord_t  seg_max;      /* 线段上限(构造期) */
    scui_coord_t  dot_max;      /* 端点上限(构造期) */
    scui_coord_t *seg_dot;      /* 线段端点数 */
    scui_point_t *vpos;         /* 端点序列(控件相对) */
} scui_obj_line_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_object_maker_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord_t  mode;         /* 模式(0:坐标;1:极坐标) */
    scui_coord_t  seg_num;      /* 线段数量 */
    scui_coord_t  dot_num;      /* 端点数量 */
} scui_obj_line_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_obj_line_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_line_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_line_invoke(scui_event_t *event);

#endif
