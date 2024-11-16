/*实现目标:
 *    控件
 *    控件组织
 *    控件基础操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 创建控件
 *@param maker  控件实例构造参数
 *@param handle 控件句柄
 *@param layout 通过布局
 */
void scui_widget_create(void *maker, scui_handle_t *handle, bool layout)
{
    scui_widget_maker_t *widget_maker = maker;
    scui_widget_cb_t *widget_cb = NULL;
    scui_widget_cb_find(widget_maker->type, &widget_cb);
    widget_cb->create(maker, handle, layout);
}

/*@brief 销毁控件
 *@param handle 控件句柄
 */
void scui_widget_destroy(scui_handle_t handle)
{
    /* 重复的销毁过滤 */
    if (scui_handle_unmap(handle))
        return;
    
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    scui_widget_cb_t *widget_cb = NULL;
    scui_widget_cb_find(widget->type, &widget_cb);
    widget_cb->destroy(handle);
}

/*@brief 控件构造器
 *@param widget 控件实例
 *@param maker  控件实例构造参数
 *@param handle 控件句柄
 *@param layout 通过布局
 */
void scui_widget_constructor(scui_widget_t *widget, scui_widget_maker_t *maker,
                             scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(widget != NULL && maker != NULL && handle != NULL);
    SCUI_ASSERT(maker->clip.w != 0);
    SCUI_ASSERT(maker->clip.h != 0);
    
    /* 非布局则句柄为动态创建 */
    *handle = layout ? maker->myself : scui_handle_find();
    
    SCUI_LOG_DEBUG("");
    /* 为句柄设置映射 */
    scui_handle_set(*handle, widget);
    
    /* 控件使用maker构造 */
    if (maker->parent != SCUI_HANDLE_INVALID)
        SCUI_ASSERT(scui_handle_remap(maker->parent));
    
    scui_event_cb_ready(&widget->list);
    
    SCUI_LOG_DEBUG("");
    widget->type   = maker->type;
    widget->style  = maker->style;
    widget->clip   = maker->clip;
    widget->parent = maker->parent;
    widget->myself = *handle;
    
    SCUI_LOG_DEBUG("");
    /* 构建孩子列表 */
    widget->child_num = maker->child_num;
    widget->child_list = NULL;
    if (widget->child_num != 0) {
        uint32_t child_list_size = widget->child_num * sizeof(scui_handle_t);
        widget->child_list = SCUI_MEM_ALLOC(scui_mem_type_mix, child_list_size);
        for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
            widget->child_list[idx] = SCUI_HANDLE_INVALID;
    }
    
    SCUI_LOG_DEBUG("");
    /* 画布的映射是根控件 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_handle_t handle = scui_widget_root(widget->myself);
        scui_widget_t  *root = scui_handle_get(handle);
        widget->surface = root->surface;
    }
    
    SCUI_LOG_DEBUG("");
    /* 子控件的坐标区域是相对根控件(递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *parent = scui_handle_get(widget->parent);
        
        if (parent->parent == SCUI_HANDLE_INVALID &&
            scui_widget_surface_only(widget));
        else {
            widget->clip.x += parent->clip.x;
            widget->clip.y += parent->clip.y;
        }
    }
    
    /* 画布剪切域重置更新 */
    scui_clip_ready(&widget->clip_set);
    scui_widget_surface_refr(widget->myself, false);
    
    widget->alpha = scui_alpha_cover;
    widget->image = maker->image;
    widget->color = maker->color;
    
    SCUI_LOG_DEBUG("");
    /* 向父控件的孩子列表添加自己 */
    if (widget->parent != SCUI_HANDLE_INVALID)
        scui_widget_child_add(widget->parent, widget->myself);
    
    SCUI_LOG_DEBUG("");
    /* 非根控件要设置为显示,否则为隐藏 */
    widget->style.state = widget->parent != SCUI_HANDLE_INVALID;
    
    /* 配置控件事件响应 */
    scui_event_cb_node_t cb_node = {0};
    cb_node.event_cb = maker->event_cb;
    if (maker->event_cb == NULL) {
        scui_widget_cb_t *widget_cb = NULL;
        scui_widget_cb_find(widget->type, &widget_cb);
        cb_node.event_cb = widget_cb->event_cb;
    }
    
    if (cb_node.event_cb != NULL) {
        
        cb_node.event = scui_event_sched_all;
        scui_widget_event_add(*handle, &cb_node);
        
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
void scui_widget_destructor(scui_widget_t *widget)
{
    if (scui_handle_unmap(widget->myself)) {
        SCUI_LOG_INFO("widget %u not load", widget->myself);
        return;
    }
    SCUI_LOG_INFO("widget %u", widget->myself);
    /* 回收用户资源句柄 */
    if (widget->user_data != SCUI_HANDLE_INVALID)
        scui_handle_set(widget->user_data, NULL);
    /* 先递归销毁自己的孩子列表 */
    scui_widget_child_list_btra(widget, idx)
        scui_widget_destroy(widget->child_list[idx]);
    /* 画布剪切域清除 */
    scui_clip_clear(&widget->clip_set);
    /* 再从父控件的孩子列表移除自己 */
    if (widget->parent != SCUI_HANDLE_INVALID)
        scui_widget_child_del(widget->parent, widget->myself);
    /* 最后回收资源 */
    SCUI_MEM_FREE(widget->child_list);
    widget->child_list = NULL;
    /* 清空事件列表 */
    scui_widget_event_clear(widget->myself);
    /* 回收句柄 */
    scui_handle_set(widget->myself, NULL);
}

/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (widget->parent != SCUI_HANDLE_INVALID)
        return scui_widget_root(widget->parent);
    return handle;
}

/*@brief 控件的父控件
 *@param handle 控件句柄
 *@retval 父控件
 */
scui_handle_t scui_widget_parent(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->parent;
}

/*@brief 子控件总数量
 *@param handle 控件句柄
 *@retval 子控件数量
 */
scui_handle_t scui_widget_child_num(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->child_num;
}

/*@brief 控件添加子控件
 *@param handle 控件句柄
 *@param child  控件子控件句柄
 */
void scui_widget_child_add(scui_handle_t handle, scui_handle_t child)
{
    SCUI_LOG_INFO("widget %u add child %u", handle, child);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] == SCUI_HANDLE_INVALID) {
            widget->child_list[idx]  = child;
            
            /* 子控件列表更新,布局更新 */
            scui_widget_cb_t *widget_cb = NULL;
            scui_widget_cb_find(widget->type, &widget_cb);
            if (widget_cb->layout != NULL)
                widget_cb->layout(handle);
            return;
        }
    SCUI_LOG_WARN("widget %u add child %u fail", handle, child);
}

/*@brief 控件移除子控件
 *@param handle 控件句柄
 *@param child  控件子控件句柄
 */
void scui_widget_child_del(scui_handle_t handle, scui_handle_t child)
{
    SCUI_LOG_INFO("widget %u del child %u", handle, child);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] == child) {
            widget->child_list[idx]  = SCUI_HANDLE_INVALID;
            
            /* 子控件列表更新,布局更新 */
            scui_widget_cb_t *widget_cb = NULL;
            scui_widget_cb_find(widget->type, &widget_cb);
            if (widget_cb->layout != NULL)
                widget_cb->layout(handle);
            return;
        }
    SCUI_LOG_WARN("widget %u del child %u fail", handle, child);
}

/*@brief 指定位置子控件
 *@param handle 控件句柄
 *@param index  子控件位置(映射点)
 *@retval 子控件句柄
 */
scui_handle_t scui_widget_child_by_index(scui_handle_t handle, scui_handle_t index)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    /* 其他范围映射到取值范围[0:num) */
    index = scui_mabs(index, widget->child_num);
    
    if (index < widget->child_num)
        return widget->child_list[index];
    return SCUI_HANDLE_INVALID;
}

/*@brief 指定位置子控件
 *@param handle 控件句柄
 *@param index  子控件句柄
 *@retval 子控件句柄
 */
scui_handle_t scui_widget_child_to_index(scui_handle_t handle, scui_handle_t child)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    scui_widget_child_list_btra(widget, idx)
        if (child == widget->child_list[idx])
            return idx;
    
    SCUI_ASSERT(false);
    return -1;
}

/*@brief 控件检查剪切域
 *@param handle  控件句柄
 *@param recurse 递归处理
 */
void scui_widget_clip_check(scui_handle_t handle, bool recurse)
{
    SCUI_LOG_WARN("widget: %u", handle);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    SCUI_LOG_WARN("widget_clip:<%d, %d, %d, %d>",
                  widget->clip.x, widget->clip.y,
                  widget->clip.w, widget->clip.h);
    SCUI_LOG_WARN("surface_clip:<%d, %d, %d, %d>",
                  widget->clip_set.clip.x, widget->clip_set.clip.y,
                  widget->clip_set.clip.w, widget->clip_set.clip.h);
    
    scui_clip_check(&widget->clip_set);
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_clip_check(handle, recurse);
    }
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
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_widget_clip_clear(child, recurse);
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
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_widget_clip_reset(child, clip, recurse);
    }
}

/*@brief 控件全覆盖剪切域检查
 *@param widget 控件实例
 */
bool scui_widget_clip_cover(scui_widget_t *widget)
{
    /* 控件需要显示 */
    if (scui_widget_style_is_show(widget->myself)) {
        /* 控件不透明 */
        if (widget->alpha != scui_alpha_trans &&
           (widget->surface->format == scui_pixel_cf_bmp565 ||
            widget->surface->format == scui_pixel_cf_bmp888)) {
            /* 控件标记完全覆盖 */
            if (widget->style.cover)
                return true;
            /* 控件背景不透明且全覆盖 */
            if (!widget->style.trans) {
                /* 纯色背景,全覆盖 */
                if (widget->image == SCUI_HANDLE_INVALID)
                    return true;
                /* 图片背景,看是否自带透明度 */
                scui_image_t *image = scui_handle_get(widget->image);
                SCUI_ASSERT(image != NULL);
                if (image->format == scui_pixel_cf_bmp565 ||
                    image->format == scui_pixel_cf_bmp888)
                    return true;
            }
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
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        /* 控件隐藏则跳过 */
        if (scui_widget_style_is_hide(handle))
            continue;
        /* 控件满足完全覆盖的条件 */
        if (scui_widget_clip_cover(child))
            scui_clip_del(&widget->clip_set, &child->clip_set.clip);
        /* 迭代到子控件 */
        scui_widget_clip_update(child);
    }
    
    /* 从父控件的当前位置开始,迭代到后面的兄弟控件,清除自己的剪切域 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        bool not_match = true;
        scui_widget_t *widget_parent = scui_handle_get(widget->parent);
        for (scui_handle_t idx = 0; idx < widget_parent->child_num; idx++) {
            if (widget_parent->child_list[idx] != SCUI_HANDLE_INVALID &&
                widget_parent->child_list[idx] != widget->myself) {
                if (not_match)
                    continue;
                scui_handle_t handle = widget_parent->child_list[idx];
                scui_widget_t *buddy = scui_handle_get(handle);
                /* 控件隐藏则跳过 */
                if (scui_widget_style_is_hide(handle))
                    continue;
                /* 控件满足完全覆盖的条件 */
                if (scui_widget_clip_cover(buddy))
                    scui_clip_del(&widget->clip_set, &buddy->clip_set.clip);
            }
            if (widget_parent->child_list[idx] == widget->myself)
                not_match = false;
        }
        SCUI_ASSERT(!not_match);
    }
}

/*@brief 控件画布
 *@param handle 控件句柄
 *@retval 控件画布
 */
scui_surface_t * scui_widget_surface(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->surface;
}

/*@brief 控件画布创建
 *@param handle  控件句柄
 *@param format  画布格式
 *@param hor_res 画布水平尺寸
 *@param ver_res 画布垂直尺寸
 */
void scui_widget_surface_create(scui_handle_t handle,  scui_pixel_cf_t format,
                                scui_coord_t  hor_res, scui_coord_t    ver_res)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    SCUI_ASSERT(widget->surface == NULL && hor_res > 0 && ver_res > 0);
    widget->surface = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_surface_t));
    memset(widget->surface, 0, sizeof(scui_surface_t));
    widget->surface->format  = format;
    widget->surface->hor_res = hor_res;
    widget->surface->ver_res = ver_res;
    widget->surface->alpha   = scui_alpha_cover;
    
    if (widget->surface->format == scui_pixel_cf_def)
        widget->surface->format  = SCUI_PIXEL_CF_DEF;
    
    scui_coord_t src_byte    = scui_pixel_bits(widget->surface->format) / 8;
    scui_coord_t src_remain  = sizeof(scui_color_wt_t) - src_byte;
    scui_multi_t src_size    = hor_res * ver_res * src_byte + src_remain;
    
    widget->surface->pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, src_size);
}

/*@brief 控件画布销毁
 *@param handle 控件句柄
 */
void scui_widget_surface_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
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
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    SCUI_LOG_DEBUG("widget %u", widget->myself);
    
    widget->surface = surface;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_surface_remap(handle, surface);
    }
}

/*@brief 控件画布剪切域刷新
 *@param handle  控件句柄
 *@param recurse 递归处理
 */
void scui_widget_surface_refr(scui_handle_t handle, bool recurse)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    SCUI_LOG_DEBUG("widget %u", widget->myself);
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
        scui_widget_t *widget_parent = scui_handle_get(widget->parent);
        scui_area_t widget_clip = widget->clip_set.clip;
        scui_area_t parent_clip = widget_parent->clip_set.clip;
        /* 子控件的坐标区域是父控件坐标区域的子集 */
        scui_area_t clip_inter = {0};
        if (scui_area_inter(&clip_inter, &widget_clip, &parent_clip))
            widget->clip_set.clip = clip_inter;
        else
            widget->clip_set.clip = (scui_area_t){0};
    }
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_surface_refr(handle, recurse);
    }
}

/*@brief 控件画布为独立画布
 *@param widget 控件实例
 */
bool scui_widget_surface_only(scui_widget_t *widget)
{
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
    scui_draw_dsc_t draw_dsc = {
        .area_copy.dst_surface = dst_surface,
        .area_copy.dst_clip    = &dst_clip,
        .area_copy.src_surface = src_surface,
        .area_copy.src_clip    = &src_clip,
    };
    scui_draw_area_copy(&draw_dsc);
}

/*@brief 控件类型
 *@param handle 控件句柄
 *@retval 控件类型
 */
scui_widget_type_t scui_widget_type(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->type;
}

/*@brief 控件剪切域
 *@param handle 控件句柄
 *@retval 控件剪切域
 */
scui_area_t scui_widget_clip(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->clip;
}

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_style_is_show(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    /* 如果它的父容器隐藏则它也隐藏(这是递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID)
    if (!scui_widget_style_is_show(widget->parent))
         return false;
    
    /* 它自己的显示状态 */
    if (widget->style.state)
        return true;
    
    return false;
}

/*@brief 控件隐藏状态获取
 *@param handle 控件句柄
 *@retval 是否隐藏
 */
bool scui_widget_style_is_hide(scui_handle_t handle)
{
    return !scui_widget_style_is_show(handle);
}

/*@brief 用户资源获取
 *@param handle 控件句柄
 *@retval 用户资源
 */
void * scui_widget_user_data_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (widget->user_data == SCUI_HANDLE_INVALID)
        return NULL;
    
    return scui_handle_get(widget->user_data);
}

/*@brief 控件透明度获取
 *@param handle 控件句柄
 *@retval 控件透明度
 */
scui_alpha_t scui_widget_alpha_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->alpha;
}

/*@brief 控件图片获取
 *@param handle 控件句柄
 *@retval 图片句柄
 */
scui_handle_t scui_widget_image_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->image;
}

/*@brief 控件颜色获取
 *@param handle 控件句柄
 *@retval 颜色
 */
scui_color_t scui_widget_color_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    return widget->color;
}

/*@brief 用户资源设置
 *@param handle    控件句柄
 *@param user_data 用户资源
 */
void scui_widget_user_data_set(scui_handle_t handle, void *user_data)
{
    SCUI_LOG_INFO("widget %u user src %u set", handle, user_data);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (widget->user_data == SCUI_HANDLE_INVALID)
        widget->user_data  = scui_handle_find();
    
    scui_handle_set(widget->user_data, user_data);
}

/*@brief 控件透明度设置
 *@param handle  控件句柄
 *@param alpha   控件透明度
 *@param recurse 递归处理
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha, bool recurse)
{
    SCUI_LOG_INFO("widget %u alpha %u set", handle, alpha);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    widget->alpha = alpha;
    
    if (!recurse)
         return;
    
    /* 必须递归设置控件透明度,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx)
        scui_widget_alpha_set(widget->child_list[idx], alpha, recurse);
}

/*@brief 控件图片设置
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_set(scui_handle_t handle, scui_handle_t image)
{
    SCUI_LOG_INFO("widget %u image %u set", handle, image);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    widget->image = image;
}

/*@brief 控件颜色设置
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_set(scui_handle_t handle, scui_color_t color)
{
    SCUI_LOG_INFO("widget %u color %x set", handle, color.color.full);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    widget->color = color;
}
