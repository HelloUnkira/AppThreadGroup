#ifndef APP_THREAD_MANAGE_H
#define APP_THREAD_MANAGE_H

typedef enum {  /* 线程模组 */
    app_thread_manage_none = 0,    /* 系统模组,占位符(线程组公共事件) */
} app_thread_manage_module_t;

/* 子线程模组 */
#include "app_module_data_center_type.h"
#include "app_module_data_center.h"

#endif
