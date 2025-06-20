#ifndef SCUI_OBJBTN_H
#define SCUI_OBJBTN_H

/*@brief objbtn备注:
 *       本控件是从lvgl中的obj吸收而来
 *       因为lv_btn与lv_obj相似性非常高
 *       实际上这就是一个常规的像素按钮
 *       按钮共分为四个部件(lvgl):
 *           背景,边界,外部线,阴影
 */

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_color_t    color[4];   /* 部件源色调(s->e) */
    scui_alpha_t    alpha[4];   /* 部件透明度(背景,边界,外部线,阴影) */
    scui_coord_t    width[4];   /* 部件宽度(背景无效) */
    scui_coord_t    radius;     /* 部件圆角(背景半径) */
    /* 内部域(模式): */
    scui_coord_t    way;
    scui_coord_t    pct;
    scui_coord_t    lim;
    uintptr_t       hold:1;
    uintptr_t       click:1;
} scui_objbtn_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
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
 */
void scui_objbtn_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_objbtn_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_objbtn_invoke(scui_event_t *event);

#endif
