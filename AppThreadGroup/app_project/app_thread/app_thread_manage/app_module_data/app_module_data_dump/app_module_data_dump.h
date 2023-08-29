#ifndef APP_MODULE_DATA_DUMP_H
#define APP_MODULE_DATA_DUMP_H

/*@brief 转储流程是否结束
 */
bool app_module_data_dump_not_over(void);

/*@brief 资源数据从内存转储到外存
 */
void app_module_data_dump_process(void);

/*@brief 系统转储模组启动
 */
void app_module_data_dump_event(void);

#endif
