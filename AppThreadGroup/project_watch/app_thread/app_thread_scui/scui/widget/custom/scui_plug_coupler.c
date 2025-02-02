/*实现目标:
 *    插件:控件连接器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 插件:连接器:创建
 *@param coupler  连接器实例地址
 *@param list_num 控件数量
 */
void scui_plug_coupler_create(scui_plug_coupler_t **coupler, scui_handle_t list_num)
{
    SCUI_ASSERT(list_num != 0);
    *coupler = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_plug_coupler_t));
    memset(*coupler, 0, sizeof(scui_plug_coupler_t));
    
    (*coupler)->list_widget_m_cb = NULL;
    (*coupler)->list_surface_s = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_surface_t *) * list_num);
    (*coupler)->list_widget_s  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_handle_t) * list_num);
    (*coupler)->list_widget_m  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_handle_t) * list_num);
    (*coupler)->list_draw_clip = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_area_t) * list_num);
    (*coupler)->list_draw_idx = -1;
    (*coupler)->list_num = list_num;
    (*coupler)->list_draw = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(bool) * list_num);
    (*coupler)->list_refr = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(bool) * list_num);
    
    memset((*coupler)->list_surface_s, 0, sizeof(scui_surface_t *) * list_num);
    memset((*coupler)->list_widget_s,  0, sizeof(scui_handle_t) * list_num);
    memset((*coupler)->list_widget_m,  0, sizeof(scui_handle_t) * list_num);
    memset((*coupler)->list_draw_clip, 0, sizeof(scui_area_t) * list_num);
    memset((*coupler)->list_draw, 0, sizeof(bool) * list_num);
    memset((*coupler)->list_refr, 0, sizeof(bool) * list_num);
}

/*@brief 插件:连接器:销毁
 *@param coupler 连接器实例
 */
void scui_plug_coupler_destroy(scui_plug_coupler_t *coupler)
{
    SCUI_MEM_FREE(coupler->list_refr);
    SCUI_MEM_FREE(coupler->list_draw);
    SCUI_MEM_FREE(coupler->list_draw_clip);
    SCUI_MEM_FREE(coupler->list_widget_m);
    SCUI_MEM_FREE(coupler->list_widget_s);
    SCUI_MEM_FREE(coupler->list_surface_s);
    SCUI_MEM_FREE(coupler);
}

/*@brief 插件:连接器:回收
 *@param coupler 连接器实例
 *@param any     回收所有
 */
void scui_plug_coupler_recycle(scui_plug_coupler_t *coupler, bool any)
{
    SCUI_ASSERT(coupler != NULL);
    
    uintptr_t surface_clear_nums   = 0;
    uintptr_t surface_clear_bytes  = 0;
    uintptr_t surface_remain_nums  = 0;
    uintptr_t surface_remain_bytes = 0;
    
    for (uint8_t idx = 0; idx < coupler->list_num; idx++) {
        
        scui_surface_t *surface_s = coupler->list_surface_s[idx];
        scui_handle_t   target_s  = coupler->list_widget_s[idx];
        scui_handle_t   target_m  = coupler->list_widget_m[idx];
        
        SCUI_LOG_DEBUG("surface index:%x", idx);
        uintptr_t surface_size = 0;
        
        if (surface_s != NULL) {
            uintptr_t cf_byte   = scui_pixel_bits(surface_s->format) / 8;
            uintptr_t cf_remain = sizeof(scui_color_wt_t) - cf_byte;
            surface_size = surface_s->hor_res * surface_s->ver_res * cf_byte + cf_remain;
        }
        
        if (coupler->list_draw[idx] && !any) {
            coupler->list_draw[idx] = false;
            
            if (surface_s != NULL) {
                surface_remain_bytes += surface_size;
                surface_remain_nums++;
            }
            continue;
        }
        surface_clear_bytes += surface_size;
        surface_clear_nums++;
        
        scui_widget_surface_remap(target_s, surface_s);
        scui_widget_surface_destroy(target_s);
        scui_widget_surface_remap(target_s, NULL);
        coupler->list_surface_s[idx] = NULL;
    }
    
    SCUI_LOG_INFO("surface clear  num:%u, bytes:%x", surface_clear_nums,  surface_clear_bytes);
    SCUI_LOG_INFO("surface remain num:%u, bytes:%x", surface_remain_nums, surface_remain_bytes);
}

/*@brief 控件事件响应回调(需要包装)
 *@param coupler 连接器实例
 *@param event   事件
 */
void scui_plug_coupler_widget_s_event_proc(scui_plug_coupler_t *coupler, scui_event_t *event)
{
    SCUI_ASSERT(coupler != NULL);
    
    switch (event->type) {
    case scui_event_draw: {
        
        if (scui_widget_surface(event->object) == NULL) {
            scui_handle_t handle = scui_widget_root(event->object);
            for (scui_handle_t idx = 0; idx < coupler->list_num; idx++) {
                if (coupler->list_widget_s[idx] != handle)
                    continue;
                // scui_widget_clip_check(scui_widget_root(handle), true);
                scui_widget_draw(coupler->list_widget_m[idx], NULL, false);
                // scui_widget_clip_check(scui_widget_root(handle), true);
                SCUI_LOG_INFO("widget idx:%u", idx);
                coupler->list_refr[idx] = true;
                break;
            }
            break;
        }
        
        scui_widget_cb_t *widget_cb = NULL;
        scui_widget_cb_find(scui_widget_type(event->object), &widget_cb);
        if (widget_cb->invoke != NULL)
            widget_cb->invoke(event);
        
        if (0) {
            scui_handle_t handle = scui_widget_root(event->object);
            for (scui_handle_t idx = 0; idx < coupler->list_num; idx++) {
                if (coupler->list_widget_s[idx] != handle)
                    continue;
                // scui_widget_clip_check(scui_widget_root(handle), true);
                coupler->list_draw_clip[idx] = scui_widget_clip(event->object);
                scui_widget_draw(coupler->list_widget_m[idx], NULL, false);
                // scui_widget_clip_check(scui_widget_root(handle), true);
                break;
            }
            break;
        }
        break;
    }
    default: {
        scui_widget_cb_t *widget_cb = NULL;
        scui_widget_cb_find(scui_widget_type(event->object), &widget_cb);
        if (widget_cb->invoke != NULL)
            widget_cb->invoke(event);
        break;
    }
    }
}

/*@brief 控件事件响应回调(需要包装)
 *@param coupler 连接器实例
 *@param event   事件
 */
void scui_plug_coupler_widget_m_event_proc(scui_plug_coupler_t *coupler, scui_event_t *event)
{
    SCUI_ASSERT(coupler != NULL);
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        // 主控件必须要保证自己启用帧动画效果
        // 否则无法派发给从控件树帧动画事件
        
        for (scui_handle_t idx = 0; idx < coupler->list_num; idx++) {
            if (coupler->list_widget_m[idx] != event->object)
                continue;
            
            scui_handle_t handle = event->object;
            event->object = coupler->list_widget_s[idx];
            scui_widget_event_dispatch(event);
            event->object = handle;
        }
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        for (scui_handle_t idx = 0; idx < coupler->list_num; idx++) {
            if (coupler->list_widget_m[idx] != event->object)
                continue;
            
            /* 获得连接器的关联目标 */
            scui_surface_t *surface_s = coupler->list_surface_s[idx];
            scui_handle_t   target_s  = coupler->list_widget_s[idx];
            scui_handle_t   target_m  = coupler->list_widget_m[idx];
            coupler->list_draw_idx    = idx;
            
            /* 没有剪切域,忽略该绘制,避免假绘制爆内存 */
            if (scui_widget_draw_empty(target_m)) {
                coupler->list_draw[idx] = false;
                
                // 主目标不能绘制,清空从目标画布
                if (surface_s != NULL) {
                    scui_widget_surface_remap(target_s, surface_s);
                    scui_widget_surface_destroy(target_s);
                    scui_widget_surface_remap(target_s, NULL);
                    coupler->list_surface_s[idx] = NULL;
                }
                return;
            }
            
            SCUI_LOG_INFO("widget idx:%u", idx);
            // 创建一个独立的子画布,将目标绘制到一个独立子画布中
            if (surface_s == NULL) {
                scui_area_t clip = scui_widget_clip(target_s);
                scui_widget_surface_create(target_s, SCUI_PIXEL_CF_DEF_A, clip.w, clip.h);
                surface_s = coupler->list_surface_s[idx] = scui_widget_surface(target_s);
                coupler->list_draw_clip[idx] = scui_widget_clip(target_s);
                coupler->list_refr[idx] = true;
            }
            scui_widget_surface_remap(target_s, surface_s);
            
            // 刷新该画布
            if (coupler->list_refr[idx]) {
                coupler->list_refr[idx] = false;
                scui_area_t clip = coupler->list_draw_clip[idx];
                coupler->list_draw_clip[idx] = (scui_area_t){0};
                scui_widget_show(target_s, false);
                scui_widget_draw(target_s, &clip, true);
                scui_event_t event = {0};
                event.object = target_s;
                event.type = scui_event_draw;
                // 移除跟主窗口相关所有绘制事件
                while (scui_event_dequeue(&event, true, false));
            }
            coupler->list_draw[idx] = true;
            
            // 流程到此,画布已经是新的了,标记进行绘制
            coupler->list_widget_m_cb(event);
            scui_widget_surface_remap(target_s, NULL);
        }
        break;
    }
    default:
        coupler->list_widget_m_cb(event);
        break;
    }
}
