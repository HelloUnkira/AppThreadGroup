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

/* 子线程组事件 */
/* 备注:每一个子线程都必须可以处理的系统公共事件 */
/* 备注:占用每一个子线程的0号模组 */
typedef enum {  /* 线程组模组 */
    app_thread_event_work,
    app_thread_event_works,
    app_thread_event_num,
} app_thread_event_t;

#include "app_thread_master.h"
#include "app_thread_mix_irq.h"
#include "app_thread_mix_custom.h"
#include "app_thread_manage.h"
#include "app_thread_lvgl.h"
#include "app_thread_jerryscript.h"

#endif
