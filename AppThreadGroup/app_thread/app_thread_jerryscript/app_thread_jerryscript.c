/*实现目标:
 *    jerryscript线程
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_sys_work.h"
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

/*@brief 混合中断线程服务例程
 */
void app_thread_jerryscript_routine(void)
{
    app_sem_t *sem = NULL;
    app_sys_pipe_t *pipe = NULL;
    app_sys_pipe_pkg_t package = {0};
    app_thread_get_sync(app_thread_id_jerryscript, &sem);
    app_thread_get_pipe(app_thread_id_jerryscript, &pipe);
    /* 因为有些准备动作只适合在子线程中完成 */
    /* 将其从上面的接口中推延到此处 */ {
    }
    /* 主流程 */
    while (true) {
        app_sem_process(sem, app_sem_take);
        /* 计算事件处理时间(开始) */
        #if APP_SYS_LOG_EXECUTE
        app_execute_us_t execute_us = {0};
        app_execute_us(&execute_us, true);
        #endif
        #if APP_SYS_LOG_THREAD_CHECK
        if (app_sys_pipe_package_num(pipe) >= APP_THREAD_PACKAGE_MAX)
            APP_SYS_LOG_WARN("thread jerryscript recv too much package:%u",
                              app_sys_pipe_package_num(pipe));
        #endif
        while (app_sys_pipe_package_num(pipe) != 0) {
            app_sys_pipe_take(pipe, &package, false);
            /* 计算事件处理时间(开始) */
            #if APP_SYS_LOG_EXECUTE_CHECK
            bool execute_us_remind = true;
            app_execute_us_t execute_us = {0};
            app_execute_us(&execute_us, true);
            #endif
            /* 现在我们需要处理这个包裹了 */
            switch (package.module) {
            case app_thread_jerryscript_system: {
                if (package.event == app_thread_event_work)
                    app_sys_work_execute((void *)package.data);
                if (package.event == app_thread_event_works)
                    app_sys_works_execute((void *)package.data);
                break;
            }


            default: {
                #if APP_SYS_LOG_THREAD_CHECK
                APP_SYS_LOG_ERROR("thread jerryscript pipe recv a unknown package");
                APP_SYS_LOG_ERROR("package thread:%u", package.thread);
                APP_SYS_LOG_ERROR("package module:%u", package.module);
                APP_SYS_LOG_ERROR("package event:%u",  package.event);
                APP_SYS_LOG_ERROR("package data:%p",   package.data);
                APP_SYS_LOG_ERROR("package size:%u",   package.size);
                #endif
                break;
            }
            }
            /* 计算事件处理时间(结束) */
            #if APP_SYS_LOG_EXECUTE_CHECK
            uint32_t ms = app_execute_us(&execute_us, false) / 1000.0;
            if (ms > APP_SYS_LOG_EXECUTE_CHECK_MS && execute_us_remind) {
                APP_SYS_LOG_WARN("thread jerryscript package execute %d ms", ms);
                APP_SYS_LOG_WARN("package thread:%u", package.thread);
                APP_SYS_LOG_WARN("package module:%u", package.module);
                APP_SYS_LOG_WARN("package event:%u",  package.event);
                APP_SYS_LOG_WARN("package data:%p",   package.data);
                APP_SYS_LOG_WARN("package size:%u",   package.size);
            }
            #endif
        }
        /* 计算事件处理时间(结束) */
        #if APP_SYS_LOG_EXECUTE
        double ms = app_execute_us(&execute_us, false) / 1000.0;
        app_thread_execute_us_set(app_thread_id_jerryscript, &ms);
        #endif
    }
}
