/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_crc.h"
#include "app_thread_master.h"
#include "app_thread_data_manage.h"
#include "app_module_protocol.h"
#include "app_module_transfer.h"
#include "app_module_transfer_adaptor.h"

static app_module_protocol_pkg_t app_module_protocol_pkg_tx = {0};
static app_module_protocol_pkg_t app_module_protocol_pkg_rx = {0};
static uint32_t app_module_protocol_pkg_off_tx = 0;
static uint32_t app_module_protocol_pkg_off_rx = 0;
static uint8_t *app_module_protocol_dat_tx = NULL;
static uint8_t *app_module_protocol_dat_rx = NULL;
static uint32_t app_module_protocol_dat_off_tx = 0;
static uint32_t app_module_protocol_dat_off_rx = 0;

/*@brief     协议适配层,接收协议数据
 *@param[in] tsf_pkg      流式传输包
 *@param[in] tsf_pkg_size 流式传输包大小
 */
bool app_module_transfer_respond(app_module_transfer_pkg_t *tsf_pkg, uint32_t tsf_pkg_size)
{
    bool discard = false;
    /* 接收协议包 */
    if (tsf_pkg->package) {
        /* 上次协议包接收未结束,丢弃,开始新协议包接收 */
        if (app_module_protocol_pkg_off_rx >= sizeof(app_module_protocol_pkg_t)) {
            APP_SYS_LOG_ERROR("ptl_pkg is not rx finish yet, discard");
            discard = true;
        }
        else
        /* 当次协议包接收过长,丢弃 */
        if (app_module_protocol_pkg_off_rx + tsf_pkg_size - 1 > sizeof(app_module_protocol_pkg_t)) {
            APP_SYS_LOG_ERROR("ptl_pkg is too long, discard");
            discard = true;
        }
        else
        /* 当次协议包接收过长,丢弃 */
        if (tsf_pkg_size - 1 > sizeof(app_module_protocol_pkg_t)) {
            APP_SYS_LOG_ERROR("ptl_pkg is too long now, discard");
            discard = true;
        }
        /* 接收协议包 */
        if (!discard) {
            uint8_t *stream = (uint8_t *)&app_module_protocol_pkg_rx + app_module_protocol_pkg_off_rx;
            app_module_protocol_pkg_off_rx = tsf_pkg_size - 1;
            memcpy(stream, tsf_pkg->stream, tsf_pkg_size - 1);
        }
        /* 接收到完整协议包,为接下来的数据包生成空间 */
        if (app_module_protocol_pkg_off_rx == sizeof(app_module_protocol_pkg_t) &&
            app_module_protocol_pkg_rx.size != 0 &&
            app_module_protocol_dat_rx == NULL)
            app_module_protocol_dat_rx = app_mem_alloc(app_module_protocol_pkg_rx.size);
    }
    /* 接收数据包 */
    if (!tsf_pkg->package) {
        /* 协议包接收错误,数据无法接收,丢弃 */
        if (app_module_protocol_pkg_off_rx != sizeof(app_module_protocol_pkg_t)) {
            APP_SYS_LOG_ERROR("ptl_pkg is not rx finish yet, discard");
            discard = true;
        }
        else
        /* 协议包接收错误,多余的数据无法接收,丢弃 */
        if (app_module_protocol_pkg_rx.size == 0 || app_module_protocol_dat_rx == NULL) {
            APP_SYS_LOG_ERROR("ptl_dat cannot not rx, discard");
            discard = true;
        }
        else
        /* 出现多余的数据,协议包失效,丢弃 */
        if (app_module_protocol_dat_off_rx >= app_module_protocol_pkg_rx.size) {
            APP_SYS_LOG_ERROR("ptl_dat too long last, discard");
            discard = true;
        }
        else
        /* 出现多余的数据,协议包失效,丢弃 */
        if (app_module_protocol_dat_off_rx + tsf_pkg_size - 1 > app_module_protocol_pkg_rx.size) {
            APP_SYS_LOG_ERROR("ptl_dat too long sum, discard");
            discard = true;
        }
        else
        /* 出现多余的数据,协议包失效,丢弃 */
        if (tsf_pkg_size - 1 > app_module_protocol_pkg_rx.size) {
            APP_SYS_LOG_ERROR("ptl_dat too long now, discard");
            discard = true;
        }
        /* 协议包接收正确,接收数据包 */
        if (!discard) {
            uint8_t *stream = (uint8_t *)&app_module_protocol_dat_rx + app_module_protocol_dat_off_rx;
            app_module_protocol_dat_off_rx = tsf_pkg_size - 1;
            memcpy(stream, tsf_pkg->stream, tsf_pkg_size - 1);
        }
    }
    /* rx协议包接收完成,差序校验 */
    if (app_module_protocol_pkg_off_rx == sizeof(app_module_protocol_pkg_t) &&
        app_module_protocol_dat_off_rx == app_module_protocol_pkg_rx.size) {
        uint8_t crc8 = app_sys_crc8(app_module_protocol_dat_rx, app_module_protocol_pkg_rx.size);
        if (app_module_protocol_pkg_rx.crc8 != crc8) {
            APP_SYS_LOG_ERROR("ptl_dat crc8 fail, discard");
            discard = true;
        }
    }
    /* rx协议接收复位 */
    if (discard) {
        if (app_module_protocol_dat_rx)
            app_mem_free(app_module_protocol_dat_rx);
        app_module_protocol_dat_rx = NULL;
        app_module_protocol_pkg_off_rx = 0;
        app_module_protocol_dat_off_rx = 0;
    }
    /* rx协议包接收完成,处理 */
    if (app_module_protocol_pkg_off_rx == sizeof(app_module_protocol_pkg_t) &&
        app_module_protocol_dat_off_rx == app_module_protocol_pkg_rx.size)
        return true;
    return false;
}

/*@brief     协议适配层,接收协议数据
 *@param[in] ptl_pkg      协议传输包
 *@param[in] ptl_pkg_size 协议传输包数据
 */
void app_module_transfer_respond_finish(app_module_protocol_pkg_t *ptl_pkg, uint8_t **ptl_dat)
{
    memcpy(ptl_pkg, &app_module_protocol_pkg_rx, sizeof(app_module_protocol_pkg_t));
    *ptl_dat = app_module_protocol_dat_rx;
    app_module_protocol_dat_rx = NULL;
    app_module_protocol_pkg_off_rx = 0;
    app_module_protocol_dat_off_rx = 0;
}

/*@brief     协议适配层,发送协议数据
 *@param[in] tsf_pkg      流式传输包
 *@param[in] tsf_pkg_size 流式传输包大小
 */
void app_module_transfer_notify(app_module_protocol_pkg_t *ptl_pkg, uint8_t *ptl_dat)
{
    /* tx协议包打包到本地 */
    memcpy(&app_module_protocol_pkg_tx, ptl_pkg, sizeof(app_module_protocol_pkg_t));
    app_module_protocol_dat_tx = ptl_dat;
    app_module_protocol_pkg_off_tx == 0;
    app_module_protocol_dat_off_tx == 0;
    /* 准备当次传输包单元 */
    uint32_t tsf_pkg_size = app_module_transfer_adaptor_tx_max();
    app_module_transfer_pkg_t *tsf_pkg = app_mem_alloc(tsf_pkg_size);
    /* tx协议包发送 */
    while (app_module_protocol_pkg_off_tx < sizeof(app_module_protocol_pkg_t)) {
        tsf_pkg->package = true;
        uint8_t *data = (uint8_t *)&app_module_protocol_pkg_tx + app_module_protocol_pkg_off_tx;
        uint32_t size = sizeof(app_module_protocol_pkg_t) - app_module_protocol_pkg_off_tx;
        size = size <= tsf_pkg_size - 1 ? size : tsf_pkg_size - 1;
        app_module_protocol_pkg_off_tx += size;
        memcpy(tsf_pkg->stream, data, size);
        app_module_transfer_adaptor_tx((void *)tsf_pkg, size + 1);
    }
    /* tx数据包发送 */
    while (app_module_protocol_dat_off_tx < app_module_protocol_pkg_tx.size) {
        tsf_pkg->package = false;
        uint8_t *data = (uint8_t *)&app_module_protocol_dat_off_tx + app_module_protocol_dat_off_tx;
        uint32_t size = app_module_protocol_pkg_tx.size - app_module_protocol_dat_off_tx;
        size = size <= tsf_pkg_size - 1 ? size : tsf_pkg_size - 1;
        app_module_protocol_pkg_off_tx += size;
        memcpy(tsf_pkg->stream, data, size);
        app_module_transfer_adaptor_tx((void *)tsf_pkg, size + 1);
    }
    /* 回收缓冲区 */
    app_mem_free(tsf_pkg);
}
