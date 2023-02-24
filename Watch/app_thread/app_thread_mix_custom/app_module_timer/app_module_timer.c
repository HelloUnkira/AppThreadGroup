/*实现目标:
 *    软件定时器相关的功能组件
 *    备注:某些平台没有软件定时器
 *         所以做一个效果类似的软件定时器
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_mix_custom.h"
#include "app_module_timer.h"

static app_mutex_t app_module_timer_mutex = {0};
static app_module_timer_t *app_module_timer_list = NULL;

/*@brief     停止,中止,终止软件定时器(中断环境下不可调用)
 *@param[in] timer 定时器实例
 *@retval    执行是否有效
 */
bool app_module_timer_stop(app_module_timer_t *timer)
{
    bool retval = false;
    app_module_timer_t *current = NULL;
    app_mutex_take(&app_module_timer_mutex);
    /* 检查等待者队列 */
    if (app_module_timer_list != NULL) {
        /* 检查第一个等待者 */
        if (app_module_timer_list == timer) {
            app_module_timer_list  = timer->near;
            /* 如果后面还有等待者,需要将约减值累加到后面 */
            if (app_module_timer_list != NULL)
                app_module_timer_list->reduce += timer->reduce;
            retval = true;
        } else {
            current = app_module_timer_list;
            /* 遍历等待者队列 */
            while (current->near != NULL) {
                /* 如果下一等待着是目标 */
                if (current->near == timer) {
                    current->near  = timer->near;
                    /* 如果后面还有等待者,需要将约减值累加到后面 */
                    if (current->near != NULL)
                        ((app_module_timer_t *)current->near)->reduce += timer->reduce;
                    retval = true;
                    break;
                }
                /* 检查下一个等待者 */
                current = current->near;
            }
        }
    }
    app_mutex_give(&app_module_timer_mutex);
    return retval;
}

/*@brief     启动软件定时器(中断环境下不可调用)
 *@param[in] timer 定时器实例
 *@retval    执行是否有效
 */
bool app_module_timer_start(app_module_timer_t *timer)
{
    bool status = true;
    app_module_timer_t *current = NULL;
    if (timer->expired == NULL || timer->peroid == 0)
        return false;
    /* 初始化软件定时器 */
    app_mutex_take(&app_module_timer_mutex);
    /* 预检查:不要出现相同参数的软件定时器 */
    if (status) {
        current = app_module_timer_list;
        /* 遍历等待者队列 */
        while (current != NULL) {
            /* 如果某一等待者与目标参数相同 */
            if (current == timer) {
                status = false;
                break;
            }
            /* 检查下一个等待者 */
            current = current->near;
        }
    }
    /* 检查等待者队列 */
    if (status) {
        timer->near = NULL;
        timer->reduce = timer->peroid;
        if (app_module_timer_list == NULL) {
            app_module_timer_list  = timer;
            status = false;
        }
    }
    /* 比较第一个等待者 */
    if (status)
    if (app_module_timer_list->reduce >= timer->reduce) {
        app_module_timer_list->reduce -= timer->reduce;
        timer->near = app_module_timer_list;
        app_module_timer_list = timer;
        status = false;
    }
    /* 遍历等待者队列 */
    if (status) {
        current = app_module_timer_list;
        timer->reduce -= current->reduce;
        /* 遍历等待者队列 */
        while (current->near != NULL) {
            /* 如果下一等待着不是目标 */
            if (timer->reduce >= ((app_module_timer_t *)current->near)->reduce) {
                timer->reduce -= ((app_module_timer_t *)current->near)->reduce;
                /* 检查下一个等待者 */
                current = current->near;
                continue;
            }
            /* 如果下一等待着是目标 */
            ((app_module_timer_t *)current->near)->reduce -= timer->reduce;
            timer->near = current->near;
            current->near = timer;
            status = false;
            break;
        }
    }
    /* 添加到末尾 */
    if (status)
        current->near = timer;
    /*  */
    app_mutex_give(&app_module_timer_mutex);
    /*  */
    return true;
}

/*@brief 约减软件定时器
 *       内部使用: 被mix custom线程使用
 */
void app_module_timer_reduce(void)
{
    bool loop = true;
    while (loop) {
        bool status = true;
        app_module_timer_t *timer = NULL;
        loop = false;
        app_mutex_take(&app_module_timer_mutex);
        /* 检查等待者队列 */
        if (status)
        if (app_module_timer_list == NULL)
            status = false;
        /* 约减首项等待者 */
        if (status)
        if (app_module_timer_list->reduce != 0)
            app_module_timer_list->reduce--;
        /* 约减首项等待者 */
        if (status)
        if (app_module_timer_list->reduce == 0) {
            timer = app_module_timer_list;
            app_module_timer_list = timer->near;
            /* 如果还能继续约减 */
            if (app_module_timer_list != NULL)
            if (app_module_timer_list->reduce == 0)
                loop = true;
        }
        app_mutex_give(&app_module_timer_mutex);
        /* 处理该约减者 */
        if (timer != NULL) {
            /* 检查是否需要重加载 */
            if (timer->reload)
                app_module_timer_start(timer);
            /* 执行过期回调 */
            timer->expired(timer);
        }
    }
}

/*@brief 定时器模组更新
 */
void app_module_timer_1ms_update(void)
{
    app_package_t package = {
        .send_tid = app_thread_id_unknown,
        .recv_tid = app_thread_id_mix_custom,
        .module   = app_thread_mix_custom_timer,
        .event    = app_thread_mix_custom_timer_reduce_update,
        .dynamic  = false,
        .size     = 0,
        .data     = NULL,
    };
    app_package_notify(&package);
}

/*@brief 软件定时器模组初始化
 *       内部使用: 被mix custom线程使用
 */
void app_module_timer_ready(void)
{
    app_mutex_process(&app_module_timer_mutex);
}
