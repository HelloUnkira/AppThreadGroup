/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_manage.h"
#include "app_module_protocol.h"

#if       APP_MODULE_PROTOCOL_USE_NANOPB
#include "pb_common.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "app_pb_msg_set.pb.h"
#include "app_nanopb_xfer.h"
#include "app_nanopb_xfer_mix.h"

/*@brief     nanopb编码流程函数
 *@param[in] message 集成化消息集合体
 *@param[in] buffer  nanopb编码流
 *@param[in] size    nanopb编码流大小
 *@retval    编码有效字节数
 */
static uint32_t app_nanopb_encode(AppPB_MsgSet *message, uint8_t *buffer, uint32_t size)
{
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, size);
    if (!pb_encode(&stream, AppPB_MsgSet_fields, message))
         APP_SYS_LOG_ERROR("encode fail:%s", stream.errmsg);
    return stream.bytes_written;
}

/*@brief     nanopb解码流程函数
 *@param[in] message 集成化消息集合体
 *@param[in] buffer  nanopb编码流
 *@param[in] size    nanopb编码流大小
 *@retval    解码剩余字节数
 */
static uint32_t app_nanopb_decode(AppPB_MsgSet *message, uint8_t *buffer, uint32_t size)
{
    pb_istream_t stream = pb_istream_from_buffer(buffer, size);
    if (!pb_decode(&stream, AppPB_MsgSet_fields, message))
         APP_SYS_LOG_ERROR("decode fail:%s", stream.errmsg);
    return stream.bytes_left;
}

/*@brief     协议适配层,推送协议数据
 *@param[in] message nanopb集合对象
 *@retval    推送是否成功
 */
bool app_nanopb_xfer_notify(AppPB_MsgSet *message)
{
    /* 创建传输缓冲区 */
    uint32_t size = 0;
    uint8_t *buffer = app_mem_alloc(APP_NANOPB_BUFFER_SIZE);
    /* nanopb编码 */
    size = app_nanopb_encode(message, buffer, APP_NANOPB_BUFFER_SIZE);
    /* 传输nanopb数据流 */
    return app_nanopb_xfer_throw(buffer, size);
}

/*@brief     协议适配层,接收协议数据
 *@param[in] buffer nanopb编码流
 *@param[in] size   nanopb编码流大小
 *@retval    解析是否成功
 */
bool app_nanopb_xfer_respond(uint8_t *buffer, uint32_t size)
{
    /* 创建传输对象 */
    AppPB_MsgSet message = AppPB_MsgSet_init_zero;
    /* nanopb解码 */
    size = app_nanopb_encode(&message, buffer, size);
    /* 传输nanopb数据流 */
    return app_nanopb_xfer_dispatch(&message);
}

/*@brief     协议适配层,接收协议数据
 *@param[in] buffer nanopb编码流
 *@param[in] size   nanopb编码流大小
 *@retval    投递是否成功
 */
bool app_nanopb_xfer_throw(uint8_t *buffer, uint32_t size)
{
    /* 检查JSON */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO_RAW("nanopb encode:%d", size);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    for (uint32_t idx = 0; idx < size; idx++)
        APP_SYS_LOG_INFO_RAW("%2x ", buffer[idx]);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    /* 本地回环 */
    #if APP_MODULE_PROTOCOL_LOCAL_LOOPBACK
    uint8_t *data = app_mem_alloc(size);
    memcpy(data, buffer, size);
    app_module_protocol_t protocol = {
        .respond.data    = data,
        .respond.size    = size,
        .respond.dynamic = true,
    };
    app_module_protocol_respond(&protocol);
    #endif
    /* 传给底层,发出去 */
    app_mem_free(buffer);
}

/*@brief     协议适配层,派发协议数据
 *@param[in] message nanopb集合对象
 *@retval    派发是否成功
 */
bool app_nanopb_xfer_dispatch(AppPB_MsgSet *message)
{
    /* 检查nanopb */
    #if APP_SYS_LOG_PROTOCOL_CHECK
    APP_SYS_LOG_INFO_RAW("nanopb type:%d", message->type);
    APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
    #endif
    /* 匹配数据包 */
    switch (message->type) {
    case AppPB_MsgTypeSet_AppPB_MsgType_Is_SystemClock:
        return app_nanopb_xfer_respond_system_clock(message);
    default:
        APP_SYS_LOG_INFO_RAW("unknown nanopb type:%d", message->type);
        APP_SYS_LOG_INFO_RAW(APP_SYS_LOG_LINE);
        return false;
    }
    return false;
}

#endif
