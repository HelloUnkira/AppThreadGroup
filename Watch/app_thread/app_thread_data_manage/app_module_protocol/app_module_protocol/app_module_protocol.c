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
#include "app_thread_data_manage.h"
#include "app_module_protocol.h"
#define   APP_MODULE_PROTOCOL_CB_H
#include "app_module_protocol_cb.h"

static const uint32_t app_module_protocol_cb_size = 
               sizeof(app_module_protocol_cb) /
               sizeof(app_module_protocol_cb[0]);

/*@brief     解析已经接收的协议包
 *@param[in] ptl_pkg 协议包
 *@param[in] ptl_dat 协议包数据
 */
void app_module_protocol_rx(app_module_protocol_pkg_t *ptl_pkg, uint8_t *ptl_dat)
{
    bool retval = false;
    for (uint32_t idx = 0; idx < app_module_protocol_cb_size; idx++)
        if (retval = app_module_protocol_cb[idx].rx_pkg_cb(ptl_pkg, ptl_dat))
            break;
    if (!retval) {
        APP_SYS_LOG_ERROR("command:%u", ptl_pkg->command);
        APP_SYS_LOG_ERROR("event:%u", ptl_pkg->event);
    }
}

/*@brief      填充准备发送的协议包
 *@param[in]  ptl_pkg 协议包
 *@param[out] ptl_dat 协议包数据
 */
void app_module_protocol_tx(app_module_protocol_pkg_t *ptl_pkg, uint8_t **ptl_dat)
{
    bool retval = false;
    for (uint32_t idx = 0; idx < app_module_protocol_cb_size; idx++)
        if (retval = app_module_protocol_cb[idx].tx_pkg_cb(ptl_pkg, ptl_dat))
            break;
    if (!retval) {
        APP_SYS_LOG_ERROR("command:%u", ptl_pkg->command);
        APP_SYS_LOG_ERROR("event:%u", ptl_pkg->event);
    }
}

/*@brief     协议传输空回调样例
 *@param[in] ptl_pkg 动态协议包
 *@param[in] ptl_dat 协议包数据
 */
bool app_module_protocol_rx_empty(app_module_protocol_pkg_t *ptl_pkg, uint8_t *ptl_dat)
{
    /* 根据ptl_pkg解析ptl_dat字段 */
    /* 注意ptl_dat由外部app_mem_alloc出来,本地进行app_mem_free销毁 */
    /* 如果处理协议包成功,返回true,否则false */
    return false;
}

/*@brief      协议传输空回调样例
 *@param[in]  ptl_pkg 动态协议包
 *@param[out] ptl_dat 协议包数据
 */
bool app_module_protocol_tx_empty(app_module_protocol_pkg_t *ptl_pkg, uint8_t **ptl_dat)
{
    /* 根据ptl_pkg填充ptl_dat字段 */
    /* 注意ptl_dat由本地app_mem_alloc出来,外部进行app_mem_free销毁 */
    /* 如果处理协议包成功,返回true,否则false */
    return false;
}
