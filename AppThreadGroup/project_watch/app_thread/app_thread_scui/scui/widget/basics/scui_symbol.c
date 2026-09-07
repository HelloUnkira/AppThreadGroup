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
void scui_symbol_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_symbol_t *symbol = widget;
    scui_symbol_maker_t *symbol_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_symbol));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    symbol->font_idx = symbol_maker->font_idx;
    symbol->lang     = symbol_maker->lang;
    symbol->code     = symbol_maker->code;
    symbol->color    = symbol_maker->color;
    
    /* symbol语言族(定位符号字库) */
    if (symbol->lang == SCUI_HANDLE_INVALID)
        symbol->lang  = scui_lang_type_symbol;
    
    /* 尝试初始更新符号字符信息 */
    scui_symbol_update(*handle, symbol->font_idx, symbol->code);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_symbol_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_symbol));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_symbol_t *symbol = (void *)widget;
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 符号控件注入(字体/符号)
 *@param handle   控件句柄
 *@param font_idx 字库索引
 *@param code     符号字符
 */
void scui_symbol_update(scui_handle_t handle, scui_handle_t font_idx, const uint8_t *code)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_symbol));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_symbol_t *symbol = (void *)widget;
    
    symbol->font_idx = font_idx;
    symbol->code     = code;
    
    scui_widget_layout_refr(handle);
    scui_widget_draw(handle, NULL, false, 0);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_symbol_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_symbol_t *symbol = (void *)widget;
    
    switch (event->type) {
    case scui_event_draw_graph: {
        scui_handle_t font = scui_font_name_match(symbol->font_idx, symbol->lang);
        uint32_t  sym_code = scui_symbol_code((uint8_t *)symbol->code);
        
        scui_widget_draw_symbol(event->object, NULL, NULL,
            symbol->color, font, sym_code);
        break;
    }
    case scui_event_layout: {
        if (symbol->font_idx == SCUI_HANDLE_INVALID) break;
        if (symbol->code == NULL) break;
        
        scui_handle_t font = scui_font_name_match(symbol->font_idx, symbol->lang);
        uint32_t  sym_code = scui_symbol_code((uint8_t *)symbol->code);
        
        scui_area_t area = scui_symbol_area(font, sym_code);
        if (area.w > widget->clip.w || area.h > widget->clip.h)
            scui_widget_adjust_size(event->object, area.w, area.h);
        break;
    }
    default:
        break;
    }
}