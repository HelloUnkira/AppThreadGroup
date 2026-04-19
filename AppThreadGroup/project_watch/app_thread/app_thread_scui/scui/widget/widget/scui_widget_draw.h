#ifndef SCUI_WIDGET_DRAW_H
#define SCUI_WIDGET_DRAW_H

typedef struct {
    
    void           *str_args;       /* 类型:(scui_string_args_t) */
    
    scui_alpha_t    alpha;          /* 像素,图像透明度 */
    scui_color_t    color;          /* 像素,图像源色调 */
    scui_sbitfd_t   way:1;          /* 渐变方向(水平:0;垂直:1;) */
    
    scui_handle_t   image;          /* 图像句柄 */
    scui_area_t    *clip;           /* 图像剪切域 */
    scui_point_t    scale;          /* 图形缩放比例 */
    scui_opt_pos_t  pos;            /* 缩放锚点 */
    scui_point_t    anchor;         /* 图像旋转轴心 */
    scui_point_t    center;         /* 图像旋转中心 */
    scui_multi_t    angle;          /* 图像旋转角度(顺:+,逆:-) */
    scui_matrix_t  *matrix;         /* 变换矩阵 */
    scui_matrix_t  *inv_matrix;     /* 变换矩阵 */
    
    scui_coord_t    angle_s;        /* 起始角度 */
    scui_coord_t    angle_e;        /* 结束角度 */
    scui_coord_t    percent;        /* 圆环进度(百分比) */
    scui_handle_t   image_e;        /* 控件句柄 */
    
    scui_multi_t    size;           /* 字符串长度 */
    uint8_t        *data;           /* url网址链接字符串 */
    
    void           *graph;          /* 类型:(scui_draw_dsc_t->graph) */
    
} scui_widget_draw_dsc_t;


/*@brief 控件剪切域为空(绘制)
 *@param handle 控件句柄
 *@retval 控件剪切域为空
 */
bool scui_widget_draw_empty(scui_handle_t handle);

/*@brief 计算绘制片段的最小区域
 *@param clip 最小绘制区域
 *@param frag 绘制片段区域
 *@param face 图形形状
 */
bool scui_widget_draw_frag(scui_area_t *clip, scui_area_t *frag, scui_face2_t *face);

/* widget draw ctx: */
/*****************************************************************************/
#define SCUI_WIDGET_DRAW_CTX_DECLARE(name) void name(scui_handle_t handle, \
    scui_area_t *target, scui_widget_draw_dsc_t *draw_dsc)
/*****************************************************************************/
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_string);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_color);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_color_grad);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_dither);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_blur);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_image);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_image_scale);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_image_rotate);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_image_matrix);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_ring);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_qrcode);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_barcode);
SCUI_WIDGET_DRAW_CTX_DECLARE(scui_widget_draw_ctx_graph);
/*****************************************************************************/
/*@brief 简化转义的宏api
 */

#define scui_widget_draw_string(handle_v, target_v, str_args_v)                 \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {.str_args = str_args_v,};              \
    scui_widget_draw_ctx_string(handle_v, target_v, &scui_wdc_i);               \
} while (0)                                                                     \

#define scui_widget_draw_color(handle_v, target_v, color_v)                     \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {.color = color_v,};                    \
    scui_widget_draw_ctx_color(handle_v, target_v, &scui_wdc_i);                \
} while (0)                                                                     \

#define scui_widget_draw_color_grad(handle_v, target_v, color_v, way_v)         \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {.color = color_v,.way = way_v,};       \
    scui_widget_draw_ctx_color_grad(handle_v, target_v, &scui_wdc_i);           \
} while (0)                                                                     \

#define scui_widget_draw_dither(handle_v, target_v)                             \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {0};                                    \
    scui_widget_draw_ctx_dither(handle_v, target_v, &scui_wdc_i);               \
} while (0)                                                                     \

#define scui_widget_draw_blur(handle_v, target_v)                               \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {0};                                    \
    scui_widget_draw_ctx_blur(handle_v, target_v, &scui_wdc_i);                 \
} while (0)                                                                     \

#define scui_widget_draw_image(handle_v, target_v,                              \
    image_v, clip_v, color_v)                                                   \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {                                       \
        .image = image_v,.clip = clip_v,.color = color_v,};                     \
    scui_widget_draw_ctx_image(handle_v, target_v, &scui_wdc_i);                \
} while (0)                                                                     \

#define scui_widget_draw_image_scale(handle_v, target_v,                        \
    image_v, clip_v, scale_v, pos_v)                                            \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {                                       \
       .image = image_v,.clip = clip_v,.scale = scale_v,.pos = pos_v,};         \
    scui_widget_draw_ctx_image_scale(handle_v, target_v, &scui_wdc_i);          \
} while (0)                                                                     \

#define scui_widget_draw_image_rotate(handle_v, target_v,                       \
    image_v, clip_v, anchor_v, center_v, angle_v)                               \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {.image = image_v,.clip = clip_v,       \
        .anchor = anchor_v,.center = center_v,.angle = angle_v,};               \
    scui_widget_draw_ctx_image_rotate(handle_v, target_v, &scui_wdc_i);         \
} while (0)                                                                     \

#define scui_widget_draw_image_matrix(handle_v, target_v,                       \
    image_v, clip_v, matrix_v, inv_matrix_v)                                    \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {.image = image_v,.clip = clip_v,       \
        .matrix = matrix_v,.inv_matrix = inv_matrix_v,};                        \
    scui_widget_draw_ctx_image_matrix(handle_v, target_v, &scui_wdc_i);         \
} while (0)                                                                     \

#define scui_widget_draw_qrcode(handle_v, target_v,                             \
    color_v, size_v, data_v)                                                    \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {                                       \
        .color = color_v,.size = size_v,.data = data_v,};                       \
    scui_widget_draw_ctx_qrcode(handle_v, target_v, &scui_wdc_i);               \
} while (0)                                                                     \

#define scui_widget_draw_barcode(handle_v, target_v,                            \
    color_v, size_v, data_v)                                                    \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {                                       \
        .color = color_v,.size = size_v,.data = data_v,};                       \
    scui_widget_draw_ctx_barcode(handle_v, target_v, &scui_wdc_i);              \
} while (0)                                                                     \

#define scui_widget_draw_ring(handle_v, target_v,                               \
    image_v, clip_v, angle_s_v, color_v, angle_e_v, percent_v, image_e_v)       \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {.image = image_v,.clip = clip_v,       \
        .color = color_v,.angle_s = angle_s_v,.angle_e = angle_e_v,             \
        .percent = percent_v,.image_e = image_e_v,};                            \
    scui_widget_draw_ctx_ring(handle_v, target_v, &scui_wdc_i);                 \
} while (0)                                                                     \

#define scui_widget_draw_graph(handle_v, target_v,                              \
    alpha_v, color_v, graph_v)                                                  \
do {                                                                            \
    scui_widget_draw_dsc_t scui_wdc_i = {                                       \
        .alpha = alpha_v,.color = color_v,.graph = graph_v,};                   \
    scui_widget_draw_ctx_graph(handle_v, target_v, &scui_wdc_i);                \
} while (0)                                                                     \



#endif
