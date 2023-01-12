#ifndef APP_THREAD_INTERFACE_H
#define APP_THREAD_INTERFACE_H

#include "app_arch_interface.h"
#include "app_os_interface.h"
#include "app_sys_interface.h"
/* 导出外部接口补充...... */
#include "app_thread_master.h"
#include "app_thread_mix_irq.h"
#include "app_thread_mix_custom.h"
#include "app_thread_source_manage.h"
#include "app_thread_lvgl.h"

/* 线程检查与线程子模组检查 */
#define APP_THREAD_CHECK    1
#define APP_MODULE_CHECK    1

#endif
