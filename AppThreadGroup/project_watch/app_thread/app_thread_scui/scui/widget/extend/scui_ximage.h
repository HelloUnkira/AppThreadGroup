#ifndef SCUI_XIMAGE_H
#define SCUI_XIMAGE_H

typedef enum {
    scui_ximage_type_none = 0,
    scui_ximage_type_vedio,
    scui_ximage_type_qrcode,
    scui_ximage_type_barcode,
    scui_ximage_type_sequence,
    scui_ximage_type_replace,
    scui_ximage_type_num,
} scui_ximage_type_t;

typedef union {
    struct {
        scui_vedio_t   frame;           /* 图像帧资源 */
        scui_multi_t   speed;           /* 播放速度 */
        scui_multi_t   tick;            /* 播放步进累积 */
        scui_multi_t   loop;            /* 播放次数(-1:infinite) */
        scui_sbitfd_t  work:1;          /* 播放/停止 */
    } vedio;
    struct {
        scui_handle_t  image;           /* 生成图像句柄 */
        scui_image_t   image_src;       /* 生成图像源 */
        scui_color_t   color;           /* 图像颜色 */
        uint8_t       *data;            /* url字符串 */
        uint32_t       size;            /* url字符串长度 */
    } qrcode;
    struct {
        scui_handle_t  image;           /* 生成图像句柄 */
        scui_image_t   image_src;       /* 生成图像源 */
        scui_color_t   color;           /* 图像颜色 */
        uint8_t       *data;            /* url字符串 */
        uint32_t       size;            /* url字符串长度 */
    } barcode;
    struct {
        scui_handle_t *image;           /* 图像句柄列表 */
        scui_color_t   color;           /* 图像源色调(alpha图使用) */
        scui_coord_t   span;            /* 图像间隙 */
        scui_coord_t   num;             /* 图像数量 */
        bool           way;             /* 方向(0:水平方向;1:垂直方向) */
    } sequence;
    struct {
        scui_handle_t *image;           /* 播放图像句柄列表 */
        scui_coord_t   num;             /* 播放图像数量 */
        scui_coord_t   curr;            /* 当前帧索引 */
        scui_multi_t   speed;           /* 播放速度 */
        scui_multi_t   tick;            /* 播放步进累积 */
        scui_multi_t   loop;            /* 播放次数(-1:infinite) */
        scui_sbitfd_t  work:1;          /* 播放/停止 */
    } replace;
    /* keep adding */
} scui_ximage_data_t;

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    /* 内部域: */
    scui_ximage_type_t type;
    scui_ximage_data_t data;
} scui_ximage_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
} scui_ximage_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_ximage_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_ximage_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_ximage_invoke(scui_event_t *event);

#endif