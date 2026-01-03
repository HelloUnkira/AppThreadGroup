#ifndef APP_SYS_TIMER_H
#define APP_SYS_TIMER_H

typedef struct {
    app_sys_list_sln_t sl_node;         /* 内部使用:链表 */
    void   (*expired)(void *timer);     /* 到期回调 */
    void    *user_data;                 /* 用户挂载数据 */
    uint32_t reduce;                    /* 内部使用:约减数 */
    uint32_t peroid;                    /* 回调周期 */
    uint32_t reload:1;                  /* 是否自重载 */
} app_sys_timer_t;

typedef struct {
    app_mutex_t mutex;
    app_sys_list_sll_t sl_list;
    app_sys_list_sll_t sl_list_e;
    uint32_t number;
    uint32_t number_e;
} app_sys_timer_list_t;

/*@brief 停止,中止,终止软件定时器(中断环境下不可调用)
 *@param timer 定时器实例
 *@retval 执行是否有效
 */
bool app_sys_timer_stop(app_sys_timer_t *timer);

/*@brief 启动软件定时器(中断环境下不可调用)
 *@param timer 定时器实例
 *@retval 执行是否有效
 */
bool app_sys_timer_start(app_sys_timer_t *timer);

/*@brief 约减软件定时器
 *       内部使用: 被线程使用,由1ms事件触发
 */
void app_sys_timer_reduce(void);

/*@brief 软件定时器模组初始化
 *       内部使用: 被线程使用
 */
void app_sys_timer_ready(void);

/*@brief 软件定时器模组测试
 */
void app_sys_timer_test(void);

#endif
