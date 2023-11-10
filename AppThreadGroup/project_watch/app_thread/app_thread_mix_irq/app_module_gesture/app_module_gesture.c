/*实现目标:
 *    手势相关的功能组件
 *    对于手表设备而言手势功能较为复杂
 *    帧数据来源一般是重力加速度计生成
 *    帧数据经过最基本的数据滤波后
 *    触发不同的算法流程生成不同的数据
 *    正常模式下本设备是一直工作的
 *    低功耗模式下只由事件触发一个周期
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief 手势模组事件中断回调
 */
static void app_module_gesture_irq_cb(void)
{
    APP_SYS_LOG_WARN("");
}

/*@brief 初始化手势模组
 */
void app_module_gesture_ready(void)
{
    app_dev_gesture_ready(&app_dev_gesture);
    app_dev_gesture_irq_cb_reg(&app_dev_gesture, app_module_gesture_irq_cb);
    app_dev_gesture_irq_switch(&app_dev_gesture, false);
}
