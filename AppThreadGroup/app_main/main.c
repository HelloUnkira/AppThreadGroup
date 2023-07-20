
#include "app_ext_lib.h"
#include "app_sys_timer.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_table.h"
#include "app_sys_log_text.h"
#include "app_thread_group.h"
#include "app_module_clock.h"
#include "app_module_rtc.h"
#include "app_module_protocol.h"
#include "app_module_stopwatch.h"
#include "app_module_countdown.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"

/*@brief 提醒闹钟模组测试
 */
static inline void app_module_remind_alarm_test(void)
{
    app_module_remind_item_t       *remind_item = NULL;
    app_module_remind_alarm_info_t *alarm_info  = NULL;
    
    app_module_remind_alarm_array_lock();
    app_module_remind_alarm_array(&remind_item, &alarm_info);
    /* 提醒闹钟0(常规模式): */
    const char *alarm_name_0 = "Alarm 0";
    alarm_info[0].snooze_count = 0;
    alarm_info[0].duration = 300;
    memcpy(alarm_info[0].name, alarm_name_0, sizeof(alarm_name_0));
    remind_item[0].valid = true;
    remind_item[0].onoff = true;
    remind_item[0].type  = app_module_remind_item_custom;
    remind_item[0].month = 0b00000001000;
    remind_item[0].week  = 0b0000100;
    remind_item[0].clock.year   = 2023;
    remind_item[0].clock.month  = 1;
    remind_item[0].clock.day    = 1;
    remind_item[0].clock.hour   = 0;
    remind_item[0].clock.minute = 0;
    remind_item[0].clock.second = 2;
    app_module_clock_to_utc(&remind_item[0].clock);
    app_module_clock_to_week(&remind_item[0].clock);
    remind_item[0].offset_utc = remind_item[0].clock.utc /* +-xxx sec */;
    /* 提醒闹钟1(滚动模式) */
    const char *alarm_name_1 = "Alarm 1";
    alarm_info[1].snooze_count = 0;
    alarm_info[1].duration = 300;
    memcpy(alarm_info[1].name, alarm_name_1, sizeof(alarm_name_1));
    remind_item[1].valid  = true;
    remind_item[1].onoff  = true;
    remind_item[1].type   = app_module_remind_item_repeat;
    remind_item[1].repeat = 3;
    remind_item[1].clock.year   = 2023;
    remind_item[1].clock.month  = 1;
    remind_item[1].clock.day    = 1;
    remind_item[1].clock.hour   = 1;
    remind_item[1].clock.minute = 1;
    remind_item[1].clock.second = 2;
    app_module_clock_to_utc(&remind_item[1].clock);
    app_module_clock_to_week(&remind_item[1].clock);
    remind_item[1].offset_utc = remind_item[1].clock.utc /* +-xxx sec */;
    /* 继续添加 */
    app_module_remind_alarm_array_unlock();
}

int main(int argc, char *argv[])
{
    /* 启动APP调度策略 */
    app_thread_group_schedule();
    /* 主线程滚动阻塞 */
    while (true) {
        #if 0
        #elif 0
        /* 生成物理外存映射,将其都刷为0xFF */
        #if APP_ARCH_IS_PC
        uint8_t buffer[1024] = {0};
        for (uint32_t idx = 0; idx < 1024; buffer[idx] = 0xff, idx++);
        fclose(fopen("ext_mem_static",  "wb")); // 64K
        fclose(fopen("ext_mem_flash",   "wb")); // 8M
        fclose(fopen("ext_mem_sd_card", "wb")); // 16M
        FILE *file1 = fopen("ext_mem_static",  "rb+");
        FILE *file2 = fopen("ext_mem_flash",   "rb+");
        FILE *file3 = fopen("ext_mem_sd_card", "rb+");
        fseek(file1, 0, SEEK_SET);
        fseek(file2, 0, SEEK_SET);
        fseek(file3, 0, SEEK_SET);
        for (uint32_t idx = 0; idx < 64; idx++)
            fwrite(buffer, 1024, 1, file1);
        for (uint32_t idx = 0; idx < 1024 * 8; idx++)
            fwrite(buffer, 1024, 1, file2);
        for (uint32_t idx = 0; idx < 1024 * 16; idx++)
            fwrite(buffer, 1024, 1, file3);
        fclose(file1);
        fclose(file2);
        fclose(file3);
        #endif
        break;
        #elif 1
        #if 0
        /* 测试日志追踪 */
        app_sys_log_text_test();
        #endif
        /* 测试中我们在主线程 */
        /* 模拟发送1ms定时器中断事件 */
        /* 我是实在没想到这种方式 */
        /* 居然是最简单的做法...... */
        #if APP_ARCH_IS_PC
        /* fake hard clock 1ms irq */
        app_module_rtc_1ms_cb();
        app_delay_us(1000);
        #else
        #endif
        /* test:... */
        #if 0
        #elif 0
        /* test timer */
        if (count == 1000)
            app_sys_timer_test();
        #elif 0
        /* test reset load and dump */
        if (count % 5000 == 0) {
            app_module_system_delay_set(2);
            app_module_system_status_set(app_module_system_reset);
        }
        #elif 0
        /* test stopwatch */
        if (count == 1000) {
            app_module_stopwatch_reset();
            app_module_stopwatch_start();
        }
        #elif 0
        /* test countdown */
        if (count == 1000) {
            app_module_countdown_reset();
            app_module_countdown_t countdown = {
                .hour   = 0,
                .minute = 0,
                .second = 13,
            };
            app_module_countdown_set(&countdown);
            app_module_countdown_start();
        }
        #elif 0
        /* test alarm group */
        if (count == 1000)
            app_module_remind_alarm_test();
        #elif 0
        /* test protocol(7s later) */
        if (count == 1000 * 7) {
            app_module_protocol_t protocol = {
              //.notify.type = app_module_protocol_system_clock,
                .notify.type = app_module_protocol_trace_text,
            };
            app_module_protocol_notify(&protocol);
        }
        #else
        #endif
        #endif
    }
    return 0;
}
