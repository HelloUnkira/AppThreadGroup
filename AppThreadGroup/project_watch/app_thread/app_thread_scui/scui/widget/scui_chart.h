#ifndef SCUI_CHART_H
#define SCUI_CHART_H

typedef enum {
    scui_chart_type_histogram,      /* 柱状图,股价图; */
    scui_chart_type_line,           /* 折线图 */
} scui_chart_type_t;

typedef struct {                    /* 柱状图,股价图 */
    scui_handle_t   edge;           /* 边界点 */
    scui_coord_t    value_min;      /* 最小取值 */
    scui_coord_t    value_max;      /* 最大取值 */
    scui_point_t    offset;         /* 条目起始偏移 */
    scui_coord_t    number;         /* 条目数量 */
    scui_coord_t    height;         /* 条目高度(条目宽度与edge一致) */
    scui_coord_t    space;          /* 条目间隙 */
    scui_color_t    color;          /* 颜色 */
} scui_chart_histogram_t;

typedef struct {
    scui_coord_t   *vlist_min;
    scui_coord_t   *vlist_max;
} scui_chart_histogram_data_t;

typedef struct {                    /* 折线图 */
    scui_handle_t   edge;           /* 边界点 */
    scui_coord_t    value_min;      /* 最小取值 */
    scui_coord_t    value_max;      /* 最大取值 */
    scui_point_t    offset;         /* 点起始偏移 */
    scui_coord_t    number;         /* 点数量 */
    scui_coord_t    height;         /* 图表限制高度 */
    scui_coord_t    space;          /* 点水平间隙 */
    scui_coord_t    width;          /* 线宽 */
    scui_color_t    color;          /* 颜色 */
} scui_chart_line_t;

typedef struct {
    scui_coord_t   *vlist;
} scui_chart_line_data_t;

typedef struct {
    scui_widget_t       widget;
    scui_chart_type_t   type;
    /* 图表类型对应数据域: */
    union {
    scui_chart_histogram_t  histogram;
    scui_chart_line_t       line;
    /* 继续补充... */
    };
    /* 内部域: */
    union {
    scui_chart_histogram_data_t histogram_data;
    scui_chart_line_data_t      line_data;
    /* 继续补充... */
    };
} scui_chart_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t widget;
    scui_chart_type_t   type;
    /* 图表类型对应数据域: */
    union {
    scui_chart_histogram_t  histogram;
    scui_chart_line_t       line;
    /* 继续补充... */
    };
} scui_chart_maker_t;
#pragma pack(pop)

/*@brief 图表控件创建
 *@param maker  图表控件创建参数
 *@param handle 图表控件句柄
 *@param layout 通过布局创建
 */
void scui_chart_create(scui_chart_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 图表控件销毁
 *@param handle 图表控件句柄
 */
void scui_chart_destroy(scui_handle_t handle);

/*@brief 图表控件数据列表更新(柱状图)
 *@param handle 图表控件句柄
 *@param vlist_min 数据列表
 *@param vlist_max 数据列表
 */
void scui_chart_histogram_data(scui_handle_t handle, scui_coord_t *vlist_min, scui_coord_t *vlist_max);

/*@brief 图表控件数据列表更新(折线图)
 *@param handle 图表控件句柄
 *@param vlist  数据列表
 */
void scui_chart_line_data(scui_handle_t handle, scui_coord_t *vlist);

/*@brief 图表控件事件处理回调
 *@param event 事件
 */
void scui_chart_event(scui_event_t *event);

#endif
