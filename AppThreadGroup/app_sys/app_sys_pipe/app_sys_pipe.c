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
#include "app_sys_lib.h"

/*@brief 优先级排序入队列比较函数
 */
static bool app_sys_pipe_sort(app_sys_list_dln_t *node1, app_sys_list_dln_t *node2)
{
    app_sys_pipe_pkg_t *pkg1 = app_sys_own_ofs(app_sys_pipe_pkg_t, dl_node, node1);
    app_sys_pipe_pkg_t *pkg2 = app_sys_own_ofs(app_sys_pipe_pkg_t, dl_node, node2);
    return pkg1->priority > pkg2->priority;
}

/*@brief 事件包匹配函数
 */
static bool app_sys_pipe_confirm(app_sys_list_dln_t *node1, app_sys_list_dln_t *node2)
{
    app_sys_pipe_pkg_t *pkg1 = app_sys_own_ofs(app_sys_pipe_pkg_t, dl_node, node1);
    app_sys_pipe_pkg_t *pkg2 = app_sys_own_ofs(app_sys_pipe_pkg_t, dl_node, node2);
    return pkg1->thread == pkg2->thread && pkg1->module == pkg2->module && pkg1->event == pkg2->event;
}

/*@brief 初始化管道
 *@param pipe 管道实例
 *@param addr 内存地址(内存自备)
 *@param size 内存大小(字节)(num * sizeof(app_sys_pipe_pkg_t))
 */
void app_sys_pipe_ready(app_sys_pipe_t *pipe, uintptr_t addr, uintptr_t size)
{
    app_sys_list_dll_reset(&pipe->dl_list);
    app_sys_mem_slab_ready(&pipe->mem_slab, addr, size, sizeof(app_sys_pipe_pkg_t));
    app_critical_process(&pipe->critical, app_critical_static);
    pipe->number = 0;
}

/*@brief 获取管道资源包数量
 *@param pipe 管道实例
 *@retval 管道资源包数量
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

/*@brief 交付一个包给管道
 *@param pipe     管道实例
 *@param package  事件资源包(栈资源,非堆资源或静态资源)
 *@retval  0:     正常接收
 *        +1:     事件被吸收
 *        -1:     内存块不足,失败
 */
int8_t app_sys_pipe_give(app_sys_pipe_t *pipe, app_sys_pipe_pkg_t *package)
{
    int8_t retval = 0;
    bool absorb_flag = false;
    app_sys_pipe_pkg_t *package_old = NULL;
    app_sys_pipe_pkg_t *package_new = NULL;
    /* 生成资源包, 转储消息资源资源 */
    app_critical_process(&pipe->critical, app_critical_enter);
    /* 事件包合并检查,如果合并回调不为空且查找到旧事件,合并它 */
    if (package->absorb != NULL) {
        app_sys_list_dll_ftra(&pipe->dl_list, node)
        if (app_sys_pipe_confirm(&package->dl_node, node)) {
            package_old = app_sys_own_ofs(app_sys_pipe_pkg_t, dl_node, node);
            absorb_flag = package->absorb(package_old, package);
            if (absorb_flag) {
                retval = +1;
                break;
            }
        }
    }
    if (!absorb_flag) {
        package_new = app_sys_mem_slab_alloc(&pipe->mem_slab);
        if (package_new == NULL)
            retval = -1;
        if (package_new != NULL) {
            memcpy(package_new, package, sizeof(app_sys_pipe_pkg_t));
            app_sys_list_dln_reset(&package_new->dl_node);
            /* 优先级调整到中间,为常规优先级 */
            if (package_new->priority == 0)
                package_new->priority  = 0xFF / 2;
            /* 资源包加入到管道(优先队列) */
            if (package_new->priority == 0)
                app_sys_list_dll_ainsert(&pipe->dl_list, NULL, &package_new->dl_node);
            else
                app_sys_queue_dlpq_enqueue(&pipe->dl_list, &package_new->dl_node, app_sys_pipe_sort);
            pipe->number++;
        }
    }
    app_critical_process(&pipe->critical, app_critical_exit);
    return retval;
}

/*@brief 从管道提取一个包
 *@param pipe     管道实例
 *@param package  事件资源包(栈资源,非堆资源或静态资源)
 *@param hit      对指定事件资源包进行命中
 */
void app_sys_pipe_take(app_sys_pipe_t *pipe, app_sys_pipe_pkg_t *package, bool hit)
{
    app_sys_pipe_pkg_t *package_new = NULL;
    app_critical_process(&pipe->critical, app_critical_enter);
    /* 资源包提取出管道 */
    if (pipe->number != 0) {
        /* 需要命中指定资源包 */
        if (hit) {
            app_sys_list_dll_btra(&pipe->dl_list, node)
            if (app_sys_pipe_confirm(&package->dl_node, node)) {
                package_new = app_sys_own_ofs(app_sys_pipe_pkg_t, dl_node, node);
                break;
            }
        } else {
            app_sys_list_dln_t *node = app_sys_list_dll_head(&pipe->dl_list);
            package_new = app_sys_own_ofs(app_sys_pipe_pkg_t, dl_node, node);
        }
        if (package_new != NULL) {
            app_sys_list_dll_remove(&pipe->dl_list, &package_new->dl_node);
            pipe->number--;
        }
    }
    /* 转储消息资源资源, 销毁资源包 */
    if (package_new != NULL) {
        memcpy(package, package_new, sizeof(app_sys_pipe_pkg_t));
        app_sys_mem_slab_free(&pipe->mem_slab, package_new);
    }
    app_critical_process(&pipe->critical, app_critical_exit);
}

/*@brief 迭代整个管道,访问所有事件包
 *@param pipe  管道实例
 *@param visit 事件包访问回调
 */
void app_sys_pipe_walk(app_sys_pipe_t *pipe, void (*visit)(app_sys_pipe_pkg_t *package))
{
    app_sys_pipe_pkg_t *package = NULL;
    app_critical_process(&pipe->critical, app_critical_enter);
    if (visit != NULL) {
        app_sys_list_dll_btra(&pipe->dl_list, node) {
            package = app_sys_own_ofs(app_sys_pipe_pkg_t, dl_node, node);
            visit(package);
        }
    }
    app_critical_process(&pipe->critical, app_critical_exit);
}
