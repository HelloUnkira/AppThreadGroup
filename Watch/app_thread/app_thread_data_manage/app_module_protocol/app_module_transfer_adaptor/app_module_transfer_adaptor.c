/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"

/*@brief     协议适配层,接收协议数据
 *@param[in] data 数据流
 *@param[in] size 数据流大小
 */
void app_module_transfer_adaptor_rx(uint8_t *data, uint32_t size)
{
    /* 如果协议包一次接收不完全,第一次接收为协议包本体 */
    /* 后面是协议包携带的数据,本地需要动态生成空间存储它 */
}

/*@brief     协议适配层,发送协议数据
 *@param[in] data 数据流
 *@param[in] size 数据流大小
 */
void app_module_transfer_adaptor_tx(uint8_t *data, uint32_t size)
{
    /* 第一次发送为协议包本体 */
    /* 后面是协议包携带的数据,本地需要使用结束后销毁它 */
}

/*@brief      协议适配层,发送协议数据最大大小
 *@retval[in] 数据流最大大小
 */
uint32_t app_module_transfer_adaptor_tx_size_max(void)
{
}
