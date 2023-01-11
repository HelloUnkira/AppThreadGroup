#ifndef APP_THREAD_SOURCE_MANAGE_H
#define APP_THREAD_SOURCE_MANAGE_H

/* 本线程管理的功能模组 */
#include "app_module_ext_mem.h"
#include "app_module_ext_mem_table.h"
#include "app_module_source.h"
#include "app_module_source_table.h"
#include "app_module_dump.h"
#include "app_module_load.h"
#include "app_module_source_mix.h"

typedef enum {  /* 线程模组 */
    app_thread_source_manage_work,         /* 工作队列模组 */
    app_thread_source_manage_dump,         /* 资源转储模组 */
    app_thread_source_manage_load,         /* 资源加载模组 */
} app_thread_source_manage_module;

typedef struct {    /* 工作队列模组数据项 */
    void (*routine)(void *parameter);   /* 数据处理例程 */
    void  *parameter;                   /* 数据处理例程参数 */
} app_thread_source_manage_work_t;

#endif
