
#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_third_fatfs.h"
#include "app_thread_group.h"

int main(int argc, char *argv[])
{
    /* 资源构建 */
    #if 0
    /* 生成物理外存映射(将其都刷为0xFF) */
    /* 通过文件系统将文件打包到外存映射中 */
    app_sys_log_ready();
    app_sys_ext_mem_remake();
    return 0;
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
        app_module_rtc_1ms_cb();
        app_delay_us(1000);
        #else
        #error "delete this error"
        #endif
        
        /* 以下是功能测试 */
        #if 0
        #else
        #endif
    }
    
    return 0;
}
