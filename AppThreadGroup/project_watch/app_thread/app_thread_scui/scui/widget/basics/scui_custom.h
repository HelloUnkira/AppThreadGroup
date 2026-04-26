#ifndef SCUI_CUSTOM_H
#define SCUI_CUSTOM_H

typedef enum {
    scui_custom_type_none = 0,
    scui_custom_type_slider,
    scui_custom_type_spinner,
    scui_custom_type_indicator,
    scui_custom_type_ring_edge,
    scui_custom_type_image_text,
    scui_custom_type_image_crect4,
    scui_custom_type_num,
} scui_custom_type_t;

typedef union {
    struct {
        scui_handle_t bar;          /* 图像句柄(背景图) */
        scui_handle_t edge;         /* 图像句柄(边界点) */
        scui_color_t  color_bar;    /* 图像源色调(alpha图使用) */
        scui_color_t  color_edge;   /* 图像源色调(alpha图使用) */
        scui_coord_t  vmin;         /* 最小值(默认可为百分比:0) */
        scui_coord_t  vmax;         /* 最大值(默认可为百分比:100) */
        scui_coord_t  cmin;         /* 当前最小值 */
        scui_coord_t  cmax;         /* 当前最大值 */
        scui_handle_t dist;         /* 宽度或高度 */
        bool          way;          /* 方向(0:水平方向;1:垂直方向) */
    } slider;
    struct {
        scui_handle_t spinner;      /* 图像句柄(alpha图:环) */
        scui_handle_t edge;         /* 图像句柄(alpha图:边界点) */
        scui_color_t  color;        /* 图像源色调(.color_l,.color_d,.color_f, .filter,) */
        scui_coord_t  percent;      /* 旋转百分比(0~100) */
        scui_coord_t  angle_s;      /* 旋转参考点(参考值270度) */
        scui_coord_t  angle_l;      /* 旋转参考点(参考值270度) */
        scui_coord_t  way;          /* 旋转方向(顺时针:+1;逆时针:-1;) */
    } spinner;
    struct {
        scui_handle_t wait;         /* 图像句柄(未选中) */
        scui_handle_t focus;        /* 图像句柄(选中) */
        scui_color_t  color_wait;   /* 图像源色调(alpha图使用) */
        scui_color_t  color_focus;  /* 图像源色调(alpha图使用) */
        scui_handle_t count;        /* 导航点数量 */
        scui_handle_t index;        /* 目标索引(选中) */
        scui_handle_t span;         /* 间隙 */
        bool          way;          /* 方向(0:水平方向;1:垂直方向) */
    } indicator;
    struct {
        scui_handle_t image;        /* 图像句柄 */
        scui_color_t  color;        /* 图像源色调(alpha图使用) */
        scui_point_t  center;       /* 旋转中心 */
        scui_coord_t  radius;       /* 旋转半径 */
        scui_multi_t  angle;        /* 旋转角度(顺时针旋转:+,逆时针旋转:-) */
    } ring_edge;
    struct {
        scui_handle_t *image;       /* 图像句柄 */
        scui_color_t   color;       /* 图像源色调(alpha图使用) */
        scui_coord_t   span;        /* 图像间隙 */
        scui_coord_t   num;         /* 图像数量 */
        bool           way;         /* 方向(0:水平方向;1:垂直方向) */
    } image_text;
    struct {
        scui_handle_t image[4];     /* 图像句柄(左上角,右上角,左下角,右下角) */
        scui_color_t  color;        /* 图像源色调(alpha图使用) */
        scui_coord_t  delta;        /* 边界填充线: */
                                    /*   0:忽略(复杂图像集成) */
                                    /*  -1:完全填充(全填充圆角矩形) */
                                    /*  其他:边界填充(空心圆角矩形) */
    } image_crect4;
    
    /* keep add... */
} scui_custom_data_t;

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_custom_type_t  type;
    scui_custom_data_t *data;   /* 不必要备份 */
    /* 内部域: */
    
    /* 让本控件获得字符串 */
    /* 基础绘制的扩展属性 */
    void **str_args;
    void **str_utf8;
} scui_custom_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_custom_type_t  type;
    scui_custom_data_t  data;
} scui_custom_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_custom_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_custom_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_custom_invoke(scui_event_t *event);

#endif
