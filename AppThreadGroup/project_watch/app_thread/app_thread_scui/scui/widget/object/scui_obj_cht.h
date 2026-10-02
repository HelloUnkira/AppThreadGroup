#ifndef SCUI_OBJ_CHT_H
#define SCUI_OBJ_CHT_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_object_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord_t   type;         /* 类型(0:hist;1:line) */
    scui_coord_t   value_min;    /* 最小取值 */
    scui_coord_t   value_max;    /* 最大取值 */
    scui_coord_t   number;       /* 条目数量 */
    scui_coord_t   step;         /* 条目步进 */
    scui_coord_t   loop;         /* 循环模式(0:关;1:开); 环上写头留白为gap */
    scui_coord_t   gap;          /* 写头留白宽度(像素; 基准=步进) */
    /* 内部域: */
    scui_object_type_t form;     /* 柱状层级(style录入, 绘制期回读) */
    scui_coord_t  *vlist_min;    /* 数据列表(hist) */
    scui_coord_t  *vlist_max;    /* 数据列表(hist) */
    scui_coord_t  *vlist_dot;    /* 数据列表(line) */
    scui_point_t  *vlist_pos;    /* 坐标列表(line) */
    scui_coord_t   ring;         /* 环首槽位(下一个写入位; 绘制期留白为gap) */
    scui_coord_t   ofs_max;      /* 可视偏移上限(内容超出部) */
    scui_coord_t   ofs_cur;      /* 可视偏移(跟手) */
    scui_coord_t   ofs_base;     /* 按下基准偏移 */
    scui_coord_t   point_base;   /* 按下基准点 */
} scui_obj_cht_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_object_maker_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord_t   type;         /* 类型(0:hist;1:line) */
    scui_coord_t   value_min;    /* 最小取值 */
    scui_coord_t   value_max;    /* 最大取值 */
    scui_coord_t   number;       /* 条目数量 */
    scui_coord_t   step;         /* 条目步进 */
    scui_coord_t   loop;         /* 循环模式(0:关;1:开) */
    scui_coord_t   gap;          /* 写头留白宽度(像素; 0: 取步进为基准) */
} scui_obj_cht_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_obj_cht_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_cht_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_cht_invoke(scui_event_t *event);

#endif
