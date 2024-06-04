#ifndef SCUI_CHART_H
#define SCUI_CHART_H

typedef struct {
    scui_widget_t widget;
    uint8_t       type;     /* 0x00:柱状图,股价图; */
    /* 内部域: */
} scui_chart_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t   widget;
    uint8_t               type;             /* 0x00:柱状图,股价图;0x01:折线图; */
    /* 柱状图,股价图: */
    #if 0
    union {
    struct {
        scui_handle_t         edge;         /* 边界点 */
        scui_coord_t          value_min;    /* 最小取值 */
        scui_coord_t          value_max;    /* 最大取值 */
        scui_coord_t          space;        /* 条目间隙 */
        scui_coord_t          width;        /* 条目宽度 */
        scui_coord_t          height;       /* 条目高度 */
        scui_coord_t          number;       /* 条目数量 */
        scui_point_t          offset;       /* 条目起始偏移 */
        scui_color_t      color;        /* 颜色(亮色调与暗色调) */
    } field_0x00;                           /* 0x00:柱状图,股价图; */
    struct {
    } field_0x01;                           /* 0x00:折现图; */
    };
    #endif
    /* 继续补充... */
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

/*@brief 图表控件事件处理回调
 *@param event 事件
 */
void scui_chart_event(scui_event_t *event);

#endif
