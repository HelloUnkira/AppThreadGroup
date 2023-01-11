#ifndef APP_SYS_EVENT_H
#define APP_SYS_EVENT_H

/*app_sys_event.h:
 *通过app_sys_event.py生成
 *参考app_sys_event.json中的模式生成源
 *它通常可以作为一个简易的桥梁
 *搭建不同线程中不同子模块的连接
 *但是要注意实际执行环境
 */

#include <stdint.h>

typedef struct {
	/* 数据交换取决于双方约定俗成 */
	uint32_t command;   /* 响应的指令 */
	uint32_t size;      /* 数据元长度 */
	void    *data;      /* 数据元 */
} app_sys_event;

extern void app_sys_event_respond(app_sys_event *event);

#define APP_EVENT1		0x0001
#define APP_EVENT2		0x0002

#endif
