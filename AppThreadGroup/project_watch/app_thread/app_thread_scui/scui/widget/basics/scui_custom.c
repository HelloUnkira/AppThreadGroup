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
void scui_custom_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_custom_t *custom = widget;
    scui_custom_maker_t *custom_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_custom));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_custom_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    /* 回收可能因为文本绘制而存留在控件内的资源 */
    if (custom->str_args != NULL) {
        
        for (scui_coord_t idx = 0; idx < SCUI_CUSTOM_STR_ITEM_LIMIT; idx++) {
            scui_string_args_t *str_args = custom->str_args[idx];
            if (str_args == NULL)
                continue;
            
            /* 在回收实例之前回收布局 */
            str_args->update = true;
            str_args->utf8   = NULL;
            str_args->name   = SCUI_HANDLE_INVALID;
            scui_string_args_process(str_args);
            
            SCUI_MEM_FREE(custom->str_args[idx]);
            SCUI_MEM_FREE(custom->str_utf8[idx]);
        }
        SCUI_MEM_FREE(custom->str_args);
        SCUI_MEM_FREE(custom->str_utf8);
    }
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_custom_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_custom_t *custom = (void *)widget;
    
    /* empty */
}

/*@brief 自定义控件文本绘制(不建议使用)
 *       备注:不建议使用,string控件更合适
 *@param handle 控件句柄
 *@param args   绘制参数(scui_string_args_t)
 *@param text   文本句柄
 */
void scui_custom_draw_text(scui_handle_t handle, void *args, scui_handle_t text)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    /* 在custom申请全局实例空间 */
    /* 以扩充其绘制资源的生命周期 */
    SCUI_ASSERT(args != NULL);
    
    if (custom->str_args == NULL) {
        scui_multi_t str_args_size = sizeof(void*) * SCUI_CUSTOM_STR_ITEM_LIMIT;
        custom->str_args  = SCUI_MEM_ZALLOC(scui_mem_type_mix, str_args_size);
    }
    if (custom->str_utf8 == NULL) {
        scui_multi_t str_utf8_size = sizeof(void *) * SCUI_CUSTOM_STR_ITEM_LIMIT;
        custom->str_utf8  = SCUI_MEM_ZALLOC(scui_mem_type_mix, str_utf8_size);
    }
    
    /* 先回收已经失效的资源集合 */
    for (scui_coord_t idx = 0; idx < SCUI_CUSTOM_STR_ITEM_LIMIT; idx++) {
        scui_string_args_t *str_args = custom->str_args[idx];
        if (str_args == NULL || str_args->local)
            continue;
        
        /* 在回收实例之前回收布局 */
        str_args->update = true;
        str_args->utf8   = NULL;
        str_args->name   = SCUI_HANDLE_INVALID;
        scui_string_args_process(str_args);
        
        SCUI_MEM_FREE(custom->str_args[idx]);
        SCUI_MEM_FREE(custom->str_utf8[idx]);
        custom->str_args[idx] = NULL;
        custom->str_utf8[idx] = NULL;
    }
    
    /* 找到一个空闲的位置, 存放本地资源 */
    for (scui_coord_t idx = 0; idx < SCUI_CUSTOM_STR_ITEM_LIMIT; idx++) {
        scui_string_args_t *str_args = custom->str_args[idx];
        if (str_args != NULL)
            continue;
        
        str_args = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_string_args_t));
        memcpy(str_args, args, sizeof(scui_string_args_t));
        custom->str_args[idx] = str_args;
        
        if (text != SCUI_HANDLE_INVALID) {
            str_args->utf8 = scui_multi_lang_str(text, str_args->lang);
        } else {
            scui_coord_t len_utf8 = strlen(str_args->utf8) + 1;
            void *str_utf8 = SCUI_MEM_ALLOC(scui_mem_type_mix, len_utf8);
            strcpy(str_utf8, str_args->utf8);
            custom->str_utf8[idx] = str_utf8;
            str_args->utf8 = str_utf8;
        }
        
        /* 绘制剪切域同步 */
        if (scui_area_empty(&str_args->clip))
            str_args->clip = widget->clip;
        
        /* 绘制时本地布局 */
        str_args->nest   = 0;
        str_args->local  = true;
        str_args->update = true;
        scui_string_args_process(str_args);
        scui_widget_draw_string(handle, &str_args->clip, str_args);
        if (str_args->nest == 0)
            str_args->local = false;
        
        return;
    }
    
    SCUI_LOG_ERROR("str item limit too small");
}
