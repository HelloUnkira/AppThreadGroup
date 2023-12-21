/*实现目标:
 *    Zephyr线程适配
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

#if APP_OS_IS_ZEPHYR

/* @线程体栈信息<Start> */

#define APP_THREAD_MASTER_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_master_stack, APP_THREAD_MASTER_STACK_SIZE);

#define APP_THREAD_MIX_IRQ_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_mix_irq_stack, APP_THREAD_MIX_IRQ_STACK_SIZE);

#define APP_THREAD_MIX_CUSTOM_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_mix_custom_stack, APP_THREAD_MIX_CUSTOM_STACK_SIZE);

#define APP_THREAD_MANAGE_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_manage_stack, APP_THREAD_MANAGE_STACK_SIZE);

#define APP_THREAD_LVGL_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_lvgl_stack, APP_THREAD_LVGL_STACK_SIZE);

#define APP_THREAD_SCUI_STACK_SIZE  (1024)
static  K_THREAD_STACK_DEFINE(app_thread_scui_stack, APP_THREAD_SCUI_STACK_SIZE);

/* @线程体栈信息<End> */

/* @线程体<Start> */

app_thread_t app_thread_master = {
    .stack      = app_thread_master_stack,
    .stack_size = APP_THREAD_MASTER_STACK_SIZE,
    .priority   = 1,
    .entry      = app_thread_master_routine,
};

app_thread_t app_thread_mix_irq = {
    .stack      = app_thread_mix_irq_stack,
    .stack_size = APP_THREAD_MIX_IRQ_STACK_SIZE,
    .priority   = 2,
    .entry      = app_thread_mix_irq_routine,
};

app_thread_t app_thread_mix_custom = {
    .stack      = app_thread_mix_custom_stack,
    .stack_size = APP_THREAD_MIX_CUSTOM_STACK_SIZE,
    .priority   = 3,
    .entry      = app_thread_mix_custom_routine,
};

app_thread_t app_thread_manage = {
    .stack      = app_thread_manage_stack,
    .stack_size = APP_THREAD_MANAGE_STACK_SIZE,
    .priority   = 5,
    .entry      = app_thread_manage_routine,
};

app_thread_t app_thread_lvgl = {
    .stack      = app_thread_lvgl_stack,
    .stack_size = APP_THREAD_LVGL_STACK_SIZE,
    .priority   = 4,
    .entry      = app_thread_lvgl_routine,
};

app_thread_t app_thread_scui = {
    .stack      = app_thread_scui_stack,
    .stack_size = APP_THREAD_SCUI_STACK_SIZE,
    .priority   = 4,
    .entry      = app_thread_scui_routine,
};

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
    thread_new->entry = handler;
    thread_new->stack = app_mem_alloc(thread_new.stack_size);
}

/*@brief 销毁新的线程信息
 *@param thread_new 新线程的基本信息
 */
void app_thread_group_destroy(app_thread_t *thread_new)
{
    app_mem_free(thread_new->stack);
}

/*@brief 线程就绪后配置
 */
void app_thread_group_extend(void)
{
}

#endif
