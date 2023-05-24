#ifndef APP_SYS_RING_BUFFER_H
#define APP_SYS_RING_BUFFER_H

typedef struct {
    /* 类型缓冲区: */
    union {
        void     *buffer;   /* 抽象缓冲区 */
        uint8_t  *buffer1;  /* 缓冲区(1字节对齐) */
        uint16_t *buffer2;  /* 缓冲区(2字节对齐) */
        uint32_t *buffer4;  /* 缓冲区(4字节对齐) */
        uint64_t *buffer8;  /* 缓冲区(8字节对齐) */
    };
    uint32_t    size;       /* 最大数据条目(byte = type * 条目) */
    uint32_t    mask;       /* 最大条目掩码(加速运算) */
    uint32_t    head;       /* 队列起始位置(进动点,非实际下标) */
    uint32_t    tail;       /* 队列结束位置(进动点,非实际下标) */
    uint8_t     type;       /* 字节对齐类型(1,2,4,8) */
    app_mutex_t mutex;      /* 资源保护 */
} app_sys_ring_buffer;

/*@brief        环形队列重置(中断环境下不可调用)
 *@param[in]    ring_buffer 实例
 */
void app_sys_ring_buffer_reset(app_sys_ring_buffer *ring_buffer);

/*@brief        环形队列为空判断(中断环境下不可调用)
 *@param[in]    ring_buffer 实例
 *@retval       是否为空
 */
bool app_sys_ring_buffer_is_empty(app_sys_ring_buffer *ring_buffer);

/*@brief        获取环形队列已有条目(中断环境下不可调用)
 *@param[in]    ring_buffer 实例
 *@retval       占用条目数量
 */
uint32_t app_sys_ring_buffer_get_item(app_sys_ring_buffer *ring_buffer);

/*@brief        获取环形队列空闲条目(中断环境下不可调用)
 *@param[in]    ring_buffer 实例
 *@retval       空闲条目数量
 */
uint32_t app_sys_ring_buffer_get_space(app_sys_ring_buffer *ring_buffer);

/*@brief        就绪环形队列(无参数检查)(中断环境下不可调用)
 *              当满足buffer为字节对齐且size为2的次方达到最大效率
 *@param[in]    ring_buffer 实例
 *@param[in]    type        数据单元类型:(1,2,4,8)(字节对齐)
 *@param[in]    buffer      指定的缓冲区,为对齐的字流(不是字节流)(如下)
 *@param[in]    size        对齐字流的长度
 */
void app_sys_ring_buffer_ready(app_sys_ring_buffer *ring_buffer, uint8_t type,
                               void *buffer, uint32_t size);

/*@brief        从环形队列获取数据(无参数检查)(中断环境下不可调用)
 *@param[in]    ring_buffer 实例
 *@param[out]   data        指定对齐类型数据的存储地址(非对齐可能会导致截断的情况)
 *@param[in]    length      所需获取数据长度
 *@retval       -1          数据不足
 *@retval       -2          实例类型错误
 */
int32_t app_sys_ring_buffer_gets(app_sys_ring_buffer *ring_buffer, void *data, uint32_t length);

/*@brief        向环形队列推送数据(无参数检查)(中断环境下不可调用)
 *@param[in]    ring_buffer 实例
 *@param[out]   data        指定对齐类型数据的存储地址(非对齐可能会导致截断的情况)
 *@param[in]    length      所需推送数据长度
 *@retval       -1          空间不足
 *@retval       -2          实例类型错误
 */
int32_t app_sys_ring_buffer_puts(app_sys_ring_buffer *ring_buffer, void *data, uint32_t length);

#endif
