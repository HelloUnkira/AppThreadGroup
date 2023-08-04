#ifndef APP_THREAD_GROUP_H
#define APP_THREAD_GROUP_H

/*@brief 初始化所有线程及其附属资源
 *       启动所有线程
 */
void app_thread_group_schedule(void);

/*@brief 获得线程组初始化状态
 */
bool app_thread_group_status_get(void);

/* 子线程组编号 */
typedef enum {
    app_thread_id_mix_irq,          /* 混合中断线程(priority:2) */
    app_thread_id_mix_custom,       /* 混合常规线程(priority:3) */
    app_thread_id_manage,           /* 数据管理线程(priority:5) */
    app_thread_id_lvgl,             /* lvgl线程(priority:4) */
    app_thread_id_jerryscript,      /* jerryscript线程(priority:5) */
    /* 继续添加其他线程... */
    app_thread_id_number,           /* 占位符:子线程组数量 */
} app_thread_id_t;

/* 子线程组公共事件 */
/* 备注:每一个子线程都必须可以处理的系统公共事件 */
/* 备注:占用每一个子线程的0号模组 */
typedef enum {  /* 线程组模组 */
    app_thread_event_work,
    app_thread_event_num,
} app_thread_event_t;

/* 主线程调度模式
 * 线程组以事件组优先级进行实时调度或分时调度
 * 实时调度会保证优先级高的事件被立即激活
 * 分时调度以指定时间片内收集的事件进行调度
 * 分时调度会降低部分并发性但能节约部分调度性能
 */
#define APP_THREAD_MASTER_REALTIME      0
#define APP_THREAD_MASTER_TIME_SLICE    10

/* 子线程执行时间检查(ms) */
#define APP_THREAD_SLAVE_EXECUTE_TIME             1
/* 线程事件包执行时间检查(ms) */
#define APP_THREAD_SLAVE_EXECUTE_TIME_CHECK       0
#define APP_THREAD_SLAVE_EXECUTE_TIME_CHECK_MS    10

/* 线程组线程包裹接收最大警告线(警告) */
#if     APP_THREAD_MASTER_REALTIME
#define APP_THREAD_PACKAGE_MAX  20
#else
#define APP_THREAD_PACKAGE_MAX  100
#endif

#include "app_thread_master.h"
#include "app_thread_slave.h"

/* 自定义子线程 */
#include "app_thread_mix_irq.h"
#include "app_thread_mix_custom.h"
#include "app_thread_manage.h"
#include "app_thread_lvgl.h"
#include "app_thread_jerryscript.h"

#endif
