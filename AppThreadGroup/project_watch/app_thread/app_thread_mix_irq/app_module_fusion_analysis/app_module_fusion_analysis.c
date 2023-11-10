/*实现目标:
 *    数据融合分析模组
 *    某些算法流程需要多个传感器的数据生成
 *    为了高效的利用传感器生成的数据
 *    多个传感器数据处理流程合并一起
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

static app_sys_timer_t app_module_fusion_analysis_timer = {0};
static app_module_fusion_analysis_t app_module_fusion_analysis = {0};

/*@brief 数据融合分析模组流程处理
 */
static void app_module_fusion_analysis_proc_exec(void)
{
    /*********************************************************************************************/
    /* step1:帧数据采集 */
    /*********************************************************************************************/
    // 手势设备数据采集:
    int32_t gesture_frame[APP_EXT_DEV_GESTURE_FRAME_MAX][3] = {0};
    uint8_t gesture_length = 0;
    app_dev_gesture_proc_exec(&app_dev_gesture);
    app_dev_gesture_get_frame(&app_dev_gesture, gesture_frame, &gesture_length);
    if (APP_EXT_DEV_GESTURE_FRAME_MAX > gesture_length)
        APP_SYS_LOG_INFO("frame is incomplete");
        APP_SYS_LOG_DEBUG_RAW("frame<x,y,z>:");
    for (uint8_t idx = 0; idx < APP_EXT_DEV_GESTURE_FRAME_MAX; idx++)
        APP_SYS_LOG_DEBUG_RAW("<0x%x,0x%x,0x%x> ",
            gesture_frame[idx][0], gesture_frame[idx][1], gesture_frame[idx][2]);
        APP_SYS_LOG_DEBUG_RAW(app_sys_log_line());
    /*********************************************************************************************/
    /* step2:帧数据滤波(通常不建议使用大型滤波器,以节约性能) */
    /*********************************************************************************************/
    /* 这里使用平滑滤波器并直接写下 */
    
    
    
    /*********************************************************************************************/
    /* step3:帧数据投入算法,根据结果,触发对应算法流程 */
    /*********************************************************************************************/
    
    
}

/*@brief 启动数据融合分析
 *       此时手势模组工作在轮询模式,中断响应关闭
 */
void app_module_fusion_analysis_start(void)
{
    APP_SYS_LOG_WARN("");
    app_sys_timer_start(&app_module_fusion_analysis_timer);
}

/*@brief 关闭数据融合分析
 *       此时手势模组工作在中断模式,轮询响应关闭
 */
void app_module_fusion_analysis_stop(void)
{
    APP_SYS_LOG_WARN("");
    app_sys_timer_stop(&app_module_fusion_analysis_timer);
}

/*@brief 数据融合分析模组事件处理
 *       内部使用: 被mix custom线程使用
 */
void app_module_fusion_analysis_xmsec_update(void)
{
    APP_SYS_LOG_INFO("start");
    app_module_fusion_analysis_proc_exec();
    APP_SYS_LOG_INFO("end");
}

/*@brief 数据融合分析软件定时器模组回调
 */
static void app_module_fusion_analysis_timer_handler(void *timer)
{
    app_thread_package_t package = {
        .thread   = app_thread_id_mix_irq,
        .module   = app_thread_mix_irq_fusion_analysis,
        .event    = app_thread_mix_irq_fusion_analysis_xmsec_update,
    };
    app_thread_package_notify(&package);
}

/*@brief 初始化数据融合分析模组
 */
void app_module_fusion_analysis_ready(void)
{
    app_module_fusion_analysis_timer.expired = app_module_fusion_analysis_timer_handler;
    app_module_fusion_analysis_timer.peroid  = APP_MODULE_FUSION_ANALYSIS_XMSEC;
    app_module_fusion_analysis_timer.reload  = true;
}
