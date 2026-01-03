/*实现目标:
 *    软件定时器相关的功能组件
 *    备注:某些平台没有RTOS软件定时器
 *         所以做一个效果类似的RTOS软件定时器
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

static app_sys_timer_list_t app_sys_timer_list = {0};

/*@brief 停止,中止,终止软件定时器(中断环境下不可调用)
 *@param timer 定时器实例
 *@retval 执行是否有效
 */
bool app_sys_timer_stop(app_sys_timer_t *timer)
{
    bool retval = false;
    app_sys_timer_t *current = NULL;
    app_mutex_process(&app_sys_timer_list.mutex, app_mutex_take);
    /* 检查等待者队列 */
    if (app_sys_timer_list.number != 0) {
        app_sys_list_sln_t *prev = NULL;
        app_sys_list_sll_tra(&app_sys_timer_list.sl_list, node) {
            current = app_sys_own_ofs(app_sys_timer_t, sl_node, node);
            if (current == timer) {
                app_sys_list_sll_remove(&app_sys_timer_list.sl_list, prev, &timer->sl_node);
                app_sys_timer_list.number--;
                /* 如果后面还有等待者,需要将约减值累加到后面 */
                if ((node = app_sys_list_sln_buddy(node)) != NULL) {
                    current = app_sys_own_ofs(app_sys_timer_t, sl_node, node);
                    current->reduce += timer->reduce;
                }
                retval = true;
                break;
            }
            prev = node;
        }
    }
    app_mutex_process(&app_sys_timer_list.mutex, app_mutex_give);
    return retval;
}

/*@brief 启动软件定时器(中断环境下不可调用)
 *@param timer 定时器实例
 *@retval 执行是否有效
 */
bool app_sys_timer_start(app_sys_timer_t *timer)
{
    bool status = true;
    app_sys_timer_t *current = NULL;
    if (timer->expired == NULL || timer->peroid == 0)
        return false;
    /* 初始化软件定时器 */
    app_mutex_process(&app_sys_timer_list.mutex, app_mutex_take);
    /* 预检查:不要出现相同参数的软件定时器 */
    if (status) {
        app_sys_list_sll_tra(&app_sys_timer_list.sl_list, node) {
            current = app_sys_own_ofs(app_sys_timer_t, sl_node, node);
            if (current == timer) {
                status = false;
                break;
            }
        }
    }
    /* 检查等待者队列 */
    if (status) {
        app_sys_list_sln_reset(&timer->sl_node);
        timer->reduce = timer->peroid;
        if (app_sys_timer_list.number == 0) {
            app_sys_list_sll_pinsert(&app_sys_timer_list.sl_list, &timer->sl_node);
            app_sys_timer_list.number++;
            status = false;
        }
    }
    /* 遍历等待者队列 */
    if (status) {
        app_sys_list_sln_t *prev = NULL;
        app_sys_list_sll_tra(&app_sys_timer_list.sl_list, node) {
            current = app_sys_own_ofs(app_sys_timer_t, sl_node, node);
            if (timer->reduce >= current->reduce)
                timer->reduce -= current->reduce;
            else {
                current->reduce -= timer->reduce;
                if (prev == NULL)
                    app_sys_list_sll_pinsert(&app_sys_timer_list.sl_list, &timer->sl_node);
                else
                    app_sys_list_sll_insert(&app_sys_timer_list.sl_list, prev, &timer->sl_node);
                app_sys_timer_list.number++;
                status = false;
                break;
            }
            prev = node;
        }
    }
    /* 添加到末尾 */
    if (status) {
        app_sys_list_sll_ainsert(&app_sys_timer_list.sl_list, &timer->sl_node);
        app_sys_timer_list.number++;
    }
    /*  */
    app_mutex_process(&app_sys_timer_list.mutex, app_mutex_give);
    /*  */
    return true;
}

/*@brief 约减软件定时器
 *       内部使用: 被线程使用,由1ms事件触发
 */
void app_sys_timer_reduce(void)
{
    /* 清空执行序列,收集执行目标(它无需被保护) */
    app_sys_list_sll_reset(&app_sys_timer_list.sl_list_e);
    app_sys_timer_list.number_e = 0;
    
    app_mutex_process(&app_sys_timer_list.mutex, app_mutex_take);
    /* 约减首项等待者,查询执行目标 */
    if (app_sys_timer_list.number != 0) {
        app_sys_list_sln_t *node = app_sys_list_sll_head(&app_sys_timer_list.sl_list);
        app_sys_timer_t *timer = app_sys_own_ofs(app_sys_timer_t, sl_node, node);
        if (timer->reduce != 0)
            timer->reduce--;
    }
    
    /* 循环检查每个首项,检查是否约减命中 */
    while (app_sys_timer_list.number != 0) {
        app_sys_list_sln_t *node = app_sys_list_sll_head(&app_sys_timer_list.sl_list);
        app_sys_timer_t *timer = app_sys_own_ofs(app_sys_timer_t, sl_node, node);
        if (timer->reduce != 0)
            break;
        /* 将节点取出,存放至执行队列 */
        app_sys_list_sll_remove(&app_sys_timer_list.sl_list, NULL, node);
        app_sys_list_sll_ainsert(&app_sys_timer_list.sl_list_e, node);
        app_sys_timer_list.number--;
        app_sys_timer_list.number_e++;
    }
    app_mutex_process(&app_sys_timer_list.mutex, app_mutex_give);
    
    /* 循环执行每个首项,直到清空队列 */
    while (app_sys_timer_list.number_e != 0) {
        app_sys_list_sln_t *node = app_sys_list_sll_head(&app_sys_timer_list.sl_list_e);
        app_sys_timer_t *timer = app_sys_own_ofs(app_sys_timer_t, sl_node, node);
        /* 先将执行节点从执行队列取出 */
        app_sys_list_sll_remove(&app_sys_timer_list.sl_list_e, NULL, node);
        app_sys_timer_list.number_e--;
        /* 检查是否需要重加载 */
        if (timer->reload)
            app_sys_timer_start(timer);
        /* 执行过期回调 */
        timer->expired(timer);
    }
}

/*@brief 软件定时器模组初始化
 *       内部使用: 被线程使用
 */
void app_sys_timer_ready(void)
{
    app_mutex_process(&app_sys_timer_list.mutex, app_mutex_static);
    app_sys_list_sll_reset(&app_sys_timer_list.sl_list);
}
