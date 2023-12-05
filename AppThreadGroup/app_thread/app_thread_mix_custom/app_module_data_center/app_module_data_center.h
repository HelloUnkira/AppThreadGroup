#ifndef APP_MODULE_DATA_CENTER_H
#define APP_MODULE_DATA_CENTER_H

/*@brief 更换目标数据类型并锁定数据中心
 *@param type 数据中心类型(app_module_data_center_src_t(type))
 *@retval 数据中心的缓存地址
 */
app_module_data_center_src_t * app_module_data_center_take(uint32_t type);

/*@brief 解锁数据中心
 */
void app_module_data_center_give(void);

/*@brief 刷新数据中心
 */
void app_module_data_center_reflush(void);

/*@brief 回收数据中心资源
 *@param force 强制回收数据中心资源(不建议使用)
 *       不建议使用,这会导致非正常的逻辑
 */
void app_module_data_center_recycle(bool force);

/*@brief 数据中心模组初始化
 *       内部使用: 被manage线程使用
 */
void app_module_data_center_ready(void);

#endif
