/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 图像控件资源重置
 *@param ximage 图像控件实例
 */
static void scui_ximage_reset(scui_ximage_t *ximage)
{
    switch (ximage->type) {
    case scui_ximage_type_vedio:
        scui_vedio_burn(&ximage->data.vedio.frame);
        break;
    case scui_ximage_type_qrcode:
        scui_handle_clear(ximage->data.qrcode.image);
        scui_image_burn(&ximage->data.qrcode.image_src);
        break;
    case scui_ximage_type_barcode:
        scui_handle_clear(ximage->data.barcode.image);
        scui_image_burn(&ximage->data.barcode.image_src);
        break;
    default:
        break;
    }
    
    ximage->type = scui_ximage_type_none;
    ximage->data = (scui_ximage_data_t){0};
}

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_ximage_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_ximage_t *ximage = widget;
    
    /* 必须标记anima事件 */
    widget_maker->style.sched_anima = true;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_ximage));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_ximage_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_ximage));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_ximage_t *ximage = (void *)widget;
    
    /* 回收现有资源 */
    scui_ximage_reset(ximage);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 图像控件图像帧播放
 *@param handle 图像控件句柄
 *@param image  图像帧句柄(gif/lottie)
 *@param speed  播放速度(SCUI_SCALE_COF:正常速度)
 *@param loop   播放次数(-1:无限)
 */
void scui_ximage_vedio_play(scui_handle_t handle, scui_handle_t image,
    scui_multi_t speed, scui_multi_t loop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_ximage));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_ximage_t *ximage = (void *)widget;
    
    scui_ximage_reset(ximage);
    
    ximage->type = scui_ximage_type_vedio;
    ximage->data.vedio.frame     = (scui_vedio_t){0};
    ximage->data.vedio.speed     = speed;
    ximage->data.vedio.tick      = 0;
    ximage->data.vedio.loop      = loop;
    ximage->data.vedio.work      = true;
    
    scui_image_t *image_src = scui_handle_source_check(image);
    scui_vedio_t *frame = &ximage->data.vedio.frame;
    frame->type   = image_src->type;
    frame->handle = image;
    switch (frame->type) {
    case scui_image_type_gif:
    case scui_image_type_lottie:
        break;
    default:
        SCUI_ASSERT(false);
        break;
    }
    
    /* 图像帧数据创建 */
    scui_vedio_make(frame);
    scui_vedio_data(frame);
    
    scui_widget_draw(handle, NULL, false, 0);
}

/*@brief 图像控件图像帧播放控制
 *@param handle 图像控件句柄
 *@param work   播放:1;停止:0;
 */
void scui_ximage_vedio_work(scui_handle_t handle, bool work)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_ximage));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_ximage_t *ximage = (void *)widget;
    
    SCUI_ASSERT(ximage->type == scui_ximage_type_vedio);
    ximage->data.vedio.work = work;
}

/*@brief 图像控件二维码
 *@param handle 图像控件句柄
 *@param data   url字符串
 *@param size   url字符串长度
 *@param color  图像颜色
 *@param scale  缩放系数(SCUI_SCALE_COF为原尺寸)
 */
void scui_ximage_qrcode(scui_handle_t handle, uint8_t *data, uint32_t size,
    scui_color_t color, scui_multi_t scale)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_ximage));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_ximage_t *ximage = (void *)widget;
    
    scui_ximage_reset(ximage);
    
    ximage->type = scui_ximage_type_qrcode;
    ximage->data.qrcode.color = color;
    ximage->data.qrcode.data  = data;
    ximage->data.qrcode.size  = size;
    
    ximage->data.qrcode.image_src = (scui_image_t){0};
    ximage->data.qrcode.image_src.format = scui_pixel_cf_alpha1;
    scui_image_make(&ximage->data.qrcode.image_src, &widget->clip);
    
    ximage->data.qrcode.image = scui_handle_find();
    scui_handle_linker(ximage->data.qrcode.image,
        &ximage->data.qrcode.image_src);
    
    /* 注入即生成 */
    scui_image_qrcode(&ximage->data.qrcode.image_src, data, size, scale);
    scui_widget_draw(handle, NULL, false, 0);
}

/*@brief 图像控件条形码
 *@param handle 图像控件句柄
 *@param data   url字符串
 *@param size   url字符串长度
 *@param color  图像颜色
 *@param scale  缩放系数(SCUI_SCALE_COF为原尺寸)
 */
void scui_ximage_barcode(scui_handle_t handle, uint8_t *data, uint32_t size,
    scui_color_t color, scui_multi_t scale)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_ximage));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_ximage_t *ximage = (void *)widget;
    
    scui_ximage_reset(ximage);
    
    ximage->type = scui_ximage_type_barcode;
    ximage->data.barcode.color = color;
    ximage->data.barcode.data  = data;
    ximage->data.barcode.size  = size;
    
    ximage->data.barcode.image_src = (scui_image_t){0};
    ximage->data.barcode.image_src.format = scui_pixel_cf_alpha1;
    scui_image_make(&ximage->data.barcode.image_src, &widget->clip);
    
    ximage->data.barcode.image = scui_handle_find();
    scui_handle_linker(ximage->data.barcode.image,
        &ximage->data.barcode.image_src);
    
    /* 注入即生成 */
    scui_image_barcode(&ximage->data.barcode.image_src, data, size, scale);
    scui_widget_draw(handle, NULL, false, 0);
}

/*@brief 图像控件图像序列陈列
 *@param handle 图像控件句柄
 *@param list   图像句柄列表
 *@param num    图像数量
 *@param span   图像间隙
 *@param way    方向(0:水平方向;1:垂直方向)
 */
void scui_ximage_sequence(scui_handle_t handle, scui_handle_t *list,
    scui_coord_t num, scui_coord_t span, bool way)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_ximage));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_ximage_t *ximage = (void *)widget;
    
    scui_ximage_reset(ximage);
    
    ximage->type = scui_ximage_type_sequence;
    ximage->data.sequence.image = list;
    ximage->data.sequence.num   = num;
    ximage->data.sequence.span  = span;
    ximage->data.sequence.way   = way;
    
    scui_widget_draw(handle, NULL, false, 0);
}

/*@brief 图像控件图像帧替换播放
 *@param handle 图像控件句柄
 *@param list   图像句柄列表
 *@param num    图像数量
 *@param speed  播放速度(SCUI_SCALE_COF:正常速度)
 *@param loop   播放次数(-1:无限)
 */
void scui_ximage_replace_play(scui_handle_t handle, scui_handle_t *list,
    scui_coord_t num, scui_multi_t speed, scui_multi_t loop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_ximage));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_ximage_t *ximage = (void *)widget;
    
    scui_ximage_reset(ximage);
    
    ximage->type = scui_ximage_type_replace;
    ximage->data.replace.image = list;
    ximage->data.replace.num   = num;
    ximage->data.replace.curr  = 0;
    ximage->data.replace.speed = speed;
    ximage->data.replace.tick  = 0;
    ximage->data.replace.loop  = loop;
    ximage->data.replace.work  = true;
    
    scui_widget_draw(handle, NULL, false, 0);
}

/*@brief 图像控件图像帧替换播放控制
 *@param handle 图像控件句柄
 *@param work   播放:1;停止:0;
 */
void scui_ximage_replace_work(scui_handle_t handle, bool work)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_ximage));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_ximage_t *ximage = (void *)widget;
    
    SCUI_ASSERT(ximage->type == scui_ximage_type_replace);
    ximage->data.replace.work = work;
    ximage->data.replace.curr = 0;
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_ximage_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_ximage_t *ximage = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        switch (ximage->type) {
        case scui_ximage_type_vedio: {
            if (!ximage->data.vedio.work)
                 break;
            
            /* 播放速度节流 */
            ximage->data.vedio.tick += SCUI_SCALE_COF;
            if (ximage->data.vedio.tick < ximage->data.vedio.speed) break;
            ximage->data.vedio.tick -= ximage->data.vedio.speed;
            
            /* 固定接口一步步向后切 */
            scui_widget_draw(widget->myself, NULL, false, 0);
            if (scui_vedio_data(&ximage->data.vedio.frame)) {
                if (ximage->data.vedio.loop > 0)
                    ximage->data.vedio.loop--;
                if (ximage->data.vedio.loop == 0)
                    ximage->data.vedio.work  = false;
                    break;
            }
            break;
        }
        case scui_ximage_type_replace: {
            if (!ximage->data.replace.work)
                 break;
            
            /* 播放速度节流: 按speed累积步进 */
            ximage->data.replace.tick += SCUI_SCALE_COF;
            if (ximage->data.replace.tick < ximage->data.replace.speed) break;
            ximage->data.replace.tick -= ximage->data.replace.speed;
            
            /* 固定接口一步步向后切 */
            scui_coord_t curr = ximage->data.replace.curr + 1;
            if (curr < ximage->data.replace.num) {
                ximage->data.replace.curr = curr;
                break;
            }
            
            /* 一轮播放结束, 更迭循环 */
            ximage->data.replace.curr = 0;
            if (ximage->data.replace.loop > 0)
                ximage->data.replace.loop--;
            if (ximage->data.replace.loop == 0)
                ximage->data.replace.work  = false;
            break;
        }
        default:
            break;
        }
        
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    }
    case scui_event_draw_graph: {
        switch (ximage->type) {
        case scui_ximage_type_vedio: {
            scui_handle_t frame = ximage->data.vedio.frame.frame;
            SCUI_ASSERT(ximage->data.vedio.frame.frame != SCUI_HANDLE_INVALID);
            scui_widget_draw_image(widget->myself, NULL, frame, NULL, SCUI_COLOR_FILTER_TRANS);
            break;
        }
        case scui_ximage_type_qrcode: {
            scui_handle_t image = ximage->data.qrcode.image;
            scui_color_t  color = ximage->data.qrcode.color;
            scui_widget_draw_image(widget->myself, NULL, image, NULL, color);
            break;
        }
        case scui_ximage_type_barcode: {
            scui_handle_t image = ximage->data.barcode.image;
            scui_color_t  color = ximage->data.barcode.color;
            scui_widget_draw_image(widget->myself, NULL, image, NULL, color);
            break;
        }
        case scui_ximage_type_sequence: {
            scui_handle_t *image_list = ximage->data.sequence.image;
            scui_color_t   color      = SCUI_COLOR_FILTER_TRANS;
            scui_coord_t   span       = ximage->data.sequence.span;
            scui_coord_t   num        = ximage->data.sequence.num;
            bool           way        = ximage->data.sequence.way;
            
            scui_area_t widget_clip = scui_widget_clip(widget->myself);
            widget_clip.x = widget_clip.y = 0;
            
            scui_point_t offset = {0};
            for (scui_coord_t idx = 0; idx < num; idx++) {
                if (image_list[idx] == SCUI_HANDLE_INVALID)
                    continue;
                
                scui_area_t dst_clip = widget_clip;
                if (scui_area_limit_offset(&dst_clip, &offset))
                    scui_widget_draw_image(widget->myself, &dst_clip, image_list[idx], NULL, color);
                
                if (way) offset.y += span + scui_image_h(image_list[idx]);
                else offset.x += span + scui_image_w(image_list[idx]);
            }
            break;
        }
        case scui_ximage_type_replace: {
            scui_handle_t *image_list = ximage->data.replace.image;
            scui_coord_t   curr       = ximage->data.replace.curr;
            SCUI_ASSERT(curr < ximage->data.replace.num);
            if (image_list[curr] != SCUI_HANDLE_INVALID)
                scui_widget_draw_image(widget->myself, NULL, image_list[curr], NULL, SCUI_COLOR_FILTER_TRANS);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}
