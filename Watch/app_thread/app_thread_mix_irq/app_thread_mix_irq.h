#ifndef APP_THREAD_MIX_IRQ_H
#define APP_THREAD_MIX_IRQ_H

typedef enum {  /* 线程模组 */
    app_thread_mix_irq_system = 0,      /* 系统模组(线程组公共事件) */
    app_thread_mix_irq_timer,           /* 定时器模组 */
    app_thread_mix_irq_clock,           /* 时钟模组 */
    app_thread_mix_irq_vibrate,         /* 震动模组 */
} app_thread_mix_irq_module;

typedef enum {  /* 定时器模组事件 */
    app_thread_mix_irq_timer_reduce_update,
} app_thread_mix_irq_timer_event;

typedef enum {  /* 时钟模组事件 */
    app_thread_mix_irq_clock_local_update,
    app_thread_mix_irq_clock_timestamp_update,
} app_thread_mix_irq_clock_event;

typedef enum {  /* 震动模组事件 */
    app_thread_mix_irq_vibrate_msec_update,
} app_thread_mix_irq_vibrate_event;

#endif
