#ifndef APP_EXT_DEV_H
#define APP_EXT_DEV_H

typedef struct {
    const char *name;   /* 设备名字 */
    const void *cfg;    /* 设备操作参数 */
    const void *api;    /* 设备操作集合 */
    void *data;         /* 设备操作数据 */
} app_dev_t;


/* 设备抽象实例(template,不做实际的使用,只做编码结构参考) */
extern const app_dev_t app_dev_empty;
/* 设备抽象实例(peripheral) */
extern const app_dev_t app_dev_rtc;
extern const app_dev_t app_dev_watchdog;
/* 设备抽象实例 */
extern const app_dev_t app_dev_log;
extern const app_dev_t app_dev_ext_mem;
extern const app_dev_t app_dev_battery;
extern const app_dev_t app_dev_vibrate;
extern const app_dev_t app_dev_backlight;
extern const app_dev_t app_dev_temperature;

/* 设备抽象动作接口(template,不做实际的使用,只做编码结构参考) */
#include "app_ext_dev_empty.h"          /* empty */
/* 设备抽象动作接口(peripheral) */
#include "app_ext_dev_rtc.h"            /* rtc */
#include "app_ext_dev_watchdog.h"       /* watchdog */
/* 设备抽象动作接口 */
#include "app_ext_dev_log.h"            /* log(common device) */
#include "app_ext_dev_ext_mem.h"        /* ext_mem */
#include "app_ext_dev_battery.h"        /* battery */
#include "app_ext_dev_vibrate.h"        /* vibrate */
#include "app_ext_dev_backlight.h"      /* backlight */
#include "app_ext_dev_temperature.h"    /* temperature */

/*****************************************************************************/
/*扩展指导1:
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
