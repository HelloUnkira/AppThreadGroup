/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 自定义控件创建
 *@param maker  自定义控件创建参数
 *@param handle 自定义控件句柄
 *@param layout 通过布局创建
 */
void scui_custom_make(scui_custom_maker_t *maker, scui_handle_t *handle, bool layout)
{
    /* 创建自定义控件实例 */
    scui_custom_t *custom = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_custom_t));
    memset(custom, 0, sizeof(scui_custom_t));
    
    /* 创建基础控件实例 */
    scui_widget_maker_t widget_maker = maker->widget;
    scui_widget_make(&custom->widget, &widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_custom));
    // 自定义控件既可以是根控件(子画布控件树),也可以是非根控件
}

/*@brief 自定义控件销毁
 *@param handle 自定义控件句柄
 */
void scui_custom_burn(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_custom_t *custom = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    /* 销毁基础控件实例 */
    scui_widget_burn(&custom->widget);
    
    /* 销毁自定义控件实例 */
    SCUI_MEM_FREE(custom);
}

/*@brief 自定义控件绘制文本
 *@param handle 自定义控件句柄
 *@param target 控件绘制区域
 *@param args   字符串绘制参数
 *@param text   文本句柄
 */
void scui_custom_draw_text(scui_handle_t handle, scui_area_t *target, scui_string_args_t *args, scui_handle_t text)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_get(handle);
    scui_custom_t *custom = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    if (text != SCUI_HANDLE_INVALID) {
        scui_multi_lang_type_t type = scui_font_type_switch(args->name);
        scui_handle_t text_ml = scui_multi_lang_switch(type, text);
        args->utf8 = scui_handle_get(text_ml);
    }
    args->clip = scui_widget_clip(handle);
    
    if (target != NULL)
        args->clip = *target;
    
    args->update = true;
    scui_string_args_process(args);
    scui_widget_draw_string(handle, target, args);
    
    // 绘制完就地回收
    args->update = true;
    args->name = SCUI_HANDLE_INVALID;
    args->utf8 = NULL;
    scui_string_args_process(args);
}
