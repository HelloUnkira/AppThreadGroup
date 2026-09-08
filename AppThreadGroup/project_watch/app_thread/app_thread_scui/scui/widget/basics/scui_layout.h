#ifndef SCUI_LAYOUT_H
#define SCUI_LAYOUT_H

typedef enum {
    scui_layout_type_item = 0,
    scui_layout_type_flex,
    scui_layout_type_grid,
} scui_layout_type_t;

typedef struct {
    scui_sbitfd_t use:1;        /* 已登记对齐 */
    scui_handle_t handle;       /* 子控件句柄 */
    scui_handle_t handle_t;     /* 对齐目标 */
    scui_align_t  align;        /* 对齐 */
    scui_point_t  offset;       /* 偏移 */
} scui_layout_item_node_t;

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 内部域: */
    scui_layout_type_t type;
    union {
    struct {
        scui_handle_t num;
        scui_layout_item_node_t *list;
    } item;
    struct {
       void *occupy;
    } flex;
    struct {
       void *occupy;
    } grid;
    };
} scui_layout_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_layout_type_t  type;
} scui_layout_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_layout_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_layout_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_layout_invoke(scui_event_t *event);

#endif
