
#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

int main(int argc, char *argv[])
{
    /* 启动APP调度策略 */
    app_thread_group_schedule();
    
    /* 主线程滚动阻塞 */
    while (true) {
        #if 0
        #elif 0
        /* 生成物理外存映射,将其都刷为0xFF */
        app_sys_ext_mem_remake();
        break;
        #elif 0
        /* 通过文件系统将文件打包到外存映射中 */
        app_third_fatfs_remake("lvgl_ext_src");
        break;
        #elif 0
        /* 测试日志追踪 */
        app_sys_log_text_test();
        break;
        #elif 0
        /* test sys util plug */
        /* 测试接口有大量内存申请未释放的动作,所以仅仅测试使用 */
        void app_sys_rbuf_test(void);
        app_sys_rbuf_test();
        void app_sys_list_dl_test(void);
        void app_sys_list_sl_test(void);
        app_sys_list_dl_test();
        app_sys_list_sl_test();
        void app_sys_tree_s_test(void);
        void app_sys_tree_rb_test(void);
        void app_sys_tree_rbs_test(void);
        void app_sys_tree_fl_test(void);
        void app_sys_tree_fw_test(void);
        app_sys_tree_s_test();
        app_sys_tree_rb_test();
        app_sys_tree_rbs_test();
        app_sys_tree_fl_test();
        app_sys_tree_fw_test();
        void app_sys_table_dl_test(void);
        void app_sys_table_rbs_test(void);
        app_sys_table_dl_test();
        app_sys_table_rbs_test();
        void app_sys_map_dlm_test(void);
        app_sys_map_dlm_test();
        void app_sys_handle_test(void);
        void app_sys_matrix_test(void);
        void app_sys_fft_test(void);
        app_sys_handle_test();
        app_sys_matrix_test();
        app_sys_fft_test();
        break;
        #elif 0
        /* test sys func */
        void app_sys_ext_mem_cache_test(void);
        app_sys_ext_mem_cache_test();
        void app_sys_timer_test(void);
        app_sys_timer_test();
        while (1);
        #elif 1
        /* 测试中我们在主线程 */
        /* 模拟发送1ms定时器中断事件 */
        #if APP_ARCH_IS_PC
        /* fake hard clock 1ms irq */
        app_module_rtc_1ms_cb();
        app_delay_us(1000);
        #else
        #error "delete this error"
        #endif
        /* test:... */
        static uint32_t count = 0;
        count++;
        #if 0
        #elif 1
        /* test protocol(3s later) */
        if (count == 1000 * 3) {
            app_module_protocol_t protocol = {
              //.notify.type = app_module_protocol_system_clock,
                .notify.type = app_module_protocol_trace_text,
            };
            app_module_protocol_notify(&protocol);
        }
        #elif 0
        /* test stopwatch */
        if (count == 1000 * 1) {
            app_module_stopwatch_reset();
            app_module_stopwatch_start();
        }
        #elif 0
        /* test countdown */
        if (count == 1000 * 1) {
            app_module_countdown_t countdown = {
                .hour   = 0,
                .minute = 0,
                .second = 13,
            };
            app_module_countdown_reset();
            app_module_countdown_set(&countdown);
            app_module_countdown_start();
        }
        #elif 0
        /* test alarm group */
        if (count == 1000 * 1)
            app_module_remind_alarm_test();
        #else
        #endif
        #endif
    }
    return 0;
}