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
    
    widget->surface_clip  = (scui_area_t){.x = widget->clip.x, .y = widget->clip.y,};
    widget->surface.clip  = (scui_area_t){.w = widget->clip.w, .h = widget->clip.h,};
    widget->surface.alpha = scui_alpha_cover;
    /* 画布的坐标区域是相对父控件 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = NULL;
        scui_handle_t  handle_parent = widget->parent;
        widget_parent = scui_handle_get(handle_parent);
        handle_parent = widget_parent->parent;
        widget->surface.clip.x += widget_parent->surface.clip.x;
        widget->surface.clip.y += widget_parent->surface.clip.y;
    }
    
    widget->alpha = maker->alpha;
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






/*@brief 控件列表为指定子控件(及其覆盖它的子控件)追加剪切域
 *@param widget     控件实例
 *@param clip       剪切域
 *@param widget_sub 子控件(如果存在则以该控件为锚点迭代)
 */
void scui_widget_clip_merge(scui_widget_t *widget, scui_area_t *clip, scui_widget_t *widget_sub)
{
    if (widget->child_list == NULL) {
        scui_widget_surface_clip_merge(widget, clip);
        return;
    }
    
    uint32_t ofs = 0;
    
    if (widget_sub == NULL)
        scui_widget_surface_clip_merge(widget, clip);
    if (widget_sub != NULL)
        for (ofs = 0; ofs < widget->child_num; ofs++)
            if (widget->child_list[ofs] != SCUI_HANDLE_INVALID) {
                scui_handle_t handle = widget->child_list[ofs];
                scui_widget_t *child = scui_handle_get(handle);
                if (child != widget_sub)
                    break;
            }
    
    scui_area_t clip_child = {0};
    scui_area_t clip_child_in = {.w = clip->w, .h = clip->h};
    
    /* 为区域中所有孩子控件添加剪切域 */
    for (ofs = ofs; ofs < widget->child_num; ofs++) {
        if (widget->child_list[ofs] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t handle = widget->child_list[ofs];
        scui_widget_t *child = scui_handle_get(handle);
        /* 不显示的子控件不添加剪切域 */
        if (!scui_widget_style_is_show(handle))
             continue;
        /* 换算成子控件相对坐标 */
        clip_child.w = child->clip.w;
        clip_child.h = child->clip.h;
        clip_child_in.x = clip->x - child->clip.x;
        clip_child_in.y = clip->y - child->clip.y;
        /* 是否为根控件,根控件对齐虚拟0坐标,其他控件相对父控件对齐 */
        if (widget->parent  != SCUI_HANDLE_INVALID) {
            clip_child_in.x += widget->clip.x;
            clip_child_in.y += widget->clip.y;
        }
        /* 计算子控件与父容器交集 */
        scui_area_t clip_child_out = {0};
        if (!scui_area_inter(&clip_child, &clip_child_in, &clip_child_out))
             scui_widget_surface_clip_merge(child, &clip_child_out);
    }
}

/*@brief 控件清除剪切域
 *@param widget 控件实例
 */
void scui_widget_clip_clear(scui_widget_t *widget)
{
    scui_widget_surface_clip_clear(widget);
    
    for (uint32_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = widget->child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            scui_widget_clip_clear(child);
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

/*@brief 控件透明度获取
 *@param handle 控件句柄
 *@param alpha  控件透明度
 */
scui_alpha_t scui_widget_alpha_get(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    return widget->alpha;
}

/*@brief 控件透明度设置
 *@param handle    控件句柄
 *@param alpha     控件透明度
 *@param recursion 递归
 */
void scui_widget_alpha_set(scui_handle_t handle, scui_alpha_t alpha, bool recursion)
{
    scui_widget_t *widget = scui_handle_get(handle);
    APP_SYS_ASSERT(widget != NULL);
    widget->surface.alpha = alpha;
    widget->alpha = alpha;
    if (!recursion)
        return;
    /* 如果需要递归设置控件透明度,迭代它的孩子列表 */
    for (uint32_t idx = 0; idx < widget->child_num; idx++)
        if (widget->child_list[idx] != SCUI_HANDLE_INVALID)
            scui_widget_alpha_set(widget->child_list[idx], alpha, recursion);
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
