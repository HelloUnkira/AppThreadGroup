#ifndef SCUI_CUSTOM_H
#define SCUI_CUSTOM_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    /* 内部域: */
    scui_handle_t            handle_m;     /* 子控件(可选) */
    scui_handle_t            handle_s;     /* 子控件树(可选) */
    scui_custom_draw_dsc_t  *draw_dsc;     /* 绘制描述符(可选,全局,唯一) */
} scui_custom_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
} scui_custom_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 *@param layout     通过布局创建
 */
void scui_custom_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_custom_burn(scui_handle_t handle);

/*@brief 自定义控件子控件(可选)
 *@param handle   自定义控件句柄
 *@param handle_s 控件句柄地址
 */
void scui_custom_handle_m(scui_handle_t handle, scui_handle_t **handle_m);

/*@brief 自定义控件子控件树(可选)
 *@param handle   自定义控件句柄
 *@param handle_s 控件句柄地址
 */
void scui_custom_handle_s(scui_handle_t handle, scui_handle_t **handle_s);

/*@brief 自定义控件绘制描述符(全局,唯一)
 *@param handle   自定义控件句柄
 *@param draw_dsc 绘制描述符
 */
void scui_custom_draw_dsc(scui_handle_t handle, scui_custom_draw_dsc_t **draw_dsc);

#endif
