#ifndef SCUI_LAYOUT_H
#define SCUI_LAYOUT_H

typedef enum {
    scui_layout_type_item = 0,
    scui_layout_type_flex,
    scui_layout_type_grid,
} scui_layout_type_t;

typedef struct {
    scui_sbitfd_t use:1;            /* 已登记对齐 */
    scui_handle_t handle;           /* 子控件句柄 */
    scui_handle_t handle_t;         /* 对齐目标 */
    scui_align_t  align;            /* 对齐 */
    scui_point_t  offset;           /* 偏移 */
} scui_layout_item_node_t;

typedef struct {
    scui_sbitfd_t use:1;            /* 已登记分组 */
    scui_handle_t handle;           /* 子控件句柄 */
    scui_coord_t  group;            /* 组编号 */
} scui_layout_flex_node_t;

typedef struct {
    scui_sbitfd_t use:1;            /* 已登记单元格 */
    scui_sbitfd_t st_x:1;           /* cell水平拉伸 */
    scui_sbitfd_t st_y:1;           /* cell垂直拉伸 */
    scui_handle_t handle;           /* 子控件句柄 */
    scui_coord_t  col;              /* 起始列 */
    scui_coord_t  row;              /* 起始行 */
    scui_coord_t  col_span;         /* 跨列数 */
    scui_coord_t  row_span;         /* 跨行数 */
    scui_opt_pos_t align;           /* cell内对齐(水平l/r/hor | 垂直u/d/ver) */
} scui_layout_grid_node_t;

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_layout_type_t  type;
    scui_sbitfd_t       use:1;      /* 对齐 */
    scui_align_t        align;      /* 对齐 */
    scui_point_t        offset;     /* 偏移 */
    /* 内部域: */
    union {
    struct {
        scui_handle_t num;
        scui_layout_item_node_t *list;
    } item;
    struct {
        scui_handle_t num;
        scui_layout_flex_node_t *list;
        
        scui_opt_pos_t  align;      /* 对齐(水平/垂直) */
        scui_sbitfd_t   way:1;      /* 方向(0:水平;1:垂直) */
        scui_point_t    span;       /* 间距(水平/垂直) */
    } flex;
    struct {
       scui_handle_t num;
       scui_layout_grid_node_t *list;
       
       scui_coord_t    col_num;        /* 列轨道数 */
       scui_coord_t   *col_size;       /* 列轨道尺寸(定值) */
       scui_coord_t    col_gap;        /* 列距 */
       scui_coord_t    row_num;        /* 行轨道数 */
       scui_coord_t   *row_size;       /* 行轨道尺寸(定值) */
       scui_coord_t    row_gap;        /* 行距 */
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
    scui_sbitfd_t       use:1;      /* 对齐 */
    scui_align_t        align;      /* 对齐 */
    scui_point_t        offset;     /* 偏移 */
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
