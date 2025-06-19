/*实现目标:
 *    slab分配器(块式分配器)
 */

#define APP_SYS_LOG_LIMIT_RECORD    1
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 初始化slab分配器
 *@param mem_slab slab分配器实例
 *@param addr     内存地址
 *@param size     内存大小(字节)
 *@param blk_size 块单元大小
 */
void app_sys_mem_slab_ready(app_sys_mem_slab_t *mem_slab, uintptr_t addr, uintptr_t size, uintptr_t blk_size)
{
    APP_SYS_ASSERT(size > sizeof(uintptr_t));

    uint8_t *ptr = NULL;
    /* 计算分配器单元值 */
    mem_slab->addr = (uintptr_t)app_sys_align_high((void *)addr, sizeof(uintptr_t));
    mem_slab->size = size - (mem_slab->addr - addr);
    mem_slab->blk_used = 0;
    mem_slab->blk_size = blk_size;
    mem_slab->blk_num  = mem_slab->size / mem_slab->blk_size;
    mem_slab->blk_list = mem_slab->addr;
    /* 索引回退 */
    mem_slab->blk_list -= mem_slab->blk_size;
    /* 初始化块链表 */
    for (uint32_t idx = 0; idx < mem_slab->blk_num; idx++) {
        /* 当前块移动到下一块 */
        mem_slab->blk_list += mem_slab->blk_size;
        /* 当前块指向前一块索引 */
        *(uint8_t **)mem_slab->blk_list = ptr;
        /* 前一块移动到当前块 */
        ptr = mem_slab->blk_list;
    }
}

/*@brief 向slab分配器获取一个块
 *@param mem_slab slab分配器实例
 *@retval 新的块
 */
void * app_sys_mem_slab_alloc(app_sys_mem_slab_t *mem_slab)
{
    uint8_t *ptr = NULL;
    if (mem_slab->blk_used < mem_slab->blk_num) {
        /* 从分配器获取首块,块索引移动到下一块,计数器加一 */
        ptr = mem_slab->blk_list;
        mem_slab->blk_list = *((uint8_t **)ptr);
        mem_slab->blk_used++;
    }
    return ptr;
}

/*@brief 向slab分配器归还一个块
 *@param ptr 旧的块
 */
void app_sys_mem_slab_free(app_sys_mem_slab_t *mem_slab, void *ptr)
{
    APP_SYS_ASSERT((uintptr_t)ptr >= mem_slab->addr);
    APP_SYS_ASSERT((uintptr_t)ptr <= mem_slab->addr + mem_slab->size);
    if (1) {
        /* 从分配器释放首块,块索引移动到下一块,计数器减一 */
        *((uint8_t **)(ptr)) = mem_slab->blk_list;
        mem_slab->blk_list = ptr;
        mem_slab->blk_used --;
    }
}

/*@brief 初始化slab分配器
 *@param mem_slab_set slab分配器实例
 *@param size         分配单元块大小
 *@param num          分配单元块数量
 *@param debounce     分配单元块数量抖动
 */
void app_sys_mem_slab_set_ready(app_sys_mem_slab_set_t *mem_slab_set, uintptr_t size, uint32_t num, uint32_t debounce)
{
    APP_SYS_ASSERT(size > sizeof(uintptr_t));
    
    app_sys_list_dll_reset(&mem_slab_set->dl_list);
    app_mutex_process(&mem_slab_set->mutex, app_mutex_static);
    
    /* 计算分配器单元值 */
    mem_slab_set->blk_size  = size;
    mem_slab_set->blk_size -= size % sizeof(uintptr_t);
    mem_slab_set->blk_size += size % sizeof(uintptr_t) == 0 ? 0 : sizeof(uintptr_t);
    mem_slab_set->blk_num   = num;
    mem_slab_set->debounce  = debounce;
}

/*@brief 向slab分配器获取一个块
 *@param mem_slab_set slab分配器实例
 *@retval 新的块
 */
void * app_sys_mem_slab_set_alloc(app_sys_mem_slab_set_t *mem_slab_set)
{
    uint8_t *ptr = NULL;
    app_mutex_process(&mem_slab_set->mutex, app_mutex_take);
    /* 先检查分配器是否还有块 */
    app_sys_mem_slab_item_t *mem_slab_item = NULL;
    app_sys_list_dll_ftra(&mem_slab_set->dl_list, node) {
        mem_slab_item = app_sys_own_ofs(app_sys_mem_slab_item_t, dl_node, node);
        if (mem_slab_item->blk_used < mem_slab_item->blk_num)
            break;
        mem_slab_item = NULL;
    }
    /* 没有空闲分配器,生成一个新的分配器 */
    if (mem_slab_item == NULL) {
        mem_slab_item  = app_mem_alloc(sizeof(app_sys_mem_slab_item_t));
        app_sys_list_dln_reset(&mem_slab_item->dl_node);
        /* 加入一个抖动用于消抖 */
        uint32_t debounce = mem_slab_set->debounce == 0 ? 0 : rand() % mem_slab_set->debounce;
        /* 平台字节对齐,配置分配器 */
        mem_slab_item->blk_num  = mem_slab_set->blk_num;
        mem_slab_item->blk_num  = debounce % 2 == 0 ? mem_slab_item->blk_num + debounce : mem_slab_item->blk_num - debounce;
        mem_slab_item->blk_size = mem_slab_set->blk_size;
        mem_slab_item->blk_list = app_mem_alloc(mem_slab_item->blk_size * mem_slab_item->blk_num);
        mem_slab_item->blk_used = 0;
        mem_slab_item->mem_s = mem_slab_item->blk_list;
        mem_slab_item->mem_e = mem_slab_item->blk_list + mem_slab_item->blk_size * mem_slab_item->blk_num;
        /* 索引回退 */
        mem_slab_item->blk_list -= mem_slab_item->blk_size;
        /* 初始化块链表 */
        for (uint32_t idx = 0; idx < mem_slab_item->blk_num; idx++) {
            /* 当前块移动到下一块 */
            mem_slab_item->blk_list += mem_slab_item->blk_size;
            /* 当前块指向前一块索引 */
            *((uint8_t **)(mem_slab_item->blk_list)) = ptr;
            /* 前一块移动到当前块 */
            ptr = mem_slab_item->blk_list;
        }
        /* 分配器加入到分配器链表 */
        app_sys_list_dll_ainsert(&mem_slab_set->dl_list, NULL, &mem_slab_item->dl_node);
    }
    /* 从分配器获取首块,块索引移动到下一块,计数器加一 */
    ptr = mem_slab_item->blk_list;
    mem_slab_item->blk_list = *((uint8_t **)ptr);
    mem_slab_item->blk_used++;
    /*  */
    app_mutex_process(&mem_slab_set->mutex, app_mutex_give);
    return ptr;
}

/*@brief 向slab分配器归还一个块
 *@param mem_slab_set slab分配器实例
 *@param ptr 旧的块
 */
void app_sys_mem_slab_set_free(app_sys_mem_slab_set_t *mem_slab_set, void *ptr)
{
    app_mutex_process(&mem_slab_set->mutex, app_mutex_take);
    /* 检查回收块是否落在此分配器内 */
    app_sys_mem_slab_item_t *mem_slab_item = NULL;
    app_sys_list_dll_ftra(&mem_slab_set->dl_list, node) {
        mem_slab_item = app_sys_own_ofs(app_sys_mem_slab_item_t, dl_node, node);
        if (mem_slab_item->mem_s <= ptr && ptr < mem_slab_item->mem_e)
            break;
        mem_slab_item = NULL;
    }
    APP_SYS_ASSERT(mem_slab_item != NULL);
    if (mem_slab_item != NULL) {
        /* 从分配器释放首块,块索引移动到下一块,计数器减一 */
        *((uint8_t **)(ptr)) = mem_slab_item->blk_list;
        mem_slab_item->blk_list = ptr;
        mem_slab_item->blk_used --;
        /* 检查回收此分配器 */
        if (mem_slab_item->blk_used == 0) {
            app_sys_list_dll_remove(&mem_slab_set->dl_list, &mem_slab_item->dl_node);
            mem_slab_item->blk_list = mem_slab_item->mem_s;
            app_mem_free(mem_slab_item->blk_list);
            app_mem_free(mem_slab_item);
        }
    }
    app_mutex_process(&mem_slab_set->mutex, app_mutex_give);
}
