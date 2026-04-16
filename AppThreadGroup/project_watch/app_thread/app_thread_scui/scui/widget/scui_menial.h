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
        scui_color_t color;
        /* 内部域: */
    } btn;
    
    
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
typedef void (*scui_menial_make_t)(scui_menial_t *menial);
typedef void (*scui_menial_burn_t)(scui_menial_t *menial);

/*@brief 控件子类型节点
 */
typedef struct {
    scui_menial_make_t make;
    scui_menial_burn_t burn;
    scui_event_cb_t  invoke;
} scui_menial_info_t;

/* menial_type:<s> */
void scui_menial_make_btn(scui_menial_t *menial);
void scui_menial_burn_btn(scui_menial_t *menial);
void scui_menial_invoke_btn(scui_event_t *event);
void scui_menial_make_arc(scui_menial_t *menial);
void scui_menial_burn_arc(scui_menial_t *menial);
void scui_menial_invoke_arc(scui_event_t *event);
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
