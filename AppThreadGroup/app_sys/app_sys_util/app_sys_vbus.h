#ifndef APP_SYS_VBUS_H
#define APP_SYS_VBUS_H

/*app_sys_vbus.h:
 *通过app_sys_vbus.py生成
 *参考app_sys_vbus.json中的模式生成源
 *它可以搭建不同线程中不同子模块的连接
 *也可以搭建同一线程中不同子模块的连接
 *由于极小的限制而成为一个万金油模组
 *可以对及其混乱的流程进行解耦合
 *但是优先不建议使用
 */

#include <stdint.h>

typedef struct {
	/* 数据交换取决于双方约定俗成 */
	uint32_t command;   /* 响应的指令 */
	uint32_t size;      /* 数据元长度 */
	void    *data;      /* 数据元 */
} app_sys_vbus;

extern void app_sys_vbus_respond(app_sys_vbus *event);

#define APP_EVENT1		0x0001
#define APP_EVENT2		0x0002

#endif
