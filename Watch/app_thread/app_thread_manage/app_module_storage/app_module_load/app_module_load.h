#ifndef APP_MODULE_LOAD_H
#define APP_MODULE_LOAD_H

/*@brief 加载流程是否结束
 */
bool app_module_load_not_over(void);

/*@brief 资源数据从外存加载到内存
 */
void app_module_load_process(void);

/*@brief 系统加载模组启动
 */
void app_module_load_event(void);

#endif
