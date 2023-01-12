/*实现目标:
 *    断言相关的功能组件
 */

#define APP_OS_LOG_LOCAL_STATUS     1
#define APP_OS_LOG_LOCAL_LEVEL      1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_os_log.h"
#include "app_sys_pipe.h"
#include "app_thread_master.h"
#include "app_thread_source_manage.h"
#include "app_module_system.h"
#include "app_module_clock.h"
#include "app_module_dump.h"
#include "app_module_load.h"

static    bool app_module_system_dump = false;
static uint8_t app_module_system_delay = 0;
static uint8_t app_module_system_status = 0;
static app_mutex_t app_module_system_mutex = {0};

/*@brief 设置系统转储成功标记
 */
void app_module_system_dump_set(bool over)
{
    app_mutex_take(&app_module_system_mutex);
    app_module_system_dump = over;
    app_mutex_give(&app_module_system_mutex);
}

/*@brief     设置系统延时
 *@param[in] delay 系统延时秒数
 */
void app_module_system_delay_set(uint8_t delay)
{
    app_mutex_take(&app_module_system_mutex);
    app_module_system_delay = delay;
    app_mutex_give(&app_module_system_mutex);
}

/*@brief     设置系统状态
 *@param[in] status 系统状态枚举量
 */
void app_module_system_status_set(uint8_t status)
{
    app_mutex_take(&app_module_system_mutex);
    app_module_system_status = status;
    app_mutex_give(&app_module_system_mutex);
}

/*@brief  获取系统状态
 *@retval 系统状态枚举量
 */
uint8_t app_module_system_status_get(void)
{
    app_mutex_take(&app_module_system_mutex);
    uint8_t status = app_module_system_status;
    app_mutex_give(&app_module_system_mutex);
    return status;
}

/*@brief     系统状态控制更新
 *@param[in] clock 时钟实例
 */
void app_module_system_ctrl_check(app_module_clock_t clock[1])
{
    static bool not_dump_yet = true;
    static bool not_load_yet = true;
    app_mutex_take(&app_module_system_mutex);
    bool      dump = app_module_system_dump;
    uint8_t  delay = app_module_system_delay;
    uint8_t status = app_module_system_status;
    bool  is_valid = app_module_system_status == app_module_system_valid;
    app_mutex_give(&app_module_system_mutex);
    /* 执行加载 */
    if (not_load_yet) {
        not_load_yet = false;
        /* 向线程发送加载事件 */
        app_package_t package = {
            .send_tid = app_thread_id_unknown,
            .recv_tid = app_thread_id_source_manage,
            .module   = app_thread_source_manage_load,
            .event    = 0,
            .dynamic  = false,
            .size     = 0,
            .data     = NULL,
        };
        app_thread_package_notify(&package);
    }
    if (is_valid)
        return;
    /* 执行转储 */
    if (not_dump_yet) {
        not_dump_yet = false;
        /* 向线程发送转储事件 */
        app_package_t package = {
            .send_tid = app_thread_id_unknown,
            .recv_tid = app_thread_id_source_manage,
            .module   = app_thread_source_manage_dump,
            .event    = 0,
            .dynamic  = false,
            .size     = 0,
            .data     = NULL,
        };
        app_thread_package_notify(&package);
    }
    /* 延时到期,数据转储结束,重启系统 */
    if (!delay && dump) {
        app_os_reset();
        app_module_system_dump_set(false);
    }
    /* 保证数据转储成功后再进行重启 */
    if (!delay)
        return;
    /* 重启系统倒计时 */
    app_mutex_take(&app_module_system_mutex);
    app_module_system_delay--;
    app_mutex_give(&app_module_system_mutex);
}

/*@brief 初始化系统模组
 */
void app_module_system(void)
{
    app_mutex_process(&app_module_system_mutex);
}

/*@brief     断言
 *@param[in] file 文件名
 *@param[in] line 文件行数
 *@param[in] cond 断言条件
 */
void app_module_assert(char *file, uint32_t line, bool cond)
{
    if (cond)
        return;
    /* 输出错误信息 */
    APP_OS_LOG_ERROR("app_module_assert:[%s][%d]", file, line);
    /* 异常导致的错误直接重启系统(不转储信息) */
    app_os_reset();
}
