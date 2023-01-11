/*app_sys_event.c:
 *通过app_sys_event.py生成
 *参考app_sys_event.json中的模式生成源
 *它通常可以作为一个简易的桥梁
 *搭建不同线程中不同子模块的连接
 *但是要注意实际执行环境
 */

#include "app_sys_event.h"

/* APP_EVENT1 */
/* this is response set: */
extern void event1_function_callback1(app_sys_event *event);
extern void event1_function_callback2(app_sys_event *event);
extern void event1_function_callback3(app_sys_event *event);
/* APP_EVENT2 */
/* this is response set: */
extern void event2_function_callback1(app_sys_event *event);
extern void event2_function_callback2(app_sys_event *event);

extern void app_sys_event_respond(app_sys_event *event)
{
	switch (event->command) {
	case APP_EVENT1:
		event1_function_callback1(event);
		event1_function_callback2(event);
		event1_function_callback3(event);
		break;
	case APP_EVENT2:
		event2_function_callback1(event);
		event2_function_callback2(event);
		break;
	}
}
