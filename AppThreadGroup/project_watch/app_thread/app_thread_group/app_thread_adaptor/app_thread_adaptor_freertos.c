/*实现目标:
 *    FreeRTOS线程适配
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

#if APP_OS_IS_FREERTOS

/* @线程体栈信息<Start> */

#define APP_THREAD_MASTER_STACK_SIZE  (1024)
static  StackType_t app_thread_master_stack[APP_THREAD_MASTER_STACK_SIZE];

#define APP_THREAD_MIX_IRQ_STACK_SIZE  (1024)
static  StackType_t app_thread_mix_irq_stack[APP_THREAD_MIX_IRQ_STACK_SIZE];

#define APP_THREAD_MIX_CUSTOM_STACK_SIZE  (1024)
static  StackType_t app_thread_mix_custom_stack[APP_THREAD_MIX_CUSTOM_STACK_SIZE];

#define APP_THREAD_MANAGE_STACK_SIZE   (1024)
static  StackType_t app_thread_manage_stack[APP_THREAD_MANAGE_STACK_SIZE];

#if 0
#elif APP_DEV_GUI_IS_LVGL
#define APP_THREAD_LVGL_STACK_SIZE  (1024)
static  StackType_t app_thread_lvgl_stack[APP_THREAD_LVGL_STACK_SIZE];
#elif APP_DEV_GUI_IS_SCUI
#define APP_THREAD_SCUI_STACK_SIZE  (1024)
static  StackType_t app_thread_scui_stack[APP_THREAD_SCUI_STACK_SIZE];
#else
#endif

/* @线程体栈信息<End> */

/* @线程体<Start> */

app_thread_t app_thread_master = {
    .stack      = app_thread_master_stack,
    .stack_size = APP_THREAD_MASTER_STACK_SIZE,
    .priority   = 1,
    .task       = app_thread_master_routine,
    .name       = "app_thread_master",
};

app_thread_t app_thread_mix_irq = {
    .stack      = app_thread_mix_irq_stack,
    .stack_size = APP_THREAD_MIX_IRQ_STACK_SIZE,
    .priority   = 2,
    .task       = app_thread_mix_irq_routine,
    .name       = "app_thread_mix_irq",
};

app_thread_t app_thread_mix_custom = {
    .stack      = app_thread_mix_custom_stack,
    .stack_size = APP_THREAD_MIX_CUSTOM_STACK_SIZE,
    .priority   = 3,
    .task       = app_thread_mix_custom_routine,
    .name       = "app_thread_mix_custom",
};

app_thread_t app_thread_manage = {
    .stack      = app_thread_manage_stack,
    .stack_size = APP_THREAD_MANAGE_STACK_SIZE,
    .priority   = 5,
    .task       = app_thread_manage_routine,
    .name       = "app_thread_manage",
};

#if 0
#elif APP_DEV_GUI_IS_LVGL
app_thread_t app_thread_lvgl = {
    .stack      = app_thread_lvgl_stack,
    .stack_size = APP_THREAD_LVGL_STACK_SIZE,
    .priority   = 4,
    .task       = app_thread_lvgl_routine,
    .name       = "app_thread_lvgl",
};
#elif APP_DEV_GUI_IS_SCUI
app_thread_t app_thread_scui = {
    .stack      = app_thread_scui_stack,
    .stack_size = APP_THREAD_SCUI_STACK_SIZE,
    .priority   = 4,
    .task       = app_thread_scui_routine,
    .name       = "app_thread_scui",
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
    thread_new->task  = handler;
    thread_new->stack = app_mem_alloc(thread_new.stack_size);
}

/*@brief 销毁新的线程信息
 *@param thread_new 新线程的基本信息
 */
void app_thread_group_destroy(app_thread_t *thread_new)
{
    app_mem_free(thread_new->stack);
}

#endif
