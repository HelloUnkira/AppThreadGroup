#ifndef SCUI_OS_H
#define SCUI_OS_H

/* 转为适配app_ext_os接口
 */

#define scui_sem_t              app_sem_t
#define scui_sem_process        app_sem_process
#define scui_sem_option_t       app_sem_option_t
#define scui_sem_static         app_sem_static
#define scui_sem_create         app_sem_create
#define scui_sem_destroy        app_sem_destroy
#define scui_sem_take           app_sem_take
#define scui_sem_give           app_sem_give

#define scui_mutex_t            app_mutex_t
#define scui_mutex_process      app_mutex_process
#define scui_mutex_option_t     app_mutex_option_t
#define scui_mutex_static       app_mutex_static
#define scui_mutex_create       app_mutex_create
#define scui_mutex_destroy      app_mutex_destroy
#define scui_mutex_take         app_mutex_take
#define scui_mutex_give         app_mutex_give

#endif
