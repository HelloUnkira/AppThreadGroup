/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#if defined(LVGL_VERSION_MAJOR) || defined(LVGL_VERSION_MINOR) || defined(LVGL_VERSION_PATCH)
#error "do not import any lvgl header files"
#endif

/* 这里只写一个思路 */
/* 需要从lvgl迁移以下的文件的内容 */
/* lv_draw_mask.c.h */
/* 以及lv_draw_系列的基础图形绘制函数 */

/*@brief 线条绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_line(scui_draw_dsc_t *draw_dsc)
{
}

/*@brief 圆绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_circle(scui_draw_dsc_t *draw_dsc)
{

}

/*@brief 圆角矩形绘制
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_crect(scui_draw_dsc_t *draw_dsc)
{

}

/*@brief 弧绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_arc(scui_draw_dsc_t *draw_dsc)
{

}

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 *@retval 支持度(true:支持;false:不支持;)
 */
bool scui_draw_graph_LVGL(scui_draw_dsc_t *draw_dsc)
{
    #if SCUI_DRAW_GRAPH_USE_EGUI
    switch (draw_dsc->type) {
    case scui_draw_type_pixel_line:
        scui_draw_line(draw_dsc);
        return false;
    case scui_draw_type_pixel_circle:
        scui_draw_circle(draw_dsc);
        return false;
    case scui_draw_type_pixel_crect:
        scui_draw_crect(draw_dsc);
        return false;
    case scui_draw_type_pixel_arc:
        scui_draw_arc(draw_dsc);
        return false;
    /* 继续补充,剩下的不移植了, 一般用不上了 */
    default:
        SCUI_LOG_ERROR("unknown type:%d", draw_dsc->type);
        return false;
    }
    #endif
    
    return false;
}
