/*实现目标:
 *    一些和提醒女性健康运算相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

static uint32_t                              app_module_remind_menstruation_group = -1;
static app_mutex_t                           app_module_remind_menstruation_mutex = {0};
static app_module_remind_item_t              app_module_remind_menstruation_item[APP_MODULE_REMIND_MENSTRUATION_MAX] = {0};
static app_module_remind_menstruation_info_t app_module_remind_menstruation_info[APP_MODULE_REMIND_MENSTRUATION_MAX] = {0};

/*@brief 提醒组是否为提醒女性健康组
 *@param remind_group 提醒组
 *@retval 是否为女性健康组
 */
bool app_module_remind_menstruation_group_check(uint32_t remind_group)
{
    app_mutex_process(&app_module_remind_menstruation_mutex, app_mutex_take);
    bool retval = remind_group == app_module_remind_menstruation_group;
    app_mutex_process(&app_module_remind_menstruation_mutex, app_mutex_give);
    return retval;
}

/*@brief 锁定提醒女性健康列表
 *       此时提醒女性健康列表不再迭代,也不生成事件
 *       可以直接对提醒女性健康列表本地修改
 */
void app_module_remind_menstruation_array_lock(void)
{
    /* 移除出提醒组,打断其事件更新 */
    app_mutex_process(&app_module_remind_menstruation_mutex, app_mutex_take);
    app_module_remind_group_del(app_module_remind_menstruation_group);
    app_module_remind_menstruation_group = -1;
    app_mutex_process(&app_module_remind_menstruation_mutex, app_mutex_give);
}

/*@brief 解锁提醒女性健康列表
 *       此时提醒女性健康列表继续迭代,继续生成事件
 *       不可以直接对提醒女性健康列表本地修改
 */
void app_module_remind_menstruation_array_unlock(void)
{
    /* 加入进提醒组,恢复其事件更新 */
    app_mutex_process(&app_module_remind_menstruation_mutex, app_mutex_take);
    app_module_remind_menstruation_group = app_module_remind_group_add(app_module_remind_menstruation_item, APP_MODULE_REMIND_ALARM_MAX);
    app_mutex_process(&app_module_remind_menstruation_mutex, app_mutex_give);
}

/*@brief 提醒女性健康列表
 *@param menstruation_item 提醒女性健康列表
 *@param menstruation_info 提醒女性健康信息列表
 */
void app_module_remind_menstruation_array(app_module_remind_item_t **menstruation_item, app_module_remind_menstruation_info_t **menstruation_info)
{
    *menstruation_item = app_module_remind_menstruation_item;
    *menstruation_info = app_module_remind_menstruation_info;
}

/*@brief 提醒女性健康转储到外存
 */
void app_module_remind_menstruation_dump(void)
{
    /* 更新数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    app_module_remind_menstruation_array_lock();
    // memcpy(&data_center_src->module_remind_menstruation.menstruation_item, app_module_remind_menstruation_item, sizeof(app_module_remind_menstruation_item));
    // memcpy(&data_center_src->module_remind_menstruation.menstruation_info, app_module_remind_menstruation_info, sizeof(app_module_remind_menstruation_info));
    app_module_remind_menstruation_array_unlock();
    app_module_data_center_give();
}

/*@brief 提醒女性健康加载到内存
 */
void app_module_remind_menstruation_load(void)
{
    /* 更新数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    app_module_remind_menstruation_array_lock();
    // memcpy(app_module_remind_menstruation_item, &data_center_src->module_remind_menstruation.menstruation_item, sizeof(app_module_remind_menstruation_item));
    // memcpy(app_module_remind_menstruation_info, &data_center_src->module_remind_menstruation.menstruation_info, sizeof(app_module_remind_menstruation_info));
    app_module_remind_menstruation_array_unlock();
    app_module_data_center_give();
}

/*@brief 初始化提醒女性健康组
 */
void app_module_remind_menstruation_ready(void)
{
    app_mutex_process(&app_module_remind_menstruation_mutex, app_mutex_static);
}
