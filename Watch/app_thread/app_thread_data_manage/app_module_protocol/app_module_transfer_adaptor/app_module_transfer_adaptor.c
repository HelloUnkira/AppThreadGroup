/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_pipe.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_data_manage.h"
#include "app_module_protocol.h"
#include "app_module_transfer.h"
#include "app_module_transfer_adaptor.h"

/*@brief     协议适配层,接收协议数据
 *@param[in] data 数据流
 *@param[in] size 数据流大小
 */
void app_module_transfer_adaptor_rx(uint8_t *data, uint32_t size)
{
    /* 默认接收的数据包为动态 */
    uint8_t *tsf_pkg = app_mem_alloc(size);
    uint32_t tsf_pkg_size = size;
    memcpy(tsf_pkg, data, size);
    /* 发送给线程协议处理模组 */
    app_package_t package = {
        .send_tid = app_thread_id_unknown,
        .recv_tid = app_thread_id_data_manage,
        .module   = app_thread_data_manage_transfer,
        .event    = app_thread_data_manage_transfer_rx,
        .dynamic  = true,
        .size     = tsf_pkg_size,
        .data     = tsf_pkg,
    };
    app_thread_package_notify(&package);
}

/*@brief     协议适配层,发送协议数据
 *@param[in] data 数据流
 *@param[in] size 数据流大小
 */
void app_module_transfer_adaptor_tx(uint8_t *data, uint32_t size)
{
    /* 本地阻塞发送(data, size) */
}

/*@brief      协议适配层,发送协议数据最大大小
 *@retval[in] 数据流最大大小
 */
uint32_t app_module_transfer_adaptor_tx_max(void)
{
    return 0;
}
