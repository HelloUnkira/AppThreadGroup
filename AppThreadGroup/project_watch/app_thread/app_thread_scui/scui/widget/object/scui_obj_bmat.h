#ifndef SCUI_OBJ_BMAT_H
#define SCUI_OBJ_BMAT_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_object_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord_t  item_num;     /* 条目数量 */
    scui_coord_t  row_num;      /* 轨道数量 */
    scui_point_t  gap;          /* 条目间距(x:列;y:行) */
    /* 内部域: */
    scui_coord_t  item_max;     /* 条目上限(构造期) */
    scui_coord_t *item_row;     /* 条目轨道(升序) */
    scui_coord_t *item_unit;    /* 条目宽度(单位数) */
    scui_area_t  *item_area;    /* 条目区域(控件相对) */
    scui_coord_t  item_click;   /* 最近点击条目(-1:无) */
    scui_coord_t  item_press;   /* 按下条目(-1:无) */
} scui_obj_bmat_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_object_maker_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord_t item_num;      /* 条目数量 */
    scui_coord_t row_num;       /* 轨道数量 */
    scui_point_t gap;           /* 条目间距(x:列;y:行) */
} scui_obj_bmat_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_obj_bmat_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_bmat_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_bmat_invoke(scui_event_t *event);

#endif
