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

/*@brief  动态协议包可携带数据最大载荷
 *@retval 数据最大载荷
 */
uint32_t app_module_protocol_data_size(void)
{
    /* 这个数字可以改成传输层向上暴露的一个接口以达到动态适配 */
    return (20 - sizeof(app_module_protocol_package_t));
}

/*@brief  动态协议包最大尺寸
 *@retval 协议包最大尺寸
 */
uint32_t app_module_protocol_package_size(void)
{
    return sizeof(app_module_protocol_package_t) +
                  app_module_protocol_data_size();
}

/*@brief     填充准备发送的协议包
 *@param[in] ptl_pkg 动态协议包
 */
static void app_module_protocol_tx(app_module_protocol_package_t *ptl_pkg)
{
    for (uint32_t idx = 0; idx < app_module_protocol_cb_size; idx++)
        app_module_protocol_cb[idx].tx_pkg_make(ptl_pkg);
}

/*@brief     解析已经接收的协议包
 *@param[in] ptl_pkg 动态协议包
 */
static void app_module_protocol_rx(app_module_protocol_package_t *ptl_pkg)
{
    for (uint32_t idx = 0; idx < app_module_protocol_cb_size; idx++)
        app_module_protocol_cb[idx].rx_pkg_parse(ptl_pkg);
}

/*@brief     通知协议包
 *@param[in] ptl_pkg 动态协议包
 */
void app_module_protocol_notify(app_module_protocol_package_t *ptl_pkg)
{
    app_package_t package = {
        .send_tid = app_thread_id_protocol,
        .recv_tid = app_thread_id_protocol,
        .module   = app_thread_protocol_transfer,
        .event    = app_thread_protocol_transfer_rx,
        .dynamic  = true,
        .size     = app_module_protocol_package_size(),
        .data     = ptl_pkg,
    };
    app_thread_package_notify(&package);
}

/*@brief     响应协议包
 *@param[in] ptl_pkg 动态协议包
 */
void app_module_protocol_respond(app_module_protocol_package_t *ptl_pkg)
{
    /* tx的处理 */
    if (ptl_pkg->fake) {
        ptl_pkg->fake = false;
        app_module_protocol_tx(ptl_pkg);
        app_package_t package = {
            .send_tid = app_thread_id_protocol,
            .recv_tid = app_thread_id_protocol,
            .module   = app_thread_protocol_transfer,
            .event    = app_thread_protocol_transfer_rx,
            .dynamic  = true,
            .size     = app_module_protocol_package_size(),
            .data     = ptl_pkg,
        };
        app_thread_package_notify(&package);
        return;
    }
    /* rx的处理 */
    if (ptl_pkg->size != 0) {
        app_module_protocol_rx(ptl_pkg);
        ptl_pkg->size = 0;
        app_package_t package = {
            .send_tid = app_thread_id_protocol,
            .recv_tid = app_thread_id_protocol,
            .module   = app_thread_protocol_transfer,
            .event    = app_thread_protocol_transfer_rx,
            .dynamic  = true,
            .size     = app_module_protocol_package_size(),
            .data     = ptl_pkg,
        };
        app_thread_package_notify(&package);
        return;
    }
    /* tx的应答,通过data和size判断 */
    if (ptl_pkg->size == 0) {
        app_mem_free(ptl_pkg);
        return;
    }
}

/*@brief     协议传输空回调
 *@param[in] ptl_pkg 动态协议包
 */
void app_module_protocol_tx_empty(app_module_protocol_package_t *ptl_pkg)
{
}

/*@brief     协议传输空回调
 *@param[in] ptl_pkg 动态协议包
 */
void app_module_protocol_rx_empty(app_module_protocol_package_t *ptl_pkg)
{
}
