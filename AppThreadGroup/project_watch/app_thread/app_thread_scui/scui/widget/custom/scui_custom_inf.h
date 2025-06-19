#ifndef SCUI_CUSTOM_INF_H
#define SCUI_CUSTOM_INF_H

/*@brief 自定义控件子控件(可选)
 *@param handle   自定义控件句柄
 *@param handle_s 控件句柄地址
 */
void scui_custom_handle_m(scui_handle_t handle, scui_handle_t **handle_m);

/*@brief 自定义控件子控件树(可选)
 *@param handle   自定义控件句柄
 *@param handle_s 控件句柄地址
 */
void scui_custom_handle_s(scui_handle_t handle, scui_handle_t **handle_s);

/*@brief 自定义控件绘制描述符(全局,唯一)
 *@param handle   自定义控件句柄
 *@param draw_dsc 绘制描述符
 */
void scui_custom_draw_dsc(scui_handle_t handle, void **draw_dsc);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

typedef enum {
    scui_custom_draw_type_none = 0,
    /* basic draw type */
    scui_custom_draw_type_text,
    /* custom draw type */
    scui_custom_draw_type_dial_ptr,
    scui_custom_draw_type_spinner,
    scui_custom_draw_type_slider,
    scui_custom_draw_type_indicator,
    scui_custom_draw_type_ring_edge,
    scui_custom_draw_type_image_text,
    scui_custom_draw_type_image_crect4,
    scui_custom_draw_type_num,
} scui_custom_draw_type_t;

typedef struct {
    bool custom_draw_anim;
    scui_custom_draw_type_t type;
    /*************************************************************************/
    scui_event_t *event;            // 绘制事件
    scui_area_t  *clip;             // 绘制区域
    union {
    /* basic draw type */
    /*************************************************************************/
    struct {
        scui_handle_t       handle; // 绘制对象
        scui_area_t        *target; // 绘制区域
        scui_string_args_t *args;   // 字符串绘制参数
        scui_handle_t       text;   // 文本句柄
    } text;
    /* custom draw type */
    /*************************************************************************/
    struct {
        scui_handle_t image[3];     // 图片句柄(hour,minute,second)
        scui_point_t  anchor[3];    // 图片围绕轴心(hour,minute,second)
        scui_point_t  center[3];    // 图片旋转中心(hour,minute,second)
        /* frame anim sched: */
        uint64_t tick_mode:1;       // 1:一度一跳;0:一秒一跳;
        uint64_t tick_curr:31;      // 当前时刻:时.分.秒.毫秒
        uint64_t tick_last:31;      // 前一时刻:时.分.秒.毫秒
    } dial_ptr;
    struct {
        scui_handle_t spinner;      // 图像句柄(alpha图:环)
        scui_handle_t edge;         // 图像句柄(alpha图:边界点)
        scui_color_t  color;        // 图像源色调(.color_l,.color_d,.color_f, .filter,)
        scui_coord_t  percent;      // 旋转百分比(0~100)
        scui_coord_t  angle_s;      // 旋转参考点(参考值270度)
        scui_coord_t  angle_l;      // 旋转参考点(参考值270度)
        scui_coord_t  way;          // 旋转方向(顺时针:+1;逆时针:-1;)
    } spinner;
    struct {
        scui_handle_t bar;          // 图像句柄(背景图)
        scui_handle_t edge;         // 图像句柄(边界点)
        scui_color_t  color_bar;    // 图像源色调(alpha图使用)
        scui_color_t  color_edge;   // 图像源色调(alpha图使用)
        scui_coord_t  vmin;         // 最小值(默认可为百分比:0)
        scui_coord_t  vmax;         // 最大值(默认可为百分比:100)
        scui_coord_t  cmin;         // 当前最小值
        scui_coord_t  cmax;         // 当前最大值
        scui_handle_t dist;         // 宽度或高度
        bool          way;          // 方向(0:水平方向;1:垂直方向)
    } slider;
    struct {
        scui_handle_t wait;         // 图像句柄(未选中)
        scui_handle_t focus;        // 图像句柄(选中)
        scui_color_t  color_wait;   // 图像源色调(alpha图使用)
        scui_color_t  color_focus;  // 图像源色调(alpha图使用)
        scui_handle_t count;        // 导航点数量
        scui_handle_t index;        // 目标索引(选中)
        scui_handle_t span;         // 间隙
        bool          way;          // 方向(0:水平方向;1:垂直方向)
    } indicator;
    struct {
        scui_handle_t image;        // 图像句柄
        scui_color_t  color;        // 图像源色调(alpha图使用)
        scui_point_t *center;       // 旋转中心
        scui_coord_t  radius;       // 旋转半径
        scui_coord_t  angle;        // 旋转角度(顺时针旋转:+,逆时针旋转:-)
    } ring_edge;
    struct {
        scui_handle_t *image;       // 图像句柄
        scui_color_t   color;       // 图像源色调(alpha图使用)
        scui_coord_t   span;        // 图像间隙
        scui_coord_t   num;         // 图像数量
        bool           way;         // 方向(0:水平方向;1:垂直方向)
    } image_text;
    struct {
        scui_handle_t image[4];     // 图像句柄(左上角,右上角,左下角,右下角)
        scui_color_t  color;        // 图像源色调(alpha图使用)
        scui_coord_t  delta;        // 边界填充线:
                                    //   0:忽略(复杂图像集成)
                                    //  -1:完全填充(全填充圆角矩形)
                                    //  其他:边界填充(空心圆角矩形)
    } image_crect4;
    /*************************************************************************/
    };
} scui_custom_draw_dsc_t;

/*@brief 自定义控件:插件:上下文绘制
 *@param draw_graph 绘制参数实例
 */
void scui_custom_draw_ctx(scui_custom_draw_dsc_t *draw_dsc);

/*@brief 自定义控件:插件:上下文绘制(帧动画调度)
 *@param draw_graph 绘制参数实例
 */
void scui_custom_draw_anim_ctx(scui_custom_draw_dsc_t *draw_dsc);

/*****************************************************************************/
/*@brief 简化转义的宏api
 */

/* scui_custom_draw_type_text */
#define scui_custom_draw_text(handle_v, target_v, args_v, text_v)           \
do {                                                                        \
    scui_custom_draw_dsc_t draw_dsc = {                                     \
        .type = scui_custom_draw_type_text,                                 \
        .text.handle = handle_v,                                            \
        .text.target = target_v,                                            \
        .text.args   = args_v,                                              \
        .text.text   = text_v,                                              \
    };                                                                      \
    scui_custom_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/*****************************************************************************/
/*@brief 简化转义的宏api
 */

/* scui_custom_draw_type_dial_ptr */
#define scui_custom_draw_dial_ptr(event_v, clip_v,                          \
    image_v, anchor_v, center_v, tick_ms_v)                                 \
do {                                                                        \
    scui_custom_draw_dsc_t draw_dsc = { .event = event_v, .clip = clip_v,   \
        .type = scui_custom_draw_type_dial_ptr,                             \
        .dial_ptr.image[0]  = image_v[0],                                   \
        .dial_ptr.image[1]  = image_v[1],                                   \
        .dial_ptr.image[2]  = image_v[2],                                   \
        .dial_ptr.anchor[0] = anchor_v[0],                                  \
        .dial_ptr.anchor[1] = anchor_v[1],                                  \
        .dial_ptr.anchor[2] = anchor_v[2],                                  \
        .dial_ptr.center[0] = center_v[0],                                  \
        .dial_ptr.center[1] = center_v[1],                                  \
        .dial_ptr.center[2] = center_v[2],                                  \
        .dial_ptr.tick_ms_c = tick_ms_v,                                    \
    };                                                                      \
    scui_custom_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_custom_draw_type_spinner */
#define scui_custom_draw_spinner(event_v, clip_v,                           \
    spinner_v, color_v, edge_v, percent_v, angle_s_v, angle_l_v, way_v)     \
do {                                                                        \
    scui_custom_draw_dsc_t draw_dsc = { .event = event_v, .clip = clip_v,   \
        .type = scui_custom_draw_type_spinner,                              \
        .spinner.spinner = spinner_v,                                       \
        .spinner.edge    = edge_v,                                          \
        .spinner.color   = color_v,                                         \
        .spinner.percent = percent_v,                                       \
        .spinner.angle_s = angle_s_v,                                       \
        .spinner.angle_l = angle_l_v,                                       \
        .spinner.way     = way_v,                                           \
    };                                                                      \
    scui_custom_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_custom_draw_type_slider */
#define scui_custom_draw_slider(event_v, clip_v,                            \
    bar_v, color_bar_v, edge_v, color_edge_v,                               \
    vmin_v, vmax_v, cmin_v, cmax_v, dist_v, way_v)                          \
do {                                                                        \
    scui_custom_draw_dsc_t draw_dsc = { .event = event_v, .clip = clip_v,   \
        .type = scui_custom_draw_type_slider,                               \
        .slider.bar        = bar_v,                                         \
        .slider.edge       = edge_v,                                        \
        .slider.color_bar  = color_bar_v,                                   \
        .slider.color_edge = color_edge_v,                                  \
        .slider.vmin       = vmin_v,                                        \
        .slider.vmax       = vmax_v,                                        \
        .slider.cmin       = cmin_v,                                        \
        .slider.cmax       = cmax_v,                                        \
        .slider.dist       = dist_v,                                        \
        .slider.way        = way_v,                                         \
    };                                                                      \
    scui_custom_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \


/* scui_custom_draw_type_indicator */
#define scui_custom_draw_indicator(event_v, clip_v,                         \
    wait_v, color_wait_v, focus_v, color_focus_v,                           \
    count_v, index_v, span_v, way_v)                                        \
do {                                                                        \
    scui_custom_draw_dsc_t draw_dsc = { .event = event_v, .clip = clip_v,   \
        .type = scui_custom_draw_type_indicator,                            \
        .indicator.wait        = wait_v,                                    \
        .indicator.focus       = focus_v,                                   \
        .indicator.color_wait  = color_wait_v,                              \
        .indicator.color_focus = color_focus_v,                             \
        .indicator.count       = count_v,                                   \
        .indicator.index       = index_v,                                   \
        .indicator.span        = span_v,                                    \
        .indicator.way         = way_v,                                     \
    };                                                                      \
    scui_custom_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_custom_draw_type_ring_edge */
#define scui_custom_draw_ring_edge(event_v, clip_v,                         \
    center_v, image_v, color_v, radius_v, angle_v)                          \
do {                                                                        \
    scui_custom_draw_dsc_t draw_dsc = { .event = event_v, .clip = clip_v,   \
        .type = scui_custom_draw_type_ring_edge,                            \
        .ring_edge.image  = image_v,                                        \
        .ring_edge.color  = color_v,                                        \
        .ring_edge.center = center_v,                                       \
        .ring_edge.radius = radius_v,                                       \
        .ring_edge.angle  = angle_v,                                        \
    };                                                                      \
    scui_custom_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_custom_draw_type_image_text */
#define scui_custom_draw_image_text(event_v, clip_v,                        \
    image_v, color_v, span_v, num_v, way_v)                                 \
do {                                                                        \
    scui_custom_draw_dsc_t draw_dsc = { .event = event_v, .clip = clip_v,   \
        .type = scui_custom_draw_type_image_text,                           \
        .image_text.image = image_v,                                        \
        .image_text.color = color_v,                                        \
        .image_text.span  = span_v,                                         \
        .image_text.way   = way_v,                                          \
        .image_text.num   = num_v,                                          \
    };                                                                      \
    scui_custom_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_custom_draw_type_image_crect4 */
#define scui_custom_draw_image_crect4(event_v, clip_v,                      \
    image_v, color_v, delta_v)                                              \
do {                                                                        \
    scui_custom_draw_dsc_t draw_dsc = { .event = event_v, .clip = clip_v,   \
        .type = scui_custom_draw_type_image_crect4,                         \
        .image_crect4.image[0] = image_v[0],                                \
        .image_crect4.image[1] = image_v[1],                                \
        .image_crect4.image[2] = image_v[2],                                \
        .image_crect4.image[3] = image_v[3],                                \
        .image_crect4.color = color_v,                                      \
        .image_crect4.delta = delta_v,                                      \
    };                                                                      \
    scui_custom_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

#endif
