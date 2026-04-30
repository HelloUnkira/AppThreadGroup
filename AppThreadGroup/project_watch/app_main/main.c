
#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_third_fatfs.h"
#include "app_thread_group.h"

#if 0
#include <Windows.h>
#include <stdio.h>

DWORD WINAPI windows_thread(LPVOID param)
{
    int id = (int)param;
    printf("thread %d work now\n", id);
    while (1);
    printf("thread %d work over\n", id);
    return 0;
}

void windows_multi_thread_test(void)
{
    #define THREAD_COUNT    10
    HANDLE threads[THREAD_COUNT];
    
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads[i] = CreateThread(
            NULL, 0, windows_thread, (LPVOID)i, 0, NULL);
    }
    
    // 等待所有线程结束
    WaitForMultipleObjects(THREAD_COUNT, threads, TRUE, INFINITE);
    
    // 关闭句柄
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        CloseHandle(threads[i]);
    }
    return 0;
}
#endif


int main(int argc, char *argv[])
{
    /* 资源构建 */
    #if 0
    /* 生成物理外存映射(将其都刷为0xFF) */
    /* 通过文件系统将文件打包到外存映射中 */
    app_sys_log_ready();
    app_sys_ext_mem_remake();
    app_third_fatfs_remake((char *[]){"lvgl_font", "lvgl_pic",}, 2);
    return 0;
    #endif
    
    #if 0
    windows_multi_thread_test();
    #endif
    
    /* 启动APP调度策略 */
    app_thread_group_schedule();
    /* 系统组件库测试 */
    if (app_sys_test())
        return 0;
    
    /* 主线程滚动阻塞 */
    for (uint32_t count = 0; true; count++) {
        
        /* 测试中我们在主线程 */
        /* 模拟发送1ms定时器中断事件 */
        #if APP_ARCH_IS_PC
        /* fake hard clock 1ms irq */
        app_module_rtc_sync();
        app_module_rtc_1ms_cb();
        app_delay_us(1000);
        
        
        
        #else
        #error "delete this error"
        #endif
        
        /* 以下是功能测试 */
        #if 0
        #elif 0
        /* test protocol(1s later) */
        if (count == 1000 * 1) {
            app_module_protocol_t protocol = {
              //.notify.type = app_module_protocol_system_clock,
                .notify.type = app_module_protocol_trace_text,
            };
            app_module_protocol_notify(&protocol);
        }
        /* 差不多的东西放在一起测试 */
        /* test stopwatch,countdown,group alarm */
        if (count == 1000 * 1) {
            app_module_stopwatch_reset();
            app_module_stopwatch_start();
            app_module_countdown_reset();
            app_module_countdown_set(&(app_module_countdown_t){.hour = 0,.minute = 0,.second = 13,});
            app_module_countdown_start();
            app_module_remind_alarm_test();
        }
        #elif 0
        #else
        #endif
    }
    
    return 0;
}
