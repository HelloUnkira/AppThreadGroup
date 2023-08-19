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
#include "app_sys_pipe.h"

static app_mutex_t app_sys_pipe_slab_mutex = {0};
static app_sys_pipe_slab_t *app_sys_pipe_slab_list = NULL;

/*@brief  向slab分配器获取一个块
 *@retval 新的块
 */
static void * app_sys_pipe_slab_alloc(void)
{
    uint8_t *ptr = NULL;
    app_mutex_process(&app_sys_pipe_slab_mutex, app_mutex_take);
    app_sys_pipe_slab_t *slab = app_sys_pipe_slab_list;
    /* 先检查分配器是否还有块 */
    for (slab = slab; slab != NULL; slab = slab->next)
        if (slab->blk_used < slab->blk_num)
            break;
    /* 没有空闲分配器,生成一个新的分配器 */
    if (slab == NULL) {
        slab  = app_mem_alloc(sizeof(app_sys_pipe_slab_t));
        /* 加入一个抖动用于消抖 */
        uint32_t debounce = rand() % APP_SYS_PIPE_SLAB_DEBOUNCE;
        /* 平台字节对齐,配置分配器 */
        slab->blk_num   = APP_SYS_PIPE_SLAB_UNIT;
        if (debounce % 2 == 0)
            slab->blk_num += debounce;
        if (debounce % 2 != 0)
            slab->blk_num -= debounce;
        slab->blk_size  = sizeof(app_sys_pipe_pkg_t);
        slab->blk_size -= sizeof(app_sys_pipe_pkg_t) % sizeof(uintptr_t);
        slab->blk_size += sizeof(uintptr_t);
        slab->blk_list  = app_mem_alloc(slab->blk_size * slab->blk_num);
        slab->blk_used  = 0;
        slab->mem_s = slab->blk_list;
        slab->mem_e = slab->blk_list + slab->blk_size * slab->blk_num;
        /* 索引回退 */
        slab->blk_list -= slab->blk_size;
        /* 初始化块链表 */
        for (uint32_t idx = 0; idx < slab->blk_num; idx++) {
            /* 当前块移动到下一块 */
            slab->blk_list += slab->blk_size;
            /* 当前块指向前一块索引 */
            *((uint8_t **)(slab->blk_list)) = ptr;
            /* 前一块移动到当前块 */
            ptr = slab->blk_list;
        }
        /* 分配器加入到分配器链表 */
        slab->prev = NULL;
        slab->next = app_sys_pipe_slab_list;
        if (app_sys_pipe_slab_list != NULL)
            app_sys_pipe_slab_list->prev = slab;
            app_sys_pipe_slab_list = slab;
    }
    /* 从分配器获取首块,块索引移动到下一块,计数器加一 */
    ptr = slab->blk_list;
    slab->blk_list = *((uint8_t **)ptr);
    slab->blk_used++;
    /*  */
    app_mutex_process(&app_sys_pipe_slab_mutex, app_mutex_give);
    return ptr;
}

/*@brief     向slab分配器归还一个块
 *@param[in] 旧的块
 */
static void app_sys_pipe_slab_free(void *ptr)
{
    app_mutex_process(&app_sys_pipe_slab_mutex, app_mutex_take);
    app_sys_pipe_slab_t *slab = app_sys_pipe_slab_list;
    /* 检查回收块是否落在此分配器内 */
    for (slab = slab; slab != NULL; slab = slab->next)
        if (slab->mem_s <= ptr && slab->mem_e > ptr) {
            /* 从分配器释放首块,块索引移动到下一块,计数器减一 */
            *((uint8_t **)(ptr)) = slab->blk_list;
            slab->blk_list = ptr;
            slab->blk_used --;
            break;
        }
    /* 回收此分配器 */
    if (slab->blk_used == 0) {
        if (slab->prev != NULL)
          ((app_sys_pipe_slab_t *)(slab->prev))->next = slab->next;
        if (slab->next != NULL)
          ((app_sys_pipe_slab_t *)(slab->next))->prev = slab->prev;
        if (app_sys_pipe_slab_list == slab)
            app_sys_pipe_slab_list  = slab->next;
        slab->blk_list = slab->mem_s;
        app_mem_free(slab->blk_list);
        app_mem_free(slab);
    }
    app_mutex_process(&app_sys_pipe_slab_mutex, app_mutex_give);
}

/*@brief 初始化管道资源
 */
void app_sys_pipe_src_ready(void)
{
    app_mutex_process(&app_sys_pipe_slab_mutex, app_mutex_static);
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
    package_new = app_sys_pipe_slab_alloc();
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
    app_sys_pipe_slab_free(package_new);
}
