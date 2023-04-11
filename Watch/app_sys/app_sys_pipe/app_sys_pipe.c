/*实现目标:
 *    一个用于线程间数据交互的管道
 *    它需要保证完成数据的基本交互
 *    注意:加入中断临界区的主要原因是防止
 *         中断打断正在线程环境执行的敏感操作及其
 *         中断打断正在中断环境执行的敏感操作
 *         导致错误管道资源包管理错误
 *         我们在设计上极可能减少资源包内容
 *         使得在临界区的时间尽可能缩短
 *         将更多的明细交付到线程环境解析
 *    此外:管道是线程专用容器,其他模组不得独自使用
 */

#include "app_ext_lib.h"
#include "app_sys_pipe.h"

/*@brief     初始化管道
 *@param[in] pipe 管道实例
 */
void app_sys_pipe_ready(app_sys_pipe_t *pipe)
{
    pipe->head = NULL;
    pipe->tail = NULL;
    pipe->number = 0;
    app_mutex_process(&pipe->mutex);
}

/*@brief     获取管道资源包数量
 *@param[in] pipe 管道实例
 *@retval    管道资源包数量
 */
uint32_t app_sys_pipe_package_num(app_sys_pipe_t *pipe)
{
    uint32_t number = 0;
    /* 入界 */
    app_mutex_take(&pipe->mutex);
    app_critical_enter();
    /* 资源检查 */
    number = pipe->number;
    /* 出界 */
    app_critical_exit();
    app_mutex_give(&pipe->mutex);
    /* 通报 */
    return number;
}

/*@brief     交付一个包给管道
 *@param[in] pipe    管道实例
 *@param[in] package 事件资源包(栈资源,非堆资源或静态资源)
 *@param[in] normal  不使用优先级
 */
void app_sys_pipe_give(app_sys_pipe_t *pipe, app_sys_pipe_pkg_t *package, bool normal)
{
    app_sys_pipe_pkg_t *nonius = NULL;
    app_sys_pipe_pkg_t *package_new = NULL;
    /* 生成资源包, 转储消息资源资源 */
    package_new = app_mem_alloc(sizeof(app_sys_pipe_pkg_t));
    package_new->buddy    = NULL;
    package_new->thread   = package->thread;
    package_new->module   = package->module;
    package_new->event    = package->event;
    package_new->priority = package->priority;
    package_new->dynamic  = package->dynamic;
    package_new->size     = package->size;
    package_new->data     = package->data;
    /* 入界 */
    app_mutex_take(&pipe->mutex);
    app_critical_enter();
    /* 资源包加入到管道(优先队列) */
    if (0) {
    } else if (pipe->number == 0) {
        pipe->head = package_new;
        pipe->tail = package_new;
    } else if (package_new->priority <= pipe->tail->priority || normal) {
        pipe->tail->buddy = package_new;
        pipe->tail = package_new;
    } else if (package_new->priority >  pipe->head->priority) {
        package_new->buddy = pipe->head;
        pipe->head = package_new;
    } else {
        for (nonius = pipe->head; nonius->buddy != NULL; nonius = nonius->buddy) {
            app_sys_pipe_pkg_t *current = nonius->buddy;
            if (current->priority < package_new->priority) {
                package_new->buddy = nonius->buddy;
                nonius->buddy = package_new;
                break;
            }
        }
    }
    pipe->number++;
    /* 出界 */
    app_critical_exit();
    app_mutex_give(&pipe->mutex);
}

/*@brief      从管道提取一个包
 *@param[in]  pipe     管道实例
 *@param[out] package  事件资源包(栈资源,非堆资源或静态资源)
 */
void app_sys_pipe_take(app_sys_pipe_t *pipe, app_sys_pipe_pkg_t *package)
{
    app_sys_pipe_pkg_t *package_new = NULL;
    /* 入界 */
    app_mutex_take(&pipe->mutex);
    app_critical_enter();
    /* 资源包提取出管道 */
    if (pipe->number != 0) {
        pipe->number--;
        package_new = pipe->head;
        pipe->head  = pipe->head->buddy;
        if (pipe->number == 0) {
            pipe->head = NULL;
            pipe->tail = NULL;
        }
    }
    /* 出界 */
    app_critical_exit();
    app_mutex_give(&pipe->mutex);
    if (package_new == NULL)
        return;
    /* 转储消息资源资源, 销毁资源包 */
    package->buddy    = NULL;
    package->thread   = package_new->thread;
    package->module   = package_new->module;
    package->event    = package_new->event;
    package->priority = package_new->priority;
    package->dynamic  = package_new->dynamic;
    package->size     = package_new->size;
    package->data     = package_new->data;
    app_mem_free(package_new);
}
