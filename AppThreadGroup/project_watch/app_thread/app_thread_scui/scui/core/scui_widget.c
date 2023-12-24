/*实现目标:
 *    控件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 控件alpha获取
 *@param widget 控件实例
 *@param alpha  控件alpha
 */
inline scui_alpha_t scui_widget_alpha_get(scui_widget_t *widget)
{
    return widget->alpha;
}

/*@brief 控件alpha设置
 *@param widget 控件实例
 *@param alpha  控件alpha
 */
inline void scui_widget_alpha_set(scui_widget_t *widget, scui_alpha_t alpha)
{
    widget->alpha = alpha;
    scui_widget_list_alpha_update(widget, alpha);
}
