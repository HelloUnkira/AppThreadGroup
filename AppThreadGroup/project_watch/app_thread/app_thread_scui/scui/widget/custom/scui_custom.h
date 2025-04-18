#ifndef SCUI_CUSTOM_H
#define SCUI_CUSTOM_H

typedef struct { scui_class_widget(widget);
    /* 外部域: */
    /* 内部域: */
    void *draw_dsc; /* 绘制描述符(全局,唯一) */
} scui_custom_t;

#pragma pack(push, 1)
typedef struct { scui_class_widget_maker(widget);
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

/*@brief 自定义控件绘制描述符(全局,唯一)
 *       一个控件仅使用一个绘制描述符
 *@param handle 自定义控件句柄
 */
void scui_custom_draw_dsc(scui_handle_t handle, void **draw_dsc);

#endif
