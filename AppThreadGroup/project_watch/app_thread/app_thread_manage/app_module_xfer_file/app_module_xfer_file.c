/*实现目标:
 *    通用文件传输引擎
 *    发送:描述符->ack拉取分包->分包->ack->结束->ack(拉取式状态机+定时轮询/超时退出)
 *    接收:描述符校验->ack,分包连续性/crc8校验->ack,结束crc32校验->ack
 *    动态部分(序列化协议)经ops注入,静态流程(状态机/校验/应答)固化于本模组
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_protocol_lib.h"

/* 文件发送状态机上下文 */
typedef struct {
    app_sys_timer_t timer;                             /* ack轮询定时器 */
    bool            active;                            /* 发送流程进行中 */
    uint8_t         image[APP_MODULE_XFER_FILE_IMAGE_SIZE]; /* 待发送文件内容 */
    uint32_t        file_size;                         /* 文件总大小 */
    uint32_t        file_crc32;                        /* 文件CRC32 */
    uint32_t        phase;                             /* 当前发送阶段 */
    bool            ack_pending;                       /* 有新ack待消费 */
    uint16_t        ack_type;                          /* 待消费ack类型 */
    uint8_t         ack_code;                          /* 待消费ack错误码 */
    uint16_t        ack_index;                         /* 待消费ack索引 */
    uint32_t        poll_count;                        /* 未收ack轮询计数 */
} app_module_xfer_file_send_t;
static app_module_xfer_file_send_t app_module_xfer_send = {0};

/* 文件接收上下文 */
typedef struct {
    bool     active;                                   /* 传输流程进行中 */
    uint8_t  buffer[APP_MODULE_XFER_FILE_IMAGE_SIZE];  /* 已组包文件内容 */
    uint32_t offset;                                   /* 已组包字节数 */
    uint16_t last_index;                               /* 上一分包索引 */
    uint32_t expect_size;                              /* 期望文件大小 */
    uint32_t expect_crc32;                             /* 期望文件CRC32 */
} app_module_xfer_file_recv_t;
static app_module_xfer_file_recv_t app_module_xfer_recv = {0};

static app_module_xfer_file_ops_t    app_module_xfer_ops  = {0}; /* 序列化协议原语 */
static void (*app_module_xfer_done)(bool) = NULL;              /* 发送完成回调 */

/*@brief 文件分包总数
 */
static uint32_t app_module_xfer_package_count(void)
{
    uint16_t chunk = app_module_xfer_ops.chunk;
    return (app_module_xfer_send.file_size + chunk - 1) / chunk;
}

/*@brief 发送文件描述符(文件传输请求)
 *@param name 文件名
 */
static void app_module_xfer_send_descriptor(const char *name)
{
    AppPB_MsgSet msg_start = {
        .which_payload = AppPB_MsgSet_file_tag,
        .payload.file = {
            .which_payload = AppPB_File_descriptor_tag,
        },
    };
    AppPB_FileDes *des = &msg_start.payload.file.payload.descriptor;
    memset(des->name, 0, sizeof(des->name));
    snprintf(des->name, sizeof(des->name), "%s", name);
    des->utc64 = 0;
    des->crc32 = app_module_xfer_send.file_crc32;
    des->size  = app_module_xfer_send.file_size;
    des->crc8  = app_module_xfer_file_descriptor_crc8(des);
    app_module_xfer_ops.send_file(&msg_start);
    APP_SYS_LOG_INFO("file send descriptor size:%u crc32:%08x", app_module_xfer_send.file_size, app_module_xfer_send.file_crc32);
}

/*@brief 发送文件分包
 *@param index 分包索引(对端ack游标,含重传)
 */
static void app_module_xfer_send_package(uint16_t index)
{
    uint16_t chunk = app_module_xfer_ops.chunk;
    uint32_t base  = (uint32_t)index * chunk;
    if (base >= app_module_xfer_send.file_size)
        return;
    uint32_t amount = app_module_xfer_send.file_size - base;
    if (amount > chunk)
        amount = chunk;
    AppPB_MsgSet msg_pkg = {
        .which_payload = AppPB_MsgSet_file_tag,
        .payload.file = {
            .which_payload = AppPB_File_package_tag,
        },
    };
    AppPB_FilePkg *pkg = &msg_pkg.payload.file.payload.package;
    pkg->index = index;
    pkg->base  = base;
    pkg->size  = amount;
    memcpy(pkg->data.bytes, app_module_xfer_send.image + base, amount);
    pkg->data.size = amount;
    pkg->crc8  = app_sys_crc8(pkg->data.bytes, amount);
    app_module_xfer_ops.send_file(&msg_pkg);
    APP_SYS_LOG_INFO("file send package index:%u base:%u size:%u", index, base, amount);
}

/*@brief 发送文件传输结束
 */
static void app_module_xfer_send_end(void)
{
    AppPB_MsgSet msg_end = {
        .which_payload = AppPB_MsgSet_file_tag,
        .payload.file = {
            .which_payload = AppPB_File_done_tag,
            .payload.done = { .code = 0 },
        },
    };
    app_module_xfer_ops.send_file(&msg_end);
    APP_SYS_LOG_INFO("file send end");
}

/*@brief ack轮询定时器回调:投递步进到manage线程
 */
static void app_module_xfer_timer_handler(void *timer)
{
    if (!app_module_xfer_send.active)
        return;
    app_module_xfer_ops.post_step();
}

/*@brief 结束文件发送流程并通报完成
 *@param ok 传输是否成功
 */
static void app_module_xfer_send_finish(bool ok)
{
    app_module_xfer_send.phase  = app_module_xfer_file_phase_done;
    app_module_xfer_send.active = false;
    app_sys_timer_stop(&app_module_xfer_send.timer);
    if (app_module_xfer_done)
        app_module_xfer_done(ok);
}

/*@brief 注入文件传输引擎原语
 *@param ops 原语表
 */
void app_module_xfer_file_config(const app_module_xfer_file_ops_t *ops)
{
    app_module_xfer_ops = *ops;
    if (app_module_xfer_ops.chunk == 0)
        app_module_xfer_ops.chunk = APP_MODULE_XFER_FILE_CHUNK;
}

/*@brief 设置文件发送完成回调
 *@param done 回调
 */
void app_module_xfer_file_set_done(void (*done)(bool))
{
    app_module_xfer_done = done;
}

/*@brief 启动文件发送(拉取式:发描述符后等对端ack)
 *@param name 文件名
 *@retval 是否启动
 */
bool app_module_xfer_file_notify(const char *name)
{
    if (app_module_xfer_send.active)
        return false;
    /* 组装文件内容并计算CRC32 */
    app_module_xfer_send.file_size  = app_module_xfer_file_build(app_module_xfer_send.image, sizeof(app_module_xfer_send.image));
    app_module_xfer_send.file_crc32 = app_sys_crc32(app_module_xfer_send.image, app_module_xfer_send.file_size);
    app_module_xfer_send.phase       = app_module_xfer_file_phase_start;
    app_module_xfer_send.ack_pending = false;
    app_module_xfer_send.poll_count  = 0;
    app_module_xfer_send.active      = true;
    /* 发送文件描述符,等对端ack拉取分包 */
    app_module_xfer_send_descriptor(name);
    /* 启动ack轮询定时器 */
    app_module_xfer_send.timer.expired = app_module_xfer_timer_handler;
    app_module_xfer_send.timer.peroid  = APP_MODULE_XFER_FILE_SEND_PERIOD;
    app_module_xfer_send.timer.reload  = 1;
    app_sys_timer_start(&app_module_xfer_send.timer);
    return true;
}

/*@brief 文件发送轮询步进:消费ack拉取或超时退出
 */
void app_module_xfer_file_step(void)
{
    if (!app_module_xfer_send.active) {
        app_sys_timer_stop(&app_module_xfer_send.timer);
        return;
    }
    /* 无新ack则计数,超时退出 */
    if (!app_module_xfer_send.ack_pending) {
        if (++app_module_xfer_send.poll_count >= APP_MODULE_XFER_FILE_POLL_MAX) {
            APP_SYS_LOG_ERROR("file ack timeout phase:%u", app_module_xfer_send.phase);
            app_module_xfer_send_finish(false);
        }
        return;
    }
    uint16_t ack_type  = app_module_xfer_send.ack_type;
    uint8_t  ack_code  = app_module_xfer_send.ack_code;
    uint16_t ack_index = app_module_xfer_send.ack_index;
    app_module_xfer_send.ack_pending = false;
    app_module_xfer_send.poll_count   = 0;
    /* 阶段:已发描述符,等对端ack拉取 */
    if (app_module_xfer_send.phase == app_module_xfer_file_phase_start) {
        if (ack_type != AppPB_File_descriptor_tag) {
            APP_SYS_LOG_WARN("file descriptor ack type mismatch:%u", ack_type);
            return;
        }
        if (ack_code != AppPB_ACK_ErrorCode_SUCCEED) {
            APP_SYS_LOG_ERROR("file descriptor ack fail code:%u", ack_code);
            app_module_xfer_send_finish(false);
            return;
        }
        /* 元数据模式或游标到末尾则直接结束 */
        if (app_module_xfer_ops.meta_only || ack_index >= app_module_xfer_package_count()) {
            app_module_xfer_send.phase = app_module_xfer_file_phase_end;
            app_module_xfer_send_end();
        } else {
            app_module_xfer_send.phase = app_module_xfer_file_phase_package;
            app_module_xfer_send_package(ack_index);
        }
        return;
    }
    /* 阶段:分包拉取应答中 */
    if (app_module_xfer_send.phase == app_module_xfer_file_phase_package) {
        if (ack_type != AppPB_File_package_tag) {
            APP_SYS_LOG_WARN("file package ack type mismatch:%u", ack_type);
            return;
        }
        if (ack_code == AppPB_ACK_ErrorCode_CHK_FAILED) {
            APP_SYS_LOG_ERROR("file package ack check fail index:%u", ack_index);
            app_module_xfer_send_finish(false);
            return;
        }
        /* SUCCEED或CRC_FAILED:按对端游标发对应分包(继续或重传) */
        if (ack_index >= app_module_xfer_package_count()) {
            app_module_xfer_send.phase = app_module_xfer_file_phase_end;
            app_module_xfer_send_end();
        } else {
            app_module_xfer_send_package(ack_index);
        }
        return;
    }
    /* 阶段:已发结束,等完成ack */
    if (app_module_xfer_send.phase == app_module_xfer_file_phase_end) {
        if (ack_type != AppPB_File_done_tag) {
            APP_SYS_LOG_WARN("file done ack type mismatch:%u", ack_type);
            return;
        }
        bool ok = (ack_code == AppPB_ACK_ErrorCode_SUCCEED);
        if (ok)
            APP_SYS_LOG_INFO("file send done size:%u", app_module_xfer_send.file_size);
        else
            APP_SYS_LOG_ERROR("file done ack fail code:%u", ack_code);
        app_module_xfer_send_finish(ok);
        return;
    }
}

/*@brief 注入对端ack(发送状态机消费)
 *@param type  消息tag
 *@param code  错误码
 *@param index 索引
 */
void app_module_xfer_file_ack(uint16_t type, uint8_t code, uint16_t index)
{
    if (!app_module_xfer_send.active)
        return;
    app_module_xfer_send.ack_pending = true;
    app_module_xfer_send.ack_type    = type;
    app_module_xfer_send.ack_code    = code;
    app_module_xfer_send.ack_index   = index;
}

/*@brief 传输接收文件(描述符/分包/结束):校验+回ack
 *       静态流程:按子类型校验,决定ack并拉取下一游标
 *@param file 文件子消息
 *@retval 是否成功
 */
bool app_module_xfer_file_respond(AppPB_File *file)
{
    switch (file->which_payload) {
    /* 子协议:文件传输开始(描述符) */
    case AppPB_File_descriptor_tag: {
        AppPB_FileDes *des = &file->payload.descriptor;
        if (des->crc8 != app_module_xfer_file_descriptor_crc8(des)) {
            APP_SYS_LOG_WARN("file descriptor crc8 fail");
            app_module_xfer_ops.send_ack(AppPB_File_descriptor_tag, AppPB_ACK_ErrorCode_CRC_FAILED, 0);
            return false;
        }
        app_module_xfer_recv.active       = true;
        app_module_xfer_recv.offset       = 0;
        app_module_xfer_recv.last_index   = 0;
        app_module_xfer_recv.expect_size  = des->size;
        app_module_xfer_recv.expect_crc32 = des->crc32;
        APP_SYS_LOG_INFO("file recv start name:%s size:%u crc32:%08x", des->name, des->size, des->crc32);
        app_module_xfer_ops.send_ack(AppPB_File_descriptor_tag, AppPB_ACK_ErrorCode_SUCCEED, 0);
        return true;
    }
    /* 子协议:文件传输包 */
    case AppPB_File_package_tag: {
        if (!app_module_xfer_recv.active) {
            APP_SYS_LOG_WARN("file package without start");
            app_module_xfer_ops.send_ack(AppPB_File_package_tag, AppPB_ACK_ErrorCode_CHK_FAILED, 0);
            return false;
        }
        AppPB_FilePkg *pkg = &file->payload.package;
        if (pkg->index != app_module_xfer_recv.last_index) {
            APP_SYS_LOG_WARN("file package index broken:%u/%u", pkg->index, app_module_xfer_recv.last_index);
            app_module_xfer_ops.send_ack(AppPB_File_package_tag, AppPB_ACK_ErrorCode_CRC_FAILED, app_module_xfer_recv.last_index);
            return false;
        }
        if (pkg->base != app_module_xfer_recv.offset) {
            APP_SYS_LOG_WARN("file package base broken:%u/%u", pkg->base, app_module_xfer_recv.offset);
            app_module_xfer_ops.send_ack(AppPB_File_package_tag, AppPB_ACK_ErrorCode_CRC_FAILED, pkg->index);
            return false;
        }
        if (pkg->crc8 != app_sys_crc8(pkg->data.bytes, pkg->data.size)) {
            APP_SYS_LOG_WARN("file package crc8 fail:%u", pkg->index);
            app_module_xfer_ops.send_ack(AppPB_File_package_tag, AppPB_ACK_ErrorCode_CRC_FAILED, pkg->index);
            return false;
        }
        if (pkg->base + pkg->data.size > app_sys_arr_len(app_module_xfer_recv.buffer)) {
            APP_SYS_LOG_WARN("file package overflow:%u", pkg->index);
            app_module_xfer_ops.send_ack(AppPB_File_package_tag, AppPB_ACK_ErrorCode_CHK_FAILED, pkg->index);
            return false;
        }
        memcpy(app_module_xfer_recv.buffer + pkg->base, pkg->data.bytes, pkg->data.size);
        app_module_xfer_recv.offset += pkg->data.size;
        app_module_xfer_recv.last_index++;
        APP_SYS_LOG_INFO("file recv package index:%u base:%u size:%u", pkg->index, pkg->base, pkg->data.size);
        app_module_xfer_ops.send_ack(AppPB_File_package_tag, AppPB_ACK_ErrorCode_SUCCEED, pkg->index + 1);
        return true;
    }
    /* 子协议:文件传输结束 */
    case AppPB_File_done_tag: {
        if (!app_module_xfer_recv.active) {
            APP_SYS_LOG_WARN("file done without start");
            return false;
        }
        app_module_xfer_recv.active = false;
        bool ok = (app_module_xfer_recv.offset == app_module_xfer_recv.expect_size) &&
                  (app_sys_crc32(app_module_xfer_recv.buffer, app_module_xfer_recv.offset) == app_module_xfer_recv.expect_crc32);
        if (ok) {
            APP_SYS_LOG_INFO("file recv end ok size:%u crc32:%08x", app_module_xfer_recv.offset, app_module_xfer_recv.expect_crc32);
            app_module_xfer_ops.send_ack(AppPB_File_done_tag, AppPB_ACK_ErrorCode_SUCCEED, 0);
            return true;
        }
        APP_SYS_LOG_ERROR("file recv end fail size:%u/%u", app_module_xfer_recv.offset, app_module_xfer_recv.expect_size);
        app_module_xfer_ops.send_ack(AppPB_File_done_tag, AppPB_ACK_ErrorCode_CRC_FAILED, 0);
        return false;
    }
    default:
        APP_SYS_LOG_ERROR("file have unknown type:%u", file->which_payload);
        return false;
    }
}

/*@brief 组装文件内容:日志条目换行拼接,无日志用样本兜底
 *@param image      内容缓冲
 *@param image_size 缓冲大小
 *@retval 文件大小
 */
uint32_t app_module_xfer_file_build(uint8_t *image, uint32_t image_size)
{
    uint32_t file_size = 0;
    app_sys_log_text_peek_reset();
    while (file_size < image_size) {
        char item[APP_SYS_LOG_TEXT_LIMIT + 1] = {0};
        if (!app_sys_log_text_peek(item) || item[0] == '\0')
            break;
        uint32_t item_len = strlen(item);
        if (file_size + item_len + 1 >= image_size)
            break;
        memcpy(image + file_size, item, item_len);
        file_size += item_len;
        image[file_size++] = '\n';
    }
    if (file_size == 0)
        file_size = app_module_protocol_test_fill_file_sample(image, image_size);
    return file_size;
}

/*@brief 文件描述符元数据CRC8:name+utc64+crc32+size
 *@param des 描述符
 *@retval CRC8
 */
uint8_t app_module_xfer_file_descriptor_crc8(const AppPB_FileDes *des)
{
    uint8_t meta[sizeof(des->name) + 12] = {0};
    uint32_t off = 0;
    uint32_t name_len = strlen(des->name);
    memcpy(meta + off, des->name, name_len); off += name_len;
    memcpy(meta + off, &des->utc64, sizeof(des->utc64)); off += sizeof(des->utc64);
    memcpy(meta + off, &des->crc32, sizeof(des->crc32)); off += sizeof(des->crc32);
    memcpy(meta + off, &des->size,  sizeof(des->size));  off += sizeof(des->size);
    return app_sys_crc8(meta, off);
}