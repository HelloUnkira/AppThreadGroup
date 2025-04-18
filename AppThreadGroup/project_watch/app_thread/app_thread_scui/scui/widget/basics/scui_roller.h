#ifndef SCUI_ROLLER_H
#define SCUI_ROLLER_H

typedef struct { scui_class_scroll(widget, scroll);
    /* 外部域: */
    scui_string_maker_t string;
    /* 内部域: */
    scui_handle_t       center;
} scui_roller_t;

typedef struct { scui_class_scroll_maker(widget, scroll);
    /* 外部域: */
    scui_string_maker_t string;
} scui_roller_maker_t;

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 *@param layout     通过布局创建
 */
void scui_roller_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_roller_burn(scui_handle_t handle);

/*@brief 控件布局更新
 *@param handle 控件句柄
 */
void scui_roller_layout(scui_handle_t handle);

/*@brief 滚轮控件中心对齐子控件
 *@param handle 滚轮控件句柄
 *@param target 中心对齐子控件
 */
void scui_roller_center_target_get(scui_handle_t handle, scui_handle_t *target);

/*@brief 滚轮控件设置字符串
 *@param handle   滚轮控件句柄
 *@param str_utf8 字符串(utf8)
 */
void scui_roller_string_str(scui_handle_t handle, uint8_t *str_utf8);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_roller_event(scui_event_t *event);

#endif
