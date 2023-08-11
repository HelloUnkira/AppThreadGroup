/*实现目标:
 *    jerryscript线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_thread_group.h"

#include "jerryscript.h"
#include "jerryscript-port.h"
#include "app_js_util.h"
#include "app_js_log.h"

/*@brief 混合中断线程模组初始化
 */
void app_thread_jerryscript_ready(void)
{
    #if 0
    /* 测试流程 */
    jerry_init(JERRY_INIT_EMPTY);
    /* 模组初始化 */
    app_js_log_ready();
    
    #include "app_js_test_stream.txt"
    
    //jerry_parse断言错误???
    //PARSER_TRY (context_p->try_buffer)
    //执行后context_p->error篡改为PARSER_ERR_EMPTY ???
    jerry_value_t parse_result = jerry_parse(acJavaScript, sizeof (acJavaScript), NULL);
    jerry_value_t run_result = jerry_run(parse_result);
    jerry_value_free (run_result);
    jerry_value_free (parse_result);
    
    jerry_cleanup();
    #endif
}

/*@brief 子线程服务例程就绪部
 */
static void app_thread_jerryscript_routine_ready_cb(void)
{
}

/*@brief 子线程服务例程处理部
 */
static bool app_thread_jerryscript_routine_package_cb(app_thread_package_t *package, uint32_t *discard_count)
{
    switch (package->module) {
    case app_thread_jerryscript_sched:
        return true;
    default:
        break;
    }
    return false;
}

/*@brief 混合中断线程服务例程
 */
void app_thread_jerryscript_routine(void)
{
    app_thread_slave_process(app_thread_id_jerryscript,
                             app_thread_jerryscript_routine_ready_cb,
                             app_thread_jerryscript_routine_package_cb);
}
