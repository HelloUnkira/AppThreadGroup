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
void scui_widget_create(scui_widget_t *widget, scui_widget_maker_t *maker, scui_handle_t *handle, bool layout)
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
        widget->child_list = SCUI_MEM_ALLOC(scui_mem_type_def, child_list_size);
        for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
            widget->child_list[idx] = SCUI_HANDLE_INVALID;
    }
    
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget_root = NULL;
    scui_handle_t  handle_root = scui_widget_root(widget->myself);
    widget_root = scui_handle_get(handle_root);
    
    /* 画布的映射是根控件, 控件画布默认为完全不透明 */
    widget->surface.alpha = scui_alpha_cover;
    widget->surface.pixel = widget_root->surface.pixel;
    widget->surface.line  = widget_root->surface.line;
    
    /* 更新画布剪切域 */
    scui_widget_surface_refr(widget, false);
    
    /* 画布剪切域更新,主域配置为空 */
    scui_clip_ready(&widget->clip_set);
    widget->clip_set.clip = (scui_area_t){0};
    
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
        
        if (widget->style.anima_sched) {
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
    /* 画布剪切域清除 */
    scui_clip_clear(&widget->clip_set);
    /* 先递归销毁自己的孩子列表 */
    scui_widget_child_list_btra(widget, idx)
        scui_widget_cb_destroy(widget->child_list[idx]);
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
        [scui_widget_type_window].create  = (scui_widget_cb_create_t)   scui_window_create,
        [scui_widget_type_window].destroy = (scui_widget_cb_destroy_t)  scui_window_destroy,
        [scui_widget_type_window].layout  = (scui_widget_cb_layout_t)   NULL,
        [scui_widget_type_custom].create  = (scui_widget_cb_create_t)   scui_custom_create,
        [scui_widget_type_custom].destroy = (scui_widget_cb_destroy_t)  scui_custom_destroy,
        [scui_widget_type_custom].layout  = (scui_widget_cb_layout_t)   NULL,
        [scui_widget_type_scroll].create  = (scui_widget_cb_create_t)   scui_scroll_create,
        [scui_widget_type_scroll].destroy = (scui_widget_cb_destroy_t)  scui_scroll_destroy,
        [scui_widget_type_scroll].layout  = (scui_widget_cb_layout_t)   scui_scroll_layout,
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

/*@brief 控件检查剪切域
 *@param widget  控件实例
 *@param recurse 递归处理
 */
void scui_widget_clip_check(scui_widget_t *widget, bool recurse)
{
    SCUI_LOG_WARN("widget: %u", widget->myself);
    SCUI_LOG_INFO("<%d, %d, %d, %d>",
                  widget->clip.x, widget->clip.y,
                  widget->clip.w, widget->clip.h);
    SCUI_LOG_INFO("<%d, %d, %d, %d>",
                  widget->surface.clip.x, widget->surface.clip.y,
                  widget->surface.clip.w, widget->surface.clip.h);
    scui_clip_check(&widget->clip_set);
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_widget_clip_check(child, recurse);
    }
}

/*@brief 控件清除剪切域
 *@param widget  控件实例
 *@param recurse 递归处理
 */
void scui_widget_clip_clear(scui_widget_t *widget, bool recurse)
{
    SCUI_LOG_DEBUG("widget: %u", widget->myself);
    widget->clip_set.clip = widget->surface.clip;
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
    
    if (clip != NULL) {
        widget->clip_set.clip.w = 0;
        widget->clip_set.clip.h = 0;
        
        scui_area_t clip_inter = {0};
        if (scui_area_inter(&clip_inter, &widget->surface.clip, clip))
            widget->clip_set.clip = clip_inter;
    }
    scui_clip_clear(&widget->clip_set);
    scui_clip_add(&widget->clip_set, &widget->clip_set.clip);
    
    if (!recurse)
         return;
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_widget_clip_reset(child, clip, recurse);
    }
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
        /* 只有子控件显示,不透明,完全覆盖才能移除 */
        bool ignore = !(child->style.state && !child->style.trans && child->surface.alpha == scui_alpha_cover);
        if (!ignore)
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
                /* 只有兄弟控件显示,不透明,完全覆盖才能移除 */
                bool ignore = !(buddy->style.state && !buddy->style.trans && buddy->surface.alpha == scui_alpha_cover);
                if (!ignore)
                     scui_clip_del(&widget->clip_set, &buddy->clip_set.clip);
            }
            if (widget_parent->child_list[idx] == widget->myself)
                not_match = false;
        }
        SCUI_ASSERT(!not_match);
    }
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
    
    if (widget->clip.x == point->x && widget->clip.y == point->y)
        return;
    
    SCUI_LOG_DEBUG("");
    /* 如果移动的是窗口,只需要改变窗口位置 */
    if (widget->parent == SCUI_HANDLE_INVALID) {
        widget->clip.x = point->x;
        widget->clip.y = point->y;
        if (!scui_widget_surface_only(widget)) {
             scui_widget_surface_refr(widget, true);
             scui_widget_clip_clear(widget, false);
        }
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
    scui_widget_surface_refr(widget, false);
    scui_widget_clip_clear(widget, false);
    
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

/*@brief 子控件坐标镜像
 *@param handle  控件句柄
 *@param dir     水平镜像或垂直镜像
 *@param recurse 递归处理
 */
void scui_widget_mirror_pos(scui_handle_t handle, scui_event_dir_t dir, bool recurse)
{
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    SCUI_ASSERT(dir == scui_event_dir_hor || dir == scui_event_dir_ver);
    
    SCUI_LOG_DEBUG("");
    /* 移动孩子,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle = widget->child_list[idx];
        scui_widget_t *child = scui_handle_get(handle);
        scui_point_t   point = {.x = child->clip.x, .y = child->clip.y,};
        
        if (dir == scui_event_dir_hor)
            point.x = widget->clip.w - child->clip.w - child->clip.x;
        if (dir == scui_event_dir_ver)
            point.y = widget->clip.h - child->clip.h - child->clip.y;
        
        scui_widget_move_pos(handle, &point);
        
        if (!recurse)
             continue;
        scui_widget_mirror_pos(handle, dir, recurse);
    }
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
    
    if (widget->clip.w == width && widget->clip.h == height)
        return;
    
    widget->clip.w = width;
    widget->clip.h = height;
    /* 更新画布剪切域 */
    scui_widget_surface_refr(widget, true);
    scui_widget_clip_clear(widget, true);
}

/*@brief 控件移动子控件
 *@param handle 控件句柄
 *@param offset 偏移量
 */
void scui_widget_refr_ofs_child_list(scui_handle_t handle, scui_point_t *offset)
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
void scui_widget_refr_ofs_child_list_loop(scui_handle_t handle, scui_point_t *offset, scui_point_t *range)
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
