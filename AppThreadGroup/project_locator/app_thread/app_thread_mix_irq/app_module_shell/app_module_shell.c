/* 实现目标: shell 模组, 定时轮询读取, 接入泛用解析器 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"
#include "app_thread_group.h"
#include "app_module_shell.h"
#include "app_module_shell_parser.h"

#define APP_MODULE_SHELL_POLL_MS    (50)

static app_sys_timer_t app_module_shell_timer = {0};

/*@brief 心跳回调,发送轮询事件
 */
static void app_module_shell_timer_handler(void *timer)
{
    app_thread_package_t package = {
        .thread   = app_thread_id_mix_irq,
        .module   = app_thread_mix_irq_shell,
        .event    = app_thread_mix_irq_shell_poll,
    };
    app_thread_package_notify(&package);
}

/*@brief 写回调 (透传给 parser)
 */
static void app_module_shell_write(const char *buf, uint32_t size, void *priv)
{
    (void)priv;
    app_dev_shell_write(&app_dev_shell, buf, size);
}

/*@brief shell 模组初始化
 */
void app_module_shell_ready(void)
{
    app_dev_shell_ready(&app_dev_shell);
    app_module_shell_parser_ready();

    app_module_shell_timer.expired = app_module_shell_timer_handler;
    app_module_shell_timer.peroid  = APP_MODULE_SHELL_POLL_MS;
    app_module_shell_timer.reload  = true;
    app_sys_timer_start(&app_module_shell_timer);
}

/*@brief shell 模组轮询
 */
void app_module_shell_poll(void)
{
    char     buf[256] = {0};
    uint32_t len = 0;
    uint32_t prompt = 0;

    app_dev_shell_read(&app_dev_shell, buf, sizeof(buf) - 1, &len, &prompt);

    /* 输入轮次: 封锁所有控制台输出 */
    app_sys_log_work(prompt == 0);

    if (len == 0) return;
    buf[len] = '\0';

    /* 交由解析器处理 */
    app_module_shell_parser_exec(buf, app_module_shell_write, NULL);
}
