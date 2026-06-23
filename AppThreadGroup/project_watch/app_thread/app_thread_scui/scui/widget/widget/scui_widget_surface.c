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
scui_area_t scui_widget_area(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_area_t widget_clip = widget->clip;
    widget_clip.x = widget_clip.y = 0;
    return widget_clip;
}

/*@brief 控件剪切域
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_clip(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    return widget->clip;
}

/*@brief 控件剪切域
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_clip_root(scui_handle_t handle)
{
    scui_area_t widget_clip = scui_widget_clip(handle);
    if (scui_widget_parent(handle) == SCUI_HANDLE_INVALID)
        return widget_clip;
    
    scui_handle_t  handle_r = scui_widget_root(handle);
    scui_widget_t *widget_r = scui_handle_source_check(handle_r);
    if (widget_r->parent != SCUI_HANDLE_INVALID) {
        widget_clip.x += widget_r->clip.x;
        widget_clip.y += widget_r->clip.y;
    }
    
    return widget_clip;
}

/*@brief 控件剪切域
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_clip_tree(scui_handle_t handle)
{
    scui_area_t widget_clip = scui_widget_clip(handle);
    if (scui_widget_parent(handle) == SCUI_HANDLE_INVALID)
        return widget_clip;
    
    while (handle != SCUI_HANDLE_INVALID) {
        scui_handle_t  handle_r = scui_widget_root(handle);
        scui_widget_t *widget_r = scui_handle_source_check(handle_r);
        
        if (handle_r == handle) break;
        if (widget_r->parent == SCUI_HANDLE_INVALID) break;
        if (widget_r->parent != SCUI_HANDLE_INVALID) {
            widget_clip.x += widget_r->clip.x;
            widget_clip.y += widget_r->clip.y;
        }
        
        /* 递归向上迭代 */
        handle = handle_r;
    }
    
    return widget_clip;
}

/*@brief 控件统计剪切域大小
 *@param handle 控件句柄
 *@param size   剪切域大小
 */
void scui_widget_clip_sizes(scui_handle_t handle, scui_multi_t *size)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    *size += widget->clip_set.size;
    
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
 */
void scui_widget_clip_reset(scui_widget_t *widget, scui_area_t *clip)
{
    SCUI_LOG_DEBUG("widget: %u", widget->myself);
    
    /* 从根控件开始更新(它会保存最终剪切域) */
    /* 如果其他子控件都不存在独立画布的话 */
    if (widget->parent == SCUI_HANDLE_INVALID) {
        SCUI_ASSERT(clip == NULL);
        
        /* 为所有控件及其子控件添加画布控件剪切域 */
        scui_clip_btra(widget->clip_set, node) {
            scui_clip_unit_t *unit = scui_clip_unit(node);
            
            scui_widget_child_list_btra(widget, idx) {
                scui_handle_t  handle_c = widget->child_list[idx];
                scui_widget_t *widget_c = scui_handle_source_check(handle_c);
                scui_widget_clip_reset(widget_c, &unit->clip);
            }
        }
        return;
    }
    
    if (widget->style.buffer) {
        
        SCUI_ASSERT(clip != NULL);
        scui_area_t clip_inter = {0};
        if (scui_area_inter(&clip_inter, &widget->clip_set_p->clip, clip))
            scui_clip_add(widget->clip_set_p, &clip_inter);
        
        /* 为所有控件及其子控件添加画布控件剪切域 */
        scui_clip_btra(widget->clip_set, node) {
            scui_clip_unit_t *unit = scui_clip_unit(node);
            scui_widget_child_list_btra(widget, idx) {
                scui_handle_t  handle_c = widget->child_list[idx];
                scui_widget_t *widget_c = scui_handle_source_check(handle_c);
                scui_widget_clip_reset(widget_c, &unit->clip);
            }
        }
        
        return;
    }
    
    SCUI_ASSERT(clip != NULL);
    scui_area_t clip_inter = {0};
    if (scui_area_inter(&clip_inter, &widget->clip_set.clip, clip))
        scui_clip_add(&widget->clip_set, &clip_inter);
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source(handle_c);
        scui_widget_clip_reset(widget_c, clip);
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
        if (scui_pixel_alpha_in(widget->surface->format))
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
            if (!scui_pixel_alpha_in(scui_image_cf(widget->image)))
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
    
    /* 从父控件的当前位置开始,迭代到后面的兄弟控件,清除自己的剪切域 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_handle_t  index_i = SCUI_HANDLE_INVALID - 1;
        scui_widget_t *widget_p = scui_handle_source_check(widget->parent);
        for (scui_multi_t idx = 0; idx < widget_p->child_num; idx++) {
            if (widget_p->child_list[idx] != widget->myself)
                continue;
            
            index_i = idx;
            break;
        }
        SCUI_ASSERT(index_i < widget_p->child_num);
        
        /* 逆向绘制要顺向裁剪 */
        scui_handle_t index_s = widget_p->style.order_draw ? 0 : index_i;
        scui_handle_t index_e = widget_p->style.order_draw ? index_i : widget_p->child_num - 1;
        
        for (scui_multi_t idx = index_s; idx <= index_e; idx++) {
            if (widget_p->child_list[idx] == SCUI_HANDLE_INVALID) continue;
            if (idx == index_i) continue;
            
            scui_handle_t  handle_s = widget_p->child_list[idx];
            scui_widget_t *widget_s = scui_handle_source_check(handle_s);
            /* 控件隐藏则跳过 */
            if (scui_widget_is_hide(handle_s))
                continue;
            
            /* 控件满足完全覆盖的条件 */
            if (scui_widget_clip_cover(widget_s) && !widget->style.fully_clip)
                scui_clip_del(&widget->clip_set, &widget_s->clip_set.clip);
        }
    }
    
    /* 清除自己的剪切域,迭代到子控件 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source(handle_c);
        /* 控件隐藏则跳过, 完全剪切域则跳过 */
        if (scui_widget_is_hide(handle_c))
            continue;
        
        if (widget->style.buffer && widget->parent != SCUI_HANDLE_INVALID) {
            /* 控件满足完全覆盖的条件 */
            /* 这里需要待定一下, 是否要考虑画布属性 */
            if (scui_widget_clip_cover(widget_c) && !widget->style.fully_clip)
                scui_clip_del(&widget->clip_set, &widget_c->clip_set_p->clip);
        } else {
            /* 控件满足完全覆盖的条件 */
            if (scui_widget_clip_cover(widget_c) && !widget->style.fully_clip)
                scui_clip_del(&widget->clip_set, &widget_c->clip_set.clip);
        }
        
        /* 迭代到子控件 */
        scui_widget_clip_update(widget_c);
    }
}

/*@brief 控件绘制剪切域
 *@param widget  控件实例
 *@param clip    剪切域
 *@param type    添加目标(0:自己;1:父源;2:两者)
 */
void scui_widget_clip_draw(scui_widget_t *widget, scui_area_t *clip, scui_handle_t type)
{
    SCUI_LOG_DEBUG("widget: %u", widget->myself);
    SCUI_ASSERT(clip != NULL);
    
    /* 自身剪切域 */
    if (type == 0 || type == 2) {
        
        if (widget->style.buffer || widget->parent == SCUI_HANDLE_INVALID) {
            
            scui_area_t clip_inter = {0};
            if (scui_area_inter(&clip_inter, &widget->clip_set.clip, clip))
                scui_clip_add(&widget->clip_set, &clip_inter);
        }
    }
    
    /* 父源剪切域 */
    if (type == 1 || type == 2) {
        
        if (widget->style.buffer || widget->parent != SCUI_HANDLE_INVALID) {
            
            scui_area_t clip_p = *clip;
            clip_p.x += widget->clip.x;
            clip_p.y += widget->clip.y;
            
            scui_area_t clip_inter = {0};
            if (scui_area_inter(&clip_inter, &widget->clip_set_p->clip, &clip_p))
                scui_clip_add(widget->clip_set_p, &clip_inter);
        }
    }
}

/*@brief 控件绘制剪切域(树)
 *@param widget 控件实例
 */
void scui_widget_clip_draw_tree(scui_widget_t *widget)
{
    SCUI_LOG_DEBUG("widget: %u", widget->myself);
    
    if (widget->style.buffer || widget->parent == SCUI_HANDLE_INVALID) {
        
        scui_area_t clip_s = scui_widget_area(widget->myself);
        scui_widget_clip_draw(widget, &clip_s, 0);
        
        if (widget->parent != SCUI_HANDLE_INVALID) {
            
            scui_area_t clip_s = scui_widget_area(widget->myself);
            scui_widget_clip_draw(widget, &clip_s, 1);
        }
    }
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source(handle_c);
        scui_widget_clip_draw_tree(widget_c);
    }
}

/*@brief 控件画布
 *@param handle 控件句柄
 *@retval 控件画布实例
 */
scui_surface_t * scui_widget_surface(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (widget->surface_s == NULL) return widget->surface;
    return widget->surface_s;
}

/*@brief 控件画布图
 *@param handle 控件句柄
 *@retval 画布图句柄
 */
scui_handle_t scui_widget_surface_image(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (widget->surface_s == NULL) return SCUI_HANDLE_INVALID;
    return widget->image_s;
}

/*@brief 控件画布创建
 *@param widget  控件实例
 *@param surface 画布参数(.format;.hor_res;.ver_res;)
 */
void scui_widget_surface_create(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_ASSERT(widget->surface_s  == NULL);
    SCUI_ASSERT(surface->hor_res != 0);
    SCUI_ASSERT(surface->ver_res != 0);
    
    scui_pixel_cf_t format = surface->format;
    if (format == scui_pixel_cf_def)
        format  = SCUI_PIXEL_CF_DEF;
    
    /* 分配画布资源 */
    widget->surface_s = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_surface_t));
    
    widget->surface_s->format  = format;
    widget->surface_s->hor_res = surface->hor_res;
    widget->surface_s->ver_res = surface->ver_res;
    scui_surface_config(widget->surface_s);
    
    /* 分配画布剪切域集合 */
    widget->clip_set_p = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_clip_set_t));
    scui_clip_ready(widget->clip_set_p);
    
    /* 创建句柄并绑定 */
    widget->image_s = scui_handle_find();
    widget->image_s_src = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_image_t));
    scui_handle_linker(widget->image_s, widget->image_s_src);
    scui_image_by_surface(widget->image_s_src, widget->surface_s);
    
    if (widget->style.buffer_d) return;
    
    /* 更新画布实例 */
    scui_widget_surface_ready(widget->myself);
}

/*@brief 控件画布销毁
 *@param widget 控件实例
 */
void scui_widget_surface_destroy(scui_widget_t *widget)
{
    if (widget->surface_s == NULL) return;
    
    /* 回收画布资源 */
    scui_widget_surface_recycle(widget->myself);
    
    /* 回收画布图句柄 */
    scui_handle_clear(widget->image_s);
    widget->image_s = SCUI_HANDLE_INVALID;
    
    /* 回收画布图实例 */
    SCUI_MEM_FREE(widget->image_s_src);
    widget->image_s_src = NULL;
    
    /* 回收画布剪切域集合 */
    scui_clip_clear(widget->clip_set_p);
    SCUI_MEM_FREE(widget->clip_set_p);
    widget->clip_set_p = NULL;
    
    /* 回收画布实例 */
    SCUI_MEM_FREE(widget->surface_s);
    widget->surface_s = NULL;
}

/*@brief 控件画布资源就绪
 *@param handle 控件句柄
 */
void scui_widget_surface_ready(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (widget->surface_s == NULL) return;
    if (widget->surface_s->pixel != NULL) return;
    
    /* 分配画布资源内存 */
    scui_coord_t res_rem  = sizeof(scui_color_wt_t) - widget->surface_s->pbyte;
    scui_multi_t res_size = widget->surface_s->ver_res * widget->surface_s->stride + res_rem;
    widget->surface_s->pixel = SCUI_MEM_ALLOC(scui_mem_type_graph, res_size);
    
    /* 同步更新画布图实例 */
    scui_image_by_surface(widget->image_s_src, widget->surface_s);
}

/*@brief 控件画布资源回收
 *@param handle 控件句柄
 */
void scui_widget_surface_recycle(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    if (widget->surface_s == NULL) return;
    if (widget->surface_s->pixel == NULL) return;
    
    /* 回收画布资源内存 */
    SCUI_MEM_FREE(widget->surface_s->pixel);
    widget->surface_s->pixel = NULL;
    
    /* 同步更新画布图实例 */
    scui_image_by_surface(widget->image_s_src, widget->surface_s);
}

/*@brief 控件画布重映射
 *@param widget  控件实例
 *@param surface 画布实例
 */
void scui_widget_surface_remap(scui_widget_t *widget, scui_surface_t *surface)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    
    /* 本控件有自己的独立画布 */
    /* 它所在的子控件树都被禁止重定向画布 */
    if (widget->surface_s != NULL) return;
    widget->surface = surface;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_widget_t *child = scui_handle_source(widget->child_list[idx]);
        scui_widget_surface_remap(child, surface);
    }
}

/*@brief 控件画布剪切域刷新
 *@param widget  控件实例
 *@param recurse 递归处理
 */
void scui_widget_surface_refr(scui_widget_t *widget, bool recurse)
{
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    scui_clip_clear(&widget->clip_set);
    
    /* 控件永远相对画布运动 */
    widget->clip_set.clip = widget->clip;
    
    /* 子控件的坐标区域是父控件坐标区域的子集(递归语义) */
    if (!widget->style.buffer && widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_p = scui_handle_source_check(widget->parent);
        scui_area_t clip_w = widget->clip_set.clip;
        scui_area_t clip_p = widget_p->clip_set.clip;
        /* 子控件的坐标区域是父控件坐标区域的子集 */
        widget->clip_set.clip = (scui_area_t){0};
        if (scui_area_inter2(&clip_w, &clip_p))
            widget->clip_set.clip = clip_w;
    }
    
    /* 坐标区域是相对独立画布运动 */
    if (widget->style.buffer && widget->surface == widget->surface_s) {
        
        widget->clip_set_p->clip = widget->clip_set.clip;
        widget->clip_set.clip.x = 0;
        widget->clip_set.clip.y = 0;
    }
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source(handle_c);
        scui_widget_surface_refr(widget_c, recurse);
    }
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
    
    scui_surface_t *surface_i = scui_widget_surface(widget->myself);
    if (surface_i == NULL || surface_i->pixel == NULL ||
        surface_i->pixel == scui_frame_buffer_draw()->pixel ||
        surface_i->pixel == scui_frame_buffer_refr()->pixel)
        return false;
    
    return true;
}
