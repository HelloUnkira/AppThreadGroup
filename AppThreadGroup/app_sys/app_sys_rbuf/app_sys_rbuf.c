/*实现目标:
 *    可兼容1,2,4,8字节对齐的拷贝式环形队列
 *    该实现参考于zephyr\lib\os
 */

#include "app_ext_lib.h"
#include "app_sys_rbuf.h"

/*@brief        索引回退运算
 *@param[in]    rbuf  实例
 *@param[in]    index 索引
 */
static inline void app_sys_rbuf_rewind_index(app_sys_rbuf *rbuf)
{
    /* 环形缓冲区回退 */
    #define APP_SYS_RING_BUFFER_MAX 0x80000000
    uint32_t rewind = APP_SYS_RING_BUFFER_MAX;
    
    if (rbuf->head >= rewind) {
        rewind /= rbuf->size;
        rewind *= rbuf->size;
        rbuf->head -= rewind;
        rbuf->tail -= rewind;
    }
}

/*@brief        环形队列重置(中断环境下不可调用)
 *@param[in]    rbuf 实例
 */
void app_sys_rbuf_reset(app_sys_rbuf *rbuf)
{
    app_mutex_process(&rbuf->mutex, app_mutex_take);
    rbuf->head = rbuf->tail = 0;
    app_mutex_process(&rbuf->mutex, app_mutex_give);
}

/*@brief        环形队列为空判断(中断环境下不可调用)
 *@param[in]    rbuf 实例
 *@retval       是否为空
 */
bool app_sys_rbuf_is_empty(app_sys_rbuf *rbuf)
{
    app_mutex_process(&rbuf->mutex, app_mutex_take);
    bool is_empty = (rbuf->head == rbuf->tail) ? true : false;
    app_mutex_process(&rbuf->mutex, app_mutex_give);
    return is_empty;
}

/*@brief        获取环形队列已有条目(中断环境下不可调用)
 *@param[in]    rbuf 实例
 *@retval       占用条目数量
 */
uint32_t app_sys_rbuf_get_item(app_sys_rbuf *rbuf)
{
    app_mutex_process(&rbuf->mutex, app_mutex_take);
    uint32_t item = rbuf->tail - rbuf->head;
    app_mutex_process(&rbuf->mutex, app_mutex_give);
    return item;
}

/*@brief        获取环形队列空闲条目(中断环境下不可调用)
 *@param[in]    rbuf 实例
 *@retval       空闲条目数量
 */
uint32_t app_sys_rbuf_get_space(app_sys_rbuf *rbuf)
{
    app_mutex_process(&rbuf->mutex, app_mutex_take);
    uint32_t space = rbuf->size - (rbuf->tail - rbuf->head);
    app_mutex_process(&rbuf->mutex, app_mutex_give);
    return space;
}

/*@brief        就绪环形队列(无参数检查)
 *              当满足buffer为字节对齐且size为2的次方达到最大效率
 *@param[in]    rbuf   实例
 *@param[in]    type   数据单元类型:(1,2,4,8)(字节对齐)
 *@param[in]    buffer 指定的缓冲区,为对齐的字流(不是字节流)(如下)
 *@param[in]    size   对齐字流的长度
 */
void app_sys_rbuf_ready(app_sys_rbuf *rbuf, uint8_t type, void *buffer, uint32_t size)
{
    /* 简要的字节对齐修正 */
    /* 地址截断,通过最后几位确定是否字节对齐 */
    uintptr_t offset = (uintptr_t)buffer & (type - 1);
    
    if (offset != 0) {
        buffer  = (void *)(((uintptr_t)buffer) + type - offset);
        size--;
    }
    
    /* 缓冲区大小字节对齐加速 */
    uint32_t mask = ((size != 0) && ((size & (size - 1)) == 0)) ? (size - 1) : 0;
    
    rbuf->buffer = buffer;
    rbuf->size   = size;
    rbuf->mask   = mask;
    rbuf->head   = 0;
    rbuf->tail   = 0;
    rbuf->type   = type;
    app_mutex_process(&rbuf->mutex, app_mutex_static);
}

/*@brief        从环形队列获取数据(无参数检查)
 *@param[in]    rbuf   实例
 *@param[out]   data   指定对齐类型数据的存储地址(非对齐可能会导致截断的情况)
 *@param[in]    length 所需获取数据长度
 *@retval       -1     数据不足
 *@retval       -2     实例类型错误
 */
int32_t app_sys_rbuf_gets(app_sys_rbuf *rbuf, void *data, uint32_t length)
{
    int32_t   result  = 0;
    uint8_t  *buffer1 = data;
    uint16_t *buffer2 = data;
    uint32_t *buffer4 = data;
    uint64_t *buffer8 = data;
    
    if (result == 0)
    if (app_sys_rbuf_is_empty(rbuf) ||
        app_sys_rbuf_get_item(rbuf) < length)
        result = -1;
    
    if (result == 0)
    app_mutex_process(&rbuf->mutex, app_mutex_take);
    if (result == 0)
    switch (rbuf->type) {
    case 1:
        if (rbuf->mask != 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->head) & rbuf->mask;
                 buffer1[idx] = rbuf->buffer1[idx1];
            }
        if (rbuf->mask == 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->head) % rbuf->size;
                 buffer1[idx] = rbuf->buffer1[idx1];
            }
        break;
    case 2:
        if (rbuf->mask != 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->head) & rbuf->mask;
                 buffer2[idx] = rbuf->buffer2[idx1];
            }
        if (rbuf->mask == 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->head) % rbuf->size;
                 buffer2[idx] = rbuf->buffer2[idx1];
            }
        break;
    case 4:
        if (rbuf->mask != 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->head) & rbuf->mask;
                 buffer4[idx] = rbuf->buffer4[idx1];
            }
        if (rbuf->mask == 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->head) % rbuf->size;
                 buffer4[idx] = rbuf->buffer4[idx1];
            }
        break;
    case 8:
        if (rbuf->mask != 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->head) & rbuf->mask;
                 buffer8[idx] = rbuf->buffer8[idx1];
            }
        if (rbuf->mask == 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->head) % rbuf->size;
                 buffer8[idx] = rbuf->buffer8[idx1];
            }
        break;
    default:
        result = -2;
    }
    if (result == 0)
    rbuf->head += length;
    if (result == 0)
    app_sys_rbuf_rewind_index(rbuf);
    if (result == 0 || result == -2)
    app_mutex_process(&rbuf->mutex, app_mutex_give);
    
    return result;
}

/*@brief        向环形队列推送数据(无参数检查)
 *@param[in]    rbuf   实例
 *@param[out]   data   指定对齐类型数据的存储地址(非对齐可能会导致截断的情况)
 *@param[in]    length 所需推送数据长度
 *@retval       -1:    空间不足
 */
int32_t app_sys_rbuf_puts(app_sys_rbuf *rbuf, void *data, uint32_t length)
{
    int32_t   result  = 0;
    uint8_t  *buffer1 = data;
    uint16_t *buffer2 = data;
    uint32_t *buffer4 = data;
    uint64_t *buffer8 = data;
    
    if (result == 0)
    if (app_sys_rbuf_get_space(rbuf) < length)
        result = -1;

    if (result == 0)
    app_mutex_process(&rbuf->mutex, app_mutex_take);
    if (result == 0)
    switch (rbuf->type) {
    case 1:
        if (rbuf->mask != 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->tail) & rbuf->mask;
                 rbuf->buffer1[idx1] = buffer1[idx];
            }
        if (rbuf->mask == 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->tail) % rbuf->size;
                 rbuf->buffer1[idx1] = buffer1[idx];
            }
        break;
    case 2:
        if (rbuf->mask != 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->tail) & rbuf->mask;
                 rbuf->buffer2[idx1] = buffer2[idx];
            }
        if (rbuf->mask == 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->tail) % rbuf->size;
                 rbuf->buffer2[idx1] = buffer2[idx];
            }
        break;
    case 4:
        if (rbuf->mask != 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->tail) & rbuf->mask;
                 rbuf->buffer4[idx1] = buffer4[idx];
            }
        if (rbuf->mask == 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->tail) % rbuf->size;
                 rbuf->buffer4[idx1] = buffer4[idx];
            }
        break;
    case 8:
        if (rbuf->mask != 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->tail) & rbuf->mask;
                 rbuf->buffer8[idx1] = buffer8[idx];
            }
        if (rbuf->mask == 0)
            for (uint32_t idx = 0; idx < length; idx++) {
                 uint32_t idx1 = (idx + rbuf->tail) % rbuf->size;
                 rbuf->buffer8[idx1] = buffer8[idx];
            }
        break;
    default:
        result = -2;
    }
    
    if (result == 0)
    rbuf->tail += length;
    if (result == 0)
    app_sys_rbuf_rewind_index(rbuf);
    if (result == 0 || result == -2)
    app_mutex_process(&rbuf->mutex, app_mutex_give);
    
    return result;
}
