#ifndef SCUI_SYMBOL_H
#define SCUI_SYMBOL_H

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t       font_idx;       /* 字库索引 */
    scui_handle_t       lang;           /* 语言类型 */
    const uint8_t      *code;           /* 符号字符 */
    scui_color_t        color;          /* 符号颜色 */
} scui_symbol_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */ 
    scui_handle_t       font_idx;       /* 字库索引 */
    scui_handle_t       lang;           /* 语言类型 */
    const uint8_t      *code;           /* 符号字符 */
    scui_color_t        color;          /* 符号颜色 */
} scui_symbol_maker_t;
#pragma pack(pop)

/*@brief 符号字符编码
 *@param symbol 符号字符串
 *@retval 符号字符编码
 */
uint32_t scui_symbol_code(uint8_t *symbol);

/*@brief 符号字符区域
 *@param font_name 符号字库名字句柄
 *@param symbol    符号字符编码
 *@retval 符号字符区域
 */
scui_area_t scui_symbol_area(scui_handle_t font_name, uint32_t symbol);

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_symbol_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_symbol_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_symbol_invoke(scui_event_t *event);

#endif
