#ifndef SCUI_CANVAS_H
#define SCUI_CANVAS_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_pixel_cf_t     format;         /* 画布基础像素格式 */
    /* 内部域: */
    scui_image_t        image_src;      /* 画布图资源 */
    scui_handle_t       image;          /* 画布图句柄 */
} scui_canvas_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_pixel_cf_t     format;         /* 画布基础像素格式 */
} scui_canvas_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_canvas_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_canvas_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_canvas_invoke(scui_event_t *event);

#endif
