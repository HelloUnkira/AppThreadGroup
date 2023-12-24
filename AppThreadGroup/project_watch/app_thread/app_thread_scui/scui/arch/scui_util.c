/*实现目标:
 *    简要的接口
 *    可选择的适配硬件加速
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 内存拷贝
 */
void scui_mem_copy(void *dst_addr, void *src_addr, uint32_t len)
{
    #if 0
    #elif APP_ARCH_IS_PC
    memcpy(dst_addr, src_addr, len);
    #else
    #endif
}
