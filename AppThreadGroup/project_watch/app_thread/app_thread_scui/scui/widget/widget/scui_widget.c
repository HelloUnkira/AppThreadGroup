/*实现目标:
 *    控件
 *    控件组织
 *    控件基础操作
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件创建
 *@param widget 控件实例
 *@param maker  控件实例构造参数
 *@param handle 控件句柄
 *@param layout 通过布局
 */
void scui_widget_create(scui_widget_t *widget, scui_widget_maker_t *maker,
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
    /* 子控件的坐标区域是相对根控件(递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = scui_handle_get(widget->parent);
        /* 父控件不能是根控件,统一相对偏移 */
        if (widget_parent->parent != SCUI_HANDLE_INVALID) {
            widget->clip.x += widget_parent->clip.x;
            widget->clip.y += widget_parent->clip.y;
        }
    }
    
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
        scui_widget_t *widget_root = NULL;
        scui_handle_t  handle_root = scui_widget_root(widget->myself);
        widget_root = scui_handle_get(handle_root);
        widget->surface = widget_root->surface;
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
    if (maker->event_cb != NULL) {
        scui_event_cb_node_t cb_node = {0};
        cb_node.event_cb = maker->event_cb;
        
        if (widget->style.sched_anima) {
            cb_node.event = scui_event_anima_elapse;
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

/*@brief 控件销毁
 *@param widget 控件实例
 */
void scui_widget_destroy(scui_widget_t *widget)
{
    if (scui_handle_unmap(widget->myself)) {
        SCUI_LOG_INFO("widget %u not load", widget->myself);
        return;
    }
    SCUI_LOG_INFO("widget %u", widget->myself);
    /* 先递归销毁自己的孩子列表 */
    scui_widget_child_list_btra(widget, idx)
        scui_widget_cb_destroy(widget->child_list[idx]);
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

/*@brief 查找控件映射表
 *@param type 控件类型
 *@retval 控件映射表
 */
scui_widget_cb_t * scui_widget_cb_find(uint32_t type)
{
    /* 控件组织表 */
    static const scui_widget_cb_t scui_widget_cb[scui_widget_type_num] = {
        [scui_widget_type_window].create    = (scui_widget_cb_create_t)   scui_window_create,
        [scui_widget_type_window].destroy   = (scui_widget_cb_destroy_t)  scui_window_destroy,
        [scui_widget_type_window].layout    = (scui_widget_cb_layout_t)   NULL,
        [scui_widget_type_custom].create    = (scui_widget_cb_create_t)   scui_custom_create,
        [scui_widget_type_custom].destroy   = (scui_widget_cb_destroy_t)  scui_custom_destroy,
        [scui_widget_type_custom].layout    = (scui_widget_cb_layout_t)   NULL,
        [scui_widget_type_scroll].create    = (scui_widget_cb_create_t)   scui_scroll_create,
        [scui_widget_type_scroll].destroy   = (scui_widget_cb_destroy_t)  scui_scroll_destroy,
        [scui_widget_type_scroll].layout    = (scui_widget_cb_layout_t)   scui_scroll_layout,
        [scui_widget_type_string].create    = (scui_widget_cb_create_t)   scui_string_create,
        [scui_widget_type_string].destroy   = (scui_widget_cb_destroy_t)  scui_string_destroy,
        [scui_widget_type_string].layout    = (scui_widget_cb_layout_t)   NULL,
        /* 扩展控件 */
        [scui_widget_type_button].create    = (scui_widget_cb_create_t)   scui_button_create,
        [scui_widget_type_button].destroy   = (scui_widget_cb_destroy_t)  scui_button_destroy,
        [scui_widget_type_button].layout    = (scui_widget_cb_layout_t)   NULL,
        [scui_widget_type_watch].create     = (scui_widget_cb_create_t)   scui_watch_create,
        [scui_widget_type_watch].destroy    = (scui_widget_cb_destroy_t)  scui_watch_destroy,
        [scui_widget_type_watch].layout     = (scui_widget_cb_layout_t)   NULL,
        [scui_widget_type_chart].create     = (scui_widget_cb_create_t)   scui_chart_create,
        [scui_widget_type_chart].destroy    = (scui_widget_cb_destroy_t)  scui_chart_destroy,
        [scui_widget_type_chart].layout     = (scui_widget_cb_layout_t)   NULL,
    };
    
    SCUI_ASSERT(type < scui_widget_type_num);
    return &scui_widget_cb[type];
}

/*@brief 通过映射表调用创建一个控件树
 *       从根控件开始到它的所有子控件(动态子控件在show之前生成)
 *@param handle 控件句柄
 */
void scui_widget_cb_create(scui_handle_t handle)
{
    if (scui_handle_remap(handle))
        return;
    
    scui_widget_t *widget = NULL;
    scui_widget_maker_t *maker = NULL;
    widget = maker = scui_handle_get(handle);
    SCUI_ASSERT(maker != NULL);
    SCUI_ASSERT(maker->parent == SCUI_HANDLE_INVALID);
    
    scui_handle_table_t *handle_table = scui_handle_table_find(handle);
    SCUI_ASSERT(handle_table != NULL);
    
    do {
        /* 先创建根控件,然后延续依次创建剩下的控件 */
        /* 静态控件规则为,一个场景为一段连续句柄,父控件在前子控件在后 */
        scui_widget_cb_t *widget_cb = scui_widget_cb_find(maker->type);
        widget_cb->create(maker, &handle, true);
        /* 迭代到下一个句柄 */
        handle++;
        widget = maker = scui_handle_get(handle);
        if (maker == NULL)
            break;
        /* 一直迭代到下一个根控件句柄前停下 */
        if ((scui_handle_unmap(handle) && maker->parent == SCUI_HANDLE_INVALID) ||
            (scui_handle_remap(handle) && widget->parent == SCUI_HANDLE_INVALID))
            break;
    } while (handle < handle_table->offset + handle_table->number);
}

/*@brief 卸载一个控件
 *@param handle 控件句柄
 */
void scui_widget_cb_destroy(scui_handle_t handle)
{
    /* 重复的销毁过滤 */
    if (scui_handle_unmap(handle))
        return;
    
    scui_widget_t *widget = NULL;
    widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    scui_widget_cb_t *widget_cb = scui_widget_cb_find(widget->type);
    widget_cb->destroy(handle);
}

/*@brief 更新一个控件布局
 *@param handle 控件句柄
 */
void scui_widget_cb_layout(scui_handle_t handle)
{
    if (scui_handle_unmap(handle))
        return;
    
    scui_widget_t *widget = NULL;
    widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    scui_widget_cb_t *widget_cb = scui_widget_cb_find(widget->type);
    if (widget_cb->layout != NULL)
        widget_cb->layout(handle);
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
            scui_widget_cb_layout(handle);
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
            scui_widget_cb_layout(handle);
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
           (widget->surface->format == scui_image_cf_bmp565 ||
            widget->surface->format == scui_image_cf_bmp888)) {
            /* 控件背景不透明且全覆盖 */
            if (!widget->style.trans) {
                /* 纯色背景,全覆盖 */
                if (widget->image == SCUI_HANDLE_INVALID)
                    return true;
                /* 图片背景,看是否自带透明度 */
                scui_image_t *image = scui_handle_get(widget->image);
                SCUI_ASSERT(image != NULL);
                if (image->format == scui_image_cf_bmp565 ||
                    image->format == scui_image_cf_bmp888)
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
    
    SCUI_ASSERT(widget->surface == NULL && hor_res != 0 && ver_res != 0);
    widget->surface = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_surface_t));
    memset(widget->surface, 0, sizeof(scui_surface_t));
    widget->surface->format  = format;
    widget->surface->hor_res = hor_res;
    widget->surface->ver_res = ver_res;
    widget->surface->alpha   = scui_alpha_cover;
    
    if (widget->surface->format == scui_pixel_cf_def)
        widget->surface->format  = SCUI_PIXEL_CF_DEF;
    
    scui_coord_t src_byte    = scui_pixel_bits(widget->surface->format) / 8;
    scui_coord_t src_remain  = sizeof(scui_color_limit_t) - src_byte;
    scui_multi_t res_surface = hor_res * ver_res * src_byte + src_remain;
    
    widget->surface->pixel = SCUI_MEM_ALLOC(scui_mem_type_graph, res_surface);
    /* 添加一个检测机制,如果surface不可创建时,清空image内存重新创建 */
    if (widget->surface->pixel == NULL) {
        SCUI_LOG_WARN("memory deficit was caught");
        scui_image_cache_clear();
        widget->surface->pixel  = SCUI_MEM_ALLOC(scui_mem_type_graph, res_surface);
    if (widget->surface->pixel == NULL) {
        scui_image_cache_visit();
        SCUI_ASSERT(false);
    }
    }
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
        widget->surface->pixel == scui_surface_fb_draw()->pixel ||
        widget->surface->pixel == scui_surface_fb_refr()->pixel)
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
    scui_area_t dst_clip = {
        .w = dst_surface->hor_res,
        .h = dst_surface->ver_res,
    };
    scui_area_t src_clip = {
        .w = src_surface->hor_res,
        .h = src_surface->ver_res,
    };
    
    scui_draw_area_copy(dst_surface, &dst_clip, src_surface, &src_clip);
}

/*@brief 控件坐标更新
 *@param handle 控件句柄
 *@param point  坐标点
 */
void scui_widget_move_pos(scui_handle_t handle, scui_point_t *point)
{
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (widget->clip.x == point->x &&
        widget->clip.y == point->y)
        return;
    
    SCUI_LOG_DEBUG("");
    /* 如果移动的是窗口,只需要改变窗口位置 */
    if (widget->parent == SCUI_HANDLE_INVALID) {
        widget->clip.x = point->x;
        widget->clip.y = point->y;
        scui_widget_refr(widget->myself, false);
        SCUI_LOG_DEBUG("<x:%d, y:%d>", point->x, point->y);
        return;
    }
    
    SCUI_LOG_DEBUG("");
    /* 计算移动偏移量 */
    scui_point_t offset = {
        .x = point->x - widget->clip.x,
        .y = point->y - widget->clip.y,
    };
    
    SCUI_LOG_DEBUG("");
    
    /* 移动自己 */
    widget->clip.x = point->x;
    widget->clip.y = point->y;
    
    SCUI_LOG_DEBUG("");
    /* 更新画布剪切域 */
    scui_widget_clip_clear(widget, false);
    scui_widget_surface_refr(widget->myself, false);
    
    SCUI_LOG_DEBUG("");
    /* 移动孩子,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_point_t point = {
            .x = offset.x + child->clip.x,
            .y = offset.y + child->clip.y,
        };
        scui_widget_move_pos(handle, &point);
    }
}

/*@brief 子控件坐标对齐
 *@param handle  控件句柄
 *@param handle  控件句柄(目标控件,不存在则相对父控件)
 *@param pos     对齐方向
 *@param offset  偏移量
 */
void scui_widget_align_pos(scui_handle_t handle, scui_handle_t target, scui_event_pos_t pos, scui_point_t *offset)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    scui_point_t point = {
        .x = widget->clip.x,
        .y = widget->clip.y,
    };
    
    if (target != SCUI_HANDLE_INVALID || widget->parent != SCUI_HANDLE_INVALID) {
        scui_handle_t  handle_target = target != SCUI_HANDLE_INVALID ? target : widget->parent;
        scui_widget_t *widget_target = scui_handle_get(handle_target);
        SCUI_ASSERT(widget_target != NULL);
        
        point.x = widget->clip.x;
        // 水平左对齐
        if ((pos & scui_event_pos_l) != 0)
            point.x = (widget_target->clip.x);
        // 水平右对齐
        if ((pos & scui_event_pos_r) != 0)
            point.x = (widget_target->clip.x + widget_target->clip.w - widget->clip.w);
        // 水平中心对齐
        if ((pos & scui_event_dir_hor) != 0)
            point.x /= 2;
        
        point.y = widget->clip.y;
        // 垂直上对齐
        if ((pos & scui_event_pos_u) != 0)
            point.y = (widget_target->clip.y);
        // 垂直下对齐
        if ((pos & scui_event_pos_d) != 0)
            point.y = (widget_target->clip.y + widget_target->clip.h - widget->clip.h);
        // 垂直中心对齐
        if ((pos & scui_event_dir_ver) != 0)
            point.y /= 2;
    }
    
    point.x += offset->x;
    point.y += offset->y;
    scui_widget_move_pos(handle, &point);
}

/*@brief 子控件坐标镜像
 *@param handle  控件句柄
 *@param child   控件子控件句柄(为空则镜像所有子控件)
 *@param dir     镜像方向(水平镜像或垂直镜像)
 *@param recurse 递归处理(全局镜像有效)
 */
void scui_widget_mirror_pos(scui_handle_t handle, scui_handle_t child, scui_event_dir_t dir, bool recurse)
{
    SCUI_LOG_DEBUG("");
    scui_handle_t  handle_child = child;
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    SCUI_ASSERT(dir == scui_event_dir_hor || dir == scui_event_dir_ver);
    
    SCUI_LOG_DEBUG("");
    /* 移动孩子,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_point_t   point = {.x = child->clip.x, .y = child->clip.y,};
        
        /* 存在指定子控件时, 只镜像子控件 */
        if (handle_child != SCUI_HANDLE_INVALID && handle_child != handle)
            continue;
        
        if (dir == scui_event_dir_hor)
            point.x = widget->clip.w - child->clip.w - child->clip.x;
        if (dir == scui_event_dir_ver)
            point.y = widget->clip.h - child->clip.h - child->clip.y;
        
        scui_widget_move_pos(handle, &point);
        
        if (!recurse)
             continue;
        
        /* 递归镜像 */
        scui_widget_mirror_pos(handle, SCUI_HANDLE_INVALID, dir, recurse);
    }
    
    if (handle_child != SCUI_HANDLE_INVALID)
        SCUI_LOG_ERROR("widget %u have not child widget %u", handle, child);
}

/*@brief 控件尺寸更新
 *@param handle 控件句柄
 *@param width  宽度
 *@param height 高度
 */
void scui_widget_adjust_size(scui_handle_t handle, scui_coord_t width, scui_coord_t height)
{
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(width  >= 0);
    SCUI_ASSERT(height >= 0);
    
    if (widget->parent == SCUI_HANDLE_INVALID) {
        SCUI_LOG_ERROR("unsupport");
        return;
    }
    
    if (widget->clip.w == width &&
        widget->clip.h == height)
        return;
    
    widget->clip.w = width;
    widget->clip.h = height;
    /* 更新画布剪切域 */
    scui_widget_clip_clear(widget, true);
    scui_widget_surface_refr(widget->myself, true);
}

/*@brief 控件移动子控件
 *@param handle 控件句柄
 *@param offset 偏移量
 */
void scui_widget_move_ofs_child_list(scui_handle_t handle, scui_point_t *offset)
{
    SCUI_LOG_INFO("widget %u offset(%u, %u)", handle, offset->x, offset->y);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (offset->x == 0 && offset->y == 0)
        return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_area_t clip_child = child->clip;
        clip_child.x += offset->x;
        clip_child.y += offset->y;
        scui_widget_move_pos(handle, &clip_child);
    }
}

/*@brief 控件移动子控件(循环模式)
 *@param handle 控件句柄
 *@param offset 偏移量
 *@param range  偏移量限制
 */
void scui_widget_move_ofs_child_list_loop(scui_handle_t handle, scui_point_t *offset, scui_point_t *range)
{
    SCUI_LOG_INFO("widget %u offset(%u, %u)", handle, offset->x, offset->y);
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    if (offset->x == 0 && offset->y == 0)
        return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_area_t clip_child = child->clip;
        scui_area_t clip_inter = {0};
        clip_child.x += offset->x;
        clip_child.y += offset->y;
        
        /* 计算是否与父控件存在交集 */
        if (scui_area_inter(&clip_inter, &widget->clip, &clip_child)) {
            scui_widget_move_pos(handle, &clip_child);
            continue;
        }
        
        /* 偏回一个range,计算是否与父控件存在交集 */
        clip_child.x -= range->x;
        clip_child.y -= range->y;
        if (scui_area_inter(&clip_inter, &widget->clip, &clip_child)) {
            scui_widget_move_pos(handle, &clip_child);
            continue;
        }
        clip_child.x += range->x;
        clip_child.y += range->y;
        
        /* 偏去一个range,计算是否与父控件存在交集 */
        clip_child.x += range->x;
        clip_child.y += range->y;
        if (scui_area_inter(&clip_inter, &widget->clip, &clip_child)) {
            scui_widget_move_pos(handle, &clip_child);
            continue;
        }
        clip_child.x -= range->x;
        clip_child.y -= range->y;
        
        /* 正常继续偏转 */
        scui_widget_move_pos(handle, &clip_child);
    }
}

/*@brief 控件对齐子控件计算
 *       中心对齐:则控件中心点与子控件中心点偏移量(最小)
 *       边界对齐:则边界中心点与子控件中心点偏移量(最小)
 *@param handle 控件句柄
 *@param target 控件句柄(目标控件)
 *@param offset 偏移量
 *@param pos    对齐目标
 */
bool scui_widget_align_pos_calc(scui_handle_t handle, scui_handle_t   *target,
                                scui_point_t *offset, scui_event_pos_t pos)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_scroll_t *scroll = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    if (widget->child_num == 0)
        return false;
    
    scui_point_t point_c = {0};
    scui_point_t point_child_c = {0};
    /* 中心对齐:则控件中心点与子控件中心点偏移量(最小) */
    if (pos == scui_event_pos_c) {
        point_c.x = widget->clip.x + widget->clip.w / 2;
        point_c.y = widget->clip.y + widget->clip.h / 2;
    }
    
    if (pos == scui_event_pos_u) {
        point_c.x = widget->clip.x + widget->clip.w / 2;
        point_c.y = widget->clip.y;
    }
    if (pos == scui_event_pos_d) {
        point_c.x = widget->clip.x + widget->clip.w / 2;
        point_c.y = widget->clip.y + widget->clip.h;
    }
    
    if (pos == scui_event_pos_l) {
        point_c.x = widget->clip.x;
        point_c.y = widget->clip.y + widget->clip.h / 2;
    }
    if (pos == scui_event_pos_r) {
        point_c.x = widget->clip.x + widget->clip.w;
        point_c.y = widget->clip.y + widget->clip.h / 2;
    }
    
    if (point_c.x == 0 && point_c.y == 0)
        return false;
    
    scui_handle_t handle_tar = SCUI_HANDLE_INVALID;
    scui_multi_t  dist_tar_a = scui_coord_max;
    scui_coord_t  dist_tar_x = 0;
    scui_coord_t  dist_tar_y = 0;
    
    /* 迭代计算每一个子控件,找到最小偏移量 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        
        if (pos == scui_event_pos_c) {
            point_child_c.x = child->clip.x + child->clip.w / 2;
            point_child_c.y = child->clip.y + child->clip.h / 2;
        }
        
        if (pos == scui_event_pos_u) {
            point_child_c.x = child->clip.x + child->clip.w / 2;
            point_child_c.y = child->clip.y;
        }
        if (pos == scui_event_pos_d) {
            point_child_c.x = child->clip.x + child->clip.w / 2;
            point_child_c.y = child->clip.y + child->clip.h;
        }
        
        if (pos == scui_event_pos_l) {
            point_child_c.x = child->clip.x;
            point_child_c.y = child->clip.y + child->clip.h / 2;
        }
        if (pos == scui_event_pos_r) {
            point_child_c.x = child->clip.x + child->clip.w;
            point_child_c.y = child->clip.y + child->clip.h / 2;
        }
        
        scui_coord_t dist_x = point_child_c.x - point_c.x;
        scui_coord_t dist_y = point_child_c.y - point_c.y;
        scui_multi_t dist_a = dist_x * dist_x + dist_y * dist_y;
        
        if (dist_tar_a > dist_a) {
            dist_tar_a = dist_a;
            dist_tar_x = dist_x;
            dist_tar_y = dist_y;
            handle_tar = child->myself;
        }
    }
    
    if (target != NULL)
       *target  = handle_tar;
    if (handle_tar == SCUI_HANDLE_INVALID)
        return false;
    
    offset->x = -dist_tar_x;
    offset->y = -dist_tar_y;
    return true;
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
