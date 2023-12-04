/*实现目标:
 *    lvgl轮盘场景调度
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static uint8_t app_lv_scene_num = 0;
static app_lv_scene_t *app_lv_ui_scene[APP_LV_SCENE_NEST] = {0};

/*@brief 默认轮盘场景
 *@param current 轮盘场景
 *@param parent  轮盘场景
 *@retval 轮盘场景
 */
app_lv_wheel_t app_lv_scene_wheel_def(app_lv_scene_t *current, app_lv_scene_t *parent)
{
    /* 左右滑动返回上一层 */
    /* 所以将左右兄弟设置为父亲以回滚 */
    app_lv_wheel_t wheel = {
        .self       = current,
        .sibling[0] = parent,
        .sibling[1] = parent,
        .style[0]   = app_lv_wheel_style_rotate,
        .style[1]   = app_lv_wheel_style_rotate,
    };
    return wheel;
}

/*@brief 当前场景嵌套层级
 *@retval 场景数量
 */
uint8_t app_lv_scene_get_nest(void)
{
    APP_SYS_ASSERT(app_lv_scene_num != 0);
    return app_lv_scene_num;
}

/*@brief 获取次上层显示场景
 *@param scene 场景(不可为空)
 */
void app_lv_scene_get_last(app_lv_scene_t **scene)
{
    APP_SYS_ASSERT(scene != NULL);
    APP_SYS_ASSERT(app_lv_scene_num > 1);
    *scene = app_lv_ui_scene[app_lv_scene_num - 2];
}

/*@brief 获取最上层显示场景
 *@param scene 场景(不可为空)
 */
void app_lv_scene_get_top(app_lv_scene_t **scene)
{
    APP_SYS_ASSERT(scene != NULL);
    APP_SYS_ASSERT(app_lv_scene_num != 0);
    *scene = app_lv_ui_scene[app_lv_scene_num - 1];
}

/*@brief 场景复位
 *@param scene 场景
 *@param reserve 保留当前场景显示
 */
void app_lv_scene_reset(app_lv_scene_t *scene, bool reserve)
{
    app_lv_scene_t *current = NULL;
    if (reserve) {
        /* 当前场景保留则只插入新场景 */
        app_lv_scene_get_top(&current);
        app_lv_scene_num = 0;
        app_lv_ui_scene[app_lv_scene_num++] = scene;
        app_lv_ui_scene[app_lv_scene_num++] = current;
    } else {
        app_lv_scene_num = 0;
        app_lv_ui_scene[app_lv_scene_num++] = scene;
        app_lv_scene_get_top(&current);
        /* 存在自定义轮盘则使用自定义轮盘, 否则使用默认轮盘 */
        if (current->wheel != NULL)
            app_lv_wheel_update(current->wheel, sizeof(app_lv_wheel_t));
        else {
            app_lv_wheel_t wheel = app_lv_scene_wheel_def(current, NULL);
            app_lv_wheel_update(&wheel, sizeof(app_lv_wheel_t));
        }
    }
}

/*@brief 场景覆盖显示场景
 *@param scene 场景
 */
void app_lv_scene_cover(app_lv_scene_t *scene)
{
    APP_SYS_ASSERT(scene != NULL);
    APP_SYS_ASSERT(app_lv_scene_num != 0);
    app_lv_scene_t *current = NULL;
    app_lv_scene_t *parent  = NULL;
    
    /* 轮盘正在工作忽略此次调度 */
    if (app_lv_wheel_status(LV_DIR_ALL))
        return;
    
    app_lv_ui_scene[app_lv_scene_num - 1] = scene;
    app_lv_scene_get_top(&current);
    if (app_lv_scene_num > 1)
        app_lv_scene_get_last(&parent);
    /* 存在自定义轮盘则使用自定义轮盘, 否则使用默认轮盘 */
    if (current->wheel != NULL)
        app_lv_wheel_update(current->wheel, sizeof(app_lv_wheel_t));
    else {
        app_lv_wheel_t wheel = app_lv_scene_wheel_def(current, parent);
        app_lv_wheel_update(&wheel, sizeof(app_lv_wheel_t));
    }
}

/*@brief 场景添加新场景
 *@param scene 场景
 *@param reserve 保留当前场景
 */
void app_lv_scene_add(app_lv_scene_t *scene, bool reserve)
{
    APP_SYS_ASSERT(scene != NULL);
    app_lv_scene_t *current = NULL;
    app_lv_scene_t *parent  = NULL;
    
    /* 轮盘正在工作忽略此次调度 */
    if (app_lv_wheel_status(LV_DIR_ALL))
        return;
    
    if (reserve) {
        /* 当前场景保留则只插入新场景 */
        app_lv_scene_get_top(&current);
        app_lv_scene_num++;
        app_lv_ui_scene[app_lv_scene_num - 2] = scene;
        app_lv_ui_scene[app_lv_scene_num - 1] = current;
    } else {
        app_lv_ui_scene[app_lv_scene_num++] = scene;
        app_lv_scene_get_top(&current);
        if (app_lv_scene_num > 1)
            app_lv_scene_get_last(&parent);
        /* 存在自定义轮盘则使用自定义轮盘, 否则使用默认轮盘 */
        if (current->wheel != NULL)
            app_lv_wheel_update(current->wheel, sizeof(app_lv_wheel_t));
        else {
            app_lv_wheel_t wheel = app_lv_scene_wheel_def(current, parent);
            app_lv_wheel_update(&wheel, sizeof(app_lv_wheel_t));
        }
    }
}

/*@brief 场景移除当前显示场景
 *@param scene 场景
 */
void app_lv_scene_del(app_lv_scene_t **scene)
{
    APP_SYS_ASSERT(scene != NULL);
    APP_SYS_ASSERT(app_lv_scene_num != 0);
    app_lv_scene_t *current = NULL;
    app_lv_scene_t *parent  = NULL;
    
    /* 轮盘正在工作忽略此次调度 */
    if (app_lv_wheel_status(LV_DIR_ALL))
        return;
    
    *scene = app_lv_ui_scene[--app_lv_scene_num];
    app_lv_scene_get_top(&current);
    if (app_lv_scene_num > 1)
        app_lv_scene_get_last(&parent);
    /* 存在自定义轮盘则使用自定义轮盘, 否则使用默认轮盘 */
    if (current->wheel != NULL)
        app_lv_wheel_update(current->wheel, sizeof(app_lv_wheel_t));
    else {
        app_lv_wheel_t wheel = app_lv_scene_wheel_def(current, parent);
        app_lv_wheel_update(&wheel, sizeof(app_lv_wheel_t));
    }
}

/*@brief 场景为轮盘更新接口
 *@param scene 场景
 *@param state 0:add;1:cover;2:del;
 */
void app_lv_scene_wheel_update(app_lv_scene_t **scene, int8_t state)
{
    APP_SYS_ASSERT(scene != NULL);
    app_lv_scene_t *current = NULL;
    app_lv_scene_t *parent  = NULL;
    
    switch (state) {
    case 0: {
        app_lv_ui_scene[app_lv_scene_num++] = scene;
        break;
    }
    case 1: {
        APP_SYS_ASSERT(app_lv_scene_num != 0);
        app_lv_ui_scene[app_lv_scene_num - 1] = *scene;
        break;
    }
    case 2: {
        APP_SYS_ASSERT(app_lv_scene_num != 0);
        *scene = app_lv_ui_scene[--app_lv_scene_num];
        break;
    }
    default:
        APP_SYS_ASSERT(true == false);
        return;
    }
    
    app_lv_scene_get_top(&current);
    if (app_lv_scene_num > 1)
        app_lv_scene_get_last(&parent);
    /* 存在自定义轮盘则使用自定义轮盘, 否则使用默认轮盘 */
    if (current->wheel != NULL)
        app_lv_wheel_update(current->wheel, sizeof(app_lv_wheel_t));
    else {
        app_lv_wheel_t wheel = app_lv_scene_wheel_def(current, parent);
        app_lv_wheel_update(&wheel, sizeof(app_lv_wheel_t));
    }
}
