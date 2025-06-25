/*实现目标:
 *    控件动画
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件动画回调(空回调)
 */
static void scui_widget_anima_recycle(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    scui_anima_destroy(anima->handle);
}

/*@brief 控件动画回调(透明度渐变)
 */
static void scui_widget_anima_expire_fade(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    
    scui_widget_alpha_set(anima->object, anima->value_c, true);
    scui_widget_draw(anima->object, NULL, false);
}

/*@brief 控件动画回调(缩小宽度)
 */
static void scui_widget_anima_expire_zoom_out_w(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    
    scui_area_t clip = scui_widget_clip(anima->object);
    scui_widget_adjust_size(anima->object, anima->value_c, clip.h);
    scui_widget_draw(anima->object, NULL, false);
}

/*@brief 控件动画回调(缩小高度)
 */
static void scui_widget_anima_expire_zoom_out_h(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    
    scui_area_t clip = scui_widget_clip(anima->object);
    scui_widget_adjust_size(anima->object, clip.w, anima->value_c);
    scui_widget_draw(anima->object, NULL, false);
}

/*@brief 控件动画回调(缩小高度)
 */
static void scui_widget_anima_finish_zoom_out(void *instance)
{
    scui_widget_anima_recycle(instance);
    
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    scui_widget_hide(anima->object, false);
    scui_widget_destroy(anima->object);
}

/*@brief 控件动画透明渐入
 *@param handle 控件句柄
 *@param time   持续时间
 *@param delay  等待时间
 *@retval 动画句柄
 */
scui_handle_t scui_widget_anima_fade_in(scui_handle_t handle, int32_t time, int32_t delay)
{
    scui_anima_t anima = {0};
    anima.expire = scui_widget_anima_expire_fade;
    anima.finish = scui_widget_anima_recycle;
    anima.object = handle;
    anima.period = time;
    anima.delay  = delay;
    
    anima.value_s = scui_alpha_trans;
    anima.value_e = scui_alpha_cover;
    
    scui_anima_create(&anima, &handle);
    scui_anima_start(handle);
    return handle;
}

/*@brief 控件动画透明渐出
 *@param handle 控件句柄
 *@param time   持续时间
 *@param delay  等待时间
 *@retval 动画句柄
 */
scui_handle_t scui_widget_anima_fade_out(scui_handle_t handle, int32_t time, int32_t delay)
{
    scui_anima_t anima = {0};
    anima.expire = scui_widget_anima_expire_fade;
    anima.finish = scui_widget_anima_recycle;
    anima.object = handle;
    anima.period = time;
    anima.delay  = delay;
    
    scui_alpha_t alpha = scui_alpha_trans;
    scui_widget_alpha_get(handle, &alpha);
    anima.value_s = alpha;
    anima.value_e = scui_alpha_trans;
    
    scui_anima_create(&anima, &handle);
    scui_anima_start(handle);
    return handle;
}

/*@brief 控件动画缩小宽度
 *@param handle 控件句柄
 *@param time   持续时间
 *@param delay  等待时间
 *@retval 动画句柄
 */
scui_handle_t scui_widget_anima_zoom_out_w(scui_handle_t handle, int32_t time, int32_t delay)
{
    scui_anima_t anima = {0};
    anima.expire = scui_widget_anima_expire_zoom_out_w;
    anima.finish = scui_widget_anima_finish_zoom_out;
    anima.object = handle;
    anima.period = time;
    anima.delay  = delay;
    
    scui_area_t clip = scui_widget_clip(handle);
    anima.value_s = clip.w;
    anima.value_e = 1; // 不应该到0 ?
    
    scui_anima_create(&anima, &handle);
    scui_anima_start(handle);
    return handle;
}

/*@brief 控件动画缩小高度
 *@param handle 控件句柄
 *@param time   持续时间
 *@param delay  等待时间
 *@retval 动画句柄
 */
scui_handle_t scui_widget_anima_zoom_out_h(scui_handle_t handle, int32_t time, int32_t delay)
{
    scui_anima_t anima = {0};
    anima.expire = scui_widget_anima_expire_zoom_out_h;
    anima.finish = scui_widget_anima_finish_zoom_out;
    anima.object = handle;
    anima.period = time;
    anima.delay  = delay;
    
    scui_area_t clip = scui_widget_clip(handle);
    anima.value_s = clip.h;
    anima.value_e = 1; // 不应该到0 ?
    
    scui_anima_create(&anima, &handle);
    scui_anima_start(handle);
    return handle;
}

