
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

static app_lv_wheel_t     app_lv_wheel = {0};
static app_lv_wheel_src_t app_lv_wheel_src = {0};
static lv_obj_t          *app_lv_wheel_obj = NULL;

/*@brief     lvgl wheel更新事件回调
 *           内部使用: 被lvgl线程使用
 *@param[in] wheel 轮盘实例
 */
void app_lv_wheel_update_handle(app_lv_wheel_t *wheel)
{
    APP_SYS_ASSERT(wheel != NULL);
    /* 隐藏旧轮盘,更新轮盘,显示新轮盘,轮盘复位 */
    app_lv_wheel_hide(&app_lv_wheel);
    app_lv_wheel = *wheel;
    app_lv_wheel_show(&app_lv_wheel);
    app_lv_wheel_reset(&app_lv_wheel);
}

/*@brief  轮盘场景对象
 *@retval 轮盘场景对象
 */
lv_obj_t * app_lv_wheel_obj_inst(void)
{
    /* 轮盘是仅次于屏幕对象的场景 */
    /* 所有用户场景都是轮盘的子场景 */
    /* 之所以要做这一个步骤则是对于关键的事件而言 */
    /* 事件必须先冒泡到轮盘中才能进行多场景的调度 */
    if (app_lv_wheel_obj == NULL)
        app_lv_wheel_obj  = app_lv_style_scene();
    
    return app_lv_wheel_obj;
}

/*@brief  轮盘场景资源
 *@retval 轮盘场景资源
 */
app_lv_wheel_src_t * app_lv_wheel_src_inst(void)
{
    return &app_lv_wheel_src;
}
