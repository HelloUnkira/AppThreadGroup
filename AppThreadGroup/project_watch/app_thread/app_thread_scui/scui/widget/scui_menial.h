#ifndef SCUI_MENIAL_H
#define SCUI_MENIAL_H

typedef enum {
    scui_menial_type_none = 0,
    scui_menial_type_btn,
    scui_menial_type_arc,
    scui_menial_type_num,
} scui_menial_type_t;

typedef union {
    
    /* field(btn): */
    struct {
        scui_color_t  color[2];     /* 颜色(未选中[0]选中[1]; l->d) */
        scui_coord_t  width;        /* 边界(实心:<= 0;空心:> 0) */
        scui_coord_t  radius;       /* 圆角半径(最大:< 0) */
        scui_sbitfd_t fixed:1;      /* 无点击动画 */
        scui_sbitfd_t check:1;      /* 可选中标记 */
        /* 内部域: */
        scui_sbitfd_t hold:1;
        scui_sbitfd_t click:1;
        scui_sbitfd_t check_c:1;
        scui_sbitfd_t state:1;
        scui_coord_t  time;
        scui_coord_t  tick;
        scui_coord_t  lim;
        scui_coord_t  pct;
        scui_coord_t  way;
    } btn;
    
    /* field(arc): */
    struct {
        void *occupy;
    } arc;
    
    /* keep add... */
    
} scui_menial_data_t;

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
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
