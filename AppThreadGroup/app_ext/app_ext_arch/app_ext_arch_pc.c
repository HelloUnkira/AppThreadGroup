/* 实现目标:
 *     APP需要对系统进行的封装体
 *     APP会在一个专门的地点和时刻准备这些资源
 *     对所有平台做一个全通配的最低配置
 */

#include "app_ext_lib.h"

#if APP_ARCH_IS_PC

/*@brief 初始化平台
 */
void app_arch_ready(void)
{
    /* rand seed */
    srand(time(0));
    rand();
    /* cpu freq and clock freq */
    
    /* peripheral: log(custom device) */
    app_arch_log_ready(&app_arch_log);
    
}

/*@brief 运行平台
 */
void app_arch_execute(void)
{
}

#endif
