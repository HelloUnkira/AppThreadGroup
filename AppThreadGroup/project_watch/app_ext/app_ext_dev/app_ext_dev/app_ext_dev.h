#ifndef APP_EXT_DEV_H
#define APP_EXT_DEV_H

typedef struct {
    const char *name;   /* 设备名字 */
    const void *cfg;    /* 设备操作参数 */
    const void *api;    /* 设备操作集合 */
    void *data;         /* 设备操作数据 */
} app_dev_t;

/* 外设设备入app_ext库 */
/* 定制设备入app_dev库 */

/* 设备抽象实例(template,不做实际的使用,只做编码结构参考) */
extern const app_dev_t app_dev_empty;
/* 设备抽象实例(peripheral) */
extern const app_dev_t app_dev_log;
extern const app_dev_t app_dev_ext_mem;
extern const app_dev_t app_dev_rtc;
extern const app_dev_t app_dev_watchdog;
/* 设备抽象实例(custom) */
extern const app_dev_t app_dev_battery;
extern const app_dev_t app_dev_vibrate;
extern const app_dev_t app_dev_gesture;
extern const app_dev_t app_dev_backlight;
extern const app_dev_t app_dev_temperature;

/* 设备抽象动作接口(template,不做实际的使用,只做编码结构参考) */
#include "app_ext_dev_empty.h"          /* empty */
/* 设备抽象动作接口(peripheral) */
#include "app_ext_dev_log.h"            /* log */
#include "app_ext_dev_ext_mem.h"        /* ext_mem */
#include "app_ext_dev_rtc.h"            /* rtc */
#include "app_ext_dev_watchdog.h"       /* watchdog */
/* 设备抽象动作接口(custom) */
#include "app_ext_dev_battery.h"        /* battery */
#include "app_ext_dev_vibrate.h"        /* vibrate */
#include "app_ext_dev_backlight.h"      /* backlight */
#include "app_ext_dev_temperature.h"    /* temperature */
#include "app_ext_dev_gesture.h"        /* gesture */

/*****************************************************************************/
/*说明:
 *    何为低功耗方案:
 *        低功耗芯片+低功耗设备控制策略
 *    何为低功耗芯片:
 *        持续一直工作时芯片本身的功耗并不高
 *    何为低功耗设备控制策略:
 *        处于低功耗或者关机流程
 *        部分设备以及其外设因为其特性,不能够直接关闭
 *        因为它需要持续不断地监控一些特殊事件
 *        此时设备需要工作在低功耗模式以节约功耗
 *        常规模式下,设备处于不稳定活跃态,也即它可能持续不断的工作或断续工作
 *        而相反的低功耗模式下,设备大部分时间处于休眠状态,但会监控一些特殊事件
 *        这些事件是不可被忽视的,一旦产生则设备需要立刻唤醒系统退出低功耗模式
 *        并为到达的该事件触发处理流程并重新评估当前环境是否进入低功耗
 *        通常来说代表的事件是硬件中断的产生
 *扩展指导1:
 *    如果需要实现多设备挂载同一总线上使用
 *    例如: app_dev_1和app_dev_2同时使用MCU上的IIC总线1时
 *          那么此时则需要再构建一个app_dev_iic1虚拟设备
 *          这个设备仅作为app_dev_1和app_dev_2一起使用的新一层设备抽象接口
 *          此时如果出现总线竞争问题,则可以使用app_mutex_t在
 *          app_dev_iic1虚拟设备动作接口内部做并发访问的互斥
 *扩展指导2:
 *    如果需要实现同一类型的不同设备的热插拔和多型号兼容(特殊类型)
 *    例如: app_dev_gsensor设备需要支持具体的俩类硬件构型的设备
 *          app_dev_gsensor(1和2),且这俩种设备通过一个指定的流程
 *          去确认型号,如iic1发送特定的指令获取芯片编号确认是gsensor(1和2)
 *          此时对于app_dev_gsensor设备的设备抽象实例则不再是const类型的
 *          并且在同一arch上需要生成俩份设备抽象实例app_dev_gsensor 1和2
 *          在每一次app_dev_gsensor的ready流程中执行该构型确认流程
 *          用以动态的将app_dev_gsensor定位到app_dev_gsensor(1或2)上面
 *          这样子就可以对一个类型的项目兼容多种类型的硬件设备
 *          或者在设备使用时进行热插拔
 *备注:
 *    扩展指导1和扩展指导2不冲突
 */
/*****************************************************************************/

/*@brief 初始化设备
 */
void app_dev_ready(void);

#endif
