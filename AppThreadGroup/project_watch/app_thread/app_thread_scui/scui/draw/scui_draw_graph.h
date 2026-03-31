#ifndef SCUI_DRAW_GRAPH_H
#define SCUI_DRAW_GRAPH_H

/*绘制语义约定:
 *    dst_surface + dst_clip
 *    src_surface + src_clip
 *    将src_surface中的src_clip范围内的区域
 *    到dst_surface中的dst_clip范围内的区域
 *    且变换要求取最小交集区域
 *    即为原点对齐变换
 */

typedef enum {
    scui_draw_type_none = 0,
    scui_draw_type_byte_copy,
    scui_draw_type_area_blur,
    scui_draw_type_area_fill,
    scui_draw_type_area_fill_grad,
    scui_draw_type_area_fill_grads,
    scui_draw_type_area_copy,
    scui_draw_type_area_blend,
    scui_draw_type_area_alpha_filter,
    scui_draw_type_area_matrix_fill,
    scui_draw_type_area_matrix_blend,
    scui_draw_type_image,
    scui_draw_type_image_scale,
    scui_draw_type_image_rotate,
    scui_draw_type_image_matrix_blend,
    scui_draw_type_letter,
    scui_draw_type_string,
    scui_draw_type_qrcode,
    scui_draw_type_barcode,
    scui_draw_type_ring,
    
    /* 基础图元放到最后 */
    scui_draw_type_pixel_line,
    scui_draw_type_pixel_circle,
    scui_draw_type_pixel_crect,
    scui_draw_type_pixel_shadow,
    scui_draw_type_pixel_arc,
    
    scui_draw_type_num,
} scui_draw_type_t;

typedef struct {
    scui_draw_type_t type;
    scui_sbitfd_t    sync:1;
    
    /* 绘制上文所有信息 */
    /* 目标:收集到一个栈帧 */
    /* 注意:资源必须以控件树为生命周期 */
    
    /*************************************************************************/
    union {
    /**************************************************************************
     * 绘制目标(备注:该结构要与下面的结构首部对齐)
     */
    struct {
        scui_surface_t     *surface;        /* 画布实例 */
        scui_area_t         clip;           /* 画布绘制区域 */
    } task_src;
    /**************************************************************************
     * draw basic(HW ACC Perhaps):
     */
    struct {
        void   *dst_addr;   /* 目标地址 */
        void   *src_addr;   /* 源初地址 */
        uint32_t src_len;   /* 数据字节长度 */
    } byte_copy;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
    } area_blur;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_alpha_t        src_alpha;      /* 像素点透明度 */
        scui_color_t        src_color;      /* 像素点协议色 */
    } area_fill;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_area_t         src_clip;       /* 源剪切域 */
        scui_alpha_t        src_alpha;      /* 像素点透明度 */
        scui_color_t        src_color;      /* 像素点协议色 */
        scui_coord_t        src_way;        /* 渐变方向(0:hor;1:ver;) */
    } area_fill_grad;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_alpha_t        src_alpha;      /* 像素点透明度 */
        scui_color_t       *src_grad_s;     /* 源渐变列表 */
        scui_coord_t        src_grad_n;     /* 源渐变列表数量 */
        scui_color_t        src_filter;     /* 源渐变滤色 */
        scui_coord_t        src_way;        /* 源渐变方向(0:hor;1:ver;) */
    } area_fill_grads;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_surface_t     *src_surface;    /* 画布实例 */
        scui_area_t         src_clip;       /* 画布绘制区域 */
    } area_copy;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_surface_t     *src_surface;    /* 画布实例 */
        scui_area_t         src_clip;       /* 画布绘制区域 */
        scui_color_t        src_color;      /* 画布协议色 */
    } area_blend;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_surface_t     *src_surface;    /* 画布实例 */
        scui_area_t         src_clip;       /* 画布绘制区域 */
    } area_alpha_filter;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_area_t         src_clip;       /* 画布绘制区域 */
        scui_alpha_t        src_alpha;      /* 像素点透明度 */
        scui_color_t        src_color;      /* 画布协议色 */
        scui_matrix_t       inv_matrix;     /* 逆变换矩阵 */
        scui_matrix_t       src_matrix;     /* 源变换矩阵 */
    } area_matrix_fill;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_surface_t     *src_surface;    /* 画布实例 */
        scui_area_t         src_clip;       /* 画布绘制区域 */
        scui_color_t        src_color;      /* 画布协议色 */
        scui_matrix_t       inv_matrix;     /* 逆变换矩阵 */
        scui_matrix_t       src_matrix;     /* 源变换矩阵 */
    } area_matrix_blend;
    /**************************************************************************
     * draw image:
     */
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_image_t       *src_image;      /* 图像源 */
        scui_area_t         src_clip;       /* 图像源绘制区域 */
        scui_alpha_t        src_alpha;      /* 图像透明度 */
        scui_color_t        src_color;      /* 图像源色调 */
    } image;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_image_t       *src_image;      /* 图像源 */
        scui_area_t         src_clip;       /* 图像源绘制区域 */
        scui_alpha_t        src_alpha;      /* 图像透明度 */
        scui_color_t        src_color;      /* 图像源色调 */
        scui_point_t        src_scale;      /* 图形缩放比例 */
        scui_point_t        dst_offset;     /* 缩放锚点 */
        scui_point_t        src_offset;     /* 缩放锚点 */
    } image_scale;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_image_t       *src_image;      /* 图像源 */
        scui_area_t         src_clip;       /* 图像源绘制区域 */
        scui_alpha_t        src_alpha;      /* 图像透明度 */
        scui_color_t        src_color;      /* 图像源色调 */
        scui_multi_t        src_angle;      /* 图形旋转角度(顺时针:+,逆时针:-) */
        scui_point_t        src_anchor;     /* 图像旋转轴心 */
        scui_point_t        src_center;     /* 图像旋转中心 */
    } image_rotate;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_image_t       *src_image;      /* 图像源 */
        scui_area_t         src_clip;       /* 图像源绘制区域 */
        scui_alpha_t        src_alpha;      /* 图像透明度 */
        scui_color_t        src_color;      /* 图像源色调 */
        scui_matrix_t       inv_matrix;     /* 逆变换矩阵 */
        scui_matrix_t       src_matrix;     /* 源变换矩阵 */
    } image_matrix_blend;
    /**************************************************************************
     * draw letter & string:
     */
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_area_t         src_clip;       /* 字符绘制区域 */
        scui_alpha_t        src_alpha;      /* 字符透明度 */
        scui_color_t        src_color;      /* 字符色调 */
        scui_font_glyph_t  *src_glyph;      /* 字符信息 */
    } letter;
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_area_t         src_clip;       /* 画布绘制区域 */
        scui_alpha_t        src_alpha;      /* 字符透明度 */
        scui_string_args_t *src_args;       /* 字符串绘制参数 */
    } string;
    /**************************************************************************
     * draw ring:
     */
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_point_t        dst_center;     /* 图像旋转点 */
        scui_image_t       *src_image_e;    /* 图像源(边界点) */
        scui_image_t       *src_image;      /* 图像源 */
        scui_area_t         src_clip;       /* 图像源绘制区域 */
        scui_coord_t        src_angle_s;    /* 起始角度 */
        scui_alpha_t        src_alpha;      /* 图像透明度 */
        scui_coord_t        src_angle_e;    /* 结束角度 */
        scui_color_t        src_color;      /* 图像源色调 */
    } ring;
    /**************************************************************************
     * draw qrcode:
     * draw barcode:
     */
     struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_area_t         src_clip;       /* 图像绘制区域 */
        scui_alpha_t        src_alpha;      /* 图像透明度 */
        scui_color_t        src_color;      /* 图像源色调 */
        scui_multi_t        src_size;       /* 字符串长度 */
        uint8_t            *src_data;       /* url网址链接字符串 */
     } qrcode;
     struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_area_t         src_clip;       /* 图像绘制区域 */
        scui_alpha_t        src_alpha;      /* 图像透明度 */
        scui_color_t        src_color;      /* 图像源色调 */
        scui_multi_t        src_size;       /* 字符串长度 */
        uint8_t            *src_data;       /* url网址链接字符串 */
     } barcode;
    /**************************************************************************
     * keep adding...
     */
    /*************************************************************************/
    /*************************************************************************/
    /*************************************************************************/
    /* 基础图元绘制单独提出(Antialias) */
    /* 因为这一部分可以使用任何库的绘制层简要摘出原理使用 */
    /**************************************************************************
     * draw graph:
     */
    struct {
        scui_surface_t     *dst_surface;    /* 画布实例 */
        scui_area_t         dst_clip;       /* 画布绘制区域 */
        scui_alpha_t        src_alpha;      /* 透明度 */
        scui_color_t        src_color;      /* 源色调 */
        
        scui_point_t        src_center;     /* 旋转中心 */
        scui_coord_t        src_angle_s;    /* 起始角度 */
        scui_coord_t        src_angle_e;    /* 结束角度 */
        scui_coord_t        src_width;      /* 线宽, 环宽, 边界宽 */
        scui_point_t        src_pos_1;      /* 坐标点 */
        scui_point_t        src_pos_2;      /* 坐标点 */
        scui_coord_t        src_radius;     /* 弧, 圆半径 */
    } graph;
    /**************************************************************************
     * keep adding...
     */
    /*************************************************************************/
    };
} scui_draw_dsc_t;

typedef struct {
    scui_list_dln_t  dl_node;       /* 任务序列节点 */
    scui_draw_dsc_t *draw_dsc;      /* 绘制描述符 */
    scui_coord_t     draw_idx;      /* 绘制序列索引 */
    uint8_t          draw_clip[0];  /* 绘制区域哈希(柔性数组) */
} scui_draw_task_node_t;

typedef struct {
    /* 多绘制任务异步: */
    scui_list_dll_t  dl_list;
    scui_mutex_t     dsc_mutex;
    scui_mutex_t     sched_mutex;
    scui_sem_t       sched_sem;
    scui_sem_t       async_sem[SCUI_DRAW_TASK_ASYNC_NUM];
    uint8_t          async_busy[SCUI_DRAW_TASK_ASYNC_NUM];
    void            *task_node[SCUI_DRAW_TASK_ASYNC_NUM];
    
    scui_coord_t     task_idx;
    scui_coord_t     hash_size;     /* 绘制区域哈希数组大小 */
    uintptr_t        node_total;    /* 节点数量(总计) */
    uintptr_t        node_frame;    /* 节点数量(当前) */
    void            *slab_mem;      /* 绘制描述符缓存资源 */
} scui_draw_task_list_t;

/*@brief 就绪绘制任务序列
 */
void scui_draw_task_ready(void);

/*@brief 软件绘制子任务调度
 *       优先级<硬件
 */
void scui_draw_task_sched_sw(void);

/*@brief 硬件绘制子任务调度
 *       优先级<系统
 */
void scui_draw_task_sched_hw(void);

/*@brief 派发绘制任务序列
 */
void scui_draw_task_dispatch(void);

/*@brief 绘制描述符实例申请
 *@param draw_dsc 绘制描述符实例地址
 */
void scui_draw_dsc_ready(scui_draw_dsc_t **draw_dsc);

/*@brief 绘制任务序列准入
 *@param draw_dsc 绘制描述符
 */
void scui_draw_dsc_task(scui_draw_dsc_t *draw_dsc);

/*****************************************************************************/

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_sched(scui_draw_dsc_t *draw_dsc);

/*@brief 绘制上下文加速
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_sched(scui_draw_dsc_t *draw_dsc);

/*@brief 绘制上下文加速检查
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_check(scui_draw_dsc_t *draw_dsc);

/*****************************************************************************/

/*@brief 线条绘制(简易绘制)
 *@param draw_dsc 绘制描述符实例
 *@param x,y,len,width 坐标点,坐标点,线长,线宽
 */
void scui_draw_sline(scui_draw_dsc_t *draw_dsc);
void scui_draw_hline(scui_draw_dsc_t *draw_dsc, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width);
void scui_draw_vline(scui_draw_dsc_t *draw_dsc, scui_coord_t x, scui_coord_t y, scui_coord_t len, scui_coord_t width);

/*****************************************************************************/
/*基础图元绘制:
 *    可以自己写(反正就是堆工作量)
 *    可以移植第三方的gui中内容
 *    其实在面向效果的Gui框架中
 *    基础图形的绘制显得不是那么重要
 *    因为随着实验效果表示,再好显示的抗锯齿基础图形
 *    它的显示效果是不如图像进行图形变换得来的要好
 *    此外,基础图象绘制的效果没有想象中的优秀
 *    它在使用过程中,限制较大
 */
#define   SCUI_DRAW_GRAPH_USE_LVGL      0
#define   SCUI_DRAW_GRAPH_USE_EGUI      1
#include "scui_draw_graph_LVGL.h"
#include "scui_draw_graph_EGUI.h"
/*****************************************************************************/

#endif
