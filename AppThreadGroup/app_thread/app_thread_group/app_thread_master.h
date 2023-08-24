#ifndef APP_THREAD_MASTER_H
#define APP_THREAD_MASTER_H

typedef   app_sys_pipe_pkg_t app_thread_package_t;

/* 事件包裹推荐优先级 */
/* 归0初始化事件包默认为最低优先级 */
/* 实时优先级专用于密集化时间敏感事件 */
typedef enum {
    app_thread_package_priority_lowest = 0,
    app_thread_package_priority_normal_below,
    app_thread_package_priority_normal,
    app_thread_package_priority_normal_above,
    app_thread_package_priority_highest,
    app_thread_package_priority_real_time,
} app_thread_package_priority_t;

/*@brief        设置子线程执行时间
 *              注意:这里的时间设置为累加设置
 *@param[in]    uint32_t thread 线程ID
 *@param[out]   子线程执行时间(us)
 */
#if APP_THREAD_SLAVE_EXECUTE_TIME
void app_thread_execute_us_set(uint32_t thread, double *execute_us);
#endif

/*@brief        获得子线程执行时间
 *@param[in]    uint32_t thread 线程ID
 *@param[out]   子线程执行时间(us)
 */
#if APP_THREAD_SLAVE_EXECUTE_TIME
void app_thread_execute_us_get(uint32_t thread, double *execute_us);
#endif

/*@brief     判断一个动态子线程id号是否合法
 *@param[in] thread 动态子线程id号
 *@retval    动态子线程id号是否合法
 */
bool app_thread_id_is_valid(uint32_t thread);

/*@brief  获得一个动态子线程id号
 *        子线程获得id号之后就获得了主从线程组的调度规则
 *        否则子线程就只能向普通子线程一样工作
 *@retval 动态子线程id号
 */
uint32_t app_thread_id_alloc(void);

/*@brief     回收一个动态子线程id号
 *@param[in] thread 动态子线程id号
 *@param[in] burn   动态子线程指定的回收流程,快速批量处理不再响应的事件包
 */
void app_thread_id_free(uint32_t thread, void (*burn)(app_thread_package_t *package));

/*@brief        通过从线程ID获得与主线程的同步资源
 *@param[in]    thread 线程ID
 *@param[out]   sem 信号量
 */
void app_thread_src_sem(uint32_t thread, app_sem_t **sem);

/*@brief        通过从线程ID获得与主线程的交互管道
 *@param[in]    thread 线程ID
 *@param[out]   pipe 管道
 */
void app_thread_src_pipe(uint32_t thread, app_sys_pipe_t **pipe);

/*@brief        线程组接收一个事件包
 *@param[in]    thread_id 线程ID
 *@param[in]    package   事件包
 #@retval       失败表明线程组中止,不接收新的事件包
 */
bool app_thread_package_notify(app_thread_package_t *package);

#endif
