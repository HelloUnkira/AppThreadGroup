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
void scui_custom_create(scui_custom_maker_t *maker, scui_handle_t *handle, bool layout)
{
    /* 创建自定义控件实例 */
    scui_custom_t *custom = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_custom_t));
    memset(custom, 0, sizeof(scui_custom_t));
    
    /* 创建基础控件实例 */
    scui_widget_maker_t widget_maker = maker->widget;
    scui_widget_constructor(&custom->widget, &widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_custom));
    // 自定义控件既可以是根控件(子画布控件树),也可以是非根控件
}

/*@brief 自定义控件销毁
 *@param handle 自定义控件句柄
 */
void scui_custom_destroy(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_get(handle);
    scui_custom_t *custom = (void *)widget;
    SCUI_ASSERT(widget != NULL);
    
    /* 销毁基础控件实例 */
    scui_widget_destructor(&custom->widget);
    
    /* 销毁自定义控件实例 */
    SCUI_MEM_FREE(custom);
}
