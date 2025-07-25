#ifndef APP_THREAD_MIX_CUSTOM_H
#define APP_THREAD_MIX_CUSTOM_H

typedef enum {  /* 线程模组 */
    app_thread_mix_custom_none = 0,     /* 系统模组,占位符(线程组公共事件) */
    app_thread_mix_custom_data_dump,    /* 资源转储模组 */
    app_thread_mix_custom_data_load,    /* 资源加载模组 */
} app_thread_mix_custom_module_t;

/* 子线程模组 */
#include "app_module_data_center_type.h"
#include "app_module_data_center_src.h"
#include "app_module_data_center.h"
#include "app_module_data_load.h"
#include "app_module_data_dump.h"

#endif
