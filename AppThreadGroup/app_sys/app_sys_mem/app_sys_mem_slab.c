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
    APP_SYS_ASSERT(blk_size > sizeof(uintptr_t));

    uint8_t *ptr = NULL;
    /* 计算分配器单元值 */
    mem_slab->addr = (uintptr_t)app_sys_align_high((void *)addr, sizeof(uintptr_t));
    mem_slab->size = size - (mem_slab->addr - addr);
    mem_slab->blk_used = 0;
    mem_slab->blk_size = blk_size;
    mem_slab->blk_num  = mem_slab->size / mem_slab->blk_size;
    /* 已用块标记集: 自块区尾部切出, 不参与分配(块数量相应削减) */
    uint32_t mark_size = (mem_slab->blk_num + 7) / 8;
    uint32_t mark_blk  = (mark_size + mem_slab->blk_size - 1) / mem_slab->blk_size;
    APP_SYS_ASSERT(mem_slab->blk_num > mark_blk);
    mem_slab->blk_num -= mark_blk;
    mem_slab->blk_mark = (uint8_t *)mem_slab->addr + mem_slab->blk_num * mem_slab->blk_size;
    for (uint32_t idx = 0; idx < mark_size; idx++)
        mem_slab->blk_mark[idx] = 0;
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
        uint32_t idx = ((uintptr_t)ptr - mem_slab->addr) / mem_slab->blk_size;
        /* 空闲块不得被标记为已用(否则空闲链表已被污染) */
        APP_SYS_ASSERT(!(mem_slab->blk_mark[idx / 8] & (1u << (idx % 8))));
        mem_slab->blk_mark[idx / 8] |= 1u << (idx % 8);
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
    APP_SYS_ASSERT((uintptr_t)ptr <  mem_slab->addr + (uintptr_t)mem_slab->blk_num * mem_slab->blk_size);
    uint32_t idx = ((uintptr_t)ptr - mem_slab->addr) / mem_slab->blk_size;
    /* 块单元必须块对齐 */
    APP_SYS_ASSERT(((uintptr_t)ptr - mem_slab->addr) % mem_slab->blk_size == 0);
    if (1) {
        /* 归还的块必须处于已用状态(重复回收与野指针在此拦下) */
        APP_SYS_ASSERT(mem_slab->blk_mark[idx / 8] & (1u << (idx % 8)));
        mem_slab->blk_mark[idx / 8] &= ~(1u << (idx % 8));
        /* 从分配器释放首块,块索引移动到下一块,计数器减一 */
        *((uint8_t **)(ptr)) = mem_slab->blk_list;
        mem_slab->blk_list = ptr;
        mem_slab->blk_used --;
    }
}

/*@brief slab分配器消耗值
 *@param mem_slab slab分配器实例
 *@retval 消耗值(字节)
 */
uintptr_t app_sys_mem_slab_used(app_sys_mem_slab_t *mem_slab)
{
    return (uintptr_t)mem_slab->blk_used * mem_slab->blk_size;
}

/*@brief slab分配器最大片段
 *@param mem_slab slab分配器实例
 *@retval 最大片段(字节)
 */
uintptr_t app_sys_mem_slab_frag(app_sys_mem_slab_t *mem_slab)
{
    /* 定长热结构无碎片, 存在空闲块即可满足一次分配 */
    if (mem_slab->blk_used < mem_slab->blk_num)
        return mem_slab->blk_size;
    return 0;
}

/*@brief slab分配器块单元尺寸
 *@param mem_slab slab分配器实例
 *@param pointer 块单元
 *@retval 块单元尺寸(字节)
 */
uintptr_t app_sys_mem_slab_size(app_sys_mem_slab_t *mem_slab, void *pointer)
{
    return mem_slab->blk_size;
}

/*@brief slab分配器归属检查
 *@param mem_slab slab分配器实例
 *@param pointer 块单元
 *@retval 是否归属
 */
bool app_sys_mem_slab_inside(app_sys_mem_slab_t *mem_slab, void *pointer)
{
    if ((uintptr_t)pointer <  mem_slab->addr)
        return false;
    if ((uintptr_t)pointer >= mem_slab->addr + (uintptr_t)mem_slab->blk_num * mem_slab->blk_size)
        return false;
    return true;
}

/*@brief slab分配器有效性检查(轻量, 供回收后断言)
 *@param mem_slab slab分配器实例
 *@retval 是否有效
 */
bool app_sys_mem_slab_valid(app_sys_mem_slab_t *mem_slab)
{
    if (mem_slab->blk_used > mem_slab->blk_num)
        return false;
    /* 空闲链表首块必须块对齐且落在块区 */
    if (mem_slab->blk_list != NULL &&
        !app_sys_mem_slab_inside(mem_slab, mem_slab->blk_list))
        return false;
    if (mem_slab->blk_list != NULL &&
        ((uintptr_t)mem_slab->blk_list - mem_slab->addr) % mem_slab->blk_size != 0)
        return false;
    return true;
}

/*@brief slab分配器完整性检查(已用标记与空闲链表互补)
 *@param mem_slab slab分配器实例
 *@retval 是否完整
 */
bool app_sys_mem_slab_check(app_sys_mem_slab_t *mem_slab)
{
    uint32_t used_num = 0;
    uint32_t free_num = 0;
    
    /* 已用块标记统计 */
    for (uint32_t idx = 0; idx < mem_slab->blk_num; idx++)
        if (mem_slab->blk_mark[idx / 8] & (1u << (idx % 8)))
            used_num++;
    /* 空闲块链表统计(同时校验块对齐, 归属与标记状态) */
    for (uint8_t *ptr = mem_slab->blk_list; ptr != NULL; ptr = *((uint8_t **)ptr)) {
        uint32_t idx = ((uintptr_t)ptr - mem_slab->addr) / mem_slab->blk_size;
        APP_SYS_ASSERT(((uintptr_t)ptr - mem_slab->addr) % mem_slab->blk_size == 0);
        APP_SYS_ASSERT(idx < mem_slab->blk_num);
        APP_SYS_ASSERT(!(mem_slab->blk_mark[idx / 8] & (1u << (idx % 8))));
        /* 空闲链表成环时数量必然溢出, 就地拦下 */
        APP_SYS_ASSERT(++free_num <= mem_slab->blk_num);
    }
    /* 三个数值必须自洽: 已用 + 空闲 == 总量, 且计数器一致 */
    APP_SYS_ASSERT(used_num == mem_slab->blk_used);
    APP_SYS_ASSERT(used_num + free_num == mem_slab->blk_num);
    return true;
}

/*@brief slab分配器块遍历
 *@param mem_slab slab分配器实例
 *@param invoke   块遍历回调
 *@retval 遍历是否完整
 */
bool app_sys_mem_slab_walk(app_sys_mem_slab_t *mem_slab, void (*invoke)(void *pointer, bool used))
{
    APP_SYS_ASSERT(invoke != NULL);
    
    for (uint32_t idx = 0; idx < mem_slab->blk_num; idx++) {
        void *ptr = (void *)(mem_slab->addr + (uintptr_t)idx * mem_slab->blk_size);
        bool   used = mem_slab->blk_mark[idx / 8] & (1u << (idx % 8)) ? true : false;
        invoke(ptr, used);
    }
    return true;
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
