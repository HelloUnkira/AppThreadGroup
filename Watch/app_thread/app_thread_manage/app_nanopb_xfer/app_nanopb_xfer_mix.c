/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_thread_master.h"
#include "app_thread_manage.h"
#include "app_module_protocol.h"
#include "app_module_clock.h"

#if      APP_MODULE_PROTOCOL_USE_NANOPB
#include "app_nanopb_xfer.h"
#include "app_nanopb_xfer_mix.h"


#endif

