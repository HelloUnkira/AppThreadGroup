/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

scui_draw_graph_t scui_draw_graph = {0};

/*@brief 就绪绘图资源
 */
void scui_draw_graph_ready(void)
{
    /* 矢量绘图引擎资源 */
    #if SCUI_DRAW_USE_THORVG
    void scui_draw_thorvg_ready(void);
    scui_draw_thorvg_ready();
    #endif
}

/*@brief 绘制上下文加速
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_sched(scui_draw_dsc_t *draw_dsc)
{
    /*@brief 绘制上下文
     *@param draw_dsc 绘制描述符实例
     */
    void scui_draw_ctx_acc_byte_copy(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_area_fill(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_area_copy(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_area_blend(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_area_matrix_fill(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_area_matrix_blend(scui_draw_dsc_t *draw_dsc);
    
    void scui_draw_ctx_acc_area_dither(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_area_blur(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_area_grad(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_area_grads(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_area_afilter(scui_draw_dsc_t *draw_dsc);
    
    void scui_draw_ctx_acc_image(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_image_scale(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_image_rotate(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_image_matrix_blend(scui_draw_dsc_t *draw_dsc);
    
    void scui_draw_ctx_acc_letter(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_string(scui_draw_dsc_t *draw_dsc);
    
    void scui_draw_ctx_acc_qrcode(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_barcode(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_ring(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_acc_graph(scui_draw_dsc_t *draw_dsc);
    
    
    
    SCUI_ASSERT(draw_dsc->type > scui_draw_type_none);
    SCUI_ASSERT(draw_dsc->type < scui_draw_type_num);
    typedef void (*scui_draw_ctx_acc_cb_t)(scui_draw_dsc_t *draw_dsc);
    static const scui_draw_ctx_acc_cb_t scui_draw_ctx_acc_cb[scui_draw_type_num] = {
        [scui_draw_type_byte_copy] =                scui_draw_ctx_acc_byte_copy,
        [scui_draw_type_area_fill] =                scui_draw_ctx_acc_area_fill,
        [scui_draw_type_area_copy] =                scui_draw_ctx_acc_area_copy,
        [scui_draw_type_area_blend] =               scui_draw_ctx_acc_area_blend,
        [scui_draw_type_area_matrix_fill] =         scui_draw_ctx_acc_area_matrix_fill,
        [scui_draw_type_area_matrix_blend] =        scui_draw_ctx_acc_area_matrix_blend,
        
        [scui_draw_type_area_dither] =              scui_draw_ctx_acc_area_dither,
        [scui_draw_type_area_blur] =                scui_draw_ctx_acc_area_blur,
        [scui_draw_type_area_grad] =                scui_draw_ctx_acc_area_grad,
        [scui_draw_type_area_grads] =               scui_draw_ctx_acc_area_grads,
        [scui_draw_type_area_afilter] =             scui_draw_ctx_acc_area_afilter,
        
        [scui_draw_type_image] =                    scui_draw_ctx_acc_image,
        [scui_draw_type_image_scale] =              scui_draw_ctx_acc_image_scale,
        [scui_draw_type_image_rotate] =             scui_draw_ctx_acc_image_rotate,
        [scui_draw_type_image_matrix_blend] =       scui_draw_ctx_acc_image_matrix_blend,
        
        [scui_draw_type_letter] =                   scui_draw_ctx_acc_letter,
        [scui_draw_type_string] =                   scui_draw_ctx_acc_string,
        [scui_draw_type_qrcode] =                   scui_draw_ctx_acc_qrcode,
        [scui_draw_type_barcode] =                  scui_draw_ctx_acc_barcode,
        [scui_draw_type_ring] =                     scui_draw_ctx_acc_ring,
        
        [scui_draw_type_pixel_line] =               scui_draw_ctx_acc_graph,
        [scui_draw_type_pixel_circle] =             scui_draw_ctx_acc_graph,
        [scui_draw_type_pixel_arc] =                scui_draw_ctx_acc_graph,
        [scui_draw_type_pixel_crect] =              scui_draw_ctx_acc_graph,
    };
    
    return scui_draw_ctx_acc_cb[draw_dsc->type](draw_dsc);
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_sched(scui_draw_dsc_t *draw_dsc)
{
    /*@brief 绘制上下文
     *@param draw_dsc 绘制描述符实例
     */
    void scui_draw_ctx_byte_copy(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_area_fill(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_area_copy(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_area_blend(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_area_matrix_fill(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_area_matrix_blend(scui_draw_dsc_t *draw_dsc);
    
    void scui_draw_ctx_area_dither(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_area_blur(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_area_grad(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_area_grads(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_area_afilter(scui_draw_dsc_t *draw_dsc);
    
    void scui_draw_ctx_image(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_image_scale(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_image_rotate(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_image_matrix_blend(scui_draw_dsc_t *draw_dsc);
    
    void scui_draw_ctx_letter(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_string(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_qrcode(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_barcode(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_ring(scui_draw_dsc_t *draw_dsc);
    void scui_draw_ctx_graph(scui_draw_dsc_t *draw_dsc);
    
    
    
    SCUI_ASSERT(draw_dsc->type > scui_draw_type_none);
    SCUI_ASSERT(draw_dsc->type < scui_draw_type_num);
    typedef void (*scui_draw_ctx_cb_t)(scui_draw_dsc_t *draw_dsc);
    static const scui_draw_ctx_cb_t scui_draw_ctx_cb[scui_draw_type_num] = {
        [scui_draw_type_byte_copy] =                scui_draw_ctx_byte_copy,
        [scui_draw_type_area_fill] =                scui_draw_ctx_area_fill,
        [scui_draw_type_area_copy] =                scui_draw_ctx_area_copy,
        [scui_draw_type_area_blend] =               scui_draw_ctx_area_blend,
        [scui_draw_type_area_matrix_fill] =         scui_draw_ctx_area_matrix_fill,
        [scui_draw_type_area_matrix_blend] =        scui_draw_ctx_area_matrix_blend,
        
        [scui_draw_type_area_dither] =              scui_draw_ctx_area_dither,
        [scui_draw_type_area_blur] =                scui_draw_ctx_area_blur,
        [scui_draw_type_area_grad] =                scui_draw_ctx_area_grad,
        [scui_draw_type_area_grads] =               scui_draw_ctx_area_grads,
        [scui_draw_type_area_afilter] =             scui_draw_ctx_area_afilter,
        
        [scui_draw_type_image] =                    scui_draw_ctx_image,
        [scui_draw_type_image_scale] =              scui_draw_ctx_image_scale,
        [scui_draw_type_image_rotate] =             scui_draw_ctx_image_rotate,
        [scui_draw_type_image_matrix_blend] =       scui_draw_ctx_image_matrix_blend,
        
        [scui_draw_type_letter] =                   scui_draw_ctx_letter,
        [scui_draw_type_string] =                   scui_draw_ctx_string,
        [scui_draw_type_qrcode] =                   scui_draw_ctx_qrcode,
        [scui_draw_type_barcode] =                  scui_draw_ctx_barcode,
        [scui_draw_type_ring] =                     scui_draw_ctx_ring,
        
        [scui_draw_type_pixel_line] =               scui_draw_ctx_graph,
        [scui_draw_type_pixel_circle] =             scui_draw_ctx_graph,
        [scui_draw_type_pixel_arc] =                scui_draw_ctx_graph,
        [scui_draw_type_pixel_crect] =              scui_draw_ctx_graph,
    };
    
    scui_draw_ctx_cb[draw_dsc->type](draw_dsc);
}
