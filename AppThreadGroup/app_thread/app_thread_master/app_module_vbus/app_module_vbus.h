#ifndef APP_MODULE_VBUS_H
#define APP_MODULE_VBUS_H

/*app_module_vbus.h:
 *通过app_module_vbus.py生成
 *参考app_module_vbus.json中的模式生成源
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
} app_module_vbus;

extern void app_module_vbus_respond(app_module_vbus *event);

#define APP_EVENT1		0x0001
#define APP_EVENT2		0x0002

#endif
