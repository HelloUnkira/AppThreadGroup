#ifndef APP_THREAD_SLAVE_H
#define APP_THREAD_SLAVE_H

typedef void (*app_thread_slave_process_ready_cb)(void);
typedef bool (*app_thread_slave_process_package_cb)(app_thread_package_t *package, bool *record);

/*@brief 从线程服务例程结构模板
 *       这是通用化的结构模板,每个子线程均使用它
 *@param app_thread_id 子线程ID号
 *@param ready_cb      子线程服务例程就绪部
 *@param package_cb    子线程服务例程处理部
 */
void app_thread_slave_process(uint32_t app_thread_id,
                              app_thread_slave_process_ready_cb   ready_cb,
                              app_thread_slave_process_package_cb package_cb);

#endif
