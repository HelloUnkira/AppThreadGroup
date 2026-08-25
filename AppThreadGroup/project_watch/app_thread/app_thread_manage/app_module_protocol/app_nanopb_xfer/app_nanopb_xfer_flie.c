/*实现目标:
 *    nanopb协议-文件(flie)消息:通用文件传输引擎(与序列化解耦)的桥接层
 *    引擎只做状态机/校验/应答决策,本模组负责AppPB_File的打包与解析、ACK翻译
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_NANOPB

/* 文件子tag:引擎子对象->AppPB文件子tag */
static const uint16_t app_nanopb_xfer_file_sub_tag[] = {
    AppPB_File_des_tag,     /* SUB_DES */
    AppPB_File_pkg_tag,     /* SUB_PKG */
    AppPB_File_xfer_tag,    /* SUB_XFER */
};

/* 传输字结果:引擎结果->AppPB传输字code */
static AppPB_FileXFer_Code app_nanopb_xfer_file_xfer_code(uint8_t result)
{
    switch (result) {
    case APP_MODULE_XFER_FILE_RESULT_CHK_FAIL: return AppPB_FileXFer_Code_XFER_ERR_CHK;
    case APP_MODULE_XFER_FILE_RESULT_OTHER:    return AppPB_FileXFer_Code_XFER_ERR_OTHER;
    default:                                   return AppPB_FileXFer_Code_XFER_OK;
    }
}

/*@brief 发送文件子消息(高速信道) */
static void app_nanopb_xfer_file_post(AppPB_MsgSet *msg)
{
    app_nanopb_xfer_notify(app_module_transfer_chan_high, msg);
}

/*@brief 原语:发送文件描述符 */
static void app_nanopb_xfer_file_send_des(uint8_t type, const char *name, uint32_t size, uint8_t crc8, uint8_t cks8)
{
    AppPB_MsgSet msg = {
        .which_payload = AppPB_MsgSet_file_tag,
        .payload.file = { .which_payload = AppPB_File_des_tag },
    };
    AppPB_FileDes *des = &msg.payload.file.payload.des;
    memset(des->name, 0, sizeof(des->name));
    snprintf(des->name, sizeof(des->name), "%s", name);
    des->type  = (AppPB_FileDes_TYPE)type;
    des->size  = size;
    des->crc8  = crc8;
    des->cks8  = cks8;
    app_nanopb_xfer_file_post(&msg);
}

/*@brief 原语:发送文件分包 */
static void app_nanopb_xfer_file_send_pkg(uint16_t index, uint32_t base, const uint8_t *data, uint16_t size)
{
    AppPB_MsgSet msg = {
        .which_payload = AppPB_MsgSet_file_tag,
        .payload.file = { .which_payload = AppPB_File_pkg_tag },
    };
    AppPB_FilePkg *pkg = &msg.payload.file.payload.pkg;
    pkg->index = index;
    pkg->base  = base;
    pkg->size  = size;
    memcpy(pkg->data.bytes, data, size);
    pkg->data.size = size;
    app_nanopb_xfer_file_post(&msg);
}

/*@brief 原语:发送文件传输字 */
static void app_nanopb_xfer_file_send_xfer(uint8_t stage, uint8_t result, uint32_t offset)
{
    AppPB_MsgSet msg = {
        .which_payload = AppPB_MsgSet_file_tag,
        .payload.file = { .which_payload = AppPB_File_xfer_tag },
    };
    AppPB_FileXFer *xfer = &msg.payload.file.payload.xfer;
    xfer->type   = (AppPB_FileXFer_Type)stage;
    xfer->code   = app_nanopb_xfer_file_xfer_code(result);
    xfer->offset = offset;
    app_nanopb_xfer_file_post(&msg);
}

/*@brief 原语:发送文件状态同步(并入传输字,type=状态同步)
 *@param offset 对端期望续传位置
 */
static void app_nanopb_xfer_file_send_state(uint32_t offset)
{
    AppPB_MsgSet msg = {
        .which_payload = AppPB_MsgSet_file_tag,
        .payload.file = { .which_payload = AppPB_File_xfer_tag },
    };
    AppPB_FileXFer *xfer = &msg.payload.file.payload.xfer;
    xfer->type   = AppPB_FileXFer_Type_FILE_STATE;
    xfer->code   = AppPB_FileXFer_Code_XFER_OK;
    xfer->offset = offset;
    app_nanopb_xfer_file_post(&msg);
}

/*@brief 原语:发送文件应答ack
 *@param result 引擎结果码(错误类型)
 *@param sub    引擎子对象
 *@param repeat 是否请求重发
 */
static void app_nanopb_xfer_file_send_ack(uint8_t result, uint8_t sub, bool repeat)
{
    AppPB_ACK_Code code = (result == APP_MODULE_XFER_FILE_RESULT_OK) ? AppPB_ACK_Code_SUCCEED
                        : (result == APP_MODULE_XFER_FILE_RESULT_CHK_FAIL) ? AppPB_ACK_Code_CRC_FAILED
                        : AppPB_ACK_Code_PKG_INVALID;
    AppPB_ACK_Info info = repeat ? AppPB_ACK_Info_REQUEST_REPEAT : AppPB_ACK_Info_NO_QUESTION;
    app_nanopb_xfer_notify_ack_async(code, info, app_nanopb_xfer_file_sub_tag[sub]);
}

/*@brief 原语:投递文件传输轮询步进到manage线程 */
static void app_nanopb_xfer_file_post_step(void)
{
    app_module_protocol_t protocol = {.status = 0,};
    protocol.type = app_module_protocol_file_step;
    app_module_protocol_notify(&protocol, 0);
}

/*@brief 文件发送完成:向通用确认引擎通报(ACK推进式测试据此进入下一指令)
 *@param ok 是否成功
 */
static void app_nanopb_xfer_file_done(bool ok)
{
    app_nanopb_xfer_ctrl_notify_done(AppPB_MsgSet_file_tag, ok);
}

/*@brief 初始化文件传输桥接(注入nanopb原语到通用引擎)
 */
void app_nanopb_xfer_file_init(void)
{
    static const app_module_xfer_file_ops_t ops = {
        .chunk      = APP_MODULE_XFER_FILE_CHUNK_SIZE,
        .send_des   = app_nanopb_xfer_file_send_des,
        .send_pkg   = app_nanopb_xfer_file_send_pkg,
        .send_xfer  = app_nanopb_xfer_file_send_xfer,
        .send_state = app_nanopb_xfer_file_send_state,
        .send_ack   = app_nanopb_xfer_file_send_ack,
        .post_step  = app_nanopb_xfer_file_post_step,
    };
    app_module_xfer_file_config(&ops);
    app_module_xfer_file_set_done(app_nanopb_xfer_file_done);
}

/*@brief 打包传输文件(日志上传)
 */
void app_nanopb_xfer_notify_file(void)
{
#if APP_MODULE_PROTOCOL_TEST_ENABLE
    const char *name = "loopback.log";
#else
    const char *name = "app.log"; /* 等待系统适配:填充真实日志文件名 */
#endif
    app_module_xfer_file_notify(name, APP_MODULE_XFER_FILE_TYPE_LOG);
}

/*@brief 文件传输轮询步进
 */
void app_nanopb_xfer_file_step(void)
{
    app_module_xfer_file_step();
}

/*@brief 传输接收应答注入(文件发送状态机推进)
 *@param ack 应答消息
 */
void app_nanopb_xfer_file_ack(AppPB_ACK *ack)
{
    uint8_t result = (ack->code == AppPB_ACK_Code_SUCCEED) ? APP_MODULE_XFER_FILE_RESULT_OK : APP_MODULE_XFER_FILE_RESULT_OTHER;
    app_module_xfer_file_ack_respond(result);
}

/*@brief 传输接收文件(开始/包/结束):解析并交引擎校验/应答
 */
bool app_nanopb_xfer_respond_file(AppPB_MsgSet *message)
{
    AppPB_File *file = &message->payload.file;
    switch (file->which_payload) {
    case AppPB_File_des_tag: {
        AppPB_FileDes *des = &file->payload.des;
        return app_module_xfer_file_recv_start((uint8_t)des->type, des->name, des->size, des->crc8, des->cks8);
    }
    case AppPB_File_pkg_tag: {
        AppPB_FilePkg *pkg = &file->payload.pkg;
        return app_module_xfer_file_recv_pkg(pkg->index, pkg->base, pkg->data.bytes, pkg->data.size);
    }
    case AppPB_File_xfer_tag: {
        AppPB_FileXFer *xfer = &file->payload.xfer;
        /* 发送方:状态同步 -> 按期望续传位置续发 */
        if (xfer->type == AppPB_FileXFer_Type_FILE_STATE) {
            app_module_xfer_file_state_respond((uint16_t)xfer->offset);
            return true;
        }
        /* 发送方:断点反馈 -> 续传 */
        if (xfer->code == AppPB_FileXFer_Code_XFER_ERR_CHK) {
            app_module_xfer_file_xfer_respond((uint16_t)xfer->offset);
            return true;
        }
        /* 接收方:结束传输字 -> 整体校验 */
        return app_module_xfer_file_recv_xfer((uint8_t)xfer->type);
    }
    default:
        return false;
    }
}

#endif