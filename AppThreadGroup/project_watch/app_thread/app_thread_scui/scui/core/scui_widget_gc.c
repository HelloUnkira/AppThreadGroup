/*实现目标:
 *    控件图形上下文(Graphic Context)
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 控件gc重置
 *@param widget 控件实例
 */
inline void scui_widget_gc_reset(scui_widget_t *widget)
{
    memset(&widget->gc.clip, 0, sizeof(scui_area_t));
    widget->gc.canvas = SCUI_HANDLE_INVALID;
}

/*@brief  控件gc获取画布
 *@param  widget 控件实例
 *@retval 画布句柄
 */
inline scui_handle_t scui_widget_gc_canvas_get(scui_widget_t *widget)
{
    return widget->gc.canvas;
}

/*@brief 控件gc设置画布
 *@param widget 控件实例
 *@param canvas 画布句柄
 */
inline void scui_widget_gc_canvas_set(scui_widget_t *widget, scui_handle_t canvas)
{
    widget->gc.canvas = canvas;
}

/*@brief 控件gc获取剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
inline void scui_widget_gc_clip_get(scui_widget_t *widget, scui_area_t *clip)
{
    *clip = widget->gc.clip;
}

/*@brief 控件gc设置剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
inline void scui_widget_gc_clip_set(scui_widget_t *widget, scui_area_t *clip)
{
    widget->gc.clip = *clip;
}

/*@brief 控件gc追加剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
inline void scui_widget_gc_clip_merge(scui_widget_t *widget, scui_area_t *clip)
{
    scui_area_merge(&widget->gc.clip, &widget->gc.clip, clip);
}

/*@brief 控件gc清除剪切域
 *@param widget 控件实例
 *@param clip   剪切域
 */
inline void scui_widget_gc_clip_clear(scui_widget_t *widget)
{
    widget->gc.clip.w = 0;
    widget->gc.clip.h = 0;
}

/*@brief 控件gc更新画布alpha
 *@param widget 控件实例
 *@param alpha  画布alpha
 */
inline void scui_widget_gc_alpha_update(scui_widget_t *widget, uint8_t alpha)
{
    /* wait adapte */
}




