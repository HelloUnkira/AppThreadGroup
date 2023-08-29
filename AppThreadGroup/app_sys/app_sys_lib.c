/* 实现目标:
 *     层级接口打包
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 初始化系统
 */
void app_sys_ready(void)
{
    /* 就绪系统子模组 */
    app_sys_log_ready();
    app_sys_ext_mem_ready();
    app_sys_ext_mem_layout();
    app_sys_log_text_ready();
    app_sys_pipe_src_ready();
    app_sys_timer_ready();
    app_sys_build_time();
}
