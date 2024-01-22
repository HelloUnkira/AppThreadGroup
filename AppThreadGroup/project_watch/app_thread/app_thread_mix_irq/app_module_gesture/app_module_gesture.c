/*实现目标:
 *    手势相关的功能组件
 *    对于手表设备而言手势功能较为复杂
 *    帧数据来源一般是重力加速度计生成
 *    帧数据经过最基本的数据滤波后
 *    触发不同的算法流程生成不同的数据
 *    正常模式下本设备是一直工作的
 *    低功耗模式下只由事件触发一个周期
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_arithmetic_lib.h"

static app_mutex_t app_module_gesture_mutex = {0};
static app_sys_timer_t app_module_gesture_timer = {0};
static app_module_gesture_t app_module_gesture = {0};

/*@brief 手势方向调整(以屏幕正常中心显示为视角)
 *       z轴:相对屏幕平面垂直为所在方向(负:垂直射入,正:垂直射出)
 *       x轴:屏幕的宽(水平方向)所在方向(负:方向向左,正:方向向右)
 *       y轴:屏幕的高(垂直方向)所在方向(负:方向向上,正:方向向下)
 */
static void app_module_gesture_adjust_dir(int32_t xyz_frame[3])
{
   /*x:*/ xyz_frame[0] = xyz_frame[0];
   /*y:*/ xyz_frame[1] = xyz_frame[1];
   /*z:*/ xyz_frame[2] = xyz_frame[2];
}

/*@brief 手势分析模组流程处理
 */
static void app_module_gesture_proc_exec(void)
{
    /*
     *手势设备帧数据采集
     */
    uint8_t gesture_length = 0;
    int32_t gesture_frame[APP_EXT_DEV_GESTURE_FRAME_LIMIT][3] = {0};
    app_dev_gesture_proc_exec(&app_dev_gesture);
    app_dev_gesture_get_frame(&app_dev_gesture, gesture_frame, &gesture_length);
    if (APP_EXT_DEV_GESTURE_FRAME_LIMIT > gesture_length)
        APP_SYS_LOG_INFO("frame is incomplete");
    if (gesture_length == 0)
        return;
        APP_SYS_LOG_DEBUG_RAW("frame<%d><x,y,z>:", gesture_length);
    for (uint8_t idx = 0; idx < gesture_length; idx++)
        APP_SYS_LOG_DEBUG_RAW("<0x%x,0x%x,0x%x>", gesture_frame[idx][0], gesture_frame[idx][1], gesture_frame[idx][2]);
        APP_SYS_LOG_DEBUG_RAW(app_sys_log_line());
    /* 手势数据方向归一化 */
    for (uint8_t idx = 0; idx < gesture_length; idx++)
        app_module_gesture_adjust_dir(gesture_frame[idx]);
    /*  */
    /*
     *算法处理:
     */
    /* 运动算法 */
    static app_arithmetic_sport_t sport = {0};
    app_arithmetic_sport_proc(&sport, gesture_frame, gesture_length);
    /* 其余手势算法 */
    app_arithmetic_gesture_wrist(gesture_frame, gesture_length);
    app_arithmetic_gesture_shake(gesture_frame, gesture_length);
    /* 睡眠算法 */
    
    /* 其他需要使用本传感器的算法,帧数据同步 */
}

/*@brief 启动手势
 *       此时手势模组工作在轮询模式,中断响应关闭
 */
void app_module_gesture_start(void)
{
    app_sys_timer_start(&app_module_gesture_timer);
}

/*@brief 关闭手势
 *       此时手势模组工作在中断模式,轮询响应关闭
 */
void app_module_gesture_stop(void)
{
    app_sys_timer_stop(&app_module_gesture_timer);
}

/*@brief 手势模组事件处理
 *       内部使用: 被mix custom线程使用
 */
void app_module_gesture_xmsec_update(void)
{
    app_module_gesture_proc_exec();
}

/*@brief 手势模组事件处理
 *       内部使用: 被mix custom线程使用
 */
void app_module_gesture_event_update(void)
{
    app_module_gesture_proc_exec();
}

/*@brief 手势分析软件定时器模组回调
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
static void app_module_gesture_irq_cb(void)
{
    app_thread_package_t package = {
        .thread   = app_thread_id_mix_irq,
        .module   = app_thread_mix_irq_gesture,
        .event    = app_thread_mix_irq_gesture_event_update,
    };
    app_thread_package_notify(&package);
}

/*@brief 初始化手势模组
 */
void app_module_gesture_ready(void)
{
    app_mutex_process(&app_module_gesture_mutex, app_mutex_static);
    app_dev_gesture_ready(&app_dev_gesture);
    app_dev_gesture_irq_cb_reg(&app_dev_gesture, app_module_gesture_irq_cb);
    app_dev_gesture_irq_switch(&app_dev_gesture, false);
    app_module_gesture_timer.expired = app_module_gesture_timer_handler;
    app_module_gesture_timer.peroid  = APP_MODULE_GESTURE_XMSEC;
    app_module_gesture_timer.reload  = true;
}
