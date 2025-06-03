#ifndef SCUI_WIDGET_INF_H
#define SCUI_WIDGET_INF_H

/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle);

/*@brief 控件的父控件
 *@param handle 控件句柄
 *@retval 父控件
 */
scui_handle_t scui_widget_parent(scui_handle_t handle);

/*@brief 子控件总数量
 *@param handle 控件句柄
 *@retval 子控件数量
 */
scui_handle_t scui_widget_child_num(scui_handle_t handle);

/*@brief 指定位置子控件
 *@param handle 控件句柄
 *@param index  子控件位置(映射点)
 *@retval 子控件句柄
 */
scui_handle_t scui_widget_child_by_index(scui_handle_t handle, scui_handle_t index);

/*@brief 指定位置子控件
 *@param handle 控件句柄
 *@param index  子控件句柄
 *@retval 子控件句柄
 */
scui_handle_t scui_widget_child_to_index(scui_handle_t handle, scui_handle_t child);

/*@brief 控件类型
 *@param handle 控件句柄
 *@retval 控件类型
 */
scui_widget_type_t scui_widget_type(scui_handle_t handle);

/*@brief 控件剪切域
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_clip(scui_handle_t handle);

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_is_show(scui_handle_t handle);

/*@brief 控件隐藏状态获取
 *@param handle 控件句柄
 *@retval 是否隐藏
 */
bool scui_widget_is_hide(scui_handle_t handle);

/*@brief 用户资源获取
 *@param handle    控件句柄
 *@param user_data 用户资源
 */
void scui_widget_user_data_get(scui_handle_t handle, void **user_data);

/*@brief 控件透明度获取
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
void scui_widget_alpha_get(scui_handle_t handle, scui_alpha_t *alpha);

/*@brief 控件图片获取
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_get(scui_handle_t handle, scui_handle_t *image);

/*@brief 控件颜色获取
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_get(scui_handle_t handle, scui_color_t *color);

/*@brief 用户资源设置
 *@param handle    控件句柄
 *@param user_data 用户资源
 */
void scui_widget_user_data_set(scui_handle_t handle, void *user_data);

/*@brief 控件透明度设置
 *@param handle  控件句柄
 *@param alpha   控件透明度
 *@param recurse 递归处理
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha, bool recurse);

/*@brief 控件图片设置
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_set(scui_handle_t handle, scui_handle_t image);

/*@brief 控件颜色设置
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_set(scui_handle_t handle, scui_color_t color);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 销毁控件
 *@param handle 控件句柄
 */
void scui_widget_destroy(scui_handle_t handle);

/*@brief 创建控件
 *@param maker  控件实例构造参数
 *@param handle 控件句柄
 *@param layout 通过布局
 */
void scui_widget_create(void *maker, scui_handle_t *handle, bool layout);

/*@brief 通过映射表调用创建一个控件树
 *@param handle 根控件句柄
 */
void scui_widget_create_layout_tree(scui_handle_t handle);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 控件坐标更新
 *@param handle 控件句柄
 *@param point  坐标点
 *@param dirty  脏标记(批量移动控件使用, 默认true)
 */
void scui_widget_move_pos(scui_handle_t handle, scui_point_t *point, bool dirty);

/*@brief 控件移动
 *@param handle 控件句柄
 *@param offset 偏移量
 *@param dirty  脏标记
 */
void scui_widget_move_ofs(scui_handle_t handle, scui_point_t *offset, bool dirty);

/*@brief 子控件坐标对齐
 *@param handle  控件句柄
 *@param handle  控件句柄(目标控件,不存在则相对父控件)
 *@param align   对齐方向
 *@param offset  偏移量
 */
void scui_widget_align_pos(scui_handle_t handle, scui_handle_t target, scui_opt_align_t align, scui_point_t *offset);

/*@brief 子控件坐标镜像
 *@param handle  控件句柄
 *@param child   控件子控件句柄(为空则镜像所有子控件)
 *@param dir     镜像方向(水平镜像或垂直镜像)
 *@param recurse 递归处理(全局镜像有效)
 */
void scui_widget_mirror_pos(scui_handle_t handle, scui_handle_t child, scui_opt_dir_t dir, bool recurse);

/*@brief 控件尺寸更新
 *@param handle 控件句柄
 *@param width  宽度
 *@param height 高度
 */
void scui_widget_adjust_size(scui_handle_t handle, scui_coord_t width, scui_coord_t height);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 绘制控件
 *@param handle 控件句柄
 *@param clip   剪切域
 *@param sync   同步绘制
 */
void scui_widget_draw(scui_handle_t handle, scui_area_t *clip, bool sync);

/*@brief 刷新控件
 *@param handle 控件句柄
 *@param sync   同步刷新
 */
void scui_widget_refr(scui_handle_t handle, bool sync);

/*@brief 控件显示
 *@param handle 控件句柄
 *@param delay  迟延调度
 */
void scui_widget_show(scui_handle_t handle, bool delay);

/*@brief 控件隐藏
 *@param handle 控件句柄
 *@param delay  迟延调度
 */
void scui_widget_hide(scui_handle_t handle, bool delay);

/*@brief 清除事件的所有自定义回调
 *@param handle 控件句柄
 */
void scui_widget_event_clear(scui_handle_t handle);

/*@brief 获取事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_find(scui_handle_t handle, scui_event_cb_node_t *node);

/*@brief 添加事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_add(scui_handle_t handle, scui_event_cb_node_t *node);

/*@brief 清除事件的自定义回调
 *@param handle 控件句柄
 *@param node   自定义控件回调
 */
void scui_widget_event_del(scui_handle_t handle, scui_event_cb_node_t *node);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

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
    scui_widget_draw_type_qrcode,
    scui_widget_draw_type_barcode,
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
        void *occupy;                           // 无效字段(占用)
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
        scui_point_t  anchor;                   // 图像旋转轴心
        scui_point_t  center;                   // 图像旋转中心
        scui_coord_t  angle;                    // 图像旋转角度(顺时针旋转:+,逆时针旋转:-)
    } image_rotate;
    struct {
        scui_handle_t  image;                   // 图像句柄
        scui_area_t   *clip;                    // 图像源绘制区域
        scui_matrix_t *matrix;                  // 变换矩阵(逆矩阵)
    } image_matrix;
    struct {
        scui_color_t color;                     // 图像源色调
        scui_multi_t size;                      // 字符串长度
        uint8_t     *data;                      // url网址链接字符串
    } qrcode;
    struct {
        scui_color_t color;                     // 图像源色调
        scui_multi_t size;                      // 字符串长度
        uint8_t     *data;                      // url网址链接字符串
    } barcode;
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
#define scui_widget_draw_blur(handle_v, target_v)                           \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_blur,                                 \
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

/* scui_widget_draw_type_qrcode */
#define scui_widget_draw_qrcode(handle_v, target_v,                         \
    color_v, size_v, data_v)                                                \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_qrcode,                               \
        .qrcode.color = color_v,                                            \
        .qrcode.size  = size_v,                                             \
        .qrcode.data  = data_v,                                             \
    };                                                                      \
    scui_widget_draw_ctx(&draw_dsc);                                        \
} while (0)                                                                 \

/* scui_widget_draw_type_barcode */
#define scui_widget_draw_barcode(handle_v, target_v,                        \
    color_v, size_v, data_v)                                                \
do {                                                                        \
    scui_widget_draw_dsc_t draw_dsc = {                                     \
        .handle = handle_v,                                                 \
        .target = target_v,                                                 \
        .type = scui_widget_draw_type_barcode,                              \
        .barcode.color = color_v,                                           \
        .barcode.size  = size_v,                                            \
        .barcode.data  = data_v,                                            \
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

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

#endif
