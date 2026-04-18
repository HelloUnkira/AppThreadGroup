/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_canvas_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_canvas_t *canvas = widget;
    scui_canvas_maker_t *canvas_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_canvas));
    SCUI_ASSERT(widget_maker->parent == SCUI_HANDLE_INVALID);
    /* 注意:要求只能是根控件才可以创建画布 */
    
    #if SCUI_MEM_FEAT_MINI
    /* 小内存方案禁用画布 */
    SCUI_ASSERT(false);
    #endif
    
    canvas->format = canvas_maker->format;
    if (canvas->format == scui_pixel_cf_def)
        canvas->format  = SCUI_PIXEL_CF_DEF_A;
    
    /* 创建surface */
    scui_surface_t surface = {
        .format  = canvas->format,
        .hor_res = widget->clip.w,
        .ver_res = widget->clip.h,
    };
    
    scui_widget_clip_clear(widget, true);
    scui_widget_surface_create(widget->myself, &surface);
    scui_widget_surface_refr(widget->myself, true);
    
    /* 配置画布图资源 */
    canvas->image_src.type           = scui_image_type_mem;
    canvas->image_src.format         = widget->surface->format;
    canvas->image_src.pixel.width    = widget->surface->hor_res;
    canvas->image_src.pixel.height   = widget->surface->ver_res;
    canvas->image_src.pixel.data_bin = widget->surface->pixel;
    
    /* 让画布资源绑定到句柄 */
    canvas->image = scui_handle_find();
    scui_handle_linker(canvas->image, &canvas->image_src);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_canvas_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_canvas));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_canvas_t *canvas = (void *)widget;
    
    /* 回收surface */
    scui_widget_surface_destroy(widget->myself);
    
    /* 回收画布图句柄 */
    scui_handle_clear(canvas->image);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_canvas_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_canvas_t *canvas = (void *)widget;
    
    /* empty */
}

/*@brief 画布控件图
 *@param handle 画布控件句柄
 *@param image  画布图资源
 */
void scui_canvas_image(scui_handle_t handle, scui_handle_t *image)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_canvas));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_canvas_t *canvas = (void *)widget;
    
    *image = canvas->image;
}

/*@brief 画布控件取控件树快照
 *@param handle   画布控件句柄
 *@param handle_t 目标控件树句柄
 */
void scui_canvas_snapshot(scui_handle_t handle, scui_handle_t handle_t)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_canvas));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_canvas_t *canvas = (void *)widget;
    /* 需要扩展剪切区域参数? */
    
    /* 处理一下控件树 */
    scui_widget_t   *widget_t = scui_handle_source_check(handle_t);
    scui_surface_t *surface_t = widget_t->surface;
    scui_handle_t    handle_p = widget_t->parent;
    scui_point_t     offset_t = {0};
    scui_area_t        clip_t = widget_t->clip;
    /* 重调子控件树的相对位置 */
    offset_t.x = -clip_t.x;
    offset_t.y = -clip_t.y;
    scui_widget_clist_move_ofs(widget_t->myself, &offset_t);
    
    /* 重调控件树信息 */
    widget_t->clip.x = 0;
    widget_t->clip.y = 0;
    widget_t->parent = widget->myself;
    /* 重调子控件树的相对位置 */
    offset_t.x = widget_t->clip.x;
    offset_t.y = widget_t->clip.y;
    scui_widget_clist_move_ofs(widget_t->myself, &offset_t);
    /* 重映射资源到画布 */
    scui_widget_clip_clear(widget_t, true);
    scui_widget_surface_remap(widget_t->myself, widget->surface);
    scui_widget_surface_refr(widget_t->myself, true);
    /* 控件树绘制到画布 */
    scui_widget_draw(widget_t->myself, NULL, true);
    
    /* 还原子控件树的相对位置 */
    offset_t.x = +clip_t.x;
    offset_t.y = +clip_t.y;
    scui_widget_clist_move_ofs(widget_t->myself, &offset_t);
    /* 还原控件树信息 */
    widget_t->clip.x = clip_t.x;
    widget_t->clip.y = clip_t.y;
    widget_t->parent = handle_p;
    /* 还原重映射资源 */
    scui_widget_clip_clear(widget_t, true);
    scui_widget_surface_remap(widget_t->myself, surface_t);
    scui_widget_surface_refr(widget_t->myself, true);
}
