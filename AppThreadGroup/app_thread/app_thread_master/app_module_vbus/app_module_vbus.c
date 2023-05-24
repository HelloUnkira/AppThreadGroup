/*app_module_vbus.c:
 *通过app_module_vbus.py生成
 *参考app_module_vbus.json中的模式生成源
 *它通常可以作为一个简易的桥梁
 *搭建不同线程中不同子模块的连接
 *但是要注意实际执行环境
 */

#include "app_module_vbus.h"

/* APP_EVENT1 */
/* this is response set: */
extern void app_module_vbus_empty(app_module_vbus *event);
extern void app_module_vbus_empty(app_module_vbus *event);
/* APP_EVENT2 */
/* this is response set: */
extern void app_module_vbus_empty(app_module_vbus *event);

extern void app_module_vbus_respond(app_module_vbus *event)
{
	switch (event->command) {
	case APP_EVENT1:
		app_module_vbus_empty(event);
		app_module_vbus_empty(event);
		break;
	case APP_EVENT2:
		app_module_vbus_empty(event);
		break;
	}
}

void app_module_vbus_empty(app_module_vbus *event)
{
}
