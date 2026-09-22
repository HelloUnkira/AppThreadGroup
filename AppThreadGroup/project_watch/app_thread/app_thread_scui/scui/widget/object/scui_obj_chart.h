#ifndef SCUI_OBJ_CHART_H
#define SCUI_OBJ_CHART_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_object_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord_t   type;         /* 类型(0:hist;1:line) */
    scui_area_t    area;         /* 区域 */
    scui_coord_t   value_min;    /* 最小取值 */
    scui_coord_t   value_max;    /* 最大取值 */
    scui_coord_t   number;       /* 条目数量 */
    scui_coord_t   space;        /* 条目间隙 */
    /* 内部域: */
    scui_coord_t  *vlist_min;    /* 数据列表(hist) */
    scui_coord_t  *vlist_max;    /* 数据列表(hist) */
    scui_coord_t  *vlist_dot;    /* 数据列表(line) */
    scui_point_t  *vlist_pos;    /* 坐标列表(line) */
} scui_obj_chart_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_object_maker_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord_t   type;         /* 类型(0:hist;1:line) */
    scui_area_t    area;         /* 区域 */
    scui_coord_t   value_min;    /* 最小取值 */
    scui_coord_t   value_max;    /* 最大取值 */
    scui_coord_t   number;       /* 条目数量 */
    scui_coord_t   space;        /* 条目间隙 */
} scui_obj_chart_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_obj_chart_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_chart_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_chart_invoke(scui_event_t *event);

#endif
