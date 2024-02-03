/*实现目标:
 *    控件
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
    SCUI_ASSERT(widget != NULL && maker != NULL && handle != NULL && *handle != SCUI_HANDLE_INVALID);
    SCUI_ASSERT(maker->clip.w != 0);
    SCUI_ASSERT(maker->clip.h != 0);
    
    /* 非布局则句柄为动态创建 */
    *handle = layout ? maker->myself : scui_handle_new();
    
    SCUI_LOG_DEBUG("");
    /* 为句柄设置映射 */
    scui_handle_set(*handle, widget);
    
    /* 控件使用maker构造 */
    if (maker->parent != SCUI_HANDLE_INVALID)
        SCUI_ASSERT(scui_handle_remap(maker->parent));
    
    scui_list_dll_reset(&widget->dl_list);
    
    SCUI_LOG_DEBUG("");
    widget->type   = maker->type;
    widget->style  = maker->style;
    widget->clip   = maker->clip;
    widget->parent = maker->parent;
    widget->myself = *handle;
    
    SCUI_LOG_DEBUG("");
    /* 子控件的坐标区域是相对父控件 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = scui_handle_get(widget->parent);
        widget->clip.x += widget_parent->clip.x;
        widget->clip.y += widget_parent->clip.y;
    }
    
    SCUI_LOG_DEBUG("");
    /* 构建孩子列表 */
    widget->child_num = maker->child_num;
    widget->child_list = NULL;
    if (widget->child_num != 0) {
        uint32_t child_list_size = widget->child_num * sizeof(scui_handle_t);
        widget->child_list = SCUI_MEM_ALLOC(scui_mem_is_part, child_list_size);
        for (scui_handle_t idx = 0; idx < widget->child_num; idx++)
            widget->child_list[idx] = SCUI_HANDLE_INVALID;
    }
    
    SCUI_LOG_DEBUG("");
    scui_widget_t *widget_root = NULL;
    scui_handle_t  handle_root = scui_widget_root(widget->myself);
    widget_root = scui_handle_get(handle_root);
    
    /* 画布的映射是根控件, 控件画布默认为完全不透明 */
    widget->surface.pixel = widget_root->surface.pixel;
    widget->surface.alpha = scui_alpha_cover;
    widget->surface_clip  = (scui_area_t){0};
    widget->surface.clip  = widget->clip;
    
    SCUI_LOG_DEBUG("");
    /* 画布的坐标区域是相对父控件 */
    if (widget->parent != SCUI_HANDLE_INVALID) {
        scui_widget_t *widget_parent = scui_handle_get(widget->parent);
        widget->surface.line = widget_parent->surface.line;
        /* 子控件的坐标区域是父控件坐标区域的子集 */
        scui_area_t clip_merge = {0};
        scui_area_inter(&clip_merge, &widget->surface.clip, &widget_parent->surface.clip);
        widget->surface.clip = clip_merge;
    }
    
    widget->image = maker->image;
    widget->color = maker->color;
    
    SCUI_LOG_DEBUG("");
    /* 向父控件的孩子列表添加自己 */
    if (widget->parent != SCUI_HANDLE_INVALID)
        scui_widget_child_add(widget->parent, widget->myself);
    
    SCUI_LOG_DEBUG("");
    /* 非根控件要设置为显示,否则为隐藏 */
    if (widget->parent != SCUI_HANDLE_INVALID)
        widget->style.state = true;
    if (widget->parent == SCUI_HANDLE_INVALID)
        widget->style.state = false;
    
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
    if (!scui_handle_remap(widget->myself)) {
         SCUI_LOG_INFO("widget %u not load", widget->myself);
         return;
    }
    SCUI_LOG_INFO("widget %u", widget->myself);
    /* 先递归销毁自己的孩子列表 */
    for (scui_handle_t idx = 0; idx < widget->child_num; idx++) {
        if (widget->child_list[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_widget_cb_destroy(widget->child_list[idx]);
    }
    /* 再从父控件的孩子列表移除自己 */
    if (widget->parent != SCUI_HANDLE_INVALID)
        scui_widget_child_del(widget->parent, widget->myself);
    /* 最后回收资源 */
    SCUI_MEM_FREE(widget->child_list);
    widget->child_list = NULL;
    /* 清空事件列表 */
    scui_widget_event_clear(widget->myself);
}
