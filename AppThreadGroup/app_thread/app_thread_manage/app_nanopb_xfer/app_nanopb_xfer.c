/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_group.h"
#include "app_module_protocol.h"
#include "app_module_transfer.h"

#if       APP_MODULE_PROTOCOL_USE_NANOPB
#include "pb_common.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "app_nanopb_set.pb.h"
#include "app_nanopb_xfer.h"
#include "app_nanopb_xfer_mix.h"

/*@brief     协议适配层,推送协议数据
 *@param[in] message nanopb集合对象
 *@retval    推送是否成功
 */
bool app_nanopb_xfer_notify(AppPB_MsgSet *message)
{
    /* 计算缓冲区的大小 */
    size_t size = 0;
    if (!pb_get_encoded_size(&size, AppPB_MsgSet_fields, message))
         APP_SYS_LOG_ERROR("encode size fail:%d", size);
    /* 创建传输缓冲区 */
    uint8_t *buffer = app_mem_alloc(size);
    /* nanopb编码 */
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, size);
    if (!pb_encode(&stream, AppPB_MsgSet_fields, message))
         APP_SYS_LOG_ERROR("encode fail:%s", stream.errmsg);
    // stream.bytes_written;
    /* 检查nanopb数据流 */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO_RAW("nanopb encode:%d", size);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    for (uint32_t idx = 0; idx < size; idx++)
        APP_SYS_LOG_INFO_RAW("%02x ", buffer[idx]);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    /* 传输nanopb数据流 */
    app_module_transfer_notify(buffer, size);
    /* 回收nanopb缓冲区 */
    app_mem_free(buffer);
}

/*@brief     协议适配层,接收协议数据
 *@param[in] buffer nanopb编码流
 *@param[in] size   nanopb编码流大小
 *@retval    解析是否成功
 */
bool app_nanopb_xfer_respond(uint8_t *buffer, uint32_t size)
{
    /* 检查nanopb */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO_RAW("nanopb decode:%d", size);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    for (uint32_t idx = 0; idx < size; idx++)
        APP_SYS_LOG_INFO_RAW("%02x ", buffer[idx]);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    /* 创建传输对象 */
    AppPB_MsgSet message = AppPB_MsgSet_init_zero;
    /* nanopb解码 */
    pb_istream_t stream = pb_istream_from_buffer(buffer, size);
    if (!pb_decode(&stream, AppPB_MsgSet_fields, &message))
         APP_SYS_LOG_ERROR("decode fail:%s", stream.errmsg);
    // stream.bytes_left;
    /* 检查nanopb */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO_RAW("nanopb type:%d", message.type);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    /* 匹配数据包 */
    bool retval = false;
    switch (message.which_payload) {
    case AppPB_MsgSet_trace_text_tag:
        retval = app_nanopb_xfer_respond_trace_text(&message);
        break;
    case AppPB_MsgSet_system_clock_tag:
        retval = app_nanopb_xfer_respond_system_clock(&message);
        break;
    default:
        APP_SYS_LOG_INFO_RAW("unknown nanopb type:%d", message.type);
        APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
        break;
    }
    return retval;
}

#endif
