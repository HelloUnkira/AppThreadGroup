/*实现目标:
 *    控件(布局)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 子控件登记项定位
 *@param layout 布局控件实例
 *@param handle 子控件句柄
 *@retval 登记项
 */
static scui_layout_node_t *scui_layout_node_claim(scui_layout_t *layout, scui_handle_t handle)
{
    if (layout->list == NULL)
        return NULL;
    
    for (scui_handle_t idx = 0; idx < layout->num; idx++) {
        scui_layout_node_t *node = &layout->list[idx];
        if (node->use && node->handle == handle) return node;
        if (node->use) continue;
        
        node->use    = true;
        node->handle = handle;
        return node;
    }
    
    return NULL;
}

/*@brief 子控件登记项查找(只查)
 *@param layout 布局控件实例
 *@param handle 子控件句柄
 *@retval 登记项
 */
static scui_layout_node_t *scui_layout_node_find(scui_layout_t *layout, scui_handle_t handle)
{
    if (layout->list == NULL)
        return NULL;
    
    for (scui_handle_t idx = 0; idx < layout->num; idx++) {
        scui_layout_node_t *node = &layout->list[idx];
        if (node->use && node->handle == handle)
            return node;
    }
    
    return NULL;
}

/*@brief auto布局执行
 *@param layout 布局控件实例
 */
static void scui_layout_auto_exec(scui_layout_t *layout)
{
    scui_widget_t *widget = &layout->widget;
    scui_handle_t  handle =  widget->myself;
    scui_coord_t wide = 0;
    scui_coord_t tall = 0;
    
    /* 阶段1: 统计子控件最大宽高 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t   handle_c = widget->child_list[idx];
        scui_widget_t  *widget_c = scui_handle_source_check(handle_c);
        scui_layout_node_t *node = scui_layout_node_find(layout, handle_c);
        
        scui_coord_t wide_c = widget_c->clip.w;
        scui_coord_t tall_c = widget_c->clip.h;
        if (node == NULL && widget_c->clip.x > widget->clip.x)
            wide_c += widget_c->clip.x - widget->clip.x;
        if (node == NULL && widget_c->clip.y > widget->clip.y)
            tall_c += widget_c->clip.y - widget->clip.y;
        
        if (wide_c > wide) wide = wide_c;
        if (tall_c > tall) tall = tall_c;
    }
    
    /* 阶段2: 更新自动尺寸(尺寸确定后align才有正确参考) */
    scui_coord_t width  = widget->state.layout_w ? wide : widget->clip.w;
    scui_coord_t height = widget->state.layout_h ? tall : widget->clip.h;
    if (width != widget->clip.w || height != widget->clip.h)
        scui_widget_adjust_size(handle, width, height);
    
    /* 阶段3: 父尺寸已确定, 再执行登记node对齐 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle_c = widget->child_list[idx];
        scui_layout_node_t *node = scui_layout_node_find(layout, handle_c);
        if (node == NULL) continue;
        
        scui_widget_align_pos(handle_c, node->handle_t, node->align, &node->offset);
    }
}

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_layout_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_layout_t *layout = widget;
    scui_layout_maker_t *layout_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_layout));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    layout->type = layout_maker->type;
    layout->num  = widget->child_num;
    
    if (layout->num > 0) {
        scui_multi_t size = layout->num * sizeof(scui_layout_node_t);
        layout->list = SCUI_MEM_ZALLOC(scui_mem_type_mix, size);
    }
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_layout_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_layout));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_layout_t *layout = (void *)widget;
    
    /* 回收布局资源 */
    SCUI_MEM_FREE(layout->list);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 布局控件子控件对齐
 *@param handle   布局控件句柄
 *@param handle_t 对齐目标控件(为空相对父)
 *@param handle_c 子控件句柄
 *@param align    对齐
 *@param offset   偏移
 */
void scui_layout_align(scui_handle_t handle, scui_handle_t handle_t,
    scui_handle_t handle_c, scui_align_t align, scui_point_t *offset)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_layout));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_layout_t *layout = (void *)widget;
    
    /* 对齐目标为空则相对父 */
    if (handle_t == SCUI_HANDLE_INVALID) handle_t = handle;
    SCUI_ASSERT(handle_t == handle || scui_widget_parent(handle_t) == handle);
    SCUI_ASSERT(scui_widget_parent(handle_c) == handle);
    
    scui_layout_node_t *node = scui_layout_node_claim(layout, handle_c);
    if (node == NULL) return;
    
    node->handle_t = handle_t;
    node->align    = align;
    if (offset != NULL) node->offset = *offset;
    
    /* 标记布局更新 */
    scui_widget_layout_refr(handle);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_layout_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_layout_t *layout = (void *)widget;
    
    switch (event->type) {
    case scui_event_layout: {
        /* 本控件没有子控件 */
        if (widget->child_num == 0)
            return;
        
        /* 当前仅auto布局 */
        scui_layout_auto_exec(layout);
        break;
    }
    default:
        break;
    }
}