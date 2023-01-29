/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_module_protocol.h"

/*@brief     协议适配层,接收协议数据
 *@param[in] ptl_pkg 动态协议包
 */
void app_adaptor_transfer_rx(app_module_protocol_package_t *ptl_pkg)
{
    /* 如果协议包一次接收不完全,第一次接收为协议包本体 */
    /* 后面是协议包携带的数据,本地需要动态生成空间存储它 */
}

/*@brief     协议适配层,发送协议数据
 *@param[in] ptl_pkg 动态协议包
 */
void app_adaptor_transfer_tx(app_module_protocol_package_t *ptl_pkg)
{
    /* 第一次发送为协议包本体 */
    /* 后面是协议包携带的数据,本地需要使用结束后销毁它 */
}
