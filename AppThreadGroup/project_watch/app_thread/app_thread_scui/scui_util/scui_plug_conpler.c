/*实现目标:
 *    插件:控件连接器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 插件:连接器:创建
 *@param conpler  连接器实例地址
 *@param list_num 控件数量
 */
void scui_plug_conpler_create(scui_plug_conpler_t **conpler, scui_handle_t list_num)
{
    SCUI_ASSERT(list_num != 0);
    *conpler = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_plug_conpler_t));
    memset(*conpler, 0, sizeof(scui_plug_conpler_t));
    
    (*conpler)->list_widget_m_cb = NULL;
    (*conpler)->list_surface_s = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_surface_t *) * list_num);
    (*conpler)->list_widget_s  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_handle_t) * list_num);
    (*conpler)->list_widget_m  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_handle_t) * list_num);
    (*conpler)->list_draw_clip = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_area_t) * list_num);
    (*conpler)->list_draw_idx = -1;
    (*conpler)->list_num = list_num;
    (*conpler)->list_draw = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(bool) * list_num);
    (*conpler)->list_refr = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(bool) * list_num);
    
    memset((*conpler)->list_surface_s, 0, sizeof(scui_surface_t *) * list_num);
    memset((*conpler)->list_widget_s,  0, sizeof(scui_handle_t) * list_num);
    memset((*conpler)->list_widget_m,  0, sizeof(scui_handle_t) * list_num);
    memset((*conpler)->list_draw_clip, 0, sizeof(scui_area_t) * list_num);
    memset((*conpler)->list_draw, 0, sizeof(bool) * list_num);
    memset((*conpler)->list_refr, 0, sizeof(bool) * list_num);
}

/*@brief 插件:连接器:销毁
 *@param conpler 连接器实例
 */
void scui_plug_conpler_destroy(scui_plug_conpler_t *conpler)
{
    SCUI_MEM_FREE(conpler->list_refr);
    SCUI_MEM_FREE(conpler->list_draw);
    SCUI_MEM_FREE(conpler->list_draw_clip);
    SCUI_MEM_FREE(conpler->list_widget_m);
    SCUI_MEM_FREE(conpler->list_widget_s);
    SCUI_MEM_FREE(conpler->list_surface_s);
    SCUI_MEM_FREE(conpler);
}

/*@brief 插件:连接器:回收
 *@param conpler 连接器实例
 *@param any     回收所有
 */
void scui_plug_conpler_recycle(scui_plug_conpler_t *conpler, bool any)
{
    SCUI_ASSERT(conpler != NULL);
    
    uintptr_t surface_clear_nums   = 0;
    uintptr_t surface_clear_bytes  = 0;
    uintptr_t surface_remain_nums  = 0;
    uintptr_t surface_remain_bytes = 0;
    
    for (uint8_t idx = 0; idx < conpler->list_num; idx++) {
        
        scui_surface_t *surface_s = conpler->list_surface_s[idx];
        scui_handle_t   target_s  = conpler->list_widget_s[idx];
        scui_handle_t   target_m  = conpler->list_widget_m[idx];
        
        SCUI_LOG_DEBUG("surface index:%x", idx);
        uintptr_t surface_size = 0;
        
        if (surface_s != NULL) {
            uintptr_t cf_byte   = scui_pixel_bits(surface_s->format) / 8;
            uintptr_t cf_remain = sizeof(scui_color_wt_t) - cf_byte;
            surface_size = surface_s->hor_res * surface_s->ver_res * cf_byte + cf_remain;
        }
        
        if (conpler->list_draw[idx] && !any) {
            conpler->list_draw[idx] = false;
            
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
        conpler->list_surface_s[idx] = NULL;
    }
    
    SCUI_LOG_INFO("surface clear  num:%u, bytes:%x", surface_clear_nums,  surface_clear_bytes);
    SCUI_LOG_INFO("surface remain num:%u, bytes:%x", surface_remain_nums, surface_remain_bytes);
}

/*@brief 控件事件响应回调(需要包装)
 *@param conpler 连接器实例
 *@param event   事件
 */
void scui_plug_conpler_widget_s_event_proc(scui_plug_conpler_t *conpler, scui_event_t *event)
{
    SCUI_ASSERT(conpler != NULL);
    
    switch (event->type) {
    case scui_event_draw: {
        
        if (scui_widget_surface(event->object) == NULL) {
            scui_handle_t handle = scui_widget_root(event->object);
            for (scui_handle_t idx = 0; idx < conpler->list_num; idx++) {
                if (conpler->list_widget_s[idx] != handle)
                    continue;
                // scui_widget_clip_check(scui_widget_root(handle), true);
                scui_widget_draw(conpler->list_widget_m[idx], NULL, false);
                // scui_widget_clip_check(scui_widget_root(handle), true);
                SCUI_LOG_INFO("widget idx:%u", idx);
                conpler->list_refr[idx] = true;
                break;
            }
            break;
        }
        
        scui_event_cb_t event_cb = NULL;
        scui_widget_cb_event_cb(event->object, &event_cb);
        if (event_cb != NULL)
            event_cb(event);
        
        if (scui_widget_surface(event->object)->protect == scui_pixel_pb_unique) {
            scui_handle_t handle = scui_widget_root(event->object);
            for (scui_handle_t idx = 0; idx < conpler->list_num; idx++) {
                if (conpler->list_widget_s[idx] != handle)
                    continue;
                // scui_widget_clip_check(scui_widget_root(handle), true);
                conpler->list_draw_clip[idx] = scui_widget_clip(event->object);
                scui_widget_draw(conpler->list_widget_m[idx], NULL, false);
                // scui_widget_clip_check(scui_widget_root(handle), true);
                SCUI_LOG_INFO("widget idx:%u", idx);
                conpler->list_refr[idx] = true;
                break;
            }
            break;
        }
        break;
    }
    default: {
        scui_event_cb_t event_cb = NULL;
        scui_widget_cb_event_cb(event->object, &event_cb);
        if (event_cb != NULL)
            event_cb(event);
        break;
    }
    }
}

/*@brief 控件事件响应回调(需要包装)
 *@param conpler 连接器实例
 *@param event   事件
 */
void scui_plug_conpler_widget_m_event_proc(scui_plug_conpler_t *conpler, scui_event_t *event)
{
    SCUI_ASSERT(conpler != NULL);
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        // 主控件必须要保证自己启用帧动画效果
        // 否则无法派发给从控件树帧动画事件
        
        for (scui_handle_t idx = 0; idx < conpler->list_num; idx++) {
            if (conpler->list_widget_m[idx] != event->object)
                continue;
            
            scui_handle_t handle = event->object;
            event->object = conpler->list_widget_s[idx];
            scui_widget_event_dispatch(event);
            event->object = handle;
        }
        break;
    }
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        for (scui_handle_t idx = 0; idx < conpler->list_num; idx++) {
            if (conpler->list_widget_m[idx] != event->object)
                continue;
            
            /* 获得连接器的关联目标 */
            scui_surface_t *surface_s = conpler->list_surface_s[idx];
            scui_handle_t   target_s  = conpler->list_widget_s[idx];
            scui_handle_t   target_m  = conpler->list_widget_m[idx];
            conpler->list_draw_idx    = idx;
            
            /* 没有剪切域,忽略该绘制,避免假绘制爆内存 */
            if (scui_widget_draw_empty(target_m)) {
                conpler->list_draw[idx] = false;
                
                // 主目标不能绘制,清空从目标画布
                if (surface_s != NULL) {
                    scui_widget_surface_remap(target_s, surface_s);
                    scui_widget_surface_destroy(target_s);
                    scui_widget_surface_remap(target_s, NULL);
                    conpler->list_surface_s[idx] = NULL;
                }
                return;
            }
            
            SCUI_LOG_INFO("widget idx:%u", idx);
            // 创建一个独立的子画布,将目标绘制到一个独立子画布中
            if (surface_s == NULL) {
                scui_area_t clip = scui_widget_clip(target_s);
                scui_widget_surface_create(target_s, SCUI_PIXEL_CF_DEF_A, scui_pixel_pb_shared, clip.w, clip.h);
                surface_s = conpler->list_surface_s[idx] = scui_widget_surface(target_s);
                conpler->list_draw_clip[idx] = scui_widget_clip(target_s);
                conpler->list_refr[idx] = true;
            }
            scui_widget_surface_remap(target_s, surface_s);
            
            // 刷新该画布
            if (conpler->list_refr[idx]) {
                conpler->list_refr[idx] = false;
                scui_area_t clip = conpler->list_draw_clip[idx];
                conpler->list_draw_clip[idx] = (scui_area_t){0};
                scui_widget_show(target_s, false);
                scui_widget_draw(target_s, &clip, true);
                scui_event_t event = {0};
                event.object = target_s;
                event.type = scui_event_draw;
                // 移除跟主窗口相关所有绘制事件
                while (scui_event_dequeue(&event, true));
            }
            conpler->list_draw[idx] = true;
            
            // 流程到此,画布已经是新的了,标记进行绘制
            conpler->list_widget_m_cb(event);
            scui_widget_surface_remap(target_s, NULL);
        }
        break;
    }
    default:
        conpler->list_widget_m_cb(event);
        break;
    }
}
