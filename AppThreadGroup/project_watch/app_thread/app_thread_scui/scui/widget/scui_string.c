/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 字符串控件创建
 *@param maker  字符串控件创建参数
 *@param handle 字符串控件句柄
 *@param layout 通过布局创建
 */
void scui_string_create(scui_string_maker_t *maker, scui_handle_t *handle, bool layout)
{
    SCUI_ASSERT(maker->widget.type == scui_widget_type_string);
    SCUI_ASSERT(maker->widget.parent != SCUI_HANDLE_INVALID);
    
    /* 创建字符串控件实例 */
    scui_string_t *string = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_string_t));
    memset(string, 0, sizeof(scui_string_t));
    
    /* 创建基础控件实例 */
    scui_widget_maker_t widget_maker = maker->widget;
    
    /* 必须标记anima事件 */
    widget_maker.style.sched_anima = true;
    
    /* 创建基础控件实例 */
    scui_widget_create(&string->widget, &widget_maker, handle, layout);
    
    string->font_idx    = maker->font_idx;
    string->args        = maker->args;
    string->unit_ms     = maker->unit_ms != 0 ? maker->unit_ms : SCUI_WIDGET_STRING_UNIT_MS;
    string->unit_dx     = maker->unit_dx != 0 ? maker->unit_dx : SCUI_WIDGET_STRING_UNIT_DX;
    string->unit_s      = maker->unit_s;
    string->unit_over   = false;
    string->unit_way    = 1;
    
    if (string->args.name == SCUI_HANDLE_INVALID)
        string->name = scui_font_name_get(string->font_idx);
    else
        string->name = string->args.name;
    
    /* 尝试初始更新字符串文本信息 */
    scui_string_update_text(*handle, maker->text);
    
    /* 更新一次字符串绘制参数 */
    string->args.update = true;
    string->args.name   = string->name;
    string->args.utf8   = string->str_utf8;
    string->args.clip   = scui_widget_clip(*handle);
    scui_string_args_process(&string->args);
}

/*@brief 字符串控件销毁
 *@param handle 字符串控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_string_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_string_t *string = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->type == scui_widget_type_string);
    
    string->args.update = true;
    string->args.name   = SCUI_HANDLE_INVALID;
    string->args.utf8   = NULL;
    scui_string_args_process(&string->args);
    scui_string_update_str(handle, NULL);
    
    /* 销毁基础控件实例 */
    scui_widget_destroy(&string->widget);
    
    /* 销毁字符串控件实例 */
    SCUI_MEM_FREE(string);
}

/*@brief 字符串控件更新文本
 *@param handle 字符串控件句柄
 *@param text   字符串句柄
 */
void scui_string_update_text(scui_handle_t handle, scui_handle_t text)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_string_t *string = (void *)widget;
    
    if (string->text == text)
        return;
    
    string->text = text;
    
    if (string->str_utf8 != NULL) {
        SCUI_MEM_FREE(string->str_utf8);
        string->str_utf8 = NULL;
    }
    
    if (string->text != SCUI_HANDLE_INVALID) {
        string->name = scui_font_name_get(string->font_idx);
        scui_multi_lang_type_t type = scui_font_type_switch(string->name);
        uint8_t *str_utf8  = scui_handle_get(scui_multi_lang_switch(type, string->text));
        uint32_t str_bytes = scui_utf8_str_bytes(str_utf8);
        string->str_utf8 = SCUI_MEM_ALLOC(scui_mem_type_mix, str_bytes + 7);
        memcpy(string->str_utf8, str_utf8, str_bytes);
        string->str_utf8[str_bytes] = '\0';
    }
    
    string->args.update = true;
    scui_widget_draw(handle, NULL, false);
}

/*@brief 字符串控件更新文本
 *@param handle   字符串控件句柄
 *@param str_utf8 字符串(utf8)
 */
void scui_string_update_str(scui_handle_t handle, uint8_t *str_utf8)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_string_t *string = (void *)widget;
    
    string->text = SCUI_HANDLE_INVALID;
    
    if (string->str_utf8 != NULL) {
        SCUI_MEM_FREE(string->str_utf8);
        string->str_utf8 = NULL;
    }
    
    if (str_utf8 != NULL) {
        uint32_t str_bytes = scui_utf8_str_bytes(str_utf8);
        string->str_utf8 = SCUI_MEM_ALLOC(scui_mem_type_mix, str_bytes + 7);
        memcpy(string->str_utf8, str_utf8, str_bytes);
        string->str_utf8[str_bytes] = '\0';
    }
    
    scui_widget_draw(handle, NULL, false);
}

/*@brief 字符串控件滚动结束(单次滚动)
 *@param handle   字符串控件句柄
 *@retval 是否结束
 */
bool scui_string_scroll_over(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_string_t *string = (void *)widget;
    
    return string->unit_over;
}

/*@brief 字符串控件事件处理回调
 *@param event 事件
 */
void scui_string_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_string_t *string = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             break;
        
        if (string->unit_over)
            break;
        
        if (string->args.limit <= 0) {
            /* 单次滚动结束标记 */
            if (string->unit_s)
                string->unit_over = true;
            break;
        }
        
        string->rcd_ms += scui_event_anima_elapse;
        if (string->rcd_ms < string->unit_ms)
            break;
        string->rcd_ms -= string->unit_ms;
        
        scui_string_args_process(&string->args);
        string->args.offset -= string->unit_dx * string->unit_way;
        
        if (string->args.offset > 0) {
            string->args.offset = 0;
            string->unit_way = -string->unit_way;
            /* 单次滚动结束标记 */
            if (string->unit_s)
                string->unit_over = true;
        }
        if (string->args.offset < -string->args.limit) {
            string->args.offset = -string->args.limit;
            string->unit_way = -string->unit_way;
            /* 单次滚动结束标记 */
            if (string->unit_s)
                string->unit_over = true;
        }
        
        scui_widget_draw(handle, NULL, false);
        break;
    }
    case scui_event_draw: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        if (string->str_utf8 != NULL) {
            
            bool update = string->args.update;
            update = update || (string->args.number == 0);
            update = update || (string->args.name != string->name);
            update = update || (memcmp(string->args.utf8, string->str_utf8, string->args.number) != 0);
            
            string->args.update = update;
            string->args.name   = string->name;
            string->args.utf8   = string->str_utf8;
            string->args.clip   = scui_widget_clip(handle);
            scui_string_args_process(&string->args);
            scui_widget_draw_string(handle, NULL, &string->args);
        }
        
        break;
    }
    case scui_event_font_change: {
        if (!scui_widget_event_check_execute(event))
             break;
        
        if (string->text != SCUI_HANDLE_INVALID) {
            scui_handle_t text = string->text;
            scui_string_update_text(handle, SCUI_HANDLE_INVALID);
            string->name = scui_font_name_get(string->font_idx);
            scui_string_update_text(handle, text);
            string->args.offset = 0;
            string->unit_over   = false;
            string->unit_way    = 1;
        }
        break;
    }
    default:
        break;
    }
}
