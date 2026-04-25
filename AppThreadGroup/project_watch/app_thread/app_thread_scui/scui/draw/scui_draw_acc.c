/*实现目标:
 *    图元绘制加速器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 绘制上下文加速检查
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_check(scui_draw_dsc_t *draw_dsc)
{
    SCUI_ASSERT(draw_dsc->type > scui_draw_type_none);
    SCUI_ASSERT(draw_dsc->type < scui_draw_type_num);
    static const bool scui_draw_ctx_acc_check_cb[scui_draw_type_num] = {
        [scui_draw_type_byte_copy] =                true,
        [scui_draw_type_area_fill] =                true,
        [scui_draw_type_area_copy] =                true,
        [scui_draw_type_area_blend] =               true,
        [scui_draw_type_area_2d_blend] =            true,
        [scui_draw_type_area_3d_blend] =            true,
        [scui_draw_type_area_3d_fill] =             true,
        
        [scui_draw_type_area_dither] =              false,
        [scui_draw_type_area_blur] =                false,
        [scui_draw_type_area_grad] =                false,
        [scui_draw_type_area_grads] =               false,
        [scui_draw_type_area_afilter] =             false,
        
        [scui_draw_type_image] =                    true,
        [scui_draw_type_image_2d] =                 true,
        [scui_draw_type_image_3d] =                 true,
        
        [scui_draw_type_letter] =                   false,
        [scui_draw_type_string] =                   false,
        [scui_draw_type_qrcode] =                   false,
        [scui_draw_type_barcode] =                  false,
        [scui_draw_type_ring] =                     false,
        
        [scui_draw_type_pixel_arc] =                false,
        [scui_draw_type_pixel_circle] =             false,
        [scui_draw_type_pixel_line] =               false,
        [scui_draw_type_pixel_crect] =              false,
        
        [scui_draw_type_pixel_tvg] =                false,
    };
    
    return scui_draw_ctx_acc_check_cb[draw_dsc->type];
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_byte_copy(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_area_fill(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_area_copy(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_area_blend(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 图形变换迁移(可以使用DMA2D-blend加速优化)
 *@param draw_dsc 绘制描述符实例
 */
bool scui_draw_ctx_acc_area_2d_blend(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_area_3d_blend(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_area_3d_fill(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_area_dither(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_area_blur(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_area_grad(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_area_grads(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_area_afilter(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_image(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_image_2d(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_image_3d(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_letter(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_string(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_qrcode(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_barcode(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_ring(scui_draw_dsc_t *draw_dsc)
{
    return false;
}

/*@brief 绘制上下文
 *@param draw_dsc 绘制描述符实例
 *@retval 支持:true;不支持:false;
 */
bool scui_draw_ctx_acc_graph(scui_draw_dsc_t *draw_dsc)
{
    return false;
}
