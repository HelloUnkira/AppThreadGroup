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
 *    补充:管道资源的加速获取与释放
 *         对于此系统而言管道资源是最频繁热数据
 *         无论是哪一种常规内存分配释放都会损毁性能
 *         这里使用一个SLAB分配器为其加速
 */

#include "app_ext_lib.h"
#include "app_sys_list.h"
#include "app_sys_slab.h"
#include "app_sys_pipe.h"

static app_sys_slab_t app_sys_pipe_slab = {0};

/*@brief 初始化管道资源
 */
void app_sys_pipe_src_ready(void)
{
    app_sys_slab_ready(&app_sys_pipe_slab, sizeof(app_sys_pipe_pkg_t), 50, 10);
}

/*@brief     初始化管道
 *@param[in] pipe 管道实例
 */
void app_sys_pipe_ready(app_sys_pipe_t *pipe)
{
    pipe->head = NULL;
    pipe->tail = NULL;
    pipe->number = 0;
    app_critical_process(&pipe->critical, app_critical_static);
}

/*@brief     获取管道资源包数量
 *@param[in] pipe 管道实例
 *@retval    管道资源包数量
 */
uint32_t app_sys_pipe_num(app_sys_pipe_t *pipe)
{
    uint32_t number = 0;
    app_critical_process(&pipe->critical, app_critical_enter);
    number = pipe->number;
    app_critical_process(&pipe->critical, app_critical_exit);
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
    package_new = app_sys_slab_alloc(&app_sys_pipe_slab);
    memcpy(package_new, package, sizeof(app_sys_pipe_pkg_t));
    package_new->buddy = NULL;
    app_critical_process(&pipe->critical, app_critical_enter);
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
    app_critical_process(&pipe->critical, app_critical_exit);
}

/*@brief      从管道提取一个包
 *@param[in]  pipe     管道实例
 *@param[out] package  事件资源包(栈资源,非堆资源或静态资源)
 *@param[in]  hit      对指定事件资源包进行命中
 */
void app_sys_pipe_take(app_sys_pipe_t *pipe, app_sys_pipe_pkg_t *package, bool hit)
{
    app_sys_pipe_pkg_t *nonius = NULL;
    app_sys_pipe_pkg_t *package_new = NULL;
    app_critical_process(&pipe->critical, app_critical_enter);
    /* 资源包提取出管道 */
    if (pipe->number != 0) {
        /* 需要命中指定资源包 */
        if (hit) {
            nonius = pipe->head;
            if (nonius->thread == package->thread &&
                nonius->module == package->module &&
                nonius->event  == package->event) {
                package_new = pipe->head;
                pipe->head  = pipe->head->buddy;
                pipe->number--;
            } else {
                for (nonius = pipe->head; nonius->buddy != NULL; nonius = nonius->buddy) {
                    app_sys_pipe_pkg_t *current = nonius->buddy;
                    if (current->thread == package->thread &&
                        current->module == package->module &&
                        current->event  == package->event) {
                        package_new   = current;
                        nonius->buddy = current->buddy;
                        if (pipe->tail == current)
                            pipe->tail  = nonius;
                        pipe->number--;
                        break;
                    }
                }
            }
        } else {
            package_new = pipe->head;
            pipe->head  = pipe->head->buddy;
            pipe->number--;
        }
        if (pipe->number == 0) {
            pipe->head = NULL;
            pipe->tail = NULL;
        }
    }
    app_critical_process(&pipe->critical, app_critical_exit);
    /* 转储消息资源资源, 销毁资源包 */
    if (package_new == NULL)
        return;
    memcpy(package, package_new, sizeof(app_sys_pipe_pkg_t));
    package->buddy = NULL;
    app_sys_slab_free(&app_sys_pipe_slab, package_new);
}
