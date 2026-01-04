/*实现目标:
 *    控件上下文
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件剪切域
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_clip(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    return widget->clip;
}

/*@brief 控件统计剪切域大小
 *@param handle 控件句柄
 *@param size   剪切域大小
 */
void scui_widget_clip_sizes(scui_handle_t handle, scui_multi_t *size)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    scui_multi_t size_c = 0;
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        size_c += unit->clip.w * unit->clip.h;
    }
    *size += size_c;
    
    scui_widget_child_list_btra(widget, idx)
    scui_widget_clip_sizes(widget->child_list[idx], size);
}

/*@brief 控件检查剪切域
 *@param handle  控件句柄
 *@param recurse 递归处理
 */
void scui_widget_clip_check(scui_handle_t handle, bool recurse)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    SCUI_LOG_INFO("widget: %u, parent: %u", handle, widget->parent);
    
    SCUI_LOG_INFO("widget_clip:<%d, %d, %d, %d>",
                  widget->clip.x, widget->clip.y,
                  widget->clip.w, widget->clip.h);
    SCUI_LOG_INFO("surface_clip:<%d, %d, %d, %d>",
                  widget->clip_set.clip.x, widget->clip_set.clip.y,
                  widget->clip_set.clip.w, widget->clip_set.clip.h);
    
    scui_clip_btra(widget->clip_set, node) {
        scui_clip_unit_t *unit = scui_clip_unit(node);
        SCUI_LOG_INFO("<%d, %d, %d, %d>",
            unit->clip.x, unit->clip.y,
            unit->clip.w, unit->clip.h);
    }
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx)
    scui_widget_clip_check(widget->child_list[idx], recurse);
}

/*@brief 控件清除剪切域
 *@param widget  控件实例
 *@param recurse 递归处理
 */
void scui_widget_clip_clear(scui_widget_t *widget, bool recurse)
{
    SCUI_LOG_DEBUG("widget: %u", widget->myself);
    scui_clip_clear(&widget->clip_set);
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source(handle_c);
        scui_widget_clip_clear(widget_c, recurse);
    }
}

/*@brief 控件还原剪切域
 *@param widget  控件实例
 *@param clip    剪切域
 *@param recurse 递归处理
 */
void scui_widget_clip_reset(scui_widget_t *widget, scui_area_t *clip, bool recurse)
{
    SCUI_LOG_DEBUG("widget: %u", widget->myself);
    SCUI_ASSERT(clip != NULL);
    
    scui_area_t clip_inter = {0};
    if (scui_area_inter(&clip_inter, &widget->clip_set.clip, clip))
        scui_clip_add(&widget->clip_set, &clip_inter);
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source(handle_c);
        scui_widget_clip_reset(widget_c, clip, recurse);
    }
}

/*@brief 控件全覆盖剪切域检查
 *@param widget 控件实例
 */
bool scui_widget_clip_cover(scui_widget_t *widget)
{
    /* 控件需要显示 */
    if (scui_widget_is_show(widget->myself)) {
        /* 控件有透明度, 不是全覆盖 */
        if (widget->alpha != scui_alpha_cover)
            return false;
        /* 画布有透明度, 不是全覆盖 */
        if (scui_pixel_have_alpha(widget->surface->format))
            return false;
        
        /* 控件标记完全覆盖 */
        if (widget->style.cover_fg)
            return true;
        /* 控件背景不透明且全覆盖 */
        if (widget->style.fully_bg) {
            /* 纯色背景,全覆盖 */
            if (widget->image == SCUI_HANDLE_INVALID)
                return true;
            /* 图片背景,看是否自带透明度 */
            if (!scui_pixel_have_alpha(scui_image_cf(widget->image)))
                return true;
        }
    }
    
    /* 默认不覆盖 */
    return false;
}

/*@brief 控件更新剪切域
 *@param widget 控件实例
 */
void scui_widget_clip_update(scui_widget_t *widget)
{
    SCUI_LOG_DEBUG("widget: %u", widget->myself);
    
    /* 迭代到子控件,清除自己的剪切域 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source(handle_c);
        /* 控件隐藏则跳过 */
        if (scui_widget_is_hide(handle_c))
            continue;
        /* 控件满足完全覆盖的条件 */
        if (scui_widget_clip_cover(widget_c))
            scui_clip_del(&widget->clip_set, &widget_c->clip_set.clip);
        /* 迭代到子控件 */
        scui_widget_clip_update(widget_c);
    }
    
    /* 从父控件的当前位置开始,迭代到后面的兄弟控件,清除自己的剪切域 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_handle_t  widget_i = SCUI_HANDLE_INVALID - 1;
        scui_widget_t *widget_p = scui_handle_source_check(widget->parent);
        for (scui_handle_t idx = 0; idx < widget_p->child_num; idx++) {
            if (widget_p->child_list[idx] != widget->myself)
                continue;
            
            widget_i = idx;
            break;
        }
        SCUI_ASSERT(widget_i < widget_p->child_num);
        
        // 逆向绘制要顺向裁剪
        scui_handle_t widget_s = widget_p->style.order_draw ? 0 : widget_i;
        scui_handle_t widget_e = widget_p->style.order_draw ? widget_i : widget_p->child_num - 1;
        
        for (scui_handle_t idx = widget_s; idx <= widget_e; idx++) {
            if (widget_p->child_list[idx] == SCUI_HANDLE_INVALID || idx == widget_i)
                continue;
            
            scui_handle_t  handle_s = widget_p->child_list[idx];
            scui_widget_t *widget_s = scui_handle_source_check(handle_s);
            /* 控件隐藏则跳过 */
            if (scui_widget_is_hide(handle_s))
                continue;
            /* 控件满足完全覆盖的条件 */
            if (scui_widget_clip_cover(widget_s))
                scui_clip_del(&widget->clip_set, &widget_s->clip_set.clip);
        }
    }
}

/*@brief 控件画布
 *@param handle 控件句柄
 *@retval 控件画布
 */
scui_surface_t * scui_widget_surface(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    return widget->surface;
}

/*@brief 控件画布创建
 *@param handle  控件句柄
 *@param surface 画布参数(.format;.hor_res;.ver_res;)
 */
void scui_widget_surface_create(scui_handle_t handle, scui_surface_t *surface)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    SCUI_ASSERT(widget->surface == NULL && surface->hor_res > 0 && surface->ver_res > 0);
    widget->surface = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_surface_t));
    
    widget->surface->format  = surface->format;
    widget->surface->hor_res = surface->hor_res;
    widget->surface->ver_res = surface->ver_res;
    widget->surface->alpha   = scui_alpha_cover;
    
    if (widget->surface->format == scui_pixel_cf_def)
        widget->surface->format  = SCUI_PIXEL_CF_DEF;
    
    scui_coord_t res_byte = scui_pixel_bits(widget->surface->format) / 8;
    scui_coord_t res_rem  = sizeof(scui_color_wt_t) - res_byte;
    scui_multi_t res_size = surface->hor_res * surface->ver_res * res_byte + res_rem;
    widget->surface->pixel = SCUI_MEM_ALLOC(scui_mem_type_graph, res_size);
}

/*@brief 控件画布销毁
 *@param handle 控件句柄
 */
void scui_widget_surface_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (widget->surface == NULL)
        return;
    
    SCUI_MEM_FREE(widget->surface->pixel);
    SCUI_MEM_FREE(widget->surface);
    widget->surface = NULL;
}

/*@brief 控件画布重映射
 *@param handle  控件句柄
 *@param surface 画布实例
 */
void scui_widget_surface_remap(scui_handle_t handle, scui_surface_t *surface)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    widget->surface = surface;
    
    scui_widget_child_list_btra(widget, idx)
    scui_widget_surface_remap(widget->child_list[idx], surface);
}

/*@brief 控件画布剪切域刷新
 *@param handle  控件句柄
 *@param recurse 递归处理
 */
void scui_widget_surface_refr(scui_handle_t handle, bool recurse)
{
    SCUI_LOG_DEBUG("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    widget->clip_set.clip = widget->clip;
    /* 有独立画布的根控件不记录原点偏移,控件树永远相对独立画布移动 */
    /* 没有独立画布的根控件保留原点偏移,控件树永远相对绘制画布移动 */
    if (widget->parent == SCUI_HANDLE_INVALID &&
        scui_widget_surface_only(widget)) {
        widget->clip_set.clip.x = 0;
        widget->clip_set.clip.y = 0;
    }
    
    /* 画布的坐标区域是相对根控件(递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_p = scui_handle_source_check(widget->parent);
        scui_area_t clip_w = widget->clip_set.clip;
        scui_area_t clip_p = widget_p->clip_set.clip;
        /* 子控件的坐标区域是父控件坐标区域的子集 */
        scui_area_t clip_inter = {0};
        if (scui_area_inter(&clip_inter, &clip_w, &clip_p))
            widget->clip_set.clip = clip_inter;
        else
            widget->clip_set.clip = (scui_area_t){0};
    }
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx)
    scui_widget_surface_refr(widget->child_list[idx], recurse);
}

/*@brief 控件画布为独立画布
 *@param widget 控件实例
 */
bool scui_widget_surface_only(scui_widget_t *widget)
{
    #if SCUI_MEM_FEAT_MINI
    /* 小内存方案无独立画布 */
    return false;
    #endif
    
    if (widget->surface == NULL ||
        widget->surface->pixel == NULL ||
        widget->surface->pixel == scui_frame_buffer_draw()->pixel ||
        widget->surface->pixel == scui_frame_buffer_refr()->pixel)
        return false;
    
    return true;
}

/*@brief 控件画布更新
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_swap(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_ASSERT(widget->surface->pixel   != surface->pixel);
    SCUI_ASSERT(widget->surface->format  == surface->format);
    SCUI_ASSERT(widget->surface->hor_res == surface->hor_res);
    SCUI_ASSERT(widget->surface->ver_res == surface->ver_res);
    
    /* 直接交换实例最简单 */
    uint8_t *pixel = widget->surface->pixel;
    widget->surface->pixel = surface->pixel;
    surface->pixel = pixel;
}

/*@brief 控件画布同步
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_sync(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_ASSERT(widget->surface->pixel   != surface->pixel);
    SCUI_ASSERT(widget->surface->format  == surface->format);
    SCUI_ASSERT(widget->surface->hor_res == surface->hor_res);
    SCUI_ASSERT(widget->surface->ver_res == surface->ver_res);
    
    scui_surface_t *dst_surface = widget->surface;
    scui_surface_t *src_surface = surface;
    scui_area_t dst_clip = scui_surface_area(dst_surface);
    scui_area_t src_clip = scui_surface_area(src_surface);
    scui_draw_area_copy(dst_surface, &dst_clip, src_surface, &src_clip);
}
