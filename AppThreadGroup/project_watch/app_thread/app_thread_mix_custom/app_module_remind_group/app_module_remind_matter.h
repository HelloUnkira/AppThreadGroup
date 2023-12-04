#ifndef APP_MODULE_REMIND_MATTER_H
#define APP_MODULE_REMIND_MATTER_H

#define APP_MODULE_REMIND_MATTER_MAX              (10)
#define APP_MODULE_REMIND_MATTER_TITLE_MAX        (64)
#define APP_MODULE_REMIND_MATTER_DESCRIPT_MAX     (128)

typedef struct {
    uint8_t  title[APP_MODULE_REMIND_MATTER_TITLE_MAX];
    uint8_t  descript[APP_MODULE_REMIND_MATTER_DESCRIPT_MAX];
} app_module_remind_matter_info_t;

/*@brief 提醒组是否为提醒事项组
 *@param remind_group 提醒组
 *@retval 是否为事项组
 */
bool app_module_remind_matter_group_check(uint32_t remind_group);

/*@brief 锁定提醒事项列表
 *       此时提醒事项列表不再迭代,也不生成事件
 *       可以直接对提醒事项列表本地修改
 */
void app_module_remind_matter_array_lock(void);

/*@brief 解锁提醒事项列表
 *       此时提醒事项列表继续迭代,继续生成事件
 *       不可以直接对提醒事项列表本地修改
 */
void app_module_remind_matter_array_unlock(void);

/*@brief 提醒事项列表
 *@param matter_item 提醒事项列表
 *@param matter_info 提醒事项信息列表
 */
void app_module_remind_matter_array(app_module_remind_item_t **matter_item, app_module_remind_matter_info_t **matter_info);

/*@brief 提醒事项转储到外存
 */
void app_module_remind_matter_dump(void);

/*@brief 提醒事项加载到内存
 */
void app_module_remind_matter_load(void);

/*@brief 初始化提醒事项组
 */
void app_module_remind_matter_ready(void);

#endif
