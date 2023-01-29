/*实现目标:
 *    协议体流程的通配层
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_thread_master.h"
#include "app_thread_protocol.h"
#include "app_module_protocol.h"
#define   APP_MODULE_PROTOCOL_CB_H
#include "app_module_protocol_cb.h"

static const uint32_t app_module_protocol_cb_size = 
               sizeof(app_module_protocol_cb) /
               sizeof(app_module_protocol_cb[0]);

/*@brief     填充准备发送的协议包
 *@param[in] ptl_pkg 动态协议包
 */
void app_module_protocol_tx(app_module_protocol_package_t *ptl_pkg)
{
    for (uint32_t idx = 0; idx < app_module_protocol_cb_size; idx++)
        app_module_protocol_cb[idx].tx_pkg_cb(ptl_pkg);
}

/*@brief     解析已经接收的协议包
 *@param[in] ptl_pkg 动态协议包
 */
void app_module_protocol_rx(app_module_protocol_package_t *ptl_pkg)
{
    for (uint32_t idx = 0; idx < app_module_protocol_cb_size; idx++)
        app_module_protocol_cb[idx].rx_pkg_cb(ptl_pkg);
}

/*@brief     协议传输空回调
 *@param[in] ptl_pkg 动态协议包
 */
void app_module_protocol_tx_empty(app_module_protocol_package_t *ptl_pkg)
{
    /* 填充前需要动态生成ptl_pkg->data字段 */
}

/*@brief     协议传输空回调
 *@param[in] ptl_pkg 动态协议包
 */
void app_module_protocol_rx_empty(app_module_protocol_package_t *ptl_pkg)
{
    /* 解析后需要动态销毁ptl_pkg->data字段 */
}
