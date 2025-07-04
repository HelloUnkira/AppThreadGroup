/*实现目标:
 *    控件动画
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 动画回调(监听)
 */
static void scui_widget_anima_expire(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t  *anima  = instance;
    scui_widget_t *widget = scui_handle_source_check(anima->object);
    SCUI_ASSERT(widget->anima != SCUI_HANDLE_INVALID);
    scui_widget_anima_t *widget_anima = NULL;
    widget_anima = scui_handle_source_check(widget->anima);
    
    /* 工步迭代数超过范围, 所有动画工步全部结束 */
    if (widget_anima->iter > widget_anima->step_lim) {
        scui_widget_anima_destroy(widget->myself);
        return;
    }
    
    /* 处理之前工步下, 残余的无效动画 */
    /* 监听当前工步下, 所有动画是否执行完毕 */
    bool scui_work_step_busy = false;
    bool scui_work_step_over = true;
    for (scui_handle_t idx = 0; idx < widget_anima->number; idx++) {
        if (widget_anima->list[idx] == SCUI_HANDLE_INVALID)
            continue;
        
        /* 还有未执行的动画 */
        if (widget_anima->step[idx] > widget_anima->iter)
            scui_work_step_over = false;
        
        /* 回收已经执行完毕的动画, 如果还存在的话 */
        if (widget_anima->step[idx] < widget_anima->iter) {
            scui_anima_destroy(widget_anima->list[idx]);
            widget_anima->list[idx] = SCUI_HANDLE_INVALID;
        }
        
        /* 检查当前是否存在动画 */
        if (widget_anima->step[idx] == widget_anima->iter)
        if (scui_anima_running(widget_anima->list[idx]))
            scui_work_step_busy = true;
    }
    
    if (scui_work_step_busy)
        return;
    if (scui_work_step_over) {
        widget_anima->iter = widget_anima->step_lim + 1;
        return;
    }
    
    /* 迭代到下一个存在动画的工步 */
    for (bool unmatch = true; unmatch; true) {
        widget_anima->iter++;
        SCUI_ASSERT(widget_anima->iter <= widget_anima->step_lim);
        /* 检查当前工步是否存在动画 */
        for (scui_handle_t idx = 0; idx < widget_anima->number; idx++) {
            if (widget_anima->list[idx] == SCUI_HANDLE_INVALID)
                continue;
            if (widget_anima->step[idx] != widget_anima->iter)
                continue;
            
            unmatch = false;
            break;
        }
    }
    
    /* 全部执行完毕后, 启动下一个工步的所有动画 */
    for (scui_handle_t idx = 0; idx < widget_anima->number; idx++)
        if (widget_anima->list[idx] != SCUI_HANDLE_INVALID)
        if (widget_anima->step[idx] == widget_anima->iter)
            scui_anima_start(widget_anima->list[idx]);
}

/*@brief 对象动画停止
 *@param handle 对象句柄
 */
void scui_widget_anima_stop(scui_handle_t handle)
{
    SCUI_LOG_INFO("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    SCUI_ASSERT(widget->anima != SCUI_HANDLE_INVALID);
    scui_widget_anima_t *widget_anima = NULL;
    widget_anima = scui_handle_source_check(widget->anima);
    
    scui_anima_stop(widget_anima->handle);
}

/*@brief 对象动画启动
 *@param handle 对象句柄
 */
void scui_widget_anima_start(scui_handle_t handle)
{
    SCUI_LOG_INFO("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    SCUI_ASSERT(widget->anima != SCUI_HANDLE_INVALID);
    scui_widget_anima_t *widget_anima = NULL;
    widget_anima = scui_handle_source_check(widget->anima);
    
    widget_anima->iter = 0;
    scui_anima_start(widget_anima->handle);
}

/*@brief 对象动画清理
 *@param handle 对象句柄
 */
void scui_widget_anima_clear(scui_handle_t handle)
{
    SCUI_LOG_INFO("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    SCUI_ASSERT(widget->anima != SCUI_HANDLE_INVALID);
    scui_widget_anima_t *widget_anima = NULL;
    widget_anima = scui_handle_source_check(widget->anima);
    
    for (scui_handle_t idx = 0; idx < widget_anima->number; idx++)
        if (widget_anima->list[idx] != SCUI_HANDLE_INVALID) {
            scui_anima_destroy(widget_anima->list[idx]);
            widget_anima->list[idx] = SCUI_HANDLE_INVALID;
        }
    
}

/*@brief 对象动画销毁
 *@param handle 对象句柄
 */
void scui_widget_anima_destroy(scui_handle_t handle)
{
    SCUI_LOG_INFO("widget %u, number %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_widget_anima_t *widget_anima = NULL;
    if (widget->anima == SCUI_HANDLE_INVALID)
        return;
    widget_anima = scui_handle_source_check(widget->anima);
    
    // 销毁所有动画
    scui_anima_destroy(widget_anima->handle);
    for (scui_handle_t idx = 0; idx < widget_anima->number; idx++)
        if (widget_anima->list[idx] != SCUI_HANDLE_INVALID)
            scui_anima_destroy(widget_anima->list[idx]);
    
    SCUI_MEM_FREE(widget_anima->step);
    SCUI_MEM_FREE(widget_anima->list);
    SCUI_MEM_FREE(widget_anima);
    
    scui_handle_clear(widget->anima);
    widget->anima = SCUI_HANDLE_INVALID;
}

/*@brief 对象动画创建
 *@param handle 对象句柄
 *@param number 动画数量
 */
void scui_widget_anima_create(scui_handle_t handle, scui_handle_t number)
{
    SCUI_LOG_INFO("widget %u, number %u", handle, number);
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_widget_anima_t *widget_anima = NULL;
    
    // 只允许存在一个动画
    if (widget->anima != SCUI_HANDLE_INVALID)
        scui_widget_anima_destroy(handle);
    
    SCUI_ASSERT(number != 0);
    widget_anima = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_widget_anima_t));
    widget_anima->list = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_handle_t) * number);
    widget_anima->step = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_handle_t) * number);
    widget_anima->number = number;
    widget_anima->object = handle;
    
    widget->anima = scui_handle_find();
    scui_handle_linker(widget->anima, widget_anima);
    
    scui_anima_t anima = {0};
    anima.expire = scui_widget_anima_expire;
    anima.object = widget_anima->object;
    anima.reload = SCUI_ANIMA_INFINITE;
    scui_anima_create(&anima, &widget_anima->handle);
}

/*@brief 对象动画提交
 *@param handle 对象句柄
 *@param anima  动画句柄
 *@param step   动画工步(范围:[1, 100])
 */
void scui_widget_anima_submit(scui_handle_t handle, scui_handle_t anima, scui_handle_t step)
{
    SCUI_LOG_INFO("widget %u anima %u step %u", handle, anima, step);
    scui_widget_t *widget = scui_handle_source_check(handle);
    SCUI_ASSERT(widget->anima != SCUI_HANDLE_INVALID);
    scui_widget_anima_t *widget_anima = NULL;
    widget_anima = scui_handle_source_check(widget->anima);
    
    SCUI_ASSERT(step > 0 && step < 100);
    for (scui_handle_t idx = 0; idx < widget_anima->number; idx++) {
        if (widget_anima->list[idx] != SCUI_HANDLE_INVALID)
            continue;
        
        widget_anima->list[idx] = anima;
        widget_anima->step[idx] = step;
        
        // 重新刷新一下step极限
        if (widget_anima->step_lim < widget_anima->step[idx])
            widget_anima->step_lim = widget_anima->step[idx];
        
        // 此处检查一下动画是否是本控件动画
        scui_anima_t *anima_inst = scui_handle_source_assert(anima);
        SCUI_ASSERT(anima_inst->object == handle);
        return;
    }
    
    SCUI_LOG_ERROR("widget anima collect too anima");
    SCUI_ASSERT(false);
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

/*@brief 控件动画透明渐入(包装器)
 */
scui_handle_t scui_widget_anima_fade_in(scui_handle_t handle, int32_t time, int32_t delay)
{
    scui_anima_t anima = {0};
    anima.expire  = scui_widget_anima_expire_fade;
    anima.object  = handle;
    anima.period  = time;
    anima.delay   = delay;
    anima.value_s = scui_alpha_trans;
    anima.value_e = scui_alpha_cover;
    scui_anima_create(&anima, &handle);
    return handle;
}

/*@brief 控件动画透明渐出(包装器)
 */
scui_handle_t scui_widget_anima_fade_out(scui_handle_t handle, int32_t time, int32_t delay)
{
    scui_alpha_t alpha = scui_alpha_trans;
    scui_widget_alpha_get(handle, &alpha);
    
    scui_anima_t anima = {0};
    anima.expire  = scui_widget_anima_expire_fade;
    anima.object  = handle;
    anima.period  = time;
    anima.delay   = delay;
    anima.value_s = alpha;
    anima.value_e = scui_alpha_trans;
    scui_anima_create(&anima, &handle);
    return handle;
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

/*@brief 控件动画回调
 */
static void scui_widget_anima_finish_zoom_out(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    scui_widget_destroy(anima->object);
}

/*@brief 控件动画缩小宽度(包装器)
 */
scui_handle_t scui_widget_anima_zoom_out_w(scui_handle_t handle, int32_t time, int32_t delay)
{
    scui_area_t clip = scui_widget_clip(handle);
    
    scui_anima_t anima = {0};
    anima.expire  = scui_widget_anima_expire_zoom_out_w;
    anima.finish  = scui_widget_anima_finish_zoom_out;
    anima.object  = handle;
    anima.period  = time;
    anima.delay   = delay;
    anima.value_s = clip.w;
    anima.value_e = 1; // 不应该到0
    scui_anima_create(&anima, &handle);
    return handle;
}

/*@brief 控件动画缩小高度(包装器)
 */
scui_handle_t scui_widget_anima_zoom_out_h(scui_handle_t handle, int32_t time, int32_t delay)
{
    scui_area_t clip = scui_widget_clip(handle);
    
    scui_anima_t anima = {0};
    anima.expire  = scui_widget_anima_expire_zoom_out_h;
    anima.finish  = scui_widget_anima_finish_zoom_out;
    anima.object  = handle;
    anima.period  = time;
    anima.delay   = delay;
    anima.value_s = clip.h;
    anima.value_e = 1; // 不应该到0
    scui_anima_create(&anima, &handle);
    return handle;
}
