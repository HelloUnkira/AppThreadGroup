#ifndef SCUI_WIDGET_DRAW_H
#define SCUI_WIDGET_DRAW_H

typedef enum {
    scui_widget_draw_type_none = 0,
    scui_widget_draw_type_string,
    scui_widget_draw_type_color,
    scui_widget_draw_type_color_grad,
    scui_widget_draw_type_blur,
    scui_widget_draw_type_image,
    scui_widget_draw_type_image_scale,
    scui_widget_draw_type_image_rotate,
    scui_widget_draw_type_image_matrix,
    scui_widget_draw_type_ring,
    scui_widget_draw_type_graph,
    scui_widget_draw_type_num,
} scui_widget_draw_type_t;

typedef struct {
    scui_widget_draw_type_t type;
    /*************************************************************************/
    scui_handle_t handle;                       // 控件句柄
    scui_area_t  *target;                       // 控件绘制区域
    union {
    /*************************************************************************/
    struct {
        scui_string_args_t *args;               // 字符串绘制参数
    } string;
    struct {
        scui_color_t color;                     // 源色调
    } color;
    struct {
        scui_color_t color;                     // 源色调
        scui_coord_t way;                       // 渐变方向(0:hor;1:ver;)
    } color_grad;
    struct {
        scui_area_t  *clip;                     // 图像源绘制区域
    } blur;
    struct {
        scui_handle_t image;                    // 图像句柄
        scui_area_t  *clip;                     // 图像源绘制区域
        scui_color_t  color;                    // 图像源色调(alpha图)
    } image;
    struct {
        scui_handle_t  image;                   // 图像句柄
        scui_area_t   *clip;                    // 图像源绘制区域
        scui_point_t   scale;                   // 图形缩放比例(1024为放大系数)
        scui_opt_pos_t pos;                     // 缩放锚点
    } image_scale;
    struct {
        scui_handle_t image;                    // 图像句柄
        scui_area_t  *clip;                     // 图像源绘制区域
        scui_point_t *anchor;                   // 图像旋转轴心
        scui_point_t *center;                   // 图像旋转中心
        scui_coord_t  angle;                    // 图像旋转角度(顺时针旋转:+,逆时针旋转:-)
    } image_rotate;
    struct {
        scui_handle_t  image;                   // 图像句柄
        scui_area_t   *clip;                    // 图像源绘制区域
        scui_matrix_t *matrix;                  // 变换矩阵(逆矩阵)
    } image_matrix;
    struct {
        scui_handle_t image;                    // 图像句柄
        scui_area_t  *clip;                     // 图像源绘制区域
        scui_color_t  color;                    // 源色调(alpha图)
        scui_coord_t  angle_s;                  // 起始角度
        scui_coord_t  angle_e;                  // 结束角度
        scui_coord_t  percent;                  // 圆环进度(百分比)
        scui_handle_t image_e;                  // 控件句柄
    } ring;
    struct {
        scui_draw_graph_dsc_t *graph_dsc;       // 绘制描述符实例
    } graph;
    /*************************************************************************/
    };
} scui_widget_draw_dsc_t;

/*@brief 控件绘制上下文
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx(scui_widget_draw_dsc_t *draw_dsc);

/*@brief 控件剪切域为空(绘制)
 *@param handle 控件句柄
 *@retval 控件剪切域为空
 */
bool scui_widget_draw_empty(scui_handle_t handle);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/*@brief 简化转义的宏api
 */

/* scui_widget_draw_type_string */
#define scui_widget_draw_string(handle_v, target_v, args_v)                 \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_string,                               \
        .string.args = args_v,                                              \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_widget_draw_type_color */
#define scui_widget_draw_color(handle_v, target_v, color_v)                 \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_color,                                \
        .color.color = color_v,                                             \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_widget_draw_type_color_grad */
#define scui_widget_draw_color_grad(handle_v, target_v, color_v, way_v)     \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_color_grad,                           \
        .color_grad.color = color_v,                                        \
        .color_grad.way   = way_v,                                          \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_widget_draw_type_blur */
#define scui_widget_draw_blur(handle_v, clip_v)                             \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = NULL,                                                     \
        .type = scui_widget_draw_type_blur,                                 \
        .blur.clip = clip_v,                                                \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_widget_draw_type_image */
#define scui_widget_draw_image(handle_v, target_v,                          \
    image_v, clip_v, color_v)                                               \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_image,                                \
        .image.image = image_v,                                             \
        .image.clip  = clip_v,                                              \
        .image.color = color_v,                                             \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_widget_draw_type_image_scale */
#define scui_widget_draw_image_scale(handle_v, target_v,                    \
    image_v, clip_v, scale_v, pos_v)                                        \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_image_scale,                          \
        .image_scale.image = image_v,                                       \
        .image_scale.clip  = clip_v,                                        \
        .image_scale.scale = scale_v,                                       \
        .image_scale.pos   = pos_v,                                         \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_widget_draw_type_image_rotate */
#define scui_widget_draw_image_rotate(handle_v, target_v,                   \
    image_v, clip_v, anchor_v, center_v, angle_v)                           \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_image_rotate,                         \
        .image_rotate.image  = image_v,                                     \
        .image_rotate.clip   = clip_v,                                      \
        .image_rotate.anchor = anchor_v,                                    \
        .image_rotate.center = center_v,                                    \
        .image_rotate.angle  = angle_v,                                     \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_widget_draw_type_image_matrix */
#define scui_widget_draw_image_matrix(handle_v, target_v,                   \
    image_v, clip_v, matrix_v)                                              \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_image_matrix,                         \
        .image_matrix.image  = image_v,                                     \
        .image_matrix.clip   = clip_v,                                      \
        .image_matrix.matrix = matrix_v,                                    \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_widget_draw_type_ring */
#define scui_widget_draw_ring(handle_v, target_v,                           \
    image_v, clip_v, angle_s_v, color_v, angle_e_v, percent_v, image_e_v);  \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_ring,                                 \
        .ring.image   = image_v,                                            \
        .ring.clip    = clip_v,                                             \
        .ring.color   = color_v,                                            \
        .ring.angle_s = angle_s_v,                                          \
        .ring.angle_e = angle_e_v,                                          \
        .ring.percent = percent_v,                                          \
        .ring.image_e = image_e_v,                                          \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_widget_draw_type_graph */
#define scui_widget_draw_graph(handle_v, target_v, graph_dsc_v)             \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_graph,                                \
        .graph.graph_dsc = graph_dsc_v,                                     \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#endif
