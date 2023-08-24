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
    /* cpu freq and clock freq */
    
}

/*@brief 重启平台
 */
void app_arch_reset(void)
{
    exit(-1);
    while (true);
}

/*@brief     重启平台
 *@param[in] ms 延时时间(ms)
 */
void app_arch_reset_delay_ms(uint32_t ms)
{
    app_delay_ms(ms);
    exit(-1);
    while (true);
}

#endif
