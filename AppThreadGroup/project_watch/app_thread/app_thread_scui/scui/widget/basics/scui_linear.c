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
    
    /* 必须标记anima事件 */
    widget_maker->style.sched_anima = true;
    
    /* 构造派生控件实例 */
    scui_scroll_make(scroll, scroll_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_linear));
    
    #if SCUI_MEM_FEAT_MINI
    /* 禁用子控件树, 画布缓存块渲染 */
    /* 本控件无效用, 拦截使用者 */
    SCUI_ASSERT(false);
    #endif
    
    linear->list_num = linear_maker->list_num;
    
    /* 列表元素数量是固定的 */
    SCUI_ASSERT(linear_maker->list_num != 0);
    scui_handle_t list_num = linear->list_num;
    
    /* 创建缓存资源 */
    linear->list_surface_s = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_surface_t *) * list_num);
    linear->list_widget_s  = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_handle_t)    * list_num);
    linear->list_widget_m  = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_handle_t)    * list_num);
    linear->list_draw_clip = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_area_t)      * list_num);
    linear->list_draw_idx  = -1;
    linear->list_draw      = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(bool) * list_num);
    linear->list_refr      = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(bool) * list_num);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_linear_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t *linear = (void *)widget;
    
    // 回收所有子控件树画布
    scui_linear_recycle(handle, true);
    // 销毁所有子控件树
    for (scui_handle_t idx = 0; idx < linear->list_num; idx++)
        scui_widget_hide(linear->list_widget_s[idx], false);
    
    /* 销毁缓存资源 */
    SCUI_MEM_FREE(linear->list_refr);
    SCUI_MEM_FREE(linear->list_draw);
    SCUI_MEM_FREE(linear->list_draw_clip);
    SCUI_MEM_FREE(linear->list_widget_m);
    SCUI_MEM_FREE(linear->list_widget_s);
    SCUI_MEM_FREE(linear->list_surface_s);
    
    /* 析构派生控件实例 */
    scui_scroll_burn(widget->myself);
}

/*@brief 列表控件缓存资源回收
 *@param handle 列表控件句柄
 *@param any    回收所有
 */
void scui_linear_recycle(scui_handle_t handle, bool any)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t *linear = (void *)widget;
    
    /* 更新回收记录 */
    linear->clear_num   = 0;
    linear->clear_byte  = 0;
    linear->remain_num  = 0;
    linear->remain_byte = 0;
    
    for (scui_handle_t idx = 0; idx < linear->list_num; idx++) {
        
        scui_surface_t *surface_s = linear->list_surface_s[idx];
        scui_handle_t   target_s  = linear->list_widget_s[idx];
        scui_handle_t   target_m  = linear->list_widget_m[idx];
        
        SCUI_LOG_DEBUG("surface index:%x", idx);
        uintptr_t surface_size = 0;
        
        if (surface_s != NULL) {
            uintptr_t cf_byte   = scui_pixel_bits(surface_s->format) / 8;
            uintptr_t cf_remain = sizeof(scui_color_wt_t) - cf_byte;
            surface_size = surface_s->hor_res * surface_s->ver_res * cf_byte + cf_remain;
        }
        
        if (linear->list_draw[idx] && !any) {
            linear->list_draw[idx] = false;
            
            if (surface_s != NULL) {
                linear->remain_byte += surface_size;
                linear->remain_num++;
            }
            continue;
        }
        linear->clear_byte += surface_size;
        linear->clear_num++;
        
        scui_widget_surface_remap(target_s, surface_s);
        scui_widget_surface_destroy(target_s);
        scui_widget_surface_remap(target_s, NULL);
        linear->list_surface_s[idx] = NULL;
    }
    
    SCUI_LOG_INFO("surface clear  num:%u, bytes:%x", linear->clear_num,  linear->clear_byte);
    SCUI_LOG_INFO("surface remain num:%u, bytes:%x", linear->remain_num, linear->remain_byte);
}

/*@brief 列表控件条目参数(获取)
 *@param handle 列表控件句柄
 *@param item   条目参数
 */
void scui_linear_item_gets(scui_handle_t handle, scui_linear_item_t *item)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t *linear = (void *)widget;
    
    if (item->draw_idx >= linear->list_num)
    item->draw_idx = linear->list_draw_idx;
    item->handle_m = linear->list_widget_m[item->draw_idx];
    item->handle_s = linear->list_widget_s[item->draw_idx];
}

/*@brief 列表控件条目参数(设置)
 *@param handle 列表控件句柄
 *@param item   条目参数
 */
void scui_linear_item_sets(scui_handle_t handle, scui_linear_item_t *item)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t *linear = (void *)widget;
    
    linear->list_widget_m[item->draw_idx] = item->handle_m;
    linear->list_widget_s[item->draw_idx] = item->handle_s;
}

/*@brief 列表子控件事件响应回调(从)
 *@param event 事件
 */
void scui_linear_s_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_custom_t *custom = (void *)widget;
    
    switch (event->type) {
        case scui_event_draw_empty: {
            // 子控件树会逆向链接到列表控件
            SCUI_ASSERT(custom->handle_m != SCUI_HANDLE_INVALID);
            scui_handle_t  handle = scui_widget_parent(custom->handle_m);
            scui_linear_t *linear = scui_handle_source_check(handle);
            
            // 这是一个特殊事件
            // 子控件树没有绘制画布时
            // 发送此事件激活子控件绘制流程
            SCUI_ASSERT(scui_widget_surface(event->object) == NULL);
            
            scui_handle_t handle_r = scui_widget_root(event->object);
            for (scui_handle_t idx = 0; idx < linear->list_num; idx++) {
                if (linear->list_widget_s[idx] != handle_r)
                    continue;
                // scui_widget_clip_check(scui_widget_root(handle), true);
                scui_widget_draw(linear->list_widget_m[idx], NULL, false);
                // scui_widget_clip_check(scui_widget_root(handle), true);
                SCUI_LOG_INFO("widget idx:%u", idx);
                linear->list_refr[idx] = true;
            }
            break;
        }
    }
}

/*@brief 列表子控件事件响应回调(主)
 *@param event 事件
 */
void scui_linear_m_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget_w = scui_handle_source_check(event->object);
    scui_widget_t *widget_p = scui_handle_source_check(widget_w->parent);
    scui_custom_t *custom = (void *)widget_w;
    scui_linear_t *linear = (void *)widget_p;
    // 列表控件是当前控件的父控件
    
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_draw: {
        
        // 寻找到绘制目标, 并且更新绘制索引
        for (scui_handle_t idx = 0; idx < linear->list_num; idx++)
            if (linear->list_widget_m[idx] == event->object) {
                linear->list_draw_idx = idx;
                break;
            }
        
        scui_handle_t   idx       = linear->list_draw_idx;
        scui_handle_t   target_s  = linear->list_widget_s[idx];
        scui_handle_t   target_m  = linear->list_widget_m[idx];
        scui_surface_t *surface_s = linear->list_surface_s[idx];
        
        
        
        // 步调:prepare
        if (scui_event_check_prepare(event)) {
            // 处理子控件树画布, 重绘子控件树
            
            /* 没有剪切域,忽略该绘制,避免假绘制爆内存 */
            if (scui_widget_draw_empty(target_m)) {
                linear->list_draw[idx] = false;
                
                // 主目标不能绘制,清空从目标画布
                if (surface_s != NULL) {
                    scui_widget_surface_remap(target_s, surface_s);
                    scui_widget_surface_destroy(target_s);
                    scui_widget_surface_remap(target_s, NULL);
                    linear->list_surface_s[idx] = NULL;
                }
                return;
            }
            
            SCUI_LOG_INFO("widget idx:%u", idx);
            // 创建一个独立的子画布,将目标绘制到一个独立子画布中
            if (surface_s == NULL) {
                scui_area_t clip = scui_widget_clip(target_s);
                scui_widget_surface_create(target_s, SCUI_PIXEL_CF_DEF_A, clip.w, clip.h);
                surface_s = linear->list_surface_s[idx] = scui_widget_surface(target_s);
                linear->list_draw_clip[idx] = scui_widget_clip(target_s);
                linear->list_refr[idx] = true;
            }
            scui_widget_surface_remap(target_s, surface_s);
            
            // 刷新该画布
            if (linear->list_refr[idx]) {
                linear->list_refr[idx] = false;
                scui_area_t clip = linear->list_draw_clip[idx];
                linear->list_draw_clip[idx] = (scui_area_t){0};
                scui_widget_show(target_s, false);
                scui_widget_draw(target_s, &clip, true);
                scui_event_t event = {0};
                event.object = target_s;
                event.type = scui_event_draw;
                // 移除跟主窗口相关所有绘制事件
                while (scui_event_dequeue(&event, true, false));
            }
            
            // 流程到此,画布已经是新的了,标记进行绘制
            linear->list_draw[idx] = true;
        }
        
        // 步调finish:
        if (scui_event_check_finish(event)) {
            // 去除画布绑定
            scui_widget_surface_remap(target_s, NULL);
        }
        break;
    }
    default:
        break;
    }
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
    case scui_event_anima_elapse: {
        // 给子控件树派发事件调度
        for (scui_handle_t idx = 0; idx < linear->list_num; idx++) {
             scui_handle_t handle = event->object;
             event->object = linear->list_widget_s[idx];
             scui_widget_event_dispatch(event);
             event->object = handle;
        }
        break;
    }
    case scui_event_draw: {
        
        // 从控件树绘制结束,回收部分不使用的画布
        if (scui_event_check_finish(event)) {
            scui_linear_recycle(event->object, false);
        }
        break;
    }
    default:
        break;
    }
    
    scui_scroll_invoke(event);
}
