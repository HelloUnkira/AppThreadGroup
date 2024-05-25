/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

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
    scui_widget_create(&string->widget, &maker->widget, handle, layout);
    
    string->align       = maker->align;
    string->mode        = maker->mode;
    string->dir         = maker->dir;
    string->font        = maker->font;
    string->text        = maker->text;
    string->color       = maker->color;
    string->filter      = maker->filter;
    string->margin      = maker->margin;
    string->space       = maker->space;
    
    /* 目前只支持水平书写 */
    SCUI_ASSERT(string->dir == scui_event_dir_hor);
    scui_string_update_text(*handle, string->text);
    
    /* 为字符串控件添加指定的事件回调 */
    scui_event_cb_node_t cb_node = {.event_cb = scui_string_event,};
    
    /* 字符串控件需要帧动画 */
    string->widget.style.sched_anima = true;
    
    /* 事件默认全局接收 */
    cb_node.event = scui_event_sched_all;
    scui_widget_event_add(*handle, &cb_node);
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
    
    scui_string_update_text(handle, SCUI_HANDLE_INVALID);
    
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
    
    string->text = text;
    
    if (string->str != NULL)
        SCUI_MEM_FREE(string->str);
    
    if (string->text != SCUI_HANDLE_INVALID) {
        scui_multi_lang_type_t type = scui_font_type_switch(string->font);
        char *str = scui_handle_get(scui_multi_lang_switch(type, string->text));
        string->str = SCUI_MEM_ALLOC(scui_mem_type_mix, strlen(str) + 1);
        strcpy(string->str, str);
    }
}

/*@brief 字符串控件更新文本
 *@param handle 字符串控件句柄
 *@param text   字符串句柄
 */
void scui_string_update_str(scui_handle_t handle, char *str)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_string_t *string = (void *)widget;
    
    string->text = SCUI_HANDLE_INVALID;
    
    if (string->str != NULL)
        SCUI_MEM_FREE(string->str);
    
    if (str != NULL) {
        string->str = SCUI_MEM_ALLOC(scui_mem_type_mix, strlen(str) + 1);
        strcpy(string->str, str);
    }
}

/*@brief 字符串控件事件处理回调
 *@param event 事件
 */
void scui_string_event(scui_event_t *event)
{
    if (!scui_widget_event_check_execute(event))
         return;
    
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    scui_string_t *string = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        
        
        
        break;
    }
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        
        if (string->str != NULL) {
            #if 1   // test
            scui_font_glyph_unit_t glyph_unit = {
                .name = string->font,
                .glyph.unicode_letter = 'B',
                .glyph.space_width = SCUI_STRING_SPACE_WIDTH,
            };
            scui_font_glyph_cache_load(&glyph_unit);
            
            scui_draw_letter(widget->surface, &widget->clip_set.clip, &glyph_unit.glyph,
                             string->color, widget->alpha, string->filter, true);
            
            #if 0   // test
            // 通过生成的lvgl_font.c的数据流做比较确认数据获取的准确性
            for (uint32_t idx = 0; idx < glyph_unit.glyph.bitmap_size; idx++) {
                 if (idx % 8 == 0)
                     SCUI_LOG_INFO_RAW(SCUI_LOG_LINE);
                     SCUI_LOG_INFO_RAW("0x%02x ", glyph_unit.glyph.bitmap[idx]);
            }
            SCUI_LOG_INFO_RAW(SCUI_LOG_LINE);
            #endif
            
            #if 0   // test
             uint32_t idx_ofs = 0;
            // 通过生成的lvgl_font.c的数据流做比较确认数据获取的准确性
            for (scui_multi_t idx_line = 0; idx_line < glyph_unit.glyph.box_h; idx_line++) {
            for (scui_multi_t idx_item = 0; idx_item < glyph_unit.glyph.box_w; idx_item++) {
                 uint32_t idx = idx_line * glyph_unit.glyph.box_w + idx_item;
                 
                 uint8_t palette = scui_font_bpp_palette(
                         glyph_unit.glyph.bitmap[idx / (8 / glyph_unit.glyph.bpp)],
                         glyph_unit.glyph.bpp, idx_ofs % (8 / glyph_unit.glyph.bpp));
                 
                 idx_ofs++;
                 if (palette != 0x00)
                     SCUI_LOG_INFO_RAW("%02x ", palette);
                 else
                     SCUI_LOG_INFO_RAW("   ");
                }
                SCUI_LOG_INFO_RAW(SCUI_LOG_LINE);
            }
            #endif
            
            scui_font_glyph_cache_unload(&glyph_unit);
            #endif
            
        }
        break;
    }
    default:
        break;
    }
}
