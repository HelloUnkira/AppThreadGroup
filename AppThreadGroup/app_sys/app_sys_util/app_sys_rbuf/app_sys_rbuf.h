#ifndef APP_SYS_RBUF_H
#define APP_SYS_RBUF_H

typedef struct {
    /* 类型缓冲区: */
    union {
        void     *buffer;   /* 抽象缓冲区 */
        uint8_t  *buffer1;  /* 缓冲区(1字节对齐) */
        uint16_t *buffer2;  /* 缓冲区(2字节对齐) */
        uint32_t *buffer4;  /* 缓冲区(4字节对齐) */
        uint64_t *buffer8;  /* 缓冲区(8字节对齐) */
    };
    uint32_t size;  /* 最大数据条目(byte = type * 条目) */
    uint32_t mask;  /* 最大条目掩码(加速运算) */
    uint32_t head;  /* 队列起始位置(进动点,非实际下标) */
    uint32_t tail;  /* 队列结束位置(进动点,非实际下标) */
    uint8_t  type;  /* 字节对齐类型(1,2,4,8) */
} app_sys_rbuf_t;

/*@brief 环形队列重置(中断环境下不可调用)
 *@param rbuf 实例
 */
void app_sys_rbuf_reset(app_sys_rbuf_t *rbuf);

/*@brief 环形队列为空判断(中断环境下不可调用)
 *@param rbuf 实例
 *@retval 是否为空
 */
bool app_sys_rbuf_empty(app_sys_rbuf_t *rbuf);

/*@brief 获取环形队列类型(中断环境下不可调用)
 *@param rbuf 实例
 *@retval 环形队列类型
 */
uint8_t app_sys_rbuf_type(app_sys_rbuf_t *rbuf);

/*@brief 获取环形队列已有条目(中断环境下不可调用)
 *@param rbuf 实例
 *@retval 占用条目数量
 */
uint32_t app_sys_rbuf_item(app_sys_rbuf_t *rbuf);

/*@brief 获取环形队列空闲条目(中断环境下不可调用)
 *@param rbuf 实例
 *@retval 空闲条目数量
 */
uint32_t app_sys_rbuf_space(app_sys_rbuf_t *rbuf);

/*@brief 就绪环形队列(无参数检查)(中断环境下不可调用)
 *              当满足buffer为字节对齐且size为2的次方达到最大效率
 *@param rbuf   实例
 *@param type   数据单元类型:(1,2,4,8)(字节对齐)
 *@param buffer 指定的缓冲区,为对齐的字流(不是字节流)(如下)
 *@param size   对齐字流的长度
 */
void app_sys_rbuf_ready(app_sys_rbuf_t *rbuf, uint8_t type, void *buffer, uint32_t size);

/*@brief 从环形队列获取数据(无参数检查)(中断环境下不可调用)
 *@param rbuf   实例
 *@param data   指定对齐类型数据的存储地址(非对齐可能会导致截断的情况)
 *@param length 所需获取数据长度
 *@retval -1     数据不足
 *@retval -2     实例类型错误
 */
int32_t app_sys_rbuf_gets(app_sys_rbuf_t *rbuf, void *data, uint32_t length);

/*@brief 向环形队列推送数据(无参数检查)(中断环境下不可调用)
 *@param rbuf   实例
 *@param data   指定对齐类型数据的存储地址(非对齐可能会导致截断的情况)
 *@param length 所需推送数据长度
 *@retval -1     空间不足
 *@retval -2     实例类型错误
 */
int32_t app_sys_rbuf_puts(app_sys_rbuf_t *rbuf, void *data, uint32_t length);

/*补充扩展:
 *    上述满足线程安全的环形缓冲区所需接口已具完备性
 *    下面是一些补充简化的接口
 *    此外gets和puts仅配套使用以免缓冲区紊乱
 */

/*@brief 定长数据的gets接口,参数细节与原型一致(中断环境下不可调用)
 *       注意:这里的环形队列类型需要和type对齐
 *       建议使用以下宏对齐type以加速:
 *           #pragma pack(push, x)
 *           #pragma pack(pop)
 */
#define APP_SYS_RBUF_GETS_FIXED(rbuf, data, type)   \
        app_sys_rbuf_gets(rbuf, (void *)data, sizeof(type) / app_sys_rbuf_type(rbuf))

/*@brief 定长数据的puts接口,参数细节与原型一致(中断环境下不可调用)
 *       注意:这里的环形队列类型需要和type对齐
 *       建议使用以下宏对齐type以加速:
 *           #pragma pack(push, x)
 *           #pragma pack(pop)
 */
#define APP_SYS_RBUF_PUTS_FIXED(rbuf, data, type)   \
        app_sys_rbuf_puts(rbuf, (void *)data, sizeof(type) / app_sys_rbuf_type(rbuf))

/*@brief 非定长数据的gets接口,参数细节与原型一致(中断环境下不可调用)
 *       注意:data的缓冲区有一个最大值上限,双方约定即可
 */
static inline int32_t app_sys_rbuf_gets_unfixed(app_sys_rbuf_t *rbuf, void *data, uint64_t *length)
{
    if (app_sys_rbuf_item(rbuf) < sizeof(uint64_t) / rbuf->type)
        return -1;
    app_sys_rbuf_gets(rbuf, (void *)length, sizeof(uint64_t) / rbuf->type);
    app_sys_rbuf_gets(rbuf, (void *)data,  *length);
    return 0;
}

/*@brief 非定长数据的puts接口,参数细节与原型一致(中断环境下不可调用)
 *       注意:data的缓冲区有一个最大值上限,双方约定即可
 */
static inline int32_t app_sys_rbuf_puts_unfixed(app_sys_rbuf_t *rbuf, void *data, uint64_t *length)
{
    if (app_sys_rbuf_space(rbuf) < sizeof(uint64_t) / rbuf->type + length)
        return -1;
    app_sys_rbuf_puts(rbuf, (void *)length, sizeof(uint64_t) / rbuf->type);
    app_sys_rbuf_puts(rbuf, (void *)data,  *length);
    return 0;
}

#endif
