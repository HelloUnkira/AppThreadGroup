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
    
    /* 控件初始默认为隐藏(无响应) */
    widget->style.state = false;
    
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
    widget->child_num = widget_maker->child_num;
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
    if (scui_handle_unmap(widget->myself)) {
        SCUI_LOG_INFO("widget %u not load", widget->myself);
        return;
    }
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

/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    return widget->parent == SCUI_HANDLE_INVALID ? handle :
        scui_widget_root(widget->parent);
}

/*@brief 控件的父控件
 *@param handle 控件句柄
 *@retval 父控件
 */
scui_handle_t scui_widget_parent(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    return widget->parent;
}

/*@brief 控件添加子控件
 *@param widget 控件实例
 *@param child  控件子控件句柄
 */
void scui_widget_child_add(scui_widget_t *widget, scui_handle_t child)
{
    SCUI_LOG_INFO("widget %u add child %u", widget->myself, child);
    
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] == SCUI_HANDLE_INVALID) {
            widget->child_list[idx]  = child;
            
            /* 子控件列表更新通知 */
            scui_event_t event = {
                .object     = widget->myself,
                .type       = scui_event_child_nums,
                .absorb     = scui_event_absorb_none,
            };
            scui_event_notify(&event);
            return;
        }
    SCUI_LOG_WARN("widget %u add child %u fail", widget->myself, child);
}

/*@brief 控件移除子控件
 *@param widget 控件实例
 *@param child  控件子控件句柄
 */
void scui_widget_child_del(scui_widget_t *widget, scui_handle_t child)
{
    SCUI_LOG_INFO("widget %u del child %u", widget->myself, child);
    
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] == child) {
            widget->child_list[idx]  = SCUI_HANDLE_INVALID;
            
            /* 子控件列表更新通知 */
            scui_event_t event = {
                .object     = widget->myself,
                .type       = scui_event_child_nums,
                .absorb     = scui_event_absorb_none,
            };
            scui_event_notify(&event);
            return;
        }
    SCUI_LOG_WARN("widget %u del child %u fail", widget->myself, child);
}

/*@brief 子控件总数量
 *@param handle 控件句柄
 *@retval 子控件数量
 */
scui_handle_t scui_widget_child_num(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    return widget->child_num;
}

/*@brief 指定偏移子控件
 *@param handle 控件句柄
 *@param offset 偏移值
 *@param way    偏移方向(0:顺向;1:逆向;)
 */
scui_handle_t scui_widget_child_offset(scui_handle_t handle, scui_handle_t offset, bool way)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    if (way) {
        scui_handle_t offset_c = 0;
        scui_widget_child_list_ftra(widget, idx) {
            if (offset_c != offset) {
                offset_c++;
                continue;
            }
            return widget->child_list[idx];
        }
    } else {
        scui_handle_t offset_c = 0;
        scui_widget_child_list_btra(widget, idx) {
            if (offset_c != offset) {
                offset_c++;
                continue;
            }
            return widget->child_list[idx];
        }
    }
    return SCUI_HANDLE_INVALID;
}

/*@brief 指定位置子控件
 *@param handle 控件句柄
 *@param index  子控件位置(映射点)
 *@retval 子控件句柄
 */
scui_handle_t scui_widget_child_by_index(scui_handle_t handle, scui_handle_t index)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 其他范围映射到取值范围[0:num) */
    index = scui_mabs(index, widget->child_num);
    
    if (index < widget->child_num)
        return widget->child_list[index];
    
    return SCUI_HANDLE_INVALID;
}

/*@brief 指定位置子控件
 *@param handle 控件句柄
 *@param child  子控件句柄
 *@retval 子控件位置(映射点)
 */
scui_handle_t scui_widget_child_to_index(scui_handle_t handle, scui_handle_t child)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    scui_widget_child_list_btra(widget, idx)
        if (widget->child_list[idx] == child)
            return idx;
    
    SCUI_ASSERT(false);
    return -1;
}

/*@brief 移动到指定位置子控件
 *@param handle 控件句柄
 *@param child  子控件句柄
 *@param index  子控件位置(映射点)
 */
void scui_widget_child_move_index(scui_handle_t handle, scui_handle_t child, scui_handle_t index)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_handle_t index_c = scui_widget_child_to_index(handle, child);
    if (index_c == index)
        return;
    
    if (index_c < index) {
        for (int64_t idx = index_c; idx + 1 <= index; idx++)
            widget->child_list[idx] = widget->child_list[idx + 1];
        widget->child_list[index] = child;
        return;
    }
    
    if (index_c > index) {
        for (int64_t idx = index_c; idx - 1 >= index; idx--)
            widget->child_list[idx] = widget->child_list[idx - 1];
        widget->child_list[index] = child;
        return;
    }
}

/*@brief 移动子控件到最前面
 *@param handle 控件句柄
 *@param child  子控件句柄
 */
void scui_widget_child_move_foreground(scui_handle_t handle, scui_handle_t child)
{
    scui_handle_t target = scui_widget_child_offset(handle, 0, 1);
    scui_handle_t index_t = scui_widget_child_to_index(handle, target);
    scui_widget_child_move_index(handle, child, index_t);
}

/*@brief 移动子控件到最后面
 *@param handle 控件句柄
 *@param child  子控件句柄
 */
void scui_widget_child_move_background(scui_handle_t handle, scui_handle_t child)
{
    scui_handle_t target = scui_widget_child_offset(handle, 0, 0);
    scui_handle_t index_t = scui_widget_child_to_index(handle, target);
    scui_widget_child_move_index(handle, child, index_t);
}

/*@brief 控件类型
 *@param handle 控件句柄
 *@retval 控件类型
 */
scui_widget_type_t scui_widget_type(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    return widget->type;
}

/*@brief 控件显示状态更新
 *@param handle  控件句柄
 *@param recurse 递归处理
 */
void scui_widget_state_show(scui_handle_t handle, bool recurse)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    SCUI_LOG_INFO("widget :%u", handle);
    
    /* 无需重复显示 */
    if (widget->style.state)
        return;
    
    /* 设置控件状态为显示 */
    widget->style.state = true;
    
    /* 同步生成控件显示事件 */
    scui_event_t event = {
        .object     = handle,
        .style.sync = true,
        .type       = scui_event_show,
    };
    scui_event_notify(&event);
    
    /* 非根控件更新父控件布局 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_event_t event = {
            .object = widget->parent,
            .type   = scui_event_layout,
            .absorb = scui_event_absorb_none,
        };
        scui_event_notify(&event);
    }
    
    if (!recurse)
         return;
    
    /* 必须递归设置控件透明度,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx)
    scui_widget_state_show(widget->child_list[idx], recurse);
}

/*@brief 控件隐藏状态更新
 *@param handle  控件句柄
 *@param recurse 递归处理
 */
void scui_widget_state_hide(scui_handle_t handle, bool recurse)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    SCUI_LOG_INFO("widget :%u", handle);
    
    /* 无需重复隐藏 */
    if (!widget->style.state)
         return;
    
    /* 设置控件状态为隐藏 */
    widget->style.state = false;
    
    /* 同步生成控件隐藏事件 */
    scui_event_t event = {
        .object     = handle,
        .style.sync = true,
        .type       = scui_event_hide,
    };
    scui_event_notify(&event);
    
    /* 非根控件更新父控件布局 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_event_t event = {
            .object = widget->parent,
            .type   = scui_event_layout,
            .absorb = scui_event_absorb_none,
        };
        scui_event_notify(&event);
    }
    
    if (!recurse)
         return;
    
    /* 必须递归设置控件透明度,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx)
    scui_widget_state_hide(widget->child_list[idx], recurse);
}

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_is_show(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 如果它的父容器隐藏则它也隐藏(这是递归语义) */
    if (widget->parent != SCUI_HANDLE_INVALID)
    if (!scui_widget_is_show(widget->parent))
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
bool scui_widget_is_hide(scui_handle_t handle)
{
    return !scui_widget_is_show(handle);
}

/*@brief 用户资源获取
 *@param handle    控件句柄
 *@param user_data 用户资源
 */
void scui_widget_user_data_get(scui_handle_t handle, void **user_data)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    *user_data = NULL;
    if (widget->user_data != SCUI_HANDLE_INVALID)
       *user_data = scui_handle_source(widget->user_data);
}

/*@brief 控件透明度获取
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
void scui_widget_alpha_get(scui_handle_t handle, scui_alpha_t *alpha)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    *alpha = widget->alpha;
}

/*@brief 控件图片获取
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_get(scui_handle_t handle, scui_handle_t *image)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    *image = widget->image;
}

/*@brief 控件颜色获取
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_get(scui_handle_t handle, scui_color_t *color)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    *color = widget->color;
}

/*@brief 用户资源设置
 *@param handle    控件句柄
 *@param user_data 用户资源
 */
void scui_widget_user_data_set(scui_handle_t handle, void *user_data)
{
    SCUI_LOG_INFO("widget %u user src %u set", handle, user_data);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    if (widget->user_data == SCUI_HANDLE_INVALID)
        widget->user_data  = scui_handle_find();
    
    scui_handle_linker(widget->user_data, user_data);
}

/*@brief 控件透明度设置撤销
 *@param handle  控件句柄
 *@param alpha   控件透明度
 *@param recurse 递归处理
 */
void scui_widget_alpha_undo(scui_handle_t handle, scui_alpha_t alpha, bool recurse)
{
    SCUI_LOG_INFO("widget %u alpha %u set", handle, alpha);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    widget->alpha = scui_alpha_undo(widget->alpha, alpha);
    
    if (!recurse)
         return;
    
    /* 必须递归设置控件透明度,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx)
    scui_widget_alpha_set(widget->child_list[idx], alpha, recurse);
}

/*@brief 控件透明度设置融合
 *@param handle  控件句柄
 *@param alpha   控件透明度
 *@param recurse 递归处理
 */
void scui_widget_alpha_mix(scui_handle_t handle, scui_alpha_t alpha, bool recurse)
{
    SCUI_LOG_INFO("widget %u alpha %u set", handle, alpha);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    widget->alpha = scui_alpha_mix(widget->alpha, alpha);
    
    if (!recurse)
         return;
    
    /* 必须递归设置控件透明度,迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx)
    scui_widget_alpha_set(widget->child_list[idx], alpha, recurse);
}

/*@brief 控件透明度设置
 *@param handle  控件句柄
 *@param alpha   控件透明度
 *@param recurse 递归处理
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha, bool recurse)
{
    SCUI_LOG_INFO("widget %u alpha %u set", handle, alpha);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
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
    scui_widget_t *widget = scui_handle_source_check(handle);
    widget->image = image;
}

/*@brief 控件颜色设置
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_set(scui_handle_t handle, scui_color_t color)
{
    SCUI_LOG_INFO("widget %u color %x set", handle, color.color.full);
    scui_widget_t *widget = scui_handle_source_check(handle);
    widget->color = color;
}
