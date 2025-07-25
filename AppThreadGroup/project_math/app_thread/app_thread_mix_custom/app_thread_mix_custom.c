/*实现目标:
 *    杂项事件不归属与其他线程模组时放于此处
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

static bool app_thread_task_test_flag = true;
static app_sys_timer_t app_thread_task_test_timer = {0};
static app_thread_task_queue_t *app_thread_task_test_queue = NULL;
static app_thread_task_t *app_thread_task_test1 = NULL;
static app_thread_task_t *app_thread_task_test2 = NULL;
static app_thread_task_t *app_thread_task_test3 = NULL;

/*@brief mix custom test子线程 子任务
 */
static void app_thread_mix_custom_test_task1_routine(void)
{
    APP_SYS_LOG_INFO("");
}

/*@brief mix custom test子线程 子任务
 */
static void app_thread_mix_custom_test_task2_routine(void)
{
    APP_SYS_LOG_INFO("");
}

/*@brief mix custom test子线程 子任务
 */
static void app_thread_mix_custom_test_task3_routine(void)
{
    APP_SYS_LOG_INFO("");
}

/*@brief mix custom test子线程 子任务 触发源
 */
static void app_thread_task_test_timer_handler(void *timer)
{
    APP_SYS_LOG_INFO("");
    static uint32_t count = 0;
    count++;
    
    if (count % 1 == 0)
        app_thread_task_toggle(app_thread_task_test_queue, app_thread_task_test1);
    if (count % 2 == 0)
        app_thread_task_toggle(app_thread_task_test_queue, app_thread_task_test2);
    if (count % 3 == 0)
        app_thread_task_toggle(app_thread_task_test_queue, app_thread_task_test3);
    
    if (count >= 10) {
        app_thread_task_test_flag = false;
        app_thread_task_queue_notify(app_thread_task_test_queue);
    }
}

/*@brief mix custom test子线程
 */
static APP_THREAD_GROUP_HANDLER(app_thread_mix_custom_test_routine)
{
    app_thread_task_test_queue = app_thread_task_queue_create();
    app_thread_task_test1 = app_thread_task_commit(app_thread_task_test_queue, app_thread_mix_custom_test_task1_routine, 1);
    app_thread_task_test2 = app_thread_task_commit(app_thread_task_test_queue, app_thread_mix_custom_test_task2_routine, 2);
    app_thread_task_test3 = app_thread_task_commit(app_thread_task_test_queue, app_thread_mix_custom_test_task3_routine, 3);
    
    app_thread_task_test_timer.expired = app_thread_task_test_timer_handler;
    app_thread_task_test_timer.peroid  = 1000;
    app_thread_task_test_timer.reload  = true;
    app_sys_timer_start(&app_thread_task_test_timer);
    
    #if 1
    while (app_thread_task_test_flag) {
        app_thread_task_queue_wait(app_thread_task_test_queue);
        app_thread_task_queue_schedule(app_thread_task_test_queue, false);
    }
    #else
    app_thread_task_queue_schedule(app_thread_task_test_queue, true);
    #endif
    
    app_thread_task_queue_destroy(app_thread_task_test_queue);
    app_sys_timer_stop(&app_thread_task_test_timer);
}

/*@brief 子线程服务例程就绪部
 */
static void app_thread_mix_custom_routine_ready_cb(void)
{
    /* 相关模组初始化 */
    
    /* 创建test子线程 */
    static app_thread_t app_thread_mix_custom_test = {0};
    app_thread_group_create(&app_thread_mix_custom, &app_thread_mix_custom_test, app_thread_mix_custom_test_routine);
    app_thread_process(&app_thread_mix_custom_test, app_thread_static);
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_mix_custom_routine_package_cb(app_thread_package_t *package, bool *record)
{
    switch (package->module) {
    case app_thread_mix_custom_data_dump: {
        /* 将系统敏感的资源转储到外存 */
        app_module_data_dump_process();
        return true;
    }
    case app_thread_mix_custom_data_load: {
        /* 将系统敏感的资源加载到内存 */
        app_module_data_load_process();
        return true;
    }
    default:
        break;
    }
    return false;
}

/*@brief 混合事件线程服务例程
 */
APP_THREAD_GROUP_HANDLER(app_thread_mix_custom_routine)
{
    app_thread_slave_process(app_thread_id_mix_custom,
                             app_thread_mix_custom_routine_ready_cb,
                             app_thread_mix_custom_routine_package_cb);
}
