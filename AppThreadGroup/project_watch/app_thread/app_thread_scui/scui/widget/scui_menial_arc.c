/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造器初始化(子类型)
 *@param menial_maker 控件构造器实例
 */
void scui_menial_arc_maker(scui_menial_maker_t *menial_maker)
{
}

/*@brief 控件初始化(子类型)
 *@param menial 控件实例
 */
void scui_menial_arc_config(scui_menial_t *menial)
{
}

/*@brief 控件反初始化(子类型)
 *@param menial 控件实例
 */
void scui_menial_arc_recycle(scui_menial_t *menial)
{
}

/*@brief 事件处理回调(子类型)
 *@param event 事件
 */
void scui_menial_arc_invoke(scui_event_t *event)
{
}
