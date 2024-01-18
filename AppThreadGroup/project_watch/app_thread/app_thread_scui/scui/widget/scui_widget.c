/*实现目标:
 *    控件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 控件创建
 *@param widget 控件实例
 *@param maker  控件实例构造参数
 *@param handle 控件句柄
 */
void scui_widget_create(scui_widget_t *widget, scui_widget_maker_t *maker, scui_handle_t handle)
{
    APP_SYS_ASSERT(widget != NULL && maker != NULL && handle != SCUI_HANDLE_INVALID);
    
    /* 控件使用maker构造 */
    if (maker->parent != SCUI_HANDLE_INVALID)
        APP_SYS_ASSERT(scui_handle_remap(maker->parent));
    
    app_sys_list_dll_reset(&widget->dl_list);
    
    widget->type   = maker->type;
    widget->style  = maker->style;
    widget->clip   = maker->clip;
    widget->myself = handle;
    widget->parent = maker->parent;
    
    /* 子控件的坐标区域是相对父控件 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = NULL;
        scui_handle_t  handle_parent = widget->parent;
        widget_parent = scui_handle_get(handle_parent);
        handle_parent = widget_parent->parent;
        widget->clip.x += widget_parent->clip.x;
        widget->clip.y += widget_parent->clip.y;
        /* 子控件的坐标区域是父控件坐标区域的子集 */
        scui_area_t clip_merge = {0};
        scui_area_merge(&clip_merge, &widget->clip, &widget_parent->clip);
        widget->clip = clip_merge;
    }
    
    /* 构建孩子列表 */
    widget->child_num = maker->child_num;
    widget->child_list = NULL;
    if (widget->child_num != 0) {
        uint32_t child_list_size = widget->child_num * sizeof(scui_handle_t);
        widget->child_list = SCUI_MEM_ALLOC(scui_mem_is_part, child_list_size);
        for (uint32_t idx = 0; idx < widget->child_num; idx++)
            widget->child_list[idx] = SCUI_HANDLE_INVALID;
    }
    
    /* 控件默认为完全不透明 */
    widget->surface.alpha = scui_alpha_cover;
    widget->surface_clip = (scui_area_t){0};
    widget->surface.clip = (scui_area_t){0};
    widget->surface_clip.x = widget->clip.x;
    widget->surface_clip.y = widget->clip.y;
    widget->surface.clip.w = widget->clip.w;
    widget->surface.clip.h = widget->clip.h;
    /* 画布的坐标区域是相对父控件 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = NULL;
        scui_handle_t  handle_parent = widget->parent;
        widget_parent = scui_handle_get(handle_parent);
        handle_parent = widget_parent->parent;
        widget->surface.clip.x += widget_parent->surface.clip.x;
        widget->surface.clip.y += widget_parent->surface.clip.y;
        /* 子控件的坐标区域是父控件坐标区域的子集 */
        scui_area_t clip_merge = {0};
        scui_area_merge(&clip_merge, &widget->surface.clip, &widget_parent->surface.clip);
        widget->surface.clip = clip_merge;
    }
    
    widget->image = maker->image;
    widget->color = maker->color;
    
    /* 向父控件的孩子列表添加自己 */
    if (widget->parent != SCUI_HANDLE_INVALID)
        scui_widget_child_add(widget->myself, widget->parent);
    
    /* 非根控件要设置为显示,否则为隐藏 */
    if (widget->parent != SCUI_HANDLE_INVALID)
        widget->style.state = true;
    if (widget->parent == SCUI_HANDLE_INVALID)
        widget->style.state = false;
}

/*@brief 控件销毁
 *@param widget     控件实例
 *@param parent_way 来自父控件的销毁
 */
void scui_widget_destroy(scui_widget_t *widget, bool parent_way)
{
    /* 向父控件的孩子列表移除自己 */
    if (widget->parent != SCUI_HANDLE_INVALID && !parent_way)
        scui_widget_child_del(widget->myself, widget->parent);
    
    SCUI_MEM_FREE(widget->child_list);
    widget->child_list = NULL;
    
    scui_widget_event_clear(widget->myself);
}

/*@brief 控件透明度获取
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
scui_alpha_t scui_widget_alpha_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    return widget->surface.alpha;
}

/*@brief 控件透明度设置
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    widget->surface.alpha = alpha;
    
    /* 必须递归设置控件透明度,迭代它的孩子列表 */
    for (uint32_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID)
            scui_widget_alpha_set(widget->child_list[idx], alpha);
}

/*@brief 控件图片获取
 *@param handle 控件句柄
 *@retval 图片句柄
 */
scui_handle_t scui_widget_image_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    return widget->image;
}

/*@brief 控件图片设置
 *@param handle 控件句柄
 *@param image  图片句柄
 */
void scui_widget_image_set(scui_handle_t handle, scui_handle_t image)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    widget->image = image;
}

/*@brief 控件颜色获取
 *@param handle 控件句柄
 *@retval 颜色
 */
scui_color_gradient_t scui_widget_color_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    return widget->color;
}

/*@brief 控件颜色设置
 *@param handle 控件句柄
 *@param color  颜色
 */
void scui_widget_color_set(scui_handle_t handle, scui_color_gradient_t color)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    widget->color = color;
}

/*@brief 控件清除剪切域
 *@param widget 控件实例
 */
void scui_widget_clip_clear(scui_widget_t *widget)
{
    widget->surface_clip = (scui_area_t){0};
    
    for (uint32_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = widget->child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            scui_widget_clip_clear(child);
        }
}

/*@brief 控件还原剪切域
 *@param widget 控件实例
 */
void scui_widget_clip_reset(scui_widget_t *widget)
{
    widget->surface_clip = widget->clip;
    
    for (uint32_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = widget->child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            scui_widget_clip_reset(child);
        }
}

/*@brief 控件添加子控件
 *@param handle 控件实例
 *@param child  控件子控件
 */
void scui_widget_child_add(scui_handle_t handle, scui_handle_t child)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    
    for (uint32_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] == SCUI_HANDLE_INVALID) {
            widget->child_list[idx]  = child;
            return;
        }
    APP_SYS_LOG_ERROR("child add fail:%u", child);
}

/*@brief 控件移除子控件
 *@param handle 控件实例
 *@param child  控件子控件
 */
void scui_widget_child_del(scui_handle_t handle, scui_handle_t child)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    
    for (uint32_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] == child) {
            widget->child_list[idx]  = SCUI_HANDLE_INVALID;
            return;
        }
    APP_SYS_LOG_ERROR("child del fail:%u", child);
}

/*@brief 事件吸收
 */
static bool scui_widget_draw_absorb(void *evt_old, void *evt_new)
{
    /* 什么都不需要做 */
    return true;
}

/*@brief 绘制控件
 *@param handle 控件句柄
 *@param sync   同步绘制
 */
void scui_widget_draw(scui_handle_t handle, bool sync)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    
    /* 只为当前控件及其子控件添加剪切域,但是绘制是从窗口开始的 */
    scui_widget_clip_reset(widget);
    
    scui_event_t event = {
        .type   = scui_event_draw,
        .object = scui_widget_root(widget->myself),
        .absorb = scui_widget_draw_absorb,
    };
    if (sync)
        event.style = scui_event_style_sync;
    scui_event_notify(&event);
}

/*@brief 事件吸收
 */
static bool scui_widget_refr_absorb(void *evt_old, void *evt_new)
{
    /* 什么都不需要做 */
    return true;
}

/*@brief 刷新控件
 *@param handle 控件句柄
 *@param sync   同步刷新
 */
void scui_widget_refr(scui_handle_t handle, bool sync)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    
    scui_event_t event = {
        .type   = scui_event_refr,
        .object = scui_widget_root(widget->myself),
        .absorb = scui_widget_refr_absorb,
    };
    if (sync)
        event.style = scui_event_style_sync;
    scui_event_notify(&event);
}

/*@brief 控件树的根控件
 *@param handle 控件句柄
 *@retval 根控件句柄
 */
scui_handle_t scui_widget_root(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    if (widget->parent != SCUI_HANDLE_INVALID)
        return scui_widget_root(widget->parent);
    return handle;
}

/*@brief 控件移动到目标坐标
 *@param handle 控件句柄
 */
void scui_widget_move(scui_handle_t handle, scui_point_t *point)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    
    if (widget->clip.x == point->x && widget->clip.y == point->y)
        return;
    
    /* 如果移动的是窗口,只需要改变窗口位置 */
    if (widget->parent == SCUI_HANDLE_INVALID) {
        widget->clip.x = point->x;
        widget->clip.y = point->y;
        widget->surface.clip.x = point->x;
        widget->surface.clip.y = point->y;
        /* 移动要清除剪切域 */
        widget->surface_clip = (scui_area_t){0};
        return;
    }
    
    /* 计算移动偏移量 */
    scui_point_t offset = {
        .x = point->x - widget->clip.x,
        .y = point->y - widget->clip.y,
    };
    
    /* 移动自己 */
    widget->clip.x = point->x;
    widget->clip.y = point->y;
    widget->surface.clip.x = point->x;
    widget->surface.clip.y = point->y;
    
    /* 移动孩子,迭代它的孩子列表 */
    for (uint32_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = widget->child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            scui_point_t point = {
                .x = offset.x + child->clip.x,
                .y = offset.y + child->clip.y,
            };
            scui_widget_move(handle, &point);
        }
    
    /* 重新追加剪切域 */
    if (!scui_area_empty(&widget->surface_clip)) {
         widget->surface_clip = (scui_area_t){0};
         scui_widget_clip_reset(widget);
    }
}

/*@brief 控件显示状态获取
 *@param handle 控件句柄
 *@retval 是否显示
 */
bool scui_widget_style_is_show(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    
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