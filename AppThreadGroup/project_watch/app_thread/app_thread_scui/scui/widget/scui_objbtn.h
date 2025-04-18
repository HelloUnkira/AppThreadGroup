#ifndef SCUI_OBJBTN_H
#define SCUI_OBJBTN_H

/*@brief objbtn备注:
 *       本控件是从lvgl中的obj吸收而来
 *       因为lv_btn与lv_obj相似性非常高
 *       实际上这就是一个常规的像素按钮
 *       按钮共分为四个部件(lvgl):
 *           背景,边界,外部线,阴影
 */

typedef struct { scui_class_widget(widget);
    /* 外部域: */
    scui_event_cb_t notify_cb;  /* 专属事件通知 */
    scui_color_t    color[4];   /* 部件源色调(s->e) */
    scui_alpha_t    alpha[4];   /* 部件透明度(背景,边界,外部线,阴影) */
    scui_coord_t    width[4];   /* 部件宽度(背景无效) */
    scui_coord_t    radius;     /* 部件圆角(背景半径) */
    /* 内部域(模式): */
    scui_coord_t    way;
    scui_coord_t    pct;
    scui_coord_t    lim;
    uint8_t         hold:1;
    uint8_t         click:1;
} scui_objbtn_t;

#pragma pack(push, 1)
typedef struct { scui_class_widget_maker(widget);
    /* 外部域: */
    scui_event_cb_t notify_cb;  /* 专属事件通知 */
    scui_color_t    color[4];   /* 部件源色调(s->e) */
    scui_alpha_t    alpha[4];   /* 部件透明度(背景,边界,外部线,阴影) */
    scui_coord_t    width[4];   /* 部件宽度(背景无效) */
    scui_coord_t    radius;     /* 部件圆角(背景半径) */
} scui_objbtn_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 *@param layout     通过布局创建
 */
void scui_objbtn_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_objbtn_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_objbtn_event(scui_event_t *event);

#endif
