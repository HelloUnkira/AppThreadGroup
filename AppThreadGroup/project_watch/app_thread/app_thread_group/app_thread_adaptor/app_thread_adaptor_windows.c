/*实现目标:
 *    Windows线程适配
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

#if APP_OS_IS_WINDOWS

/* @线程体<Start> */

app_thread_t app_thread_master = {
   .priority = THREAD_PRIORITY_HIGHEST,
   .routine  = app_thread_master_routine,
   .args     = NULL,
};

app_thread_t app_thread_mix_irq = {
   .priority = THREAD_PRIORITY_ABOVE_NORMAL,
   .routine  = app_thread_mix_irq_routine,
   .args     = NULL,
};

app_thread_t app_thread_mix_custom = {
   .priority = THREAD_PRIORITY_NORMAL,
   .routine  = app_thread_mix_custom_routine,
   .args     = NULL,
};

app_thread_t app_thread_manage = {
   .priority = THREAD_PRIORITY_BELOW_NORMAL,
   .routine  = app_thread_manage_routine,
   .args     = NULL,
};

#if 0
#elif APP_DEV_GUI_IS_LVGL
app_thread_t app_thread_lvgl = {
   .priority = THREAD_PRIORITY_NORMAL,
   .routine  = app_thread_lvgl_routine,
   .args     = NULL,
};
#elif APP_DEV_GUI_IS_SCUI
app_thread_t app_thread_scui = {
  .priority = THREAD_PRIORITY_NORMAL,
  .routine  = app_thread_scui_routine,
  .args     = NULL,
};
#else
#endif

/* @线程体<End> */

/*@brief 生成新的线程信息通过旧的线程信息
 *       旧线程的基本信息全部被新线程基础,除去执行回调
 *@param thread_old 旧线程的基本信息
 *@param thread_new 新线程的基本信息
 *@param handler    新线程的线程体
 */
void app_thread_group_create(app_thread_t *thread_old,
                             app_thread_t *thread_new,
                             app_thread_handler handler)
{
    memcpy(thread_new, thread_old, sizeof(app_thread_t));
    thread_new->routine = handler;
}

/*@brief 销毁新的线程信息
 *@param thread_new 新线程的基本信息
 */
void app_thread_group_destroy(app_thread_t *thread_new)
{
}

#endif
