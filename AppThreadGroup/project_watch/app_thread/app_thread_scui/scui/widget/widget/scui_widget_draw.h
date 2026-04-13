#ifndef SCUI_WIDGET_DRAW_H
#define SCUI_WIDGET_DRAW_H



typedef enum {
    scui_widget_draw_type_none = 0,
    scui_widget_draw_type_string,
    scui_widget_draw_type_color,
    scui_widget_draw_type_color_grad,
    scui_widget_draw_type_dither,
    scui_widget_draw_type_blur,
    scui_widget_draw_type_image,
    scui_widget_draw_type_image_scale,
    scui_widget_draw_type_image_rotate,
    scui_widget_draw_type_image_matrix,
    scui_widget_draw_type_qrcode,
    scui_widget_draw_type_barcode,
    scui_widget_draw_type_ring,
    scui_widget_draw_type_graph,
    scui_widget_draw_type_num,
} scui_widget_draw_type_t;

typedef struct {
    scui_widget_draw_type_t type;
    
    scui_handle_t   handle;                     /* 控件句柄 */
    scui_area_t    *target;                     /* 控件绘制域 */
    
    void           *str_args;                   /* 字符串绘制参数(scui_string_args_t) */
    
    scui_alpha_t    alpha;                      /* 像素,图像透明度 */
    scui_color_t    color;                      /* 像素,图像源色调 */
    scui_coord_t    way;                        /* 渐变方向(0:hor;1:ver;) */
    
    scui_handle_t   image;                      /* 图像句柄 */
    scui_area_t    *clip;                       /* 图像剪切域 */
    scui_point_t    scale;                      /* 图形缩放比例 */
    scui_opt_pos_t  pos;                        /* 缩放锚点 */
    scui_point_t    anchor;                     /* 图像旋转轴心 */
    scui_point_t    center;                     /* 图像旋转中心 */
    scui_multi_t    angle;                      /* 图像旋转角度(顺时针旋转:+,逆时针旋转:-) */
    scui_matrix_t  *matrix;                     /* 变换矩阵(逆矩阵) */
    
    scui_coord_t    angle_s;                    /* 起始角度 */
    scui_coord_t    angle_e;                    /* 结束角度 */
    scui_coord_t    percent;                    /* 圆环进度(百分比) */
    scui_handle_t   image_e;                    /* 控件句柄 */
    
    scui_multi_t    size;                       /* 字符串长度 */
    uint8_t        *data;                       /* url网址链接字符串 */
    
    void           *graph_dsc;                  /* 绘制描述符实例(scui_draw_dsc_t) */
    
} scui_widget_draw_dsc_t;


/*@brief 控件剪切域为空(绘制)
 *@param handle 控件句柄
 *@retval 控件剪切域为空
 */
bool scui_widget_draw_empty(scui_handle_t handle);

/*@breaf 计算绘制片段的最小区域
 *@param clip 最小绘制区域
 *@param frag 绘制片段区域
 *@param face 图形形状
 */
bool scui_widget_draw_frag(scui_area_t *clip, scui_area_t *frag, scui_face2_t *face);

/*@brief 控件绘制上下文
 *@param draw_graph 绘制参数实例
 */
void scui_widget_draw_ctx(scui_widget_draw_dsc_t *draw_dsc);

/*****************************************************************************/
/*@brief 简化转义的宏api
 */

/* scui_widget_draw_type_string */
#define scui_widget_draw_string(handle_v, target_v, str_args_v)             \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_string,                               \
        .str_args = str_args_v,                                             \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_color */
#define scui_widget_draw_color(handle_v, target_v, color_v)                 \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_color,                                \
        .color = color_v,                                                   \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_color_grad */
#define scui_widget_draw_color_grad(handle_v, target_v, color_v, way_v)     \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_color_grad,                           \
        .color = color_v,                                                   \
        .way   = way_v,                                                     \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_dither */
#define scui_widget_draw_dither(handle_v, target_v)                         \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_dither,                               \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_blur */
#define scui_widget_draw_blur(handle_v, target_v)                           \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_blur,                                 \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_image */
#define scui_widget_draw_image(handle_v, target_v,                          \
    image_v, clip_v, color_v)                                               \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_image,                                \
        .image = image_v,                                                   \
        .clip  = clip_v,                                                    \
        .color = color_v,                                                   \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_image_scale */
#define scui_widget_draw_image_scale(handle_v, target_v,                    \
    image_v, clip_v, scale_v, pos_v)                                        \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_image_scale,                          \
        .image = image_v,                                                   \
        .clip  = clip_v,                                                    \
        .scale = scale_v,                                                   \
        .pos   = pos_v,                                                     \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_image_rotate */
#define scui_widget_draw_image_rotate(handle_v, target_v,                   \
    image_v, clip_v, anchor_v, center_v, angle_v)                           \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_image_rotate,                         \
        .image  = image_v,                                                  \
        .clip   = clip_v,                                                   \
        .anchor = anchor_v,                                                 \
        .center = center_v,                                                 \
        .angle  = angle_v,                                                  \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_image_matrix */
#define scui_widget_draw_image_matrix(handle_v, target_v,                   \
    image_v, clip_v, matrix_v)                                              \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_image_matrix,                         \
        .image  = image_v,                                                  \
        .clip   = clip_v,                                                   \
        .matrix = matrix_v,                                                 \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_qrcode */
#define scui_widget_draw_qrcode(handle_v, target_v,                         \
    color_v, size_v, data_v)                                                \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_qrcode,                               \
        .color = color_v,                                                   \
        .size  = size_v,                                                    \
        .data  = data_v,                                                    \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_barcode */
#define scui_widget_draw_barcode(handle_v, target_v,                        \
    color_v, size_v, data_v)                                                \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_barcode,                              \
        .color = color_v,                                                   \
        .size  = size_v,                                                    \
        .data  = data_v,                                                    \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_ring */
#define scui_widget_draw_ring(handle_v, target_v,                           \
    image_v, clip_v, angle_s_v, color_v, angle_e_v, percent_v, image_e_v);  \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_ring,                                 \
        .image   = image_v,                                                 \
        .clip    = clip_v,                                                  \
        .color   = color_v,                                                 \
        .angle_s = angle_s_v,                                               \
        .angle_e = angle_e_v,                                               \
        .percent = percent_v,                                               \
        .image_e = image_e_v,                                               \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

/* scui_widget_draw_type_graph */
#define scui_widget_draw_graph(handle_v, target_v,                          \
    alpha_v, color_v, graph_dsc_v)                                          \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc_inst = {                                \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_graph,                                \
        .alpha = alpha_v,                                                   \
        .color = color_v,                                                   \
        .graph_dsc = graph_dsc_v,                                           \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc_inst);                                   \
} while (0)                                                                 \

#endif
