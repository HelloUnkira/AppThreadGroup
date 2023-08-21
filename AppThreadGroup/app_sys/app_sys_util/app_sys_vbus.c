/*app_sys_vbus.c:
 *通过app_sys_vbus.py生成
 *参考app_sys_vbus.json中的模式生成源
 *它可以搭建不同线程中不同子模块的连接
 *也可以搭建同一线程中不同子模块的连接
 *由于极小的限制而成为一个万金油模组
 *可以对及其混乱的流程进行解耦合
 *但是优先不建议使用
 */

#include "app_sys_vbus.h"

/* APP_EVENT1 */
/* this is response set: */
extern void app_sys_vbus_empty(app_sys_vbus *event);
extern void app_sys_vbus_empty(app_sys_vbus *event);
/* APP_EVENT2 */
/* this is response set: */
extern void app_sys_vbus_empty(app_sys_vbus *event);

extern void app_sys_vbus_respond(app_sys_vbus *event)
{
	switch (event->command) {
	case APP_EVENT1:
		app_sys_vbus_empty(event);
		app_sys_vbus_empty(event);
		break;
	case APP_EVENT2:
		app_sys_vbus_empty(event);
		break;
	}
}

void app_sys_vbus_empty(app_sys_vbus *event)
{
}
