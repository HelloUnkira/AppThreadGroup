/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_linear_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 继承对象 */
    scui_scroll_t *scroll = widget;
    scui_scroll_maker_t *scroll_maker = widget_maker;
    /* 本类对象 */
    scui_linear_t *linear = widget;
    scui_linear_maker_t *linear_maker = widget_maker;
    
    /* 构造派生控件实例 */
    scui_scroll_make(scroll, scroll_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_linear));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    #if SCUI_MEM_FEAT_MINI
    /* 禁用子控件树, 画布缓存块渲染 */
    /* 本控件无效用, 拦截使用者 */
    SCUI_ASSERT(false);
    #endif
}

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_linear_m_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_linear_m_t *linear_m = widget;
    scui_linear_m_maker_t *linear_m_maker = widget_maker;
    
    /* 必须标记anima事件 */
    widget_maker->style.sched_anima = true;
    //备注:这个控件自身并不需要anima事件,但是如果子控件树有需求,
    //     它必须要被标记激活以驱动子控件树更新
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_linear_m));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    linear_m->handle_s = SCUI_HANDLE_INVALID;
    linear_m->draw     = false;
    linear_m->refr     = false;
}

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_linear_s_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_linear_s_t *linear_s = widget;
    scui_linear_s_maker_t *linear_s_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_linear_s));
    SCUI_ASSERT(widget_maker->parent == SCUI_HANDLE_INVALID);
    // 子控件树控件一定是根控件(子画布控件树)
    
    linear_s->surface_s = NULL;
    linear_s->handle_m  = SCUI_HANDLE_INVALID;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_linear_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t *linear = (void *)widget;
    
    /* 析构派生控件实例 */
    scui_scroll_burn(widget->myself);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_linear_m_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear_m));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_linear_m_t *linear_m = (void *)widget;
    
    // 连带销毁关联绑定的子控件
    if (linear_m->handle_s != SCUI_HANDLE_INVALID)
        scui_widget_hide(linear_m->handle_s, false);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_linear_s_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear_s));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_linear_s_t *linear_s = (void *)widget;
    
    // 子控件树回收画布
    if (linear_s->surface_s != NULL) {
        scui_widget_surface_remap(handle, linear_s->surface_s);
        scui_widget_surface_destroy(handle);
        scui_widget_surface_remap(handle, NULL);
    }
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 列表子控件事件响应回调(主)
 *@param handle   控件
 *@param handle_s 控件
 */
void scui_linear_m_get(scui_handle_t handle, scui_handle_t *handle_s)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear_m));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_linear_m_t *linear_m = (void *)widget;
    
    *handle_s = linear_m->handle_s;
}

/*@brief 列表子控件事件响应回调(主)
 *@param handle   控件
 *@param handle_s 控件
 */
void scui_linear_m_set(scui_handle_t handle, scui_handle_t *handle_s)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear_m));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_linear_m_t *linear_m = (void *)widget;
    
    linear_m->handle_s = *handle_s;
}

/*@brief 列表子控件事件响应回调(从)
 *@param handle   控件
 *@param handle_m 控件
 */
void scui_linear_s_get(scui_handle_t handle, scui_handle_t *handle_m)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear_s));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_linear_s_t *linear_s = (void *)widget;
    
    *handle_m = linear_s->handle_m;
}

/*@brief 列表子控件事件响应回调(从)
 *@param handle   控件
 *@param handle_m 控件
 */
void scui_linear_s_set(scui_handle_t handle, scui_handle_t *handle_m)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear_s));
    scui_widget_t   *widget   = scui_handle_source_check(handle);
    scui_linear_s_t *linear_s = (void *)widget;
    
    linear_s->handle_m = *handle_m;
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_linear_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_linear_t *linear = (void *)widget;
    
    switch (event->type) {
    case scui_event_draw: {
        
        // 从控件树绘制结束,回收部分不使用的画布
        if (scui_event_check_finish(event)) {
            
            /* 更新回收记录 */
            linear->clear_num   = 0;
            linear->clear_byte  = 0;
            linear->remain_num  = 0;
            linear->remain_byte = 0;
            
            // 遍历子控件列表
            scui_widget_child_list_btra(widget, idx) {
                // 取出子控件(主)的相关信息
                scui_handle_t    handle_m = widget->child_list[idx];
                scui_widget_t   *widget_m = scui_handle_source_assert(handle_m);
                scui_linear_m_t *linear_m = (void *)widget_m;
                
                // 不是所有子控件(主)都有子控件树(从)
                if (linear_m->handle_s == SCUI_HANDLE_INVALID)
                    continue;
                
                // 取出子控件树(从)的相关信息
                scui_handle_t    handle_s = linear_m->handle_s;
                scui_widget_t   *widget_s = scui_handle_source_assert(handle_s);
                scui_linear_s_t *linear_s = (void *)widget_s;
                
                SCUI_LOG_DEBUG("surface index:%x", idx);
                scui_multi_t surface_size = 0;
                
                // 从子控件树无需清理
                if (linear_s->surface_s == NULL)
                    continue;
                if (linear_s->surface_s != NULL) {
                    scui_coord_t hor_res   = linear_s->surface_s->hor_res;
                    scui_coord_t ver_res   = linear_s->surface_s->ver_res;
                    scui_coord_t cf_byte   = scui_pixel_bits(linear_s->surface_s->format) / 8;
                    scui_coord_t cf_remain = sizeof(scui_color_wt_t) - cf_byte;
                    surface_size = hor_res * ver_res * cf_byte + cf_remain;
                }
                
                if (linear_m->draw) {
                    linear_m->draw = false;
                    
                    linear->remain_byte += surface_size;
                    linear->remain_num  += surface_size == 0 ? 0 : 1;
                    continue;
                }
                linear->clear_byte += surface_size;
                linear->clear_num  += surface_size == 0 ? 0 : 1;
                
                scui_widget_surface_remap(handle_s, linear_s->surface_s);
                scui_widget_surface_destroy(handle_s);
                scui_widget_surface_remap(handle_s, NULL);
                linear_s->surface_s = NULL;
            }
            
            SCUI_LOG_INFO("surface clear  num:%u, bytes:%x", linear->clear_num,  linear->clear_byte);
            SCUI_LOG_INFO("surface remain num:%u, bytes:%x", linear->remain_num, linear->remain_byte);
        }
        break;
    }
    default:
        break;
    }
    
    scui_scroll_invoke(event);
}

/*@brief 列表子控件事件响应回调(主)
 *@param event 事件
 */
void scui_linear_m_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    // 列表控件是当前控件的父控件
    
    switch (event->type) {
    case scui_event_draw: {
        scui_handle_t    handle_m = event->object;
        scui_widget_t   *widget_m = scui_handle_source_check(handle_m);
        scui_linear_m_t *linear_m = (void *)widget_m;
        
        // 不是所有子控件(主)都有子控件树(从)
        if (linear_m->handle_s == SCUI_HANDLE_INVALID)
            break;
        
        scui_handle_t    handle_s = linear_m->handle_s;
        scui_widget_t   *widget_s = scui_handle_source_check(handle_s);
        scui_linear_s_t *linear_s = (void *)widget_s;
        
        // 步调:prepare
        if (scui_event_check_prepare(event)) {
            // 处理子控件树画布,重绘子控件树
            
            /* 没有剪切域,忽略该绘制,避免假绘制爆内存 */
            if (scui_widget_draw_empty(handle_m)) {
                linear_m->draw = false;
                
                // 主目标不能绘制,清空从目标画布
                if (linear_s->surface_s != NULL) {
                    scui_widget_surface_remap(handle_s, linear_s->surface_s);
                    scui_widget_surface_destroy(handle_s);
                    scui_widget_surface_remap(handle_s, NULL);
                    linear_s->surface_s = NULL;
                }
                return;
            }
            
            // 创建一个独立的子画布,将目标绘制到一个独立子画布中
            if (linear_s->surface_s == NULL) {
                scui_area_t clip = scui_widget_clip(handle_s);
                scui_surface_t surface = {
                    .format  = SCUI_PIXEL_CF_DEF_A,
                    .hor_res = clip.w,
                    .ver_res = clip.h,
                };
                scui_widget_surface_create(handle_s, &surface);
                linear_s->surface_s = scui_widget_surface(handle_s);
                linear_m->refr = true;
            }
            scui_widget_surface_remap(handle_s, linear_s->surface_s);
            
            // 刷新该画布
            if (linear_m->refr) {
                linear_m->refr = false;
                
                // 移除跟子控件树相关所有绘制事件
                scui_event_define(event, handle_s, false, scui_event_draw, NULL);
                while (scui_event_dequeue(&event, true, false));
                
                scui_widget_show(handle_s, false);
            }
            
            // 流程到此,画布已经是新的了,标记进行绘制
            linear_m->draw = true;
        }
        
        // 步调finish:
        if (scui_event_check_finish(event)) {
            // 去除画布绑定
            scui_widget_surface_remap(handle_s, NULL);
        }
        break;
    }
    default: {
        // 给子控件树派发事件调度
        scui_handle_t    handle_m = event->object;
        scui_widget_t   *widget_m = scui_handle_source_check(handle_m);
        scui_linear_m_t *linear_m = (void *)widget_m;
        
        // 不是所有子控件(主)都有子控件树(从)
        if (linear_m->handle_s == SCUI_HANDLE_INVALID)
            break;
        
        scui_handle_t    handle_s = linear_m->handle_s;
        scui_widget_t   *widget_s = scui_handle_source_check(handle_s);
        scui_linear_s_t *linear_s = (void *)widget_s;
        
        event->object = linear_m->handle_s;
        scui_widget_event_dispatch(event);
        event->object = handle_m;
        break;
    }
    }
}

/*@brief 列表子控件事件响应回调(从)
 *@param event 事件
 */
void scui_linear_s_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    
    switch (event->type) {
        case scui_event_draw_empty: {
            scui_handle_t    handle_s = event->object;
            scui_widget_t   *widget_s = scui_handle_source_check(handle_s);
            scui_linear_s_t *linear_s = (void *)widget_s;
            
            scui_handle_t    handle_m = linear_s->handle_m;
            scui_widget_t   *widget_m = scui_handle_source_check(handle_m);
            scui_linear_m_t *linear_m = (void *)widget_m;
            
            // 这是一个特殊事件
            // 子控件树没有绘制画布时
            // 发送此事件激活子控件绘制流程
            // 子控件树会逆向链接到列表控件
            SCUI_ASSERT(scui_widget_surface(handle_s) == NULL);
            SCUI_ASSERT(handle_m != SCUI_HANDLE_INVALID);
            
            // scui_widget_clip_check(scui_widget_root(handle_m), true);
            scui_widget_draw(handle_m, NULL, false);
            // scui_widget_clip_check(scui_widget_root(handle_m), true);
            linear_m->refr = true;
            break;
        }
    }
}
