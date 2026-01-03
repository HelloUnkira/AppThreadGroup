#ifndef SCUI_XVEDIO_H
#define SCUI_XVEDIO_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
	scui_image_frame_t iframe;
    /* 内部域: */
    bool abort;
} scui_xvedio_t;

#pragma pack(push, 1)
typedef struct {
	/* 继承域: */
	SCUI_EXTEND_FIELD_S
	scui_widget_maker_t widget;
	SCUI_EXTEND_FIELD_E
	/* 外部域: */
	scui_image_frame_t  iframe;
} scui_xvedio_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_xvedio_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_xvedio_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_xvedio_invoke(scui_event_t *event);

#endif
