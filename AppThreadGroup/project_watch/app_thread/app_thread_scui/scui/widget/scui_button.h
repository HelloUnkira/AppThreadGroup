#ifndef SCUI_BUTTON_H
#define SCUI_BUTTON_H

typedef enum {
    scui_button_type_image = 0,
    scui_button_type_pixel,
} scui_button_type_t;

typedef enum {
    scui_button_mode_static = 0,    // 静态按钮
    scui_button_mode_scale,         // 缩放按钮
} scui_button_mode_t;

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_button_type_t      type;           /* 按钮类型 */
    scui_button_mode_t      mode;           /* 按钮模式 */
    union {
    struct {
        scui_handle_t       image[4];       /* 图像句柄:四角(左上,右上,左下,右下) */
        scui_color_t        color;          /* 图像源色调 */
        scui_coord_t        delta;          /* 边界填充线(0:忽略;-1:所有;其他:边界) */
    } image;
    struct {
        scui_color_t        color[4];       /* 部件源色调(s->e) */
        scui_alpha_t        alpha[4];       /* 部件透明度(背景,边界,外部线,阴影) */
        scui_coord_t        width[4];       /* 部件宽度(背景无效) */
        scui_coord_t        radius;         /* 部件圆角(背景半径) */
    } pixel;
    };
    /* 内部域: */
    union {
        /* 缩放按钮 */
        struct {
            scui_coord_t    btn1_lim;
            scui_coord_t    btn1_pct;
            scui_coord_t    btn1_way;
        };
    };
    uintptr_t   btn_hold:1;
    uintptr_t   btn_click:1;
} scui_button_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_button_type_t      type;           /* 按钮类型 */
    scui_button_mode_t      mode;           /* 按钮模式 */
    union {
    struct {
        scui_handle_t       image[4];       /* 图像句柄:四角(左上,右上,左下,右下) */
        scui_color_t        color;          /* 图像源色调 */
        scui_coord_t        delta;          /* 边界填充线(0:忽略;-1:所有;其他:边界) */
    } image;
    struct {
        scui_color_t        color[4];       /* 部件源色调(s->e) */
        scui_alpha_t        alpha[4];       /* 部件透明度(背景,边界,外部线,阴影) */
        scui_coord_t        width[4];       /* 部件宽度(背景无效) */
        scui_coord_t        radius;         /* 部件圆角(背景半径) */
    } pixel;
    };
} scui_button_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_button_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_button_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_button_invoke(scui_event_t *event);

#endif
