#ifndef APP_THREAD_GROUP_H
#define APP_THREAD_GROUP_H

/*@brief 初始化所有线程及其附属资源
 *       启动所有线程
 */
void app_thread_group_schedule(void);

/*@brief 获得线程组初始化状态
 */
bool app_thread_group_run_status(void);

/* 子线程组编号 */
typedef enum {
    /* 静态线程id号 */
    app_thread_id_s_s = 0x00,       /* 静态线程配置开始(static start) */
    app_thread_id_mix_irq,          /* 混合中断线程(priority:2) */
    app_thread_id_mix_custom,       /* 混合常规线程(priority:3) */
    app_thread_id_jerryscript,      /* jerryscript线程(priority:5) */
    app_thread_id_s_e,              /* 静态线程配置结束(static end) */
    /* 动态线程id号 */
    /* 备注:不使用动态线程时缩小该范围节约空间 */
    app_thread_id_d_s = 0x80,       /* 动态线程配置开始(dynamic start) */
    app_thread_id_d_e = 0x90,       /* 动态线程配置结束(dynamic end) */
} app_thread_id_t;

/* 子线程组公共事件 */
/* 备注:每一个子线程都必须可以处理的系统公共事件 */
/* 备注:占用每一个子线程的0号模组 */
typedef enum {  /* 线程组模组 */
    app_thread_event_work,
    app_thread_event_num,
} app_thread_event_t;

#include "app_thread_adaptor.h"
#include "app_thread_master.h"
#include "app_thread_slave.h"
#include "app_thread_task.h"

void app_thread_master_prepare(void);
void app_thread_master_schedule(void);
void app_thread_slave_prepare(void);
void app_thread_slave_schedule(void);

/* 自定义子线程 */
#include "app_thread_mix_irq.h"
#include "app_thread_mix_custom.h"
#include "app_thread_jerryscript.h"

#endif
