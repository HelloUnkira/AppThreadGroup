/*实现目标:
 *    控件基础操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造器
 *@param widget 控件实例
 *@param maker  控件构造实例
 *@param handle 控件句柄
 */
void scui_widget_make(scui_widget_t *widget, void *maker, scui_handle_t *handle)
{
    scui_widget_maker_t *widget_maker = maker;
    SCUI_ASSERT(widget != NULL && widget_maker != NULL && handle != NULL);
    SCUI_ASSERT(widget_maker->clip.w != 0);
    SCUI_ASSERT(widget_maker->clip.h != 0);
    
    /* 构造器携带了句柄, 应该与目标句柄一致 */
    if (widget_maker->myself != SCUI_HANDLE_INVALID)
        SCUI_ASSERT(widget_maker->myself == *handle);
    
    /* 构造器句柄有效, 使用构造器句柄 */
    /* 构造器句柄无效, 使用动态句柄 */
    if (widget_maker->myself == SCUI_HANDLE_INVALID)
        *handle = scui_handle_find();
    
    /* 为句柄设置映射 */
    scui_handle_linker(*handle, widget);
    
    /* 控件使用maker构造 */
    if (widget_maker->parent != SCUI_HANDLE_INVALID)
        SCUI_ASSERT(scui_handle_remap(widget_maker->parent));
    
    widget->type   = widget_maker->type;
    widget->style  = widget_maker->style;
    widget->clip   = widget_maker->clip;
    widget->parent = widget_maker->parent;
    widget->myself = *handle;
    
    /* 控件初始默认为隐藏 */
    widget->state.view = false;
    
    /* 配置控件事件响应 */
    scui_event_cb_ready(&widget->list);
    scui_event_cb_node_t cb_node = {0};
    cb_node.event_cb = widget_maker->event_cb;
    if (widget_maker->event_cb == NULL) {
        scui_widget_map_t *widget_map = NULL;
        scui_widget_map_find(widget->type, &widget_map);
        cb_node.event_cb = widget_map->invoke;
    }
    
    if (cb_node.event_cb != NULL) {
        
        cb_node.event = scui_event_sched_all;
        scui_widget_event_add(*handle, &cb_node);
        
        if (widget->style.sched_widget) {
            cb_node.event = scui_event_widget_all;
            scui_widget_event_add(*handle, &cb_node);
        }
        if (widget->style.indev_ptr) {
            cb_node.event = scui_event_ptr_all;
            scui_widget_event_add(*handle, &cb_node);
        }
        if (widget->style.indev_enc) {
            cb_node.event = scui_event_enc_all;
            scui_widget_event_add(*handle, &cb_node);
        }
        if (widget->style.indev_key) {
            cb_node.event = scui_event_key_all;
            scui_widget_event_add(*handle, &cb_node);
        }
    }
    
    /* 构建孩子列表 */
    widget->child_now  = 0;
    widget->child_num  = widget_maker->child_num;
    widget->child_list = NULL;
    if (widget->child_num != 0) {
        scui_handle_t list_size = widget->child_num * sizeof(scui_handle_t);
        widget->child_list = SCUI_MEM_ALLOC(scui_mem_type_mix, list_size);
        for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
            widget->child_list[idx] = SCUI_HANDLE_INVALID;
    }
    
    /* 构建父子关联 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_handle_t  handle_r = scui_widget_root(widget->myself);
        scui_widget_t *widget_r = scui_handle_source_check(handle_r);
        scui_widget_t *widget_p = scui_handle_source_check(widget->parent);
        
        /* 画布的映射是根控件 */
        widget->surface = widget_r->surface;
        
        /* 子控件的坐标区域是相对根控件(递归语义) */
        if (widget_p->parent == SCUI_HANDLE_INVALID &&
            scui_widget_surface_only(widget));
        else {
            widget->clip.x += widget_p->clip.x;
            widget->clip.y += widget_p->clip.y;
        }
        
        /* 向父控件的孩子列表添加自己 */
        scui_widget_child_add(widget_p, widget->myself);
    }
    
    /* 画布剪切域重置更新 */
    scui_clip_ready(&widget->clip_set);
    scui_widget_surface_refr(widget->myself, false);
    
    /* 构建控件动画 */
    if (widget_maker->anima_num != 0)
        scui_widget_anima_create(widget->myself, widget_maker->anima_num);
    
    /* 默认控件透明度为全覆盖 */
    widget->alpha = scui_alpha_cover;
    widget->image = widget_maker->image;
    widget->color = widget_maker->color;
    
    SCUI_LOG_INFO("widget %u",              widget->myself);
    SCUI_LOG_INFO("widget type %u",         widget->type);
    SCUI_LOG_INFO("widget style %x",        widget->style);
    SCUI_LOG_INFO("widget clip.x %d",       widget->clip.x);
    SCUI_LOG_INFO("widget clip.y %d",       widget->clip.y);
    SCUI_LOG_INFO("widget clip.w %d",       widget->clip.w);
    SCUI_LOG_INFO("widget clip.h %d",       widget->clip.h);
    SCUI_LOG_INFO("widget myself %u",       widget->myself);
    SCUI_LOG_INFO("widget parent %u",       widget->parent);
    SCUI_LOG_INFO("widget child_num %u",    widget->child_num);
    SCUI_LOG_INFO("widget image %u",        widget->image);
    SCUI_LOG_INFO("widget color %08x",      widget->color.color.full);
}

/*@brief 控件析构器
 *@param widget 控件实例
 */
void scui_widget_burn(scui_widget_t *widget)
{
    SCUI_LOG_INFO("widget %u", widget->myself);
    
    /* 先递归销毁自己的孩子 */
    scui_widget_child_list_btra(widget, idx)
    scui_widget_destroy(widget->child_list[idx]);
    
    /* 回收用户资源句柄 */
    if (widget->user_data != SCUI_HANDLE_INVALID)
        scui_handle_clear(widget->user_data);
    
    /* 回收控件动画 */
    scui_widget_anima_destroy(widget->myself);
    
    /* 回收对象动画 */
    scui_anima_object_recycle(widget->myself);
    
    /* 画布剪切域清除 */
    scui_clip_clear(&widget->clip_set);
    
    /* 清空事件列表 */
    scui_widget_event_clear(widget->myself);
    
    /* 清空事件队列所有事件 */
    scui_event_t event = {.object = widget->myself};
    while (scui_event_dequeue(&event, true, true));
    
    /* 再从父控件的孩子列表移除自己 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_p = scui_handle_source_check(widget->parent);
        scui_widget_child_del(widget_p, widget->myself);
    }
    
    /* 回收孩子列表 */
    SCUI_MEM_FREE(widget->child_list);
    widget->child_list = NULL;
    
    /* 回收句柄 */
    scui_handle_clear(widget->myself);
}
