#ifndef APP_THREAD_MIX_IRQ_H
#define APP_THREAD_MIX_IRQ_H

typedef enum {  /* 线程模组 */
    app_thread_mix_irq_none = 0,        /* 系统模组,占位符(线程组公共事件) */
    app_thread_mix_irq_timer,           /* 定时器模组 */
    app_thread_mix_irq_clock,           /* 时钟模组 */
} app_thread_mix_irq_module_t;

typedef enum {  /* 定时器模组事件 */
    app_thread_mix_irq_timer_reduce_update,
} app_thread_mix_irq_timer_event_t;

typedef enum {  /* 时钟模组事件 */
    app_thread_mix_irq_clock_local_update,
    app_thread_mix_irq_clock_timestamp_update,
} app_thread_mix_irq_clock_event_t;

/* 子线程模组 */
#include "app_module_work.h"
#include "app_module_timer.h"
#include "app_module_clock.h"
#include "app_module_rtc.h"
#include "app_module_watchdog.h"

#endif
