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

static app_sys_timer_t app_module_gesture_timer = {0};
static app_module_gesture_t app_module_gesture = {0};

/*@brief 手势模组流程处理
 */
static void app_module_gesture_proc_exec(void)
{
    uint32_t frame[APP_EXT_DEV_GESTURE_FRAME_MAX][3] = {0};
    uint32_t length = 0;
    /* step1:帧数据采集 */
    app_dev_gesture_proc_exec(&app_dev_gesture);
    app_dev_gesture_get_frame(&app_dev_gesture, frame, &length);
    if (length < APP_EXT_DEV_GESTURE_FRAME_MAX) {
        APP_SYS_LOG_INFO("frame is incomplete");
        return;
    }
    APP_SYS_LOG_INFO("frame:");
    for (uint8_t idx = 0; idx < APP_EXT_DEV_GESTURE_FRAME_MAX; idx++)
        APP_SYS_LOG_DEBUG_RAW("<0x%x,0x%x,0x%x> ", frame[idx][0], frame[idx][1], frame[idx][2]);
    /* step2:帧数据滤波 */

    /* step3:帧数据投入算法,根据结果,触发对应算法流程 */

}

/*@brief 手势模组事件处理
 *       内部使用: 被mix custom线程使用
 */
void app_module_gesture_xmsec_update(void)
{
    APP_SYS_LOG_INFO("start");
    app_module_gesture_proc_exec();
    APP_SYS_LOG_INFO("end");
}

/*@brief 手势模组事件流程处理
 *       内部使用: 被mix custom线程使用
 */
void app_module_gesture_event_update(void)
{
    APP_SYS_LOG_INFO("start");
    app_module_gesture_proc_exec();
    APP_SYS_LOG_INFO("end");
}

/*@brief 启动手势
 *       此时手势模组工作在轮询模式,中断响应关闭
 */
void app_module_gesture_start(void)
{
    app_sys_timer_start(&app_module_gesture_timer);
    APP_SYS_LOG_WARN("");
}

/*@brief 关闭手势
 *       此时手势模组工作在中断模式,轮询响应关闭
 */
void app_module_gesture_stop(void)
{
    app_sys_timer_stop(&app_module_gesture_timer);
    APP_SYS_LOG_WARN("");
}

/*@brief 手势软件定时器模组回调
 */
static void app_module_gesture_timer_handler(void *timer)
{
    app_thread_package_t package = {
        .thread   = app_thread_id_mix_irq,
        .module   = app_thread_mix_irq_gesture,
        .event    = app_thread_mix_irq_gesture_xmsec_update,
    };
    app_thread_package_notify(&package);
}

/*@brief 手势模组事件中断回调
 */
static void app_module_gesture_1ms_cb(void)
{
    app_thread_package_t package = {
        .thread   = app_thread_id_mix_irq,
        .module   = app_thread_mix_irq_gesture,
        .event    = app_thread_mix_irq_gesture_event,
        .priority = app_thread_package_priority_highest,
    };
    app_thread_package_notify(&package);
}

/*@brief 初始化手势模组
 */
void app_module_gesture_ready(void)
{
    app_module_gesture_timer.expired = app_module_gesture_timer_handler;
    app_module_gesture_timer.peroid  = APP_MODULE_GESTURE_XMSEC;
    app_module_gesture_timer.reload  = true;
    app_dev_gesture_ready(&app_dev_gesture);
    app_dev_gesture_irq_cb_reg(&app_dev_gesture, app_module_gesture_1ms_cb);
    app_dev_gesture_irq_switch(&app_dev_gesture, false);
}
