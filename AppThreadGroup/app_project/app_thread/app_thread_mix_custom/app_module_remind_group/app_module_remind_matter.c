/*实现目标:
 *    一些和提醒事项运算相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

static uint32_t                        app_module_remind_matter_group = -1;
static app_mutex_t                     app_module_remind_matter_mutex = {0};
static app_module_remind_item_t        app_module_remind_matter_item[APP_MODULE_REMIND_MATTER_MAX] = {0};
static app_module_remind_matter_info_t app_module_remind_matter_info[APP_MODULE_REMIND_MATTER_MAX] = {0};

/*@brief      提醒组是否为提醒事项组
 *@param[out] remind_group 提醒组
 *@retval     是否为事项组
 */
bool app_module_remind_matter_group_check(uint32_t remind_group)
{
    app_mutex_process(&app_module_remind_matter_mutex, app_mutex_take);
    bool retval = remind_group == app_module_remind_matter_group;
    app_mutex_process(&app_module_remind_matter_mutex, app_mutex_give);
    return retval;
}

/*@brief 锁定提醒事项列表
 *       此时提醒事项列表不再迭代,也不生成事件
 *       可以直接对提醒事项列表本地修改
 */
void app_module_remind_matter_array_lock(void)
{
    /* 移除出提醒组,打断其事件更新 */
    app_mutex_process(&app_module_remind_matter_mutex, app_mutex_take);
    app_module_remind_group_del(app_module_remind_matter_group);
    app_module_remind_matter_group = -1;
    app_mutex_process(&app_module_remind_matter_mutex, app_mutex_give);
}

/*@brief 解锁提醒事项列表
 *       此时提醒事项列表继续迭代,继续生成事件
 *       不可以直接对提醒事项列表本地修改
 */
void app_module_remind_matter_array_unlock(void)
{
    /* 加入进提醒组,恢复其事件更新 */
    app_mutex_process(&app_module_remind_matter_mutex, app_mutex_take);
    app_module_remind_matter_group = app_module_remind_group_add(app_module_remind_matter_item, APP_MODULE_REMIND_ALARM_MAX);
    app_mutex_process(&app_module_remind_matter_mutex, app_mutex_give);
}

/*@brief      提醒事项列表
 *@param[out] matter_item 提醒事项列表
 *@param[out] matter_info 提醒事项信息列表
 */
void app_module_remind_matter_array(app_module_remind_item_t **matter_item, app_module_remind_matter_info_t **matter_info)
{
    *matter_item = app_module_remind_matter_item;
    *matter_info = app_module_remind_matter_info;
}

/*@brief 提醒事项转储到外存
 */
void app_module_remind_matter_dump(void)
{
    /* 更新数据中心资源 */
    app_module_data_center_t *data_center = app_module_data_center_take(app_module_data_center_module_source);
    app_module_remind_matter_array_lock();
    memcpy(&data_center->remind_matter.matter_item, app_module_remind_matter_item, sizeof(app_module_remind_matter_item));
    memcpy(&data_center->remind_matter.matter_info, app_module_remind_matter_info, sizeof(app_module_remind_matter_info));
    app_module_remind_matter_array_unlock();
    app_module_data_center_give();
}

/*@brief 提醒事项加载到内存
 */
void app_module_remind_matter_load(void)
{
    /* 更新数据中心资源 */
    app_module_data_center_t *data_center = app_module_data_center_take(app_module_data_center_module_source);
    app_module_remind_matter_array_lock();
    memcpy(app_module_remind_matter_item, &data_center->remind_matter.matter_item, sizeof(app_module_remind_matter_item));
    memcpy(app_module_remind_matter_info, &data_center->remind_matter.matter_info, sizeof(app_module_remind_matter_info));
    app_module_remind_matter_array_unlock();
    app_module_data_center_give();
}

/*@brief 初始化提醒事项组
 */
void app_module_remind_matter_ready(void)
{
    app_mutex_process(&app_module_remind_matter_mutex, app_mutex_static);
}