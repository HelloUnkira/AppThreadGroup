/*实现目标:
 *    slab分配器(块式分配器)
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief     初始化slab分配器
 *@param[in] mem_slab slab分配器实例
 *@param[in] size     分配单元块大小
 *@param[in] num      分配单元块数量
 *@param[in] debounce 分配单元块数量抖动
 */
void app_sys_mem_slab_ready(app_sys_mem_slab_t *mem_slab, uintptr_t size, uint32_t num, uint32_t debounce)
{
    APP_SYS_ASSERT(size > sizeof(uintptr_t));
    
    app_sys_list_dll_reset(&mem_slab->dl_list);
    app_mutex_process(&mem_slab->mutex, app_mutex_static);
    
    /* 计算分配器单元值 */
    mem_slab->blk_size  = size;
    mem_slab->blk_size -= size % sizeof(uintptr_t);
    mem_slab->blk_size += size % sizeof(uintptr_t) == 0 ? 0 : sizeof(uintptr_t);
    mem_slab->blk_num   = num;
    mem_slab->debounce  = debounce;
}

/*@brief     向slab分配器获取一个块
 *@param[in] mem_slab slab分配器实例
 *@retval    新的块
 */
void * app_sys_mem_slab_alloc(app_sys_mem_slab_t *mem_slab)
{
    uint8_t *ptr = NULL;
    app_mutex_process(&mem_slab->mutex, app_mutex_take);
    /* 先检查分配器是否还有块 */
    app_sys_mem_slab_item_t *item = NULL;
    app_sys_list_dll_ftra(&mem_slab->dl_list, node) {
        item = app_sys_own_ofs(app_sys_mem_slab_item_t, dl_node, node);
        if (item->blk_used < item->blk_num)
            break;
        item = NULL;
    }
    /* 没有空闲分配器,生成一个新的分配器 */
    if (item == NULL) {
        item  = app_mem_alloc(sizeof(app_sys_mem_slab_item_t));
        app_sys_list_dln_reset(&item->dl_node);
        /* 加入一个抖动用于消抖 */
        uint32_t debounce = mem_slab->debounce == 0 ? 0 : rand() % mem_slab->debounce;
        /* 平台字节对齐,配置分配器 */
        item->blk_num  = mem_slab->blk_num;
        item->blk_num  = debounce % 2 == 0 ? item->blk_num + debounce : item->blk_num - debounce;
        item->blk_size = mem_slab->blk_size;
        item->blk_list = app_mem_alloc(item->blk_size * item->blk_num);
        item->blk_used = 0;
        item->mem_s = item->blk_list;
        item->mem_e = item->blk_list + item->blk_size * item->blk_num;
        /* 索引回退 */
        item->blk_list -= item->blk_size;
        /* 初始化块链表 */
        for (uint32_t idx = 0; idx < item->blk_num; idx++) {
            /* 当前块移动到下一块 */
            item->blk_list += item->blk_size;
            /* 当前块指向前一块索引 */
            *((uint8_t **)(item->blk_list)) = ptr;
            /* 前一块移动到当前块 */
            ptr = item->blk_list;
        }
        /* 分配器加入到分配器链表 */
        app_sys_list_dll_ainsert(&mem_slab->dl_list, NULL, &item->dl_node);
    }
    /* 从分配器获取首块,块索引移动到下一块,计数器加一 */
    ptr = item->blk_list;
    item->blk_list = *((uint8_t **)ptr);
    item->blk_used++;
    /*  */
    app_mutex_process(&mem_slab->mutex, app_mutex_give);
    return ptr;
}

/*@brief     向slab分配器归还一个块
 *@param[in] ptr 旧的块
 */
void app_sys_mem_slab_free(app_sys_mem_slab_t *mem_slab, void *ptr)
{
    app_mutex_process(&mem_slab->mutex, app_mutex_take);
    /* 检查回收块是否落在此分配器内 */
    app_sys_mem_slab_item_t *item = NULL;
    app_sys_list_dll_ftra(&mem_slab->dl_list, node) {
        item = app_sys_own_ofs(app_sys_mem_slab_item_t, dl_node, node);
        if (item->mem_s <= ptr && ptr < item->mem_e)
            break;
        item = NULL;
    }
    APP_SYS_ASSERT(item != NULL);
    if (item != NULL) {
        /* 从分配器释放首块,块索引移动到下一块,计数器减一 */
        *((uint8_t **)(ptr)) = item->blk_list;
        item->blk_list = ptr;
        item->blk_used --;
        /* 检查回收此分配器 */
        if (item->blk_used == 0) {
            app_sys_list_dll_remove(&mem_slab->dl_list, &item->dl_node);
            item->blk_list = item->mem_s;
            app_mem_free(item->blk_list);
            app_mem_free(item);
        }
    }
    app_mutex_process(&mem_slab->mutex, app_mutex_give);
}
