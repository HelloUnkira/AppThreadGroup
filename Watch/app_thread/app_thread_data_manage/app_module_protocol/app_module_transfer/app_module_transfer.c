/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_crc.h"
#include "app_sys_log.h"
#include "app_module_protocol.h"
#include "app_module_transfer.h"

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
void app_module_transfer_rx(app_module_transfer_pkg_t *tsf_pkg, uint32_t tsf_pkg_size)
{
    bool discard = false;
    /* 协议包 */
    if (tsf_pkg->package) {
        /* 上次协议包接收未结束,丢弃,开始新协议包接收 */
        if (app_module_protocol_pkg_off_rx >= sizeof(app_module_protocol_pkg_t)) {
            APP_SYS_LOG_ERROR("app_module_transfer_rx : ptl_pkg is not rx finish yet, discard");
            discard = true;
        }
        else
        /* 当次协议包接收过长,丢弃 */
        if (app_module_protocol_pkg_off_rx + tsf_pkg_size - 1 > sizeof(app_module_protocol_pkg_t)) {
            APP_SYS_LOG_ERROR("app_module_transfer_rx : ptl_pkg is too long, discard");
            discard = true;
        }
        else
        /* 当次协议包接收过长,丢弃 */
        if (tsf_pkg_size - 1 > sizeof(app_module_protocol_pkg_t)) {
            APP_SYS_LOG_ERROR("app_module_transfer_rx : ptl_pkg is too long now, discard");
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
    /* 数据包 */
    if (!tsf_pkg->package) {
        /* 协议包接收错误,数据无法接收,丢弃 */
        if (app_module_protocol_pkg_off_rx != sizeof(app_module_protocol_pkg_t)) {
            APP_SYS_LOG_ERROR("app_module_transfer_rx : ptl_pkg is not rx finish yet, discard");
            discard = true;
        }
        else
        /* 协议包接收错误,多余的数据无法接收,丢弃 */
        if (app_module_protocol_pkg_rx.size == 0 || app_module_protocol_dat_rx == NULL) {
            APP_SYS_LOG_ERROR("app_module_transfer_rx : ptl_dat cannot not rx, discard");
            discard = true;
        }
        else
        /* 出现多余的数据,协议包失效,丢弃 */
        if (app_module_protocol_dat_off_rx >= app_module_protocol_pkg_rx.size) {
            APP_SYS_LOG_ERROR("app_module_transfer_rx : ptl_dat too long last, discard");
            discard = true;
        }
        else
        /* 出现多余的数据,协议包失效,丢弃 */
        if (app_module_protocol_dat_off_rx + tsf_pkg_size - 1 > app_module_protocol_pkg_rx.size) {
            APP_SYS_LOG_ERROR("app_module_transfer_rx : ptl_dat too long sum, discard");
            discard = true;
        }
        else
        /* 出现多余的数据,协议包失效,丢弃 */
        if (tsf_pkg_size - 1 > app_module_protocol_pkg_rx.size) {
            APP_SYS_LOG_ERROR("app_module_transfer_rx : ptl_dat too long now, discard");
            discard = true;
        }
        /* 协议包接收正确,接收数据包 */
        if (!discard) {
            uint8_t *stream = (uint8_t *)&app_module_protocol_dat_rx + app_module_protocol_dat_off_rx;
            app_module_protocol_dat_off_rx = tsf_pkg_size - 1;
            memcpy(stream, tsf_pkg->stream, tsf_pkg_size - 1);
        }
    }
    /* rx协议包完整接收完成,校验 */
    if (app_module_protocol_pkg_off_rx == sizeof(app_module_protocol_pkg_t) &&
        app_module_protocol_dat_off_rx == app_module_protocol_pkg_rx.size) {
        uint8_t crc8 = app_sys_crc8(app_module_protocol_dat_rx, app_module_protocol_pkg_rx.size);
        if (app_module_protocol_pkg_rx.crc8 != crc8) {
            APP_SYS_LOG_ERROR("app_module_transfer_rx : ptl_dat crc8 fail, discard");
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
    /* rx协议包完整接收完成,处理 */
    if (app_module_protocol_pkg_off_rx == sizeof(app_module_protocol_pkg_t) &&
        app_module_protocol_dat_off_rx == app_module_protocol_pkg_rx.size) {
        app_module_protocol_rx(&app_module_protocol_pkg_rx, app_module_protocol_dat_rx);
        app_module_protocol_dat_rx = NULL;
    }
}

/*@brief     协议适配层,发送协议数据
 *@param[in] tsf_pkg      流式传输包
 *@param[in] tsf_pkg_size 流式传输包大小
 */
void app_module_transfer_tx(app_module_transfer_pkg_t *tsf_pkg, uint32_t tsf_pkg_size)
{
}
