#ifndef SCUI_OBJ_CHK_H
#define SCUI_OBJ_CHK_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t  widget;
    scui_object_t  object;
    scui_obj_btn_t obj_btn;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t  font;        /* 符号字库索引 */
    scui_handle_t  lang;        /* 语言类型 */
    const uint8_t *sym_def;     /* 符号(默认态,空=不绘制) */
    const uint8_t *sym_chk;     /* 符号(选中态,默认对勾) */
    scui_color_t   sym_color;   /* 符号颜色 */
    scui_sbitfd_t  fixed:1;     /* 固定标记(不响应点击) */
    scui_object_type_t state;   /* 初始状态(def/chk) */
    /* 内部域: */
} scui_obj_chk_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t  widget;
    scui_object_maker_t  object;
    scui_obj_btn_maker_t obj_btn;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t  font;        /* 符号字库索引 */
    scui_handle_t  lang;        /* 语言类型 */
    const uint8_t *sym_def;     /* 符号(默认态,空=不绘制) */
    const uint8_t *sym_chk;     /* 符号(选中态,默认对勾) */
    scui_color_t   sym_color;   /* 符号颜色 */
    scui_sbitfd_t  fixed:1;     /* 固定标记(不响应点击) */
    scui_object_type_t state;   /* 初始状态(def/chk) */
} scui_obj_chk_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_obj_chk_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_chk_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_chk_invoke(scui_event_t *event);

#endif
