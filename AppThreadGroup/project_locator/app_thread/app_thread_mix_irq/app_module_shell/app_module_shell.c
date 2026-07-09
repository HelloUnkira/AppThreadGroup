/* 实现目标: shell 模组, 定时轮询读取 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"
#include "app_thread_group.h"
#include "app_module_shell.h"

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

/*@brief shell 模组初始化
 */
void app_module_shell_ready(void)
{
    app_dev_shell_ready(&app_dev_shell);
    
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
    
    APP_SYS_LOG_INFO("--test--");
    app_dev_shell_read(&app_dev_shell, buf, sizeof(buf) - 1, &len, &prompt);
    
    /* 输入轮次: 封锁所有控制台输出 */
    app_sys_log_work(prompt == 0);
    
    if (len == 0) return;
    buf[len] = '\0';
    
    /* shell 自己的输出走 write, 绕过 log work 开关 */
    app_dev_shell_write(&app_dev_shell, "shell input: ", 13);
    app_dev_shell_write(&app_dev_shell, buf, len);
    app_dev_shell_write(&app_dev_shell, "\r\n", 2);
    
    char *argv[16] = {0};
    uint32_t argc = 0;
    char *tok = strtok(buf, " \t");
    while (tok != NULL && argc < 16) {
        argv[argc++] = tok;
        tok = strtok(NULL, " \t");
    }
}
