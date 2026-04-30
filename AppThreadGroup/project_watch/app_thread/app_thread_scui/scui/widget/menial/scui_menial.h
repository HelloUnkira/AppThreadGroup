#ifndef SCUI_MENIAL_H
#define SCUI_MENIAL_H

typedef enum {
    scui_menial_type_none = 0,
    scui_menial_type_btn,
    scui_menial_type_arc,
    scui_menial_type_bar,
    scui_menial_type_num,
} scui_menial_type_t;

typedef union {
    
    /* field(btn): */
    struct {
        scui_color_t  color[2];     /* 颜色(未选中[0]选中[1]; l->d) */
        scui_coord_t  width;        /* 边界(实心:<=0;空心:>0) */
        scui_coord_t  radius;       /* 圆角半径(最大:<0) */
        scui_sbitfd_t fixed:1;      /* 无点击动画(可选) */
        scui_sbitfd_t check:1;      /* 选中标记(可选) */
        scui_coord_t  time;         /* 动画时间(ms) */
        scui_coord_t  lim;          /* 缩小限制(pct) */
        /* 内部域: */
    } btn;
    
    /* field(arc): */
    struct {
        scui_point_t  center;       /* 弧心 */
        scui_coord_t  width;        /* 弧宽(扇形:<= 0;弧型:>0) */
        scui_coord_t  radius;       /* 半径(>0) */
        scui_coord3_t angle_s;      /* 起始角度(默认:0.0f) */
        scui_coord3_t angle_e;      /* 结束角度(默认:360.0f) */
        scui_color_t  color[2];     /* 颜色(背景[0]前景[1]) */
        scui_sbitfd_t spinner:1;    /* 加载圆环(可选) */
        scui_sbitfd_t round:1;      /* 端点圆角(可选) */
        scui_sbitfd_t gradw:1;      /* 渐变方向(水平:0;垂直:1) */
        scui_sbitfd_t grad:1;       /* 渐变(可选)(s->e) */
        scui_sbitfd_t anti:1;       /* 反方向 */
        /* 内部域: */
        scui_coord3_t angle_dist;
        scui_coord3_t angle_cur;
        scui_coord3_t angle_way;
        scui_coord_t  time;
        scui_coord_t  tick;
    } arc;
    
    /* field(bar): */
    struct {
        scui_color_t  color[2];     /* 颜色(背景[0]前景[1]) */
        scui_coord_t  radius;       /* 圆角半径(最大:<0) */
        scui_coord3_t value_lim;    /* 进度限制(默认:100.0f) */
        scui_sbitfd_t value_int:1;  /* 进度单元(可选) */
        scui_sbitfd_t ext_slider:1; /* 滑动交互(扩展:可选) */
        scui_sbitfd_t ext_switch:1; /* 点击开关(扩展:可选) */
        scui_sbitfd_t grad:1;       /* 渐变(可选)(s->e) */
        scui_sbitfd_t way:1;        /* 方向(水平:0;垂直:1) */
        /* 内部域: */
        scui_coord3_t value_cur;
        scui_coord3_t value_way;
        scui_coord_t  time;
        scui_coord_t  tick;
    } bar;
    
    /* keep add... */
    
} scui_menial_data_t;

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    scui_object_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_menial_type_t  type;
    scui_menial_data_t  data;
    /* 内部域: */
} scui_menial_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    scui_object_maker_t object;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_menial_type_t  type;
    scui_menial_data_t  data;
} scui_menial_maker_t;
#pragma pack(pop)

/*@brief 控件子类型信息
 */
typedef struct {
    void (*maker)(scui_menial_maker_t *menial_maker);
    void (*config)(scui_menial_t *menial);
    void (*recycle)(scui_menial_t *menial);
    scui_event_cb_t invoke;
} scui_menial_info_t;

/* menial_type:<s> */
void scui_menial_btn_maker(scui_menial_maker_t *menial_maker);
void scui_menial_btn_config(scui_menial_t *menial);
void scui_menial_btn_recycle(scui_menial_t *menial);
void scui_menial_btn_invoke(scui_event_t *event);
void scui_menial_arc_maker(scui_menial_maker_t *menial_maker);
void scui_menial_arc_config(scui_menial_t *menial);
void scui_menial_arc_recycle(scui_menial_t *menial);
void scui_menial_arc_invoke(scui_event_t *event);
void scui_menial_bar_maker(scui_menial_maker_t *menial_maker);
void scui_menial_bar_config(scui_menial_t *menial);
void scui_menial_bar_recycle(scui_menial_t *menial);
void scui_menial_bar_invoke(scui_event_t *event);
/* menial_type:<e> */

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_menial_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_menial_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_menial_invoke(scui_event_t *event);

#endif
