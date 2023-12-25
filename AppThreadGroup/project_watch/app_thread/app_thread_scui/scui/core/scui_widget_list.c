/*实现目标:
 *    控件列表
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 控件列表初始化
 *@param widget    控件实例
 *@param child_num 控件子控件数量
 */
void scui_widget_list_init(scui_widget_t *widget, uint32_t child_num)
{
    if (child_num == 0) {
        APP_SYS_ASSERT(widget->list.child_num  == 0);
        APP_SYS_ASSERT(widget->list.child_list == NULL);
        return;
    }
    
    widget->list.child_num  = child_num;
    widget->list.child_list = SCUI_MEM_ALLOC(scui_mem_is_part, sizeof(scui_handle_t) * child_num);
    for (uint32_t idx = 0; idx < widget->list.child_num; idx++)
        widget->list.child_list[idx] = SCUI_HANDLE_INVALID;
}

/*@brief 控件列表反初始化
 *@param widget 控件实例
 */
void scui_widget_list_deinit(scui_widget_t *widget)
{
    if (widget->list.child_list != NULL) {
        for (uint32_t idx = 0; idx < widget->list.child_num; idx++)
            if (widget->list.child_list[idx] != SCUI_HANDLE_INVALID) {
                #if 0
                scui_widget_ogz_destroy(handle, true);
                #endif
            }
        
        for (uint32_t idx = 0; idx < widget->list.child_num; idx++)
            if (widget->list.child_list[idx] != SCUI_HANDLE_INVALID) {
                APP_SYS_LOG_ERROR("child still exist now");
                return;
            }
    }
    SCUI_MEM_FREE(widget->list.child_list);
    widget->list.child_list = NULL;
}

/*@brief 控件列表添加子控件
 *@param widget 控件实例
 *@param handle 控件子控件
 */
void scui_widget_list_child_add(scui_widget_t *widget, scui_handle_t handle)
{
    for (uint32_t idx = 0; idx < widget->list.child_num; idx++)
        if (widget->list.child_list[idx] == SCUI_HANDLE_INVALID) {
            widget->list.child_list[idx]  = handle;
            return;
        }
    APP_SYS_LOG_ERROR("child add fail:%u", handle);
}

/*@brief 控件列表移除子控件
 *@param widget 控件实例
 *@param handle 控件子控件
 */
void scui_widget_list_child_del(scui_widget_t *widget, scui_handle_t handle)
{
    for (uint32_t idx = 0; idx < widget->list.child_num; idx++)
        if (widget->list.child_list[idx] == handle) {
            widget->list.child_list[idx]  = SCUI_HANDLE_INVALID;
            return;
        }
    APP_SYS_LOG_ERROR("child del fail:%u", handle);
}

/*@brief 控件列表清除剪切域
 *@param widget 控件实例
 */
void scui_widget_list_clip_clear(scui_widget_t *widget)
{
    scui_widget_gc_clip_clear(widget);
    if (widget->list.child_list == NULL) {
        APP_SYS_ASSERT(widget->list.child_num == 0);
        return;
    }
    
    for (uint32_t idx = 0; idx < widget->list.child_num; idx++)
        if (widget->list.child_list[idx] != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = widget->list.child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            scui_widget_list_clip_clear(child);
        }
}

/*@brief 控件列表为指定子控件(及其覆盖它的子控件)追加剪切域
 *@param widget     控件实例
 *@param clip       剪切域
 *@param widget_sub 子控件(如果存在则以该控件为锚点迭代)
 */
void scui_widget_list_clip_merge(scui_widget_t *widget, scui_area_t *clip, scui_widget_t *widget_sub)
{
    if (widget->list.child_list == NULL) {
        APP_SYS_ASSERT(widget->list.child_num == 0);
        scui_widget_gc_clip_merge(widget, clip);
        return;
    }
    
    uint32_t ofs = 0;
    
    if (widget_sub == NULL)
        scui_widget_gc_clip_merge(widget, clip);
    if (widget_sub != NULL)
        for (ofs = 0; ofs < widget->list.child_num; ofs++)
            if (widget->list.child_list[ofs] != SCUI_HANDLE_INVALID) {
                scui_handle_t handle = widget->list.child_list[ofs];
                scui_widget_t *child = scui_handle_get(handle);
                if (child != widget_sub)
                    break;
            }
    
    scui_area_t clip_child = {0};
    scui_area_t clip_child_in = {.w = clip->w, .h = clip->h};
    
    /* 为区域中所有孩子控件添加剪切域 */
    for (ofs = ofs; ofs < widget->list.child_num; ofs++) {
        if (widget->list.child_list[ofs] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t handle = widget->list.child_list[ofs];
        scui_widget_t *child = scui_handle_get(handle);
        /* 不显示的子控件不添加剪切域 */
        if (!scui_widget_status_is_show(handle))
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
             scui_widget_gc_clip_merge(child, &clip_child_out);
    }
}

/*@brief 控件列表更新画布alpha
 *@param widget 控件实例
 *@param alpha  画布alpha
 */
void scui_widget_list_alpha_update(scui_widget_t *widget, uint8_t alpha)
{
    scui_widget_gc_alpha_update(widget, alpha);
    if (widget->list.child_list == NULL) {
        APP_SYS_ASSERT(widget->list.child_num == 0);
        return;
    }
    
    for (uint32_t idx = 0; idx < widget->list.child_num; idx++)
        if (widget->list.child_list[idx] != SCUI_HANDLE_INVALID) {
            scui_handle_t handle = widget->list.child_list[idx];
            scui_widget_t *child = scui_handle_get(handle);
            scui_widget_list_alpha_update(child, alpha);
        }
}



