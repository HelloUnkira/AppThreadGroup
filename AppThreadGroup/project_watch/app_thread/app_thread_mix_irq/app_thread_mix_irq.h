#ifndef APP_THREAD_MIX_IRQ_H
#define APP_THREAD_MIX_IRQ_H

typedef enum {  /* 线程模组 */
    app_thread_mix_irq_none = 0,        /* 系统模组,占位符(线程组公共事件) */
    app_thread_mix_irq_timer,           /* 定时器模组 */
    app_thread_mix_irq_clock,           /* 时钟模组 */
    app_thread_mix_irq_battery,         /* 电池模组 */
    app_thread_mix_irq_vibrate,         /* 震动模组 */
    app_thread_mix_irq_gesture,         /* 手势模组 */
    app_thread_mix_irq_temperature,     /* 温度模组 */
} app_thread_mix_irq_module_t;

typedef enum {  /* 定时器模组事件 */
    app_thread_mix_irq_timer_reduce_update,
} app_thread_mix_irq_timer_event_t;

typedef enum {  /* 时钟模组事件 */
    app_thread_mix_irq_clock_local_update,
    app_thread_mix_irq_clock_timestamp_update,
} app_thread_mix_irq_clock_event_t;

typedef enum {  /* 充电模组事件 */
    app_thread_mix_irq_battery_charge,
    app_thread_mix_irq_battery_discharge,
    app_thread_mix_irq_battery_charge_check,
    app_thread_mix_irq_battery_voltage_check,
    app_thread_mix_irq_battery_charge_xms_update,
    app_thread_mix_irq_battery_voltage_xms_update,
} app_thread_mix_irq_battery_event_t;

typedef enum {  /* 震动模组事件 */
    app_thread_mix_irq_vibrate_msec_update,
} app_thread_mix_irq_vibrate_event_t;

typedef enum {  /* 手势模组事件 */
    app_thread_mix_irq_gesture_event,
    app_thread_mix_irq_gesture_xmsec_update,
} app_thread_mix_irq_gesture_event_t;

typedef enum {  /* 温度模组事件 */
    app_thread_mix_irq_temperature_xms_update,
    app_thread_mix_irq_temperature_xs_update,
} app_thread_mix_irq_temperature_event_t;

/* 子线程模组 */
#include "app_module_work.h"
#include "app_module_timer.h"
#include "app_module_clock.h"
#include "app_module_rtc.h"
#include "app_module_watchdog.h"
#include "app_module_battery.h"
#include "app_module_vibrate.h"
#include "app_module_gesture.h"
#include "app_module_backlight.h"
#include "app_module_temperature.h"

#endif
