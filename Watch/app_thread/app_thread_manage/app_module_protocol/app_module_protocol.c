/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_manage.h"
#include "app_module_transfer.h"
#include "app_module_transfer_mix.h"
#include "app_module_protocol.h"

#include "cJSON.h"

/*@brief     传输协议
 *@param[in] type   传输类型
 *@param[in] status 传输流程状态,内部约定
 */
void app_module_protocol_notify(uint32_t type, uint32_t status)
{
    switch (type) {
    case app_module_protocol_system_clock: {
         app_module_transfer_notify_system_clock();
         break;
    }
    default: {
        #if APP_SYS_LOG_MODULE_CHECK
        APP_SYS_LOG_ERROR("protocol have unknown type:%d", type);
        #endif
        break;
    }
    }
}
