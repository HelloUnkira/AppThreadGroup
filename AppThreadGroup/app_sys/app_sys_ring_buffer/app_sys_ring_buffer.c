/*实现目标:
 *    可兼容1,2,4,8字节对齐的拷贝式环形队列
 *    该实现参考于zephyr\lib\os
 */

#include "app_ext_lib.h"
#include "app_sys_ring_buffer.h"

/*@brief        索引回退运算
 *@param[in]    ring_buffer 实例
 *@param[in]    index       索引
 */
static inline void app_sys_ring_buffer_rewind_index(app_sys_ring_buffer *ring_buffer)
{
    /* 环形缓冲区回退 */
    #define APP_SYS_RING_BUFFER_MAX     0x80000000
    if (ring_buffer->head >= APP_SYS_RING_BUFFER_MAX) {
        uint32_t  rewind   = APP_SYS_RING_BUFFER_MAX;
        rewind /= ring_buffer->size;
        rewind *= ring_buffer->size;
        ring_buffer->head -= rewind;
        ring_buffer->tail -= rewind;
    }
}

/*@brief        环形队列重置(中断环境下不可调用)
 *@param[in]    ring_buffer 实例
 */
void app_sys_ring_buffer_reset(app_sys_ring_buffer *ring_buffer)
{
    app_mutex_take(&ring_buffer->mutex);
    ring_buffer->head = 0;
    ring_buffer->tail = 0;
    app_mutex_give(&ring_buffer->mutex);
}

/*@brief        环形队列为空判断(中断环境下不可调用)
 *@param[in]    ring_buffer 实例
 *@retval       是否为空
 */
bool app_sys_ring_buffer_is_empty(app_sys_ring_buffer *ring_buffer)
{
    app_mutex_take(&ring_buffer->mutex);
    bool result = (ring_buffer->head == ring_buffer->tail) ? true : false;
    app_mutex_give(&ring_buffer->mutex);
    return result;
}

/*@brief        获取环形队列已有条目(中断环境下不可调用)
 *@param[in]    ring_buffer 实例
 *@retval       占用条目数量
 */
uint32_t app_sys_ring_buffer_get_item(app_sys_ring_buffer *ring_buffer)
{
    app_mutex_take(&ring_buffer->mutex);
    uint32_t item = ring_buffer->tail - ring_buffer->head;
    app_mutex_give(&ring_buffer->mutex);
    return item;
}

/*@brief        获取环形队列空闲条目(中断环境下不可调用)
 *@param[in]    ring_buffer 实例
 *@retval       空闲条目数量
 */
uint32_t app_sys_ring_buffer_get_space(app_sys_ring_buffer *ring_buffer)
{
    app_mutex_take(&ring_buffer->mutex);
    uint32_t space = ring_buffer->size - (ring_buffer->tail - ring_buffer->head);
    app_mutex_give(&ring_buffer->mutex);
    return space;
}

/*@brief        就绪环形队列(无参数检查)
 *              当满足buffer为字节对齐且size为2的次方达到最大效率
 *@param[in]    ring_buffer 实例
 *@param[in]    type        数据单元类型:(1,2,4,8)(字节对齐)
 *@param[in]    buffer      指定的缓冲区,为对齐的字流(不是字节流)(如下)
 *@param[in]    size        对齐字流的长度
 */
void app_sys_ring_buffer_ready(app_sys_ring_buffer *ring_buffer, uint8_t type,
                               void *buffer, uint32_t size)
{
    /* 简要的字节对齐修正 */
    /* 地址截断,通过最后几位确定是否字节对齐 */
    uint8_t offset = (uint8_t)(uintptr_t)buffer & (type - 1);
    
    if (offset != 0) {
        buffer = (((uint8_t *)buffer) + type - offset);
        size--;
    }
    
    /* 缓冲区大小字节对齐加速 */
    uint32_t mask = ((size != 0) && ((size & (size - 1)) == 0)) ? (size - 1) : 0;
    
    ring_buffer->buffer = buffer;
    ring_buffer->size   = size;
    ring_buffer->mask   = mask;
    ring_buffer->head   = 0;
    ring_buffer->tail   = 0;
    ring_buffer->type   = type;
    app_mutex_process(&ring_buffer->mutex);
}

/*@brief        从环形队列获取数据(无参数检查)
 *@param[in]    ring_buffer 实例
 *@param[out]   data        指定对齐类型数据的存储地址(非对齐可能会导致截断的情况)
 *@param[in]    length      所需获取数据长度
 *@retval       -1          数据不足
 *@retval       -2          实例类型错误
 */
int32_t app_sys_ring_buffer_gets(app_sys_ring_buffer *ring_buffer, void *data, uint32_t length)
{
    int32_t   result  = 0;
    uint8_t  *buffer1 = data;
    uint16_t *buffer2 = data;
    uint32_t *buffer4 = data;
    uint64_t *buffer8 = data;
    
    if (result == 0)
    if (app_sys_ring_buffer_is_empty(ring_buffer) ||
        app_sys_ring_buffer_get_item(ring_buffer) < length)
        result = -1;
    
    if (result == 0)
    app_mutex_take(&ring_buffer->mutex);
    if (result == 0)
    switch (ring_buffer->type) {
    case 1:
        if (ring_buffer->mask != 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->head) & ring_buffer->mask;
                 buffer1[Index] = ring_buffer->buffer1[Index1];
            }
        if (ring_buffer->mask == 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->head) % ring_buffer->size;
                 buffer1[Index] = ring_buffer->buffer1[Index1];
            }
        break;
    case 2:
        if (ring_buffer->mask != 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->head) & ring_buffer->mask;
                 buffer2[Index] = ring_buffer->buffer2[Index1];
            }
        if (ring_buffer->mask == 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->head) % ring_buffer->size;
                 buffer2[Index] = ring_buffer->buffer2[Index1];
            }
        break;
    case 4:
        if (ring_buffer->mask != 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->head) & ring_buffer->mask;
                 buffer4[Index] = ring_buffer->buffer4[Index1];
            }
        if (ring_buffer->mask == 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->head) % ring_buffer->size;
                 buffer4[Index] = ring_buffer->buffer4[Index1];
            }
        break;
    case 8:
        if (ring_buffer->mask != 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->head) & ring_buffer->mask;
                 buffer8[Index] = ring_buffer->buffer8[Index1];
            }
        if (ring_buffer->mask == 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->head) % ring_buffer->size;
                 buffer8[Index] = ring_buffer->buffer8[Index1];
            }
        break;
    default:
        result = -2;
    }
    if (result == 0)
    ring_buffer->head += length;
    if (result == 0)
    app_sys_ring_buffer_rewind_index(ring_buffer);
    if (result == 0 || result == -2)
    app_mutex_give(&ring_buffer->mutex);
    
    return result;
}

/*@brief        向环形队列推送数据(无参数检查)
 *@param[in]    ring_buffer    实例
 *@param[out]   data        指定对齐类型数据的存储地址(非对齐可能会导致截断的情况)
 *@param[in]    length      所需推送数据长度
 *@retval       -1:         空间不足
 */
int32_t app_sys_ring_buffer_puts(app_sys_ring_buffer *ring_buffer, void *data, uint32_t length)
{
    int32_t   result  = 0;
    uint8_t  *buffer1 = data;
    uint16_t *buffer2 = data;
    uint32_t *buffer4 = data;
    uint64_t *buffer8 = data;
    
    if (result == 0)
    if (app_sys_ring_buffer_get_space(ring_buffer) < length)
        result = -1;

    if (result == 0)
    app_mutex_take(&ring_buffer->mutex);
    if (result == 0)
    switch (ring_buffer->type) {
    case 1:
        if (ring_buffer->mask != 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->tail) & ring_buffer->mask;
                 ring_buffer->buffer1[Index1] = buffer1[Index];
            }
        if (ring_buffer->mask == 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->tail) % ring_buffer->size;
                 ring_buffer->buffer1[Index1] = buffer1[Index];
            }
        break;
    case 2:
        if (ring_buffer->mask != 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->tail) & ring_buffer->mask;
                 ring_buffer->buffer2[Index1] = buffer2[Index];
            }
        if (ring_buffer->mask == 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->tail) % ring_buffer->size;
                 ring_buffer->buffer2[Index1] = buffer2[Index];
            }
        break;
    case 4:
        if (ring_buffer->mask != 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->tail) & ring_buffer->mask;
                 ring_buffer->buffer4[Index1] = buffer4[Index];
            }
        if (ring_buffer->mask == 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->tail) % ring_buffer->size;
                 ring_buffer->buffer4[Index1] = buffer4[Index];
            }
        break;
    case 8:
        if (ring_buffer->mask != 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->tail) & ring_buffer->mask;
                 ring_buffer->buffer8[Index1] = buffer8[Index];
            }
        if (ring_buffer->mask == 0)
            for (uint32_t Index = 0; Index < length; Index++) {
                 uint32_t Index1 = (Index + ring_buffer->tail) % ring_buffer->size;
                 ring_buffer->buffer8[Index1] = buffer8[Index];
            }
        break;
    default:
        result = -2;
    }
    
    if (result == 0)
    ring_buffer->tail += length;
    if (result == 0)
    app_sys_ring_buffer_rewind_index(ring_buffer);
    if (result == 0 || result == -2)
    app_mutex_give(&ring_buffer->mutex);
    
    return result;
}
