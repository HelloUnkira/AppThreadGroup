/*实现目标:
 *    heap分配器(一级隔离策略分配堆)
 */

#define APP_SYS_LOG_RECORD_LIMIT    1
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 计算field的域数据占用
 *@param mem_olsf    一级隔离策略分配堆实例
 *@param chunk_align 块单元对齐(默认字节对齐,实际大小)
 *@retval 域数据占用长度
 */
static inline uintptr_t app_sys_mem_olsf_field_size(app_sys_mem_olsf_t *mem_olsf, bool chunk_align)
{
    uintptr_t field_size = 0;
    /* 根据number的数量确认chunk实际使用的类型, 它也会随sizeof(uintptr_t)而变化 */
     if (mem_olsf->number < 0x7F)
         field_size = sizeof(app_sys_mem_olsf_field_8_t);
     if (mem_olsf->number < 0x7FFF)
         field_size = sizeof(app_sys_mem_olsf_field_16_t);
     if (mem_olsf->number < 0x7FFFFFFF)
         field_size = sizeof(app_sys_mem_olsf_field_32_t);
     if (mem_olsf->number < 0x7FFFFFFFFFFFFFFF)
         field_size = sizeof(app_sys_mem_olsf_field_64_t);
     /*  */
     if (field_size == 0)
         APP_SYS_ASSERT(false);
     if (!chunk_align)
          return field_size;
     /* 需要chunk unit对齐 */
     return (uintptr_t)app_sys_align_high((void *)field_size, sizeof(uintptr_t));
}

/*@brief 偏移到chunk的第0个块头
 *       此时索引是类似于chunk size的chunk unit类型数组
 *@param mem_olsf 一级隔离策略分配堆实例
 *@retval 返回chunk 0的地址
 */
static inline uintptr_t * app_sys_mem_olsf_chunk_zero(app_sys_mem_olsf_t *mem_olsf)
{
    /* 通过内存布局规则,偏移到第0个chunk,它刚好就是chunk field的起始位置 */
    return (uintptr_t *)((uintptr_t)mem_olsf - app_sys_mem_olsf_field_size(mem_olsf, true));
}

/*@brief 设置chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@param type     指定chunk的type
 *@param value    指定chunk的value
 */
static inline void app_sys_mem_olsf_field_set(app_sys_mem_olsf_t *mem_olsf, app_sys_mem_olsf_type_t type, uintptr_t chunk, uintptr_t value)
{
    APP_SYS_ASSERT(chunk <= mem_olsf->number);
    uintptr_t *chunk_x = &app_sys_mem_olsf_chunk_zero(mem_olsf)[chunk];
    app_sys_mem_olsf_field_8_t  *chunk_x_8  = (void *)chunk_x;
    app_sys_mem_olsf_field_16_t *chunk_x_16 = (void *)chunk_x;
    app_sys_mem_olsf_field_32_t *chunk_x_32 = (void *)chunk_x;
    app_sys_mem_olsf_field_64_t *chunk_x_64 = (void *)chunk_x;
    uintptr_t field_size = app_sys_mem_olsf_field_size(mem_olsf, false);
    /* 根据field的类型设置 */
    switch (field_size) {
    case sizeof(app_sys_mem_olsf_field_8_t):
         chunk_x_8->field[type] = value; APP_SYS_ASSERT(value ==  (uint8_t)value); break;
    case sizeof(app_sys_mem_olsf_field_16_t):
        chunk_x_16->field[type] = value; APP_SYS_ASSERT(value == (uint16_t)value); break;
    case sizeof(app_sys_mem_olsf_field_32_t):
        chunk_x_32->field[type] = value; APP_SYS_ASSERT(value == (uint32_t)value); break;
    case sizeof(app_sys_mem_olsf_field_64_t):
        chunk_x_64->field[type] = value; APP_SYS_ASSERT(value == (uint64_t)value); break;
    default: APP_SYS_ASSERT(false);
    }
}

/*@brief 获得chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@param type     指定chunk的type
 *@retval 指定chunk的value
 */
static inline uintptr_t app_sys_mem_olsf_field_get(app_sys_mem_olsf_t *mem_olsf, app_sys_mem_olsf_type_t type, uintptr_t chunk)
{
    APP_SYS_ASSERT(chunk <= mem_olsf->number);
    uintptr_t *chunk_x = &app_sys_mem_olsf_chunk_zero(mem_olsf)[chunk];
    app_sys_mem_olsf_field_8_t  *chunk_x_8  = (void *)chunk_x;
    app_sys_mem_olsf_field_16_t *chunk_x_16 = (void *)chunk_x;
    app_sys_mem_olsf_field_32_t *chunk_x_32 = (void *)chunk_x;
    app_sys_mem_olsf_field_64_t *chunk_x_64 = (void *)chunk_x;
    uintptr_t field_size = app_sys_mem_olsf_field_size(mem_olsf, false);
    /* 根据field的类型获取 */
    switch (field_size) {
    case sizeof(app_sys_mem_olsf_field_8_t):  return  chunk_x_8->field[type];
    case sizeof(app_sys_mem_olsf_field_16_t): return chunk_x_16->field[type];
    case sizeof(app_sys_mem_olsf_field_32_t): return chunk_x_32->field[type];
    case sizeof(app_sys_mem_olsf_field_64_t): return chunk_x_64->field[type];
    default: APP_SYS_ASSERT(false); return 0;
    }
}

/*@brief 设置chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@param value    指定chunk的value
 */
static inline void app_sys_mem_olsf_size_set(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk, uintptr_t size)
{
    /* 注意: 这里不需要保留已使用的位,因为当修改块的大小时它永远不会被使用 */
    /*       并在app_sys_mem_olsf_size_set之后 */
    /*       总是app_sys_mem_olsf_used_set会潜在的调用 */
    app_sys_mem_olsf_field_set(mem_olsf, app_sys_mem_olsf_size_used, chunk, size << 1);
}

/*@brief 设置chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@param value    指定chunk的value
 */
static inline void app_sys_mem_olsf_used_set(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk, uintptr_t used)
{
    uintptr_t value = app_sys_mem_olsf_field_get(mem_olsf, app_sys_mem_olsf_size_used, chunk);
    app_sys_mem_olsf_field_set(mem_olsf, app_sys_mem_olsf_size_used, chunk, used ? value | 1 : value & ~1);
}

/*@brief 设置chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@param value    指定chunk的value
 */
static inline void app_sys_mem_olsf_free_prev_set(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk, uintptr_t free_prev)
{
    app_sys_mem_olsf_field_set(mem_olsf, app_sys_mem_olsf_free_prev, chunk, free_prev);
}

/*@brief 设置chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@param value    指定chunk的value
 */
static inline void app_sys_mem_olsf_free_next_set(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk, uintptr_t free_next)
{
    app_sys_mem_olsf_field_set(mem_olsf, app_sys_mem_olsf_free_next, chunk, free_next);
}

/*@brief 设置chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@param value    指定chunk的value
 */
static inline void app_sys_mem_olsf_size_near_set(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk, uintptr_t size_near)
{
    app_sys_mem_olsf_field_set(mem_olsf, app_sys_mem_olsf_size_near, chunk, size_near);
}

/*@brief 获得chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@retval 指定chunk的value
 */
static inline uintptr_t app_sys_mem_olsf_size_get(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk)
{
    return app_sys_mem_olsf_field_get(mem_olsf, app_sys_mem_olsf_size_used, chunk) >> 1;
}

/*@brief 获得chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@retval 指定chunk的value
 */
static inline uintptr_t app_sys_mem_olsf_used_get(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk)
{
    return app_sys_mem_olsf_field_get(mem_olsf, app_sys_mem_olsf_size_used, chunk) & 1;
}

/*@brief 获得chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@retval 指定chunk的value
 */
static inline uintptr_t app_sys_mem_olsf_free_prev_get(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk)
{
    return app_sys_mem_olsf_field_get(mem_olsf, app_sys_mem_olsf_free_prev, chunk);
}

/*@brief 获得chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@retval 指定chunk的value
 */
static inline uintptr_t app_sys_mem_olsf_free_next_get(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk)
{
    return app_sys_mem_olsf_field_get(mem_olsf, app_sys_mem_olsf_free_next, chunk);
}

/*@brief 获得chunk field字段值
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@retval 指定chunk的value
 */
static inline uintptr_t app_sys_mem_olsf_size_near_get(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk)
{
    return app_sys_mem_olsf_field_get(mem_olsf, app_sys_mem_olsf_size_near, chunk);
}

/*@brief 获得临近chunk
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@retval 指定chunk的左临近chunk
 */
static inline uintptr_t app_sys_mem_olsf_chunk_left(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk)
{
    return chunk - app_sys_mem_olsf_size_near_get(mem_olsf, chunk);
}

/*@brief 获得临近chunk
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@retval 指定chunk的右临近chunk
 */
static inline uintptr_t app_sys_mem_olsf_chunk_right(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk)
{
    return chunk + app_sys_mem_olsf_size_get(mem_olsf, chunk);
}

/*@brief 计算size处于哪一个bucket
 *@param size 字节大小
 */
static inline uintptr_t app_sys_mem_olsf_calc_bkt_idx(uintptr_t size)
{
    for (uint32_t idx = 0; idx < sizeof(uintptr_t) * 8; idx++)
        if (((uintptr_t)1 << (sizeof(uintptr_t) * 8 - 1 - idx)) & (size + 1))
            return sizeof(uintptr_t) * 8 - 1 - idx;
    /*  */
    APP_SYS_ASSERT(false);
    return 0;
}

/*@brief 空闲链表移除chunk,从指定index的bucket中
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk     指定chunk
 *@param index     指定bucket的index
 */
static void app_sys_mem_olsf_free_list_del_bkt_idx(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk, uintptr_t index)
{
    uintptr_t *bucket = &mem_olsf->bucket[index];
    /* bucket不为空,标记存在,chunk未使用 */
    APP_SYS_ASSERT(*bucket != 0);
    APP_SYS_ASSERT(app_sys_mem_olsf_used_get(mem_olsf, chunk) == 0);
    APP_SYS_ASSERT((mem_olsf->bitmap & ((uintptr_t)1 << index)) != 0);
    uintptr_t chunk_next = app_sys_mem_olsf_free_next_get(mem_olsf, chunk);
    uintptr_t chunk_prev = app_sys_mem_olsf_free_prev_get(mem_olsf, chunk);
    /* 循环链表中如果只剩下一项chunk时 */
    if (chunk_next == chunk) {
        mem_olsf->bitmap &= ~((uintptr_t)1 << index);
        *bucket = 0;
    } else {
        /* 更新Chunk的前向索引以及后项索引 */
        app_sys_mem_olsf_free_prev_set(mem_olsf, chunk_next, chunk_prev);
        app_sys_mem_olsf_free_next_set(mem_olsf, chunk_prev, chunk_next);
        *bucket = chunk_next;
    }
}

/*@brief 空闲链表添加chunk,从指定index的bucket中
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@param index    指定bucket的index
 */
static void app_sys_mem_olsf_free_list_add_bkt_idx(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk, uintptr_t index)
{
    uintptr_t *bucket = &mem_olsf->bucket[index];
    /* 循环链表中如果一项都没有时 */
    if (*bucket == 0) {
        APP_SYS_ASSERT((mem_olsf->bitmap & ((uintptr_t)1 << index)) == 0);
        *bucket = chunk;
        mem_olsf->bitmap |= ((uintptr_t)1 << index);
        app_sys_mem_olsf_free_prev_set(mem_olsf, chunk, chunk);
        app_sys_mem_olsf_free_next_set(mem_olsf, chunk, chunk);
    } else {
        APP_SYS_ASSERT((mem_olsf->bitmap & ((uintptr_t)1 << index)) != 0);
        uintptr_t chunk_next = *bucket;
        uintptr_t chunk_prev = app_sys_mem_olsf_free_prev_get(mem_olsf, chunk_next);
        app_sys_mem_olsf_free_prev_set(mem_olsf, chunk, chunk_prev);
        app_sys_mem_olsf_free_next_set(mem_olsf, chunk, chunk_next);
        app_sys_mem_olsf_free_prev_set(mem_olsf, chunk_next, chunk);
        app_sys_mem_olsf_free_next_set(mem_olsf, chunk_prev, chunk);
    }
}

/*@brief 空闲链表移除指定chunk
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@param index    指定bucket的index
 */
static inline void app_sys_mem_olsf_free_list_del(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk)
{
    uintptr_t  size = app_sys_mem_olsf_size_get(mem_olsf, chunk);
    uintptr_t index = app_sys_mem_olsf_calc_bkt_idx(size);
    app_sys_mem_olsf_free_list_del_bkt_idx(mem_olsf, chunk, index);
}

/*@brief 空闲链表添加指定chunk
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@param index    指定bucket的index
 */
static inline void app_sys_mem_olsf_free_list_add(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk)
{
    uintptr_t  size = app_sys_mem_olsf_size_get(mem_olsf, chunk);
    uintptr_t index = app_sys_mem_olsf_calc_bkt_idx(size);
    app_sys_mem_olsf_free_list_add_bkt_idx(mem_olsf, chunk, index);
}

/*@brief 将chunk1分裂为chunk2和chunk3
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk1   指定chunk
 *@param chunk2   指定chunk
 */
static void app_sys_mem_olsf_chunk_split(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk1, uintptr_t chunk2)
{
    /* 计算新chunk1和chunk2的大小 */
    uintptr_t chunk3 = 0;
    uintptr_t size_0 = app_sys_mem_olsf_size_get(mem_olsf, chunk1);
    uintptr_t size_1 = chunk2 - chunk1;
    uintptr_t size_2 = size_0 - size_1;
    /* 检查chunk1和chunk2 */
    APP_SYS_ASSERT(chunk1 < chunk2);
    APP_SYS_ASSERT(size_1 < size_0);
    /* 拆分chunk1为chunk2和chunk3 */
    app_sys_mem_olsf_size_set(mem_olsf, chunk1, size_1);
    app_sys_mem_olsf_size_set(mem_olsf, chunk2, size_2);
    chunk3 = app_sys_mem_olsf_chunk_right(mem_olsf, chunk2);
    app_sys_mem_olsf_size_near_set(mem_olsf, chunk2, size_1);
    app_sys_mem_olsf_size_near_set(mem_olsf, chunk3, size_2);
}

/*@brief 将chunk1和chunk2合并为chunk1
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk1   指定chunk
 *@param chunk2   指定chunk
 */
static void app_sys_mem_olsf_chunk_merge(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk1, uintptr_t chunk2)
{
    uintptr_t chunk3 = 0;
    uintptr_t size_1 = app_sys_mem_olsf_size_get(mem_olsf, chunk1);
    uintptr_t size_2 = app_sys_mem_olsf_size_get(mem_olsf, chunk2);
    uintptr_t size_0 = size_1 + size_2;
    app_sys_mem_olsf_size_set(mem_olsf, chunk1, size_0);
    chunk3 = app_sys_mem_olsf_chunk_right(mem_olsf, chunk2);
    app_sys_mem_olsf_size_near_set(mem_olsf, chunk3, size_0);
}

/*@brief 释放一个chunk
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 */
static void app_sys_mem_olsf_chunk_free(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk)
{
    /* 先尝试性的向右合并 */
    uintptr_t chunk_right = app_sys_mem_olsf_chunk_right(mem_olsf, chunk);
    if (app_sys_mem_olsf_used_get(mem_olsf, chunk_right) == 0) {
        app_sys_mem_olsf_free_list_del(mem_olsf, chunk_right);
        app_sys_mem_olsf_chunk_merge(mem_olsf, chunk, chunk_right);
    }
    /* 后尝试性的向左合并 */
    uintptr_t chunk_left = app_sys_mem_olsf_chunk_left(mem_olsf, chunk);
    if (app_sys_mem_olsf_used_get(mem_olsf, chunk_left) == 0) {
        app_sys_mem_olsf_free_list_del(mem_olsf, chunk_left);
        app_sys_mem_olsf_chunk_merge(mem_olsf, chunk_left, chunk);
        chunk = chunk_left;
    }
    /* 进行最后一次释放,将其加入到循环链表 */
    app_sys_mem_olsf_free_list_add(mem_olsf, chunk);
}

/*@brief 申请一个chunk
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param size     字节大小
 *@retval 指定chunk
 */
static uintptr_t app_sys_mem_olsf_chunk_alloc(app_sys_mem_olsf_t *mem_olsf, uintptr_t size)
{
    uintptr_t  index  = app_sys_mem_olsf_calc_bkt_idx(size);
    uintptr_t *bucket = &mem_olsf->bucket[index];
    APP_SYS_ASSERT(index <= app_sys_mem_olsf_calc_bkt_idx(mem_olsf->number));
    /* 尝试性的在循环链表中命中一个 */
    if (*bucket != 0) {
        uintptr_t record = *bucket;
        /* bucket会轮转,这也能有效平衡新旧块 */
        for (uintptr_t count = app_sys_mem_olsf_loops; count != 0; count--) {
            if (app_sys_mem_olsf_size_get(mem_olsf, *bucket) >= size) {
                uintptr_t chunk = *bucket;
                app_sys_mem_olsf_free_list_del_bkt_idx(mem_olsf, chunk, index);
                return chunk;
            }
            /* 让这个循环链表转起来 */
            *bucket = app_sys_mem_olsf_free_next_get(mem_olsf, *bucket);
            APP_SYS_ASSERT(*bucket != 0);
            if (*bucket == record)
                 break;
        }
    }
    /* 从最小的一个单元内提取一个非空的块,并拆碎它 */
    uintptr_t bitmask = mem_olsf->bitmap & ~(((uintptr_t)1 << (index + 1)) - 1);
    if (bitmask != 0) {
        for (uintptr_t ofs = 0; ofs < sizeof(uintptr_t) * 8; ofs++) 
            if (((uintptr_t)1 <<  (sizeof(uintptr_t) * 8 - 1 - ofs)) & bitmask) {
                index = sizeof(uintptr_t) * 8 - 1 - ofs;
                break;
            }
        bucket = &mem_olsf->bucket[index];
        uintptr_t chunk = *bucket;
        APP_SYS_ASSERT(chunk != 0);
        app_sys_mem_olsf_free_list_del_bkt_idx(mem_olsf, chunk, index);
        APP_SYS_ASSERT(size <= app_sys_mem_olsf_size_get(mem_olsf, chunk));
        return chunk;
    }
    /* 找不到一个有效的块 */
    return 0;
}

/*@brief addr转为chunk
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param addr     相对内存地址
 *@retval 指定chunk
 */
static uintptr_t app_sys_mem_olsf_addr_to_chunk(app_sys_mem_olsf_t *mem_olsf, uintptr_t addr)
{
    uintptr_t chunk = addr - (uintptr_t)app_sys_mem_olsf_chunk_zero(mem_olsf);
    chunk -= app_sys_mem_olsf_field_size(mem_olsf, true);
    chunk /= sizeof(uintptr_t);
    return chunk;
}

/*@brief chunk转为addr
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param chunk    指定chunk
 *@retval 相对内存地址
 */
static uintptr_t app_sys_mem_olsf_chunk_to_addr(app_sys_mem_olsf_t *mem_olsf, uintptr_t chunk)
{
    uintptr_t addr = &app_sys_mem_olsf_chunk_zero(mem_olsf)[chunk];
    addr += app_sys_mem_olsf_field_size(mem_olsf, true);
    APP_SYS_ASSERT(app_sys_align_check((void *)addr, sizeof(uintptr_t)));
    return addr;
}

/*@brief 对齐size到uintptr_t且加上field对齐的size
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param size     字节大小
 *@retval chunk
 */
static uintptr_t app_sys_mem_olsf_calc_size(app_sys_mem_olsf_t *mem_olsf, uintptr_t size)
{
    size  = (uintptr_t)app_sys_align_high((void *)size, sizeof(uintptr_t));
    size += app_sys_mem_olsf_field_size(mem_olsf, true);
    APP_SYS_ASSERT(app_sys_align_check((void *)size, sizeof(uintptr_t)));
    return size / sizeof(uintptr_t);
}

/*@brief 一级隔离策略分配堆内存使用(最大片段)
 *@param mem_olsf 一级隔离策略分配堆实例
 *@retval 内存大小
 */
uintptr_t app_sys_mem_olsf_frag(app_sys_mem_olsf_t *mem_olsf)
{
    uintptr_t chunk_frag = 0;
    uintptr_t size_free = 0;
    uintptr_t size_used = 0;
    
    for (uintptr_t chunk = 0; chunk < mem_olsf->number; ) {
        
        uintptr_t chunk_size = app_sys_mem_olsf_size_get(mem_olsf, chunk);
        if (chunk_frag < chunk_size && !app_sys_mem_olsf_used_get(mem_olsf, chunk))
            chunk_frag = chunk_size;
        
        chunk += chunk_size;
    }
    
    uintptr_t field_size = app_sys_mem_olsf_field_size(mem_olsf, true);
    return chunk_frag * sizeof(uintptr_t) - field_size;
}

/*@brief 一级隔离策略分配堆内存使用
 *@param mem_olsf 一级隔离策略分配堆实例
 *@retval 内存大小
 */
uintptr_t app_sys_mem_olsf_used(app_sys_mem_olsf_t *mem_olsf)
{
    return mem_olsf->size_used;
}

/*@brief 一级隔离策略分配堆计算指定内存
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param pointer  内存地址
 *@retval 内存大小
 */
uintptr_t app_sys_mem_olsf_size(app_sys_mem_olsf_t *mem_olsf, void *pointer)
{
    if (pointer == NULL)
        return 0;
    uintptr_t chunk = app_sys_mem_olsf_addr_to_chunk(mem_olsf, (uintptr_t)pointer);
    uintptr_t chunk_size = app_sys_mem_olsf_size_get(mem_olsf, chunk);
    uintptr_t field_size = app_sys_mem_olsf_field_size(mem_olsf, true);
    return chunk_size * sizeof(uintptr_t) - field_size;
}

/*@brief 一级隔离策略分配堆计算指定内存所属
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param pointer  内存地址
 *@retval 包含与否
 */
bool app_sys_mem_olsf_inside(app_sys_mem_olsf_t *mem_olsf, void *pointer)
{
    if (pointer == NULL)
        return false;
    
    if (mem_olsf->addr_hdr <= (uintptr_t)pointer &&
        mem_olsf->addr_end >= (uintptr_t)pointer)
        return true;
    
    return false;
}

/*@brief 一级隔离策略分配堆释放内存
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param pointer  内存地址
 */
void app_sys_mem_olsf_free(app_sys_mem_olsf_t *mem_olsf, void *pointer)
{
    if (pointer == NULL)
        return;
    mem_olsf->size_used -= app_sys_mem_olsf_size(mem_olsf, pointer);
    uintptr_t chunk = app_sys_mem_olsf_addr_to_chunk(mem_olsf, (uintptr_t)pointer);
    /* 意外的堆状态(二次释放?) */
    APP_SYS_ASSERT(app_sys_mem_olsf_used_get(mem_olsf, chunk) != 0);
    /* 损坏的堆边界(缓冲溢出?) */
    uintptr_t chunk_left  = app_sys_mem_olsf_chunk_left(mem_olsf,  chunk);
    uintptr_t chunk_right = app_sys_mem_olsf_chunk_right(mem_olsf, chunk);
    uintptr_t left_right  = app_sys_mem_olsf_chunk_right(mem_olsf, chunk_left);
    uintptr_t right_left  = app_sys_mem_olsf_chunk_left(mem_olsf,  chunk_right);
    APP_SYS_ASSERT(left_right == chunk && right_left == chunk);
    /*  */
    app_sys_mem_olsf_used_set(mem_olsf, chunk, false);
    app_sys_mem_olsf_chunk_free(mem_olsf, chunk);
}

/*@brief 一级隔离策略分配堆申请内存
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param size     字节大小
 *@retval 内存地址
 */
void * app_sys_mem_olsf_alloc(app_sys_mem_olsf_t *mem_olsf, uintptr_t size)
{
    void *pointer = NULL;
    /* 分配长度容错检查 */
    if (size == 0 || size / sizeof(uintptr_t) >= mem_olsf->number)
        return NULL;
    /*  */
    size = app_sys_mem_olsf_calc_size(mem_olsf, size);
    uintptr_t chunk = app_sys_mem_olsf_chunk_alloc(mem_olsf, size);
    if (chunk == 0)
        return NULL;
    /* 将剩余的部分切开(保证剩下的空间至少成一个头) */
    if (app_sys_mem_olsf_size_get(mem_olsf, chunk) >
        app_sys_mem_olsf_field_size(mem_olsf, true) + size) {
        app_sys_mem_olsf_chunk_split(mem_olsf, chunk, chunk + size);
        app_sys_mem_olsf_free_list_add(mem_olsf, chunk + size);
    }
    app_sys_mem_olsf_used_set(mem_olsf, chunk, true);
    pointer = (void *)app_sys_mem_olsf_chunk_to_addr(mem_olsf, chunk);
    mem_olsf->size_used += app_sys_mem_olsf_size(mem_olsf, pointer);
    return pointer;
}

/*@brief 一级隔离策略分配堆申请内存
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param size     字节大小
 *@param align    指定字节对齐(sizeof(uintptr_t) * 2^k)
 *@retval 内存地址
 */
void * app_sys_mem_olsf_alloc_align(app_sys_mem_olsf_t *mem_olsf, uintptr_t size, uintptr_t align)
{
    void *pointer = NULL;
    /* 分配长度容错检查 */
    if (size == 0 || size / sizeof(uintptr_t) >= mem_olsf->number)
        return NULL;
    /* 对齐尺寸检查 */
    if (!app_sys_pow2_check(align) || align < sizeof(uintptr_t)) {
         APP_SYS_LOG_WARN("align size fail:%d", align);
         return NULL;
    }
    if (align == sizeof(uintptr_t))
        return app_sys_mem_olsf_alloc(mem_olsf, size);
    /*默认平台字节对齐:
     *为了实现更高粒度的对齐效果
     *在分配空间时额外多申请一个小块
     *在对齐后切碎前面的小块并将其还回
     *此时获得的地址为目标对齐的
     */
    size = app_sys_mem_olsf_calc_size(mem_olsf, size);
    uintptr_t size_pad = app_sys_mem_olsf_calc_size(mem_olsf, align);
    uintptr_t chunk1 = app_sys_mem_olsf_chunk_alloc(mem_olsf, size + size_pad);
    uintptr_t chunk = chunk1;
    if (chunk1 == 0)
        return NULL;
    /* 转为地址,然后向上对齐 */
    void *addr = (void *)app_sys_mem_olsf_chunk_to_addr(mem_olsf, chunk1);
    /* 如果已经对齐了,不需要额外的对齐了 */
    if (addr != app_sys_align_high(addr, align)) {
        addr  = (void *)((uintptr_t)addr + app_sys_mem_olsf_field_size(mem_olsf, true));
        addr  = app_sys_align_high(addr, align);
        chunk = app_sys_mem_olsf_addr_to_chunk(mem_olsf, (uintptr_t)addr);
        APP_SYS_ASSERT(app_sys_mem_olsf_field_size(mem_olsf, true) <= (chunk - chunk1) * sizeof(uintptr_t));
    }
    APP_SYS_ASSERT(chunk1 <= chunk);
    /* 分裂chunk */
    if (chunk1 < chunk) {
        app_sys_mem_olsf_chunk_split(mem_olsf, chunk1, chunk);
        app_sys_mem_olsf_free_list_add(mem_olsf, chunk1);
    }
    /* 将剩余的部分切开(保证剩下的空间至少成一个头) */
    if (app_sys_mem_olsf_size_get(mem_olsf, chunk) >
        app_sys_mem_olsf_field_size(mem_olsf, true) + size) {
        app_sys_mem_olsf_chunk_split(mem_olsf, chunk, chunk + size);
        app_sys_mem_olsf_free_list_add(mem_olsf, chunk + size);
    }
    app_sys_mem_olsf_used_set(mem_olsf, chunk, true);
    pointer = (void *)app_sys_mem_olsf_chunk_to_addr(mem_olsf, chunk);
    APP_SYS_ASSERT(app_sys_align_check(pointer, align));
    mem_olsf->size_used += app_sys_mem_olsf_size(mem_olsf, pointer);
    return pointer;
}

/*@brief 一级隔离策略分配堆实例初始化
 *@param addr 内存地址
 *@param size 字节大小
 *@retval 返回分配器(分配器在内存头部)
 */
app_sys_mem_olsf_t * app_sys_mem_olsf_ready(void *addr, uintptr_t size)
{
    /* 堆太小检查 */
    if (size / sizeof(uintptr_t) < sizeof(app_sys_mem_olsf_field_8_t) * 4 * 2) {
        APP_SYS_LOG_ERROR("size too small");
        return NULL;
    }
    /* 堆太大检查 */
    if (size / sizeof(uintptr_t) > ~((uintptr_t)1 << (sizeof(uintptr_t) * 8 - 1))) {
        APP_SYS_LOG_ERROR("size too large");
        return NULL;
    }
    /* 地址空间转化 */
    uintptr_t addr_hdr = (uintptr_t)addr;
    uintptr_t addr_end = (uintptr_t)addr + size;
    addr_hdr = (uintptr_t)app_sys_align_high((void *)addr_hdr, sizeof(uintptr_t));
    addr_end = (uintptr_t)app_sys_align_high((void *)addr_end, sizeof(uintptr_t));
    APP_SYS_ASSERT(addr_hdr < addr_end);
    /* 地址空间转化 */
    uintptr_t mem_olsf_size = sizeof(app_sys_mem_olsf_t);
    mem_olsf_size = (uintptr_t)app_sys_align_high((void *)mem_olsf_size, sizeof(uintptr_t));
    /* 地址空间转化 */
    uintptr_t size_old = (addr_end - addr_hdr) / sizeof(uintptr_t);
    app_sys_mem_olsf_t mem_olsf_old = {.number = size_old,};
    uintptr_t size_field = app_sys_mem_olsf_field_size(&mem_olsf_old, size);
    /* 计算切除头尾chunk头和分配器的大小后的size */
    uintptr_t size_new = size_old - (2 * size_field + mem_olsf_size) / sizeof(uintptr_t);
    app_sys_mem_olsf_t *mem_olsf = (void *)(addr_hdr + size_field);
    /* 分配器是第一个chunk的内部内容,中间是空闲内存,最后一个chunk为空 */
    mem_olsf->number = size_old;
    mem_olsf->bitmap = 0;
    /* 分配器bucket生成 */
    uintptr_t bkt_idx  = app_sys_mem_olsf_calc_bkt_idx(size_new) + 1;
    uintptr_t bkt_size = sizeof(uintptr_t) * (bkt_idx + 1);
    /* 初始化循环空闲链表 */
    for (uintptr_t idx = 0; idx < bkt_idx; mem_olsf->bucket[idx] = 0, idx++);
    /* 将该空间切碎成三块,头,中,尾 */
    uintptr_t chunk0 = (0);
    uintptr_t size_0 = (size_field + mem_olsf_size + bkt_size) / sizeof(uintptr_t);
    uintptr_t chunk1 = (chunk0 + size_0);
    uintptr_t size_1 = (size_old - size_0) - size_field / sizeof(uintptr_t);
    uintptr_t chunk2 = (chunk1 + size_1);
    uintptr_t size_2 = (size_field / sizeof(uintptr_t));
    /* 记录内部维护块 */
    mem_olsf->chunk_hdr = chunk0;
    mem_olsf->chunk_end = chunk2;
    /* 初始化第一个chunk: */
    app_sys_mem_olsf_free_prev_set(mem_olsf, chunk0, chunk0);
    app_sys_mem_olsf_free_next_set(mem_olsf, chunk0, chunk0);
    app_sys_mem_olsf_size_set(mem_olsf, chunk0, size_0);
    app_sys_mem_olsf_size_near_set(mem_olsf, chunk0, 0);
    app_sys_mem_olsf_used_set(mem_olsf, chunk0, true);
    /* 初始化中间的chunk: */
    app_sys_mem_olsf_size_set(mem_olsf, chunk1, size_1);
    app_sys_mem_olsf_size_near_set(mem_olsf, chunk1, size_0);
    app_sys_mem_olsf_used_set(mem_olsf, chunk1, false);
    /* 初始化最后一个chunk: */
    app_sys_mem_olsf_free_prev_set(mem_olsf, chunk2, chunk2);
    app_sys_mem_olsf_free_next_set(mem_olsf, chunk2, chunk2);
    app_sys_mem_olsf_size_set(mem_olsf, chunk2, size_2);
    app_sys_mem_olsf_size_near_set(mem_olsf, chunk2, size_1);
    app_sys_mem_olsf_used_set(mem_olsf, chunk2, true);
    /* 将中间的大块加入到循环链表中 */
    app_sys_mem_olsf_free_list_add(mem_olsf, chunk1);
    /* 记录部分数据 */
    mem_olsf->addr_base = addr;
    mem_olsf->size_base = size;
    mem_olsf->addr_hdr  = addr_hdr;
    mem_olsf->addr_end  = addr_end;
    mem_olsf->size_old  = size_old;
    mem_olsf->size_new  = size_new;
    mem_olsf->size_used = 0;
    return mem_olsf;
}

/*@brief 一级隔离策略分配堆内存布局使用
 *@param mem_olsf 一级隔离策略分配堆实例
 */
void app_sys_mem_olsf_check(app_sys_mem_olsf_t *mem_olsf)
{
    APP_SYS_LOG_INFO("mem_olsf mem:");
    APP_SYS_LOG_INFO("addr_base:%p",   (void *)mem_olsf->addr_base);
    APP_SYS_LOG_INFO("size_base:0x%x", mem_olsf->size_base);
    APP_SYS_LOG_INFO("mem_olsf align:");
    APP_SYS_LOG_INFO("addr_hdr:%p", (void *)mem_olsf->addr_hdr);
    APP_SYS_LOG_INFO("addr_end:%p", (void *)mem_olsf->addr_end);
    APP_SYS_LOG_INFO("size_old:%u",  mem_olsf->size_old);
    APP_SYS_LOG_INFO("size_new:%u",  mem_olsf->size_new);
    APP_SYS_LOG_INFO("mem_olsf:");
    APP_SYS_LOG_INFO("field_size:%u",   app_sys_mem_olsf_field_size(mem_olsf, false));
    APP_SYS_LOG_INFO("number:%u",       mem_olsf->number);
    APP_SYS_LOG_INFO("bitmap:0x%x:",    mem_olsf->bitmap);
    
    uintptr_t size_free_max = 0;
    uintptr_t size_free = 0;
    uintptr_t size_used = 0;
    
    APP_SYS_LOG_INFO_RAW("chunk \t size \t used \t near \t prev \t next \t left \t right");
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    for (uintptr_t chunk = 0; chunk < mem_olsf->number; ) {
        APP_SYS_LOG_INFO_RAW("%08u \t ", chunk);
        APP_SYS_LOG_INFO_RAW("%08u \t ", app_sys_mem_olsf_size_get(mem_olsf, chunk));
        APP_SYS_LOG_INFO_RAW("%u \t ",   app_sys_mem_olsf_used_get(mem_olsf, chunk));
        APP_SYS_LOG_INFO_RAW("%08u \t ", app_sys_mem_olsf_size_near_get(mem_olsf, chunk));
        APP_SYS_LOG_INFO_RAW("%08u \t ", app_sys_mem_olsf_free_prev_get(mem_olsf, chunk));
        APP_SYS_LOG_INFO_RAW("%08u \t ", app_sys_mem_olsf_free_next_get(mem_olsf, chunk));
        APP_SYS_LOG_INFO_RAW("%08u \t ", app_sys_mem_olsf_chunk_left(mem_olsf, chunk));
        APP_SYS_LOG_INFO_RAW("%08u \t ", app_sys_mem_olsf_chunk_right(mem_olsf, chunk));
        APP_SYS_LOG_INFO_RAW(app_sys_log_line());
        
        bool used = app_sys_mem_olsf_used_get(mem_olsf, chunk);
        uintptr_t size = app_sys_mem_olsf_size_get(mem_olsf, chunk);
        size_free += used ? 0 : size;
        size_used += used ? size : 0;
        if (size_free_max < size && !used)
            size_free_max = size;
        
        chunk += size;
    }
    
    APP_SYS_LOG_INFO("total:");
    APP_SYS_LOG_INFO("size_used:%u", mem_olsf->size_used);
    APP_SYS_LOG_INFO("size_used:%u", size_used);
    APP_SYS_LOG_INFO("size_free:%u", size_free);
    APP_SYS_LOG_INFO("size_free_max:%u", size_free_max);
}

/*@brief 一级隔离策略分配堆内存遍历检查
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param invoke   回调实例
 */
void app_sys_mem_olsf_walk(app_sys_mem_olsf_t *mem_olsf, void (*invoke)(void *pointer, bool used))
{
    for (uintptr_t chunk = 0; chunk < mem_olsf->number; ) {
        bool chunk_inner = false;
        
        // 跳过内部维护的chunk
        if (app_sys_mem_olsf_free_prev_get(mem_olsf, chunk) == 
            app_sys_mem_olsf_free_next_get(mem_olsf, chunk))
            chunk_inner = true;
        
        bool  used = app_sys_mem_olsf_used_get(mem_olsf, chunk);
        void *pointer = (void *)app_sys_mem_olsf_chunk_to_addr(mem_olsf, chunk);
        chunk += app_sys_mem_olsf_size_get(mem_olsf, chunk);
        
        if (!chunk_inner)
            invoke(pointer, used);
    }
}
