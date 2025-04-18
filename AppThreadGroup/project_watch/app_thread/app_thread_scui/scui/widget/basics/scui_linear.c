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
 *@param layout     通过布局创建
 */
void scui_linear_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 继承对象 */
    scui_scroll_t *scroll = widget;
    scui_scroll_maker_t *scroll_maker = widget_maker;
    /* 本类对象 */
    scui_linear_t *list = widget;
    scui_linear_maker_t *list_maker = widget_maker;
    
    /* 必须标记anima事件 */
    widget_maker->style.sched_anima = true;
    
    /* 构造派生控件实例 */
    scui_scroll_make(scroll, scroll_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_linear));
    
    list->list_num = list_maker->list_num;
    
    /* 列表元素数量是固定的 */
    SCUI_ASSERT(list_maker->list_num != 0);
    scui_handle_t list_num = list->list_num;
    
    /* 创建缓存资源 */
    list->list_surface_s = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_surface_t *) * list_num);
    list->list_widget_s  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_handle_t)    * list_num);
    list->list_widget_m  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_handle_t)    * list_num);
    list->list_draw_clip = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_area_t)      * list_num);
    list->list_draw_idx  = -1;
    list->list_draw      = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(bool) * list_num);
    list->list_refr      = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(bool) * list_num);
    
    memset(list->list_surface_s, 0, sizeof(scui_surface_t *) * list_num);
    memset(list->list_widget_s,  0, sizeof(scui_handle_t)    * list_num);
    memset(list->list_widget_m,  0, sizeof(scui_handle_t)    * list_num);
    memset(list->list_draw_clip, 0, sizeof(scui_area_t)      * list_num);
    memset(list->list_draw,      0, sizeof(bool)             * list_num);
    memset(list->list_refr,      0, sizeof(bool)             * list_num);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_linear_burn(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t   *list   = (void *)widget;
    
    // 回收所有子控件树画布
    scui_linear_recycle(handle, true);
    // 销毁所有子控件树
    for (uint8_t idx = 0; idx < list->list_num; idx++)
        scui_widget_hide(list->list_widget_s[idx], false);
    
    /* 销毁缓存资源 */
    SCUI_MEM_FREE(list->list_refr);
    SCUI_MEM_FREE(list->list_draw);
    SCUI_MEM_FREE(list->list_draw_clip);
    SCUI_MEM_FREE(list->list_widget_m);
    SCUI_MEM_FREE(list->list_widget_s);
    SCUI_MEM_FREE(list->list_surface_s);
    
    /* 析构派生控件实例 */
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_scroll_burn(widget->myself);
}

/*@brief 控件布局更新
 *@param handle 控件句柄
 */
void scui_linear_layout(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t *list = (void *)widget;
    
    
    
    scui_widget_map_t *widget_map = NULL;
    scui_widget_map_find(widget->type, &widget_map);
    scui_widget_map_find(widget_map->base, &widget_map);
    widget_map->layout(handle);
}

/*@brief 列表控件缓存资源回收
 *@param handle 列表控件句柄
 *@param any    回收所有
 */
void scui_linear_recycle(scui_handle_t handle, bool any)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t   *list   = (void *)widget;
    
    /* 更新回收记录 */
    list->clear_num   = 0;
    list->clear_byte  = 0;
    list->remain_num  = 0;
    list->remain_byte = 0;
    
    for (uint8_t idx = 0; idx < list->list_num; idx++) {
        
        scui_surface_t *surface_s = list->list_surface_s[idx];
        scui_handle_t   target_s  = list->list_widget_s[idx];
        scui_handle_t   target_m  = list->list_widget_m[idx];
        
        SCUI_LOG_DEBUG("surface index:%x", idx);
        uintptr_t surface_size = 0;
        
        if (surface_s != NULL) {
            uintptr_t cf_byte   = scui_pixel_bits(surface_s->format) / 8;
            uintptr_t cf_remain = sizeof(scui_color_wt_t) - cf_byte;
            surface_size = surface_s->hor_res * surface_s->ver_res * cf_byte + cf_remain;
        }
        
        if (list->list_draw[idx] && !any) {
            list->list_draw[idx] = false;
            
            if (surface_s != NULL) {
                list->remain_byte += surface_size;
                list->remain_num++;
            }
            continue;
        }
        list->clear_byte += surface_size;
        list->clear_num++;
        
        scui_widget_surface_remap(target_s, surface_s);
        scui_widget_surface_destroy(target_s);
        scui_widget_surface_remap(target_s, NULL);
        list->list_surface_s[idx] = NULL;
    }
    
    SCUI_LOG_INFO("surface clear  num:%u, bytes:%x", list->clear_num,  list->clear_byte);
    SCUI_LOG_INFO("surface remain num:%u, bytes:%x", list->remain_num, list->remain_byte);
}

/*@brief 列表控件绘制索引
 *@param handle   列表控件句柄
 *@param draw_idx 绘制索引
 */
void scui_linear_draw_idx(scui_handle_t handle, scui_handle_t *draw_idx)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t   *list   = (void *)widget;
    
    *draw_idx = list->list_draw_idx;
}

/*@brief 列表子控件事件响应回调(从)
 *       子控件树的根控件事件响应
 *       仅响应:scui_event_draw_empty
 *@param event 事件
 */
void scui_linear_s_event_draw_empty(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_custom_t *custom = (void *)widget;
    
    // 子控件树会逆向链接到列表控件
    SCUI_ASSERT(custom->linker != NULL);
    scui_linear_t *list = custom->linker;
    
    // 这是一个特殊事件, 子控件树没有绘制画布时, 发送此事件激活子控件绘制流程
    SCUI_ASSERT(event->type == scui_event_draw_empty);
    SCUI_ASSERT(scui_widget_surface(event->object) == NULL);
    
    scui_handle_t handle = scui_widget_root(event->object);
    for (scui_handle_t idx = 0; idx < list->list_num; idx++) {
        if (list->list_widget_s[idx] != handle)
            continue;
        // scui_widget_clip_check(scui_widget_root(handle), true);
        scui_widget_draw(list->list_widget_m[idx], NULL, false);
        // scui_widget_clip_check(scui_widget_root(handle), true);
        SCUI_LOG_INFO("widget idx:%u", idx);
        list->list_refr[idx] = true;
    }
}

/*@brief 列表控件关联
 *@param handle   列表控件句柄
 *@param handle_s 子控件树句柄
 */
void scui_linear_s_linker(scui_handle_t handle, scui_handle_t handle_s)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t   *list   = (void *)widget;
    
    scui_widget_t *widget_s = scui_handle_source_check(handle);
    scui_custom_t *custom_s = (void *)widget_s;
    
    // 这是一个以custom为根控件的子控件树
    SCUI_ASSERT(scui_widget_type_check(handle_s, scui_widget_type_custom));
    SCUI_ASSERT(scui_widget_parent(handle_s) == SCUI_HANDLE_INVALID);
    
    void **linker = NULL;
    scui_custom_linker(handle_s, &linker);
    *linker = list;
}

/*@brief 列表控件指定条目
 *@param handle   列表控件句柄
 *@param handle_s 子控件树句柄
 *@param idx      列表索引
 */
void scui_linear_s_item(scui_handle_t handle, scui_handle_t **handle_s, scui_handle_t idx)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t   *list   = (void *)widget;
    
    *handle_s = &list->list_widget_s[idx];
}

/*@brief 列表控件指定条目
 *@param handle   列表控件句柄
 *@param handle_m 子控件句柄
 *@param idx      列表索引
 */
void scui_linear_m_item(scui_handle_t handle, scui_handle_t **handle_m, scui_handle_t idx)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_linear));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_linear_t   *list   = (void *)widget;
    
    *handle_m = &list->list_widget_m[idx];
}

/*@brief 列表子控件事件响应回调(主)
 *@param event 事件
 */
void scui_linear_m_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_custom_t *custom = (void *)widget;
    
    // 列表控件是当前控件的父控件
    widget = scui_handle_source_check(widget->parent);
    scui_linear_t *list = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_draw: {
        
        // 寻找到绘制目标, 并且更新绘制索引
        for (scui_handle_t idx = 0; idx < list->list_num; idx++)
            if (list->list_widget_m[idx] == event->object) {
                list->list_draw_idx = idx;
                break;
            }
        
        scui_handle_t   idx       = list->list_draw_idx;
        scui_handle_t   target_s  = list->list_widget_s[idx];
        scui_handle_t   target_m  = list->list_widget_m[idx];
        scui_surface_t *surface_s = list->list_surface_s[idx];
        
        
        
        // 步调:prepare
        if (scui_event_check_prepare(event)) {
            // 处理子控件树画布, 重绘子控件树
            
            /* 没有剪切域,忽略该绘制,避免假绘制爆内存 */
            if (scui_widget_draw_empty(target_m)) {
                list->list_draw[idx] = false;
                
                // 主目标不能绘制,清空从目标画布
                if (surface_s != NULL) {
                    scui_widget_surface_remap(target_s, surface_s);
                    scui_widget_surface_destroy(target_s);
                    scui_widget_surface_remap(target_s, NULL);
                    list->list_surface_s[idx] = NULL;
                }
                return;
            }
            
            SCUI_LOG_INFO("widget idx:%u", idx);
            // 创建一个独立的子画布,将目标绘制到一个独立子画布中
            if (surface_s == NULL) {
                scui_area_t clip = scui_widget_clip(target_s);
                scui_widget_surface_create(target_s, SCUI_PIXEL_CF_DEF_A, clip.w, clip.h);
                surface_s = list->list_surface_s[idx] = scui_widget_surface(target_s);
                list->list_draw_clip[idx] = scui_widget_clip(target_s);
                list->list_refr[idx] = true;
            }
            scui_widget_surface_remap(target_s, surface_s);
            
            // 刷新该画布
            if (list->list_refr[idx]) {
                list->list_refr[idx] = false;
                scui_area_t clip = list->list_draw_clip[idx];
                list->list_draw_clip[idx] = (scui_area_t){0};
                scui_widget_show(target_s, false);
                scui_widget_draw(target_s, &clip, true);
                scui_event_t event = {0};
                event.object = target_s;
                event.type = scui_event_draw;
                // 移除跟主窗口相关所有绘制事件
                while (scui_event_dequeue(&event, true, false));
            }
            
            // 流程到此,画布已经是新的了,标记进行绘制
            list->list_draw[idx] = true;
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
void scui_linear_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_linear_t *list = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        // 给子控件树派发事件调度
        for (scui_handle_t idx = 0; idx < list->list_num; idx++) {
             scui_handle_t handle = event->object;
             event->object = list->list_widget_s[idx];
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
    
    scui_widget_map_t *widget_map = NULL;
    scui_widget_map_find(widget->type, &widget_map);
    scui_widget_map_find(widget_map->base, &widget_map);
    widget_map->invoke(event);
}
