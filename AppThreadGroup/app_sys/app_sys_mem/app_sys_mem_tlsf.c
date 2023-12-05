/*实现目标:
 *    heap分配器(二级隔离策略分配堆)
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/* 块大小描述 */
static const uintptr_t app_sys_mem_tlsf_blk_ofs = (uintptr_t)((uintptr_t)&((app_sys_mem_tlsf_block_t *)0)->size_free + sizeof(uintptr_t));
static const uintptr_t app_sys_mem_tlsf_blk_max = (uintptr_t)((uintptr_t)1 << app_sys_mem_tlsf_fl_index);
static const uintptr_t app_sys_mem_tlsf_blk_min = (uintptr_t)(sizeof(app_sys_mem_tlsf_block_t) - sizeof(uintptr_t));
static const uintptr_t app_sys_mem_tlsf_blk_hrd = (uintptr_t)(sizeof(uintptr_t));

/*@breif     类似内置功能(__bulitin_ffs)
 *           __bulitin_ffs:从低位起,第一个1的索引+1,为0则返回0
 *           此处的语义有变化,因为明确参数不为0,这里索引不+1
 *@param val 解算值
 *@retval 解算值
 */
static inline uintptr_t app_sys_mem_tlsf_ffs(uintptr_t val)
{
    APP_SYS_ASSERT(val != 0);
    uintptr_t bits = sizeof(uintptr_t) * 8;
    for (uintptr_t idx = 0; idx < bits; idx++)
        if ((val & ((uintptr_t)1 << idx)) != 0)
            return idx;
    APP_SYS_ASSERT(false);
}

/*@breif     类似内置功能(__bulitin_fls)
 *           __bulitin_fls:从高位起,第一个1的位置+1,为0则返回0
 *           此处的语义有变化,因为明确参数不为0,这里索引不+1
 *@param val 解算值
 *@retval 解算值
 */
static inline uintptr_t app_sys_mem_tlsf_fls(uintptr_t val)
{
    APP_SYS_ASSERT(val != 0);
    uintptr_t bits = sizeof(uintptr_t) * 8;
    uintptr_t bits_1 = bits - (uintptr_t)1;
    for (uintptr_t idx = 0; idx < bits; idx++)
        if ((val & ((uintptr_t)1 << (bits_1 - idx))) != 0)
            return bits_1 - idx;
    APP_SYS_ASSERT(false);
}

/*@brief 设置block字段
 *@param block 块实例
 *@param value block字段
 */
static inline void app_sys_mem_tlsf_block_size_set(app_sys_mem_tlsf_block_t *block, uintptr_t size)
{
    uintptr_t mask_free = (uintptr_t)(0b11);
    uintptr_t mask_size = ~mask_free;
    /* 块占用实际大小设置 */
    block->size_free = (block->size_free & mask_free) | (mask_size & size);
}

/*@brief 设置block字段
 *@param block 块实例
 *@param value block字段
 */
static inline void app_sys_mem_tlsf_block_free_curr_set(app_sys_mem_tlsf_block_t *block, uintptr_t free)
{
    uintptr_t mask_free = (uintptr_t)(0b01);
    /* 设置当前块的空闲状态 */
    block->size_free = free ? (block->size_free | mask_free) : (block->size_free & ~mask_free);
}

/*@brief 设置block字段
 *@param block 块实例
 *@param value block字段
 */
static inline void app_sys_mem_tlsf_block_free_prev_set(app_sys_mem_tlsf_block_t *block, uintptr_t free)
{
    uintptr_t mask_free = (uintptr_t)(0b10);
    /* 设置当前块的空闲状态 */
    block->size_free = free ? (block->size_free | mask_free) : (block->size_free & ~mask_free);
}

/*@brief 设置block字段
 *@param block 二级隔离策略分配堆块实例
 *@retval block字段
 */
static inline uintptr_t app_sys_mem_tlsf_block_size_get(app_sys_mem_tlsf_block_t *block)
{
    uintptr_t mask_free = (uintptr_t)(0b11);
    uintptr_t mask_size = ~mask_free;
    return (block->size_free & mask_size);
}

/*@brief 设置block字段
 *@param block 二级隔离策略分配堆块实例
 *@retval block字段
 */
static inline uintptr_t app_sys_mem_tlsf_block_free_curr_get(app_sys_mem_tlsf_block_t *block)
{
    uintptr_t mask_free = (uintptr_t)(0b01);
    return (block->size_free & mask_free) != 0;
}

/*@brief 设置block字段
 *@param block 二级隔离策略分配堆块实例
 *@retval block字段
 */
static inline uintptr_t app_sys_mem_tlsf_block_free_prev_get(app_sys_mem_tlsf_block_t *block)
{
    uintptr_t mask_free = (uintptr_t)(0b10);
    return (block->size_free & mask_free) != 0;
}

/*@brief 地址转换block到addr
 *@param block 块实例
 *@retval 内存地址
 */
static inline void * app_sys_mem_tlsf_block_to_ptr(app_sys_mem_tlsf_block_t *block)
{
    return (void *)((uintptr_t)block + app_sys_mem_tlsf_blk_ofs);
}

/*@brief 地址转换addr到block
 *@param ptr 内存地址
 *@retval 块实例
 */
static inline app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_ptr_to_block(void *ptr)
{
    return (app_sys_mem_tlsf_block_t *)((uintptr_t)ptr - app_sys_mem_tlsf_blk_ofs);
}

/*@brief 地址转换addr到block(带区域大小)
 *@param ptr  内存地址
 *@param size 带区域大小
 *@retval 块实例
 */
static inline app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_ptr_to_block_with_size(void *ptr, uintptr_t size)
{
    return (app_sys_mem_tlsf_block_t *)((uintptr_t)ptr + size);
}

/*@brief 获得前一块
 *@param block 块实例
 *@retval 块实例
 */
static inline app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_block_prev_get(app_sys_mem_tlsf_block_t *block)
{
    APP_SYS_ASSERT(app_sys_mem_tlsf_block_free_prev_get(block) != 0);
    return block->prev_tail;
}

/*@brief 获得后一块
 *@param block 块实例
 *@retval 块实例
 */
static inline app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_block_next_get(app_sys_mem_tlsf_block_t *block)
{
    uintptr_t size = app_sys_mem_tlsf_block_size_get(block);
    APP_SYS_ASSERT(size != 0);
    /* 地址偏移计算1: */
    return (app_sys_mem_tlsf_block_t *)((uintptr_t)&block->size_free + size);
    /* 地址偏移计算2: */
    void *ptr = app_sys_mem_tlsf_block_to_ptr(block);
    return app_sys_mem_tlsf_ptr_to_block_with_size(ptr, size - app_sys_mem_tlsf_blk_hrd);
}

/*@brief 块关联下一个块
 *@param block 块实例
 *@retval 块实例
 */
static inline app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_block_link_next(app_sys_mem_tlsf_block_t *block)
{
    app_sys_mem_tlsf_block_t *next = app_sys_mem_tlsf_block_next_get(block);
    next->prev_tail = block;
    return next;
}

/*@brief 标记块空闲或者使用状态
 *@param block 块实例
 *@param value block字段
 *@retval 块实例
 */
static inline void app_sys_mem_tlsf_block_mark(app_sys_mem_tlsf_block_t *block, uintptr_t free)
{
    app_sys_mem_tlsf_block_t *block_next = free ? app_sys_mem_tlsf_block_link_next(block) :
                                                  app_sys_mem_tlsf_block_next_get(block);
    
    app_sys_mem_tlsf_block_free_prev_set(block_next, free);
    app_sys_mem_tlsf_block_free_curr_set(block, free);
}

/*@brief 调整分配大小使之与字大小一致,不能更小于内部最小值
 *@param size  字节
 *@param align 对齐
 *@retval 字对齐
 */
static inline uintptr_t app_sys_mem_tlsf_block_size_align(uintptr_t size, uintptr_t align)
{
    uintptr_t size_align = (uintptr_t)app_sys_align_high((void *)size, align);
    size_align = app_sys_max(size_align, app_sys_mem_tlsf_blk_min);
    if (size_align >= app_sys_mem_tlsf_blk_max)
        size_align = 0;
    return size_align;
}

/*@brief 计算大小的映射值
 *@param size 字节
 *@param fl_i 一级索引
 *@param sl_i 二级索引
 */
static void app_sys_mem_tlsf_map(uintptr_t size, uintptr_t *fl_i, uintptr_t *sl_i)
{
    uintptr_t fl = 0;
    uintptr_t sl = 0;
    /* 小于最小块时收集到第0块去 */
    if (size < app_sys_mem_tlsf_fl_limit) {
        fl = 0;
        sl = size / (app_sys_mem_tlsf_fl_limit / app_sys_mem_tlsf_sl_index);
    } else {
        fl  = app_sys_mem_tlsf_fls(size);
        sl  = (size >> (fl - app_sys_mem_tlsf_sl_index)) ^ ((uintptr_t)1 << app_sys_mem_tlsf_sl_index);
        fl -= app_sys_mem_tlsf_fl_shift - 1;
    }
    *fl_i = fl;
    *sl_i = sl;
    /* 检查一下插入时映射: */
    APP_SYS_LOG_DEBUG("size:%d, fl:%d, sl:%d", size, fl, sl);
}

/*@brief 计算大小的映射值
 *@param size 字节
 *@param fl_i 一级索引
 *@param sl_i 二级索引
 */
static void app_sys_mem_tlsf_map_find(uintptr_t size, uintptr_t *fl_i, uintptr_t *sl_i)
{
    if (size >= app_sys_mem_tlsf_fl_limit)
        size += ((uintptr_t)1 << (app_sys_mem_tlsf_fls(size) - app_sys_mem_tlsf_sl_index)) - 1;
    app_sys_mem_tlsf_map(size, fl_i, sl_i);
}

/*@brief 查找到合适块
 *@param control 控制块
 *@param fl_i    一级索引
 *@param sl_i    二级索引
 *@retval 块地址
 */
static app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_block_find(app_sys_mem_tlsf_control_t *control, uintptr_t *fl_i, uintptr_t *sl_i)
{
    uintptr_t fl = *fl_i, fl_map = 0;
    uintptr_t sl = *sl_i, sl_map = 0;
    /* 查找二级索引图,注意:比目标索引大的索引都需要找到,以便迭代 */
    sl_map = control->bitmap_sl[fl] & ((~(uintptr_t)0) << (sl + 0));
    /* 当前一级索引下,查找不到比目标大的二级索引图,查找一级索引图 */
    if (sl_map == 0) {
        fl_map  = control->bitmap_fl & ((~(uintptr_t)0) << (fl + 1));
        /* 一级索引图也找不到有效块,内存被耗尽了 */
        if (fl_map == 0)
            return NULL;
        /* 在一级索引图找到最邻近的下一级索引 */
        fl = app_sys_mem_tlsf_ffs(fl_map);
        sl_map = control->bitmap_sl[fl];
    }
    /* 在二级索引图找到最邻近的下一级索引 */
    sl = app_sys_mem_tlsf_ffs(sl_map);
    /* 索引可能会调整,更新它 */
    *fl_i = fl;
    *sl_i = sl;
    /* 返回空闲链表 */
    return control->block[fl][sl];
}

/*@brief 移除块
 *@param control 控制块
 *@param block   块
 *@param fl      一级索引
 *@param sl      二级索引
 */
static void app_sys_mem_tlsf_block_del_free(app_sys_mem_tlsf_control_t *control, app_sys_mem_tlsf_block_t *block, uintptr_t fl, uintptr_t sl)
{
    /* 即使是链表头和链表尾都会指向一个空节点 */
    /* 所以前向索引和后向索引都不能为空 */
    APP_SYS_ASSERT(block->prev != NULL);
    APP_SYS_ASSERT(block->next != NULL);
    block->next->prev = block->prev;
    block->prev->next = block->next;
    /* 更新连接索引以及位索引图 */
    if (control->block[fl][sl] == block) {
        control->block[fl][sl]  = block->next;
        /* 索引被更新到了空节点,更新索引图 */
        if (control->block[fl][sl] == &control->block_none)
            control->bitmap_sl[fl] &= ~((uintptr_t)1 << sl);
        if (control->bitmap_sl[fl] == 0)
            control->bitmap_fl &= ~((uintptr_t)1 << fl);
    }
}

/*@brief 添加块
 *@param control 控制块
 *@param block   块
 *@param fl      一级索引
 *@param sl      二级索引
 */
static void app_sys_mem_tlsf_block_add_free(app_sys_mem_tlsf_control_t *control, app_sys_mem_tlsf_block_t *block, uintptr_t fl, uintptr_t sl)
{
    app_sys_mem_tlsf_block_t *current = control->block[fl][sl];
    APP_SYS_ASSERT(block != NULL);
    APP_SYS_ASSERT(current != NULL);
    /* 更新目标索引 */
    current->prev = block;
    block->next =  current;
    block->prev = &control->block_none;
    /* 简要核查一下,块地址要平台字节对齐 */
    APP_SYS_ASSERT(app_sys_mem_tlsf_block_to_ptr(block) == app_sys_align_low(app_sys_mem_tlsf_block_to_ptr(block), app_sys_mem_tlsf_byte));
    /* 更新连接索引以及位索引图 */
    control->block[fl][sl] = block;
    control->bitmap_sl[fl] |= (uintptr_t)1 << sl;
    control->bitmap_fl |= (uintptr_t)1 << fl;
}

/*@brief 移除块
 *@param control 控制块
 *@param block   块
 */
static void app_sys_mem_tlsf_block_del(app_sys_mem_tlsf_control_t *control, app_sys_mem_tlsf_block_t *block)
{
    uintptr_t fl = 0, sl = 0;
    app_sys_mem_tlsf_map(app_sys_mem_tlsf_block_size_get(block), &fl, &sl);
    app_sys_mem_tlsf_block_del_free(control, block, fl, sl);
}

/*@brief 添加块
 *@param control 控制块
 *@param block   块
 */
static void app_sys_mem_tlsf_block_add(app_sys_mem_tlsf_control_t *control, app_sys_mem_tlsf_block_t *block)
{
    uintptr_t fl = 0, sl = 0;
    app_sys_mem_tlsf_map(app_sys_mem_tlsf_block_size_get(block), &fl, &sl);
    app_sys_mem_tlsf_block_add_free(control, block, fl, sl);
}

/*@brief 检查块是否可分裂
 *@param block 块
 *@param size  尺寸
 */
static bool app_sys_mem_tlsf_block_split_check(app_sys_mem_tlsf_block_t *block, uintptr_t size)
{
    return app_sys_mem_tlsf_block_size_get(block) > sizeof(app_sys_mem_tlsf_block_t) + size;
}

/*@brief 块分裂
 *@param block 块
 *@param size  尺寸
 *@retval 新的块
 */
static app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_block_split(app_sys_mem_tlsf_block_t *block, uintptr_t size)
{
    uintptr_t size_remain = 0;
    app_sys_mem_tlsf_block_t *block_remain = NULL;
    /*  */
    void *ptr = app_sys_mem_tlsf_block_to_ptr(block);
    /* 计算剩余块的总量并获得断点 */
    size_remain = app_sys_mem_tlsf_block_size_get(block) - (size + app_sys_mem_tlsf_blk_hrd);
    block_remain = app_sys_mem_tlsf_ptr_to_block_with_size(ptr, size - app_sys_mem_tlsf_blk_hrd);
    /* 简要核查一下,块地址要平台字节对齐 */
    APP_SYS_ASSERT(app_sys_align_check(app_sys_mem_tlsf_block_to_ptr(block_remain), sizeof(uintptr_t)));
    /* 检查计算时是否发生截断 */
    APP_SYS_ASSERT(app_sys_mem_tlsf_block_size_get(block) == size_remain + size + app_sys_mem_tlsf_blk_hrd);
    app_sys_mem_tlsf_block_size_set(block_remain, size_remain);
    APP_SYS_ASSERT(app_sys_mem_tlsf_block_size_get(block_remain) >= app_sys_mem_tlsf_blk_min);
    /* 更新原有块大小 */
    app_sys_mem_tlsf_block_size_set(block, size);
    app_sys_mem_tlsf_block_mark(block_remain, true);
    return block_remain;
}

/*@brief 前一块吸收目标块
 *@param block_prev 前一块
 *@param block      块
 *@retval 新的块
 */
static app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_block_absorb(app_sys_mem_tlsf_block_t *block_prev, app_sys_mem_tlsf_block_t *block)
{
    APP_SYS_ASSERT(app_sys_mem_tlsf_block_size_get(block_prev) != 0);
    uintptr_t size = app_sys_mem_tlsf_blk_hrd;
    size += app_sys_mem_tlsf_block_size_get(block);
    size += app_sys_mem_tlsf_block_size_get(block_prev);
    app_sys_mem_tlsf_block_size_set(block_prev, size);
    app_sys_mem_tlsf_block_link_next(block_prev);
    return block_prev;
}

/*@brief 合并前一块
 *@param control 控制块
 *@param block   块
 *@retval 新的块
 */
static app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_block_merge_prev(app_sys_mem_tlsf_control_t *control, app_sys_mem_tlsf_block_t *block)
{
    if (app_sys_mem_tlsf_block_free_prev_get(block)) {
        app_sys_mem_tlsf_block_t *block_prev = app_sys_mem_tlsf_block_prev_get(block);
        APP_SYS_ASSERT(block_prev != NULL);
        APP_SYS_ASSERT(app_sys_mem_tlsf_block_free_curr_get(block_prev));
        app_sys_mem_tlsf_block_del(control, block_prev);
        block = app_sys_mem_tlsf_block_absorb(block_prev, block);
    }
    return block;
}

/*@brief 合并后一块
 *@param control 控制块
 *@param block   块
 *@retval 新的块
 */
static app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_block_merge_next(app_sys_mem_tlsf_control_t *control, app_sys_mem_tlsf_block_t *block)
{
    app_sys_mem_tlsf_block_t *block_next = app_sys_mem_tlsf_block_next_get(block);
    APP_SYS_ASSERT(block_next != NULL);
    
    if (app_sys_mem_tlsf_block_free_curr_get(block_next)) {
        APP_SYS_ASSERT(app_sys_mem_tlsf_block_size_get(block) != 0);
        app_sys_mem_tlsf_block_del(control, block_next);
        block = app_sys_mem_tlsf_block_absorb(block, block_next);
    }
    return block;
}

/*@brief 块裁剪并标记空闲
 *@param control 控制块
 *@param block   块
 *@param size    尺寸
 */
static void app_sys_mem_tlsf_block_trim_free(app_sys_mem_tlsf_control_t *control, app_sys_mem_tlsf_block_t *block, uintptr_t size)
{
    APP_SYS_ASSERT(app_sys_mem_tlsf_block_free_curr_get(block));
    
    if (app_sys_mem_tlsf_block_split_check(block, size)) {
        app_sys_mem_tlsf_block_t *block_remain = app_sys_mem_tlsf_block_split(block, size);
        app_sys_mem_tlsf_block_link_next(block);
        
        app_sys_mem_tlsf_block_free_prev_set(block_remain, true);
        app_sys_mem_tlsf_block_add(control, block_remain);
    }
}

/*@brief 块裁剪并标记使用
 *@param control 控制块
 *@param block   块
 *@param size    尺寸
 */
static void app_sys_mem_tlsf_block_trim_used(app_sys_mem_tlsf_control_t *control, app_sys_mem_tlsf_block_t *block, uintptr_t size)
{
    APP_SYS_ASSERT(!app_sys_mem_tlsf_block_free_curr_get(block));
    
    if (app_sys_mem_tlsf_block_split_check(block, size)) {
        app_sys_mem_tlsf_block_t *block_remain = app_sys_mem_tlsf_block_split(block, size);
        app_sys_mem_tlsf_block_free_prev_set(block_remain, false);
    
        block_remain = app_sys_mem_tlsf_block_merge_next(control, block_remain);
        app_sys_mem_tlsf_block_add(control, block_remain);
    }
}

/*@brief 块裁剪并标记空闲
 *@param control 控制块
 *@param block   块
 *@param size    尺寸
 */
static app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_block_trim_free_lead(app_sys_mem_tlsf_control_t *control, app_sys_mem_tlsf_block_t *block, uintptr_t size)
{
    app_sys_mem_tlsf_block_t *block_remain = block;
    
    if (app_sys_mem_tlsf_block_split_check(block, size)) {
        block_remain = app_sys_mem_tlsf_block_split(block, size - app_sys_mem_tlsf_blk_hrd);
        app_sys_mem_tlsf_block_free_prev_set(block_remain, true);
        
        app_sys_mem_tlsf_block_link_next(block);
        app_sys_mem_tlsf_block_add(control, block);
    }
    return block_remain;
}

/*@brief 定位空闲块
 *@param control 控制块
 *@param size    尺寸
 *@retval 新的块
 */
static app_sys_mem_tlsf_block_t * app_sys_mem_tlsf_block_free_locate(app_sys_mem_tlsf_control_t *control, uintptr_t size)
{
    uintptr_t fl = 0, sl = 0;
    app_sys_mem_tlsf_block_t *block = NULL;
    
    if (size) {
        app_sys_mem_tlsf_map_find(size, &fl, &sl);
        
        if (fl < app_sys_mem_tlsf_fl_count)
            block = app_sys_mem_tlsf_block_find(control, &fl, &sl);
    }
    
    if (block != NULL) {
        APP_SYS_ASSERT(app_sys_mem_tlsf_block_size_get(block) >= size);
        app_sys_mem_tlsf_block_del_free(control, block, fl, sl);
    }
    
    return block;
}

/*@brief 转化空闲块
 *@param control 控制块
 *@param block   块
 *@param size    尺寸
 *@retval 内存地址
 */
static void * app_sys_mem_tlsf_block_free_convert(app_sys_mem_tlsf_control_t *control, app_sys_mem_tlsf_block_t *block, uintptr_t size)
{
    void *ptr = NULL;
    
    if (block != NULL) {
        APP_SYS_ASSERT(size != 0);
        
        app_sys_mem_tlsf_block_trim_free(control, block, size);
        app_sys_mem_tlsf_block_mark(block, false);
        ptr = app_sys_mem_tlsf_block_to_ptr(block);
    }
    
    return ptr;
}

/*@brief 就绪控制块
 *@param control 控制块
 */
static void app_sys_mem_tlsf_control_ready(app_sys_mem_tlsf_control_t *control)
{
    control->block_none.prev = &control->block_none;
    control->block_none.next = &control->block_none;
    
    for (uintptr_t idx = 0; idx < app_sys_mem_tlsf_fl_count; idx++)
         control->bitmap_sl[idx] = 0;
         control->bitmap_fl = 0;
    
    for (uintptr_t idx1 = 0; idx1 < app_sys_mem_tlsf_fl_count; idx1++)
    for (uintptr_t idx2 = 0; idx2 < app_sys_mem_tlsf_sl_count; idx2++)
         control->block[idx1][idx2] = &control->block_none;
}

/*@brief 二级隔离策略分配堆释放内存
 *@param mem_tlsf 二级隔离策略分配堆实例
 *@param pointer  内存地址
 */
void app_sys_mem_tlsf_free(app_sys_mem_tlsf_t *mem_tlsf, void *pointer)
{
    if (app_sys_mem_tlsf_byte == 0)
        return;
    
    if (pointer == NULL)
        return;
    
    app_sys_mem_tlsf_control_t *control = mem_tlsf;
    app_sys_mem_tlsf_block_t   *block   = NULL;
    
    block = app_sys_mem_tlsf_ptr_to_block(pointer);
    APP_SYS_ASSERT(!app_sys_mem_tlsf_block_free_curr_get(block));
    
    app_sys_mem_tlsf_block_mark(block, true);
    block = app_sys_mem_tlsf_block_merge_prev(control, block);
    block = app_sys_mem_tlsf_block_merge_next(control, block);
    app_sys_mem_tlsf_block_add(control, block);
}

/*@brief 二级隔离策略分配堆申请内存
 *@param mem_tlsf 二级隔离策略分配堆实例
 *@param size     字节大小
 *@retval 内存地址
 */
void * app_sys_mem_tlsf_alloc(app_sys_mem_tlsf_t *mem_tlsf, uintptr_t size)
{
    if (app_sys_mem_tlsf_byte == 0)
        return NULL;
    
    if (size == 0)
        return NULL;
    
    if (size <= sizeof(uintptr_t))
        size  = sizeof(uintptr_t);
    
    void *pointer = NULL;
    app_sys_mem_tlsf_control_t *control = mem_tlsf;
    app_sys_mem_tlsf_block_t   *block   = NULL;
    
    uintptr_t size_req = app_sys_mem_tlsf_block_size_align(size, sizeof(uintptr_t));
    block   = app_sys_mem_tlsf_block_free_locate(control, size_req);
    pointer = app_sys_mem_tlsf_block_free_convert(control, block, size_req);
    
    return pointer;
}

/*@brief 二级隔离策略分配堆申请内存
 *@param mem_tlsf 一级隔离策略分配堆实例
 *@param size     字节大小
 *@param align    指定字节对齐(不小于平台字节对齐, 2的指数)
 *@retval 内存地址
 */
void * app_sys_mem_tlsf_alloc_align(app_sys_mem_tlsf_t *mem_tlsf, uintptr_t size, uintptr_t align)
{
    if (app_sys_mem_tlsf_byte == 0)
        return NULL;
    
    if (size == 0)
        return NULL;
    /* 对齐尺寸检查 */
    if (!app_sys_pow2check(align) || align < sizeof(uintptr_t)) {
         APP_SYS_LOG_WARN("align size fail:%d", align);
         return NULL;
    }
    if (align == sizeof(uintptr_t))
        return app_sys_mem_tlsf_alloc(mem_tlsf, size);
    
    if (size <= sizeof(uintptr_t))
        size  = sizeof(uintptr_t);
    /*默认平台字节对齐:
     *为了实现更高粒度的对齐效果
     *在分配空间时额外多申请一个小块
     *在对齐后切碎前面的小块并将其还回
     *此时获得的地址为目标对齐的
     */
    void *pointer = NULL;
    app_sys_mem_tlsf_control_t *control = mem_tlsf;
    app_sys_mem_tlsf_block_t   *block   = NULL;
    
    uintptr_t size_pad = sizeof(app_sys_mem_tlsf_block_t);
    uintptr_t size_req = app_sys_mem_tlsf_block_size_align(size, sizeof(uintptr_t));
    uintptr_t size_align = app_sys_mem_tlsf_block_size_align(size_req + align + size_pad, sizeof(uintptr_t));
    APP_SYS_ASSERT(sizeof(app_sys_mem_tlsf_block_t) == app_sys_mem_tlsf_blk_hrd + app_sys_mem_tlsf_blk_min);
    
    block = app_sys_mem_tlsf_block_free_locate(control, size_align);
    if (block != NULL) {
        void *ptr = app_sys_mem_tlsf_block_to_ptr(block);
        void *ptr_align = app_sys_align_high(ptr, align);
        uintptr_t size_gap = (uintptr_t)ptr_align - (uintptr_t)ptr;
        /* 对齐到下一个对齐地址 */
        if (size_gap != 0 && size_gap < size_pad) {
            uintptr_t size_remain = size_pad - size_gap;
            uintptr_t size_offset = app_sys_max(size_remain, align);
            ptr_align = app_sys_align_high((void *)((uintptr_t)ptr_align + size_offset), align);
            size_gap  = (uintptr_t)ptr_align - (uintptr_t)ptr;
        }
        if (size_gap != 0) {
            APP_SYS_ASSERT(size_gap >= size_pad);
            block = app_sys_mem_tlsf_block_trim_free_lead(control, block, size_gap);
        }
    }
    pointer = app_sys_mem_tlsf_block_free_convert(control, block, size_req);
    
    return pointer;
}

/*@brief 二级隔离策略分配堆实例初始化
 *@param addr     内存地址
 *@param size     字节大小
 *@retval 返回分配器(分配器在内存头部)
 */
app_sys_mem_tlsf_t * app_sys_mem_tlsf_ready(void *addr, uintptr_t size)
{
    app_sys_mem_tlsf_control_t *control = NULL;
    app_sys_mem_tlsf_block_t   *block   = NULL;
    
    if (app_sys_mem_tlsf_byte == 0)
        return NULL;
    
    void *mem = NULL;
    
    /* 平台字节对齐 */
    mem   = app_sys_align_high(addr, sizeof(uintptr_t));
    size -= (uintptr_t)mem - (uintptr_t)addr;
    addr  = mem;
    
    /* 控制块字节对齐到头部 */
    control = addr;
    app_sys_mem_tlsf_control_ready(control);
    addr  = (void *)((uintptr_t)control + sizeof(app_sys_mem_tlsf_control_t));
    mem   = app_sys_align_high(addr, sizeof(uintptr_t));
    size -= (uintptr_t)mem - (uintptr_t)control;
    addr  = mem;
    
    /* 计算保留字节 */
    uintptr_t size_rev = app_sys_mem_tlsf_blk_hrd * 2;
    uintptr_t size_rev_align = (uintptr_t)app_sys_align_low((void *)(size - size_rev), sizeof(uintptr_t));
    APP_SYS_ASSERT(app_sys_mem_tlsf_blk_min <= size_rev_align);
    APP_SYS_ASSERT(app_sys_mem_tlsf_blk_max >= size_rev_align);
    
    /* 初始空闲块 */
    block = app_sys_mem_tlsf_ptr_to_block_with_size(addr, 0 - app_sys_mem_tlsf_blk_hrd);
    app_sys_mem_tlsf_block_size_set(block, size_rev_align);
    app_sys_mem_tlsf_block_free_curr_set(block, true);
    app_sys_mem_tlsf_block_free_prev_set(block, false);
    app_sys_mem_tlsf_block_add(control, block);
    
    /* 链接下一块 */
    block = app_sys_mem_tlsf_block_link_next(block);
    app_sys_mem_tlsf_block_size_set(block, 0);
    app_sys_mem_tlsf_block_free_curr_set(block, false);
    app_sys_mem_tlsf_block_free_prev_set(block, true);
    
    return control;
}

/*@brief 二级隔离策略分配堆内存布局使用
 *@param mem_tlsf 二级隔离策略分配堆实例
 */
void app_sys_mem_tlsf_check(app_sys_mem_tlsf_t *mem_tlsf)
{
    app_sys_mem_tlsf_control_t *control = mem_tlsf;
    app_sys_mem_tlsf_block_t   *block   = NULL;
    
    if (app_sys_mem_tlsf_byte == 0)
        return;
    
    APP_SYS_LOG_INFO("const:");
    APP_SYS_LOG_INFO("byte:%d ",     app_sys_mem_tlsf_byte);
    APP_SYS_LOG_INFO("fl_index:%d ", app_sys_mem_tlsf_fl_index);
    APP_SYS_LOG_INFO("sl_index:%d ", app_sys_mem_tlsf_sl_index);
    APP_SYS_LOG_INFO("fl_shift:%d ", app_sys_mem_tlsf_fl_shift);
    APP_SYS_LOG_INFO("fl_count:%d ", app_sys_mem_tlsf_fl_count);
    APP_SYS_LOG_INFO("sl_count:%d ", app_sys_mem_tlsf_sl_count);
    APP_SYS_LOG_INFO("fl_limit:%d ", app_sys_mem_tlsf_fl_limit);
    APP_SYS_LOG_INFO("blk_max:%d ",  app_sys_mem_tlsf_blk_max);
    APP_SYS_LOG_INFO("blk_min:%d ",  app_sys_mem_tlsf_blk_min);
    APP_SYS_LOG_INFO("blk_hrd:%d ",  app_sys_mem_tlsf_blk_hrd);
    APP_SYS_LOG_INFO("blk_ofs:%d ",  app_sys_mem_tlsf_blk_ofs);

    APP_SYS_LOG_INFO("control:");
    APP_SYS_LOG_INFO("bitmap fl:0x%x", control->bitmap_fl);
    for (uintptr_t idx1 = 0; idx1 < app_sys_mem_tlsf_fl_count; idx1++)
    for (uintptr_t idx2 = 0; idx2 < app_sys_mem_tlsf_sl_count; idx2++)
    if (control->block[idx1][idx2] != &control->block_none) {
        APP_SYS_LOG_INFO("bitmap sl:0x%x, block list(%d-%d):", control->bitmap_sl[idx1], idx1, idx2);
        for (block = control->block[idx1][idx2]; block != &control->block_none; block = block->next) {
            APP_SYS_LOG_INFO_RAW("< size:%d ", app_sys_mem_tlsf_block_size_get(block));
            APP_SYS_LOG_INFO_RAW("- free_curr:%d ", app_sys_mem_tlsf_block_free_curr_get(block));
            APP_SYS_LOG_INFO_RAW("- free_prev:%d ", app_sys_mem_tlsf_block_free_prev_get(block));
            APP_SYS_LOG_INFO_RAW("- prev:%p ", block->prev);
            APP_SYS_LOG_INFO_RAW("- next:%p ", block->next);
            APP_SYS_LOG_INFO_RAW("- prev_tail:%p >", block->prev_tail);
            APP_SYS_LOG_INFO_RAW(app_sys_log_line());
        }
    }
}
