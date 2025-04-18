#ifndef APP_SYS_MEM_OLSF_H
#define APP_SYS_MEM_OLSF_H

/*
 *(动态内存管理)一级隔离策略分配堆(One Level Segregated Fit)
 *该实现参考于zephyr\lib\os\heap
 *该分配实现主要以去除首地址偏移
 *将数据按小的单元切分成块
 *并直接使用类似数组索引的方式去搜索数据块
 *以进一步压缩内存块维护头部的额外开销
 *使用目标:泛用(建议分堆使用)
 *chunk描述:
 *    [field(chunk align)][buffer]
 *初始内存布局:
 *chunk_0:
 *    [field(chunk align)][mem_olsf]
 *chunk_1:
 *    [field(chunk align)][buffer]
 *chunk_2:
 *    [field(chunk align)][0]
 *size: 包括field(chunk align)的大小
 */

/* chunk field type: */
typedef enum {
    app_sys_mem_olsf_size_used = 0,    /* 压缩:最后一位作为Chunk使用标志位 */
    app_sys_mem_olsf_free_prev = 1,
    app_sys_mem_olsf_free_next = 2,
    app_sys_mem_olsf_size_near = 3,    /* 因为数组特性,这里默认指size left */
    app_sys_mem_olsf_type_num,
} app_sys_mem_olsf_type_t;

/* chunk field: */
#define app_sys_mem_olsf_field_declare(bits)           \
typedef struct {                                       \
    uint##bits##_t field[app_sys_mem_olsf_type_num];   \
} app_sys_mem_olsf_field_##bits##_t;                   \

app_sys_mem_olsf_field_declare(8);
app_sys_mem_olsf_field_declare(16);
app_sys_mem_olsf_field_declare(32);
app_sys_mem_olsf_field_declare(64);

typedef struct {
    uintptr_t addr_base;
    uintptr_t size_base;
    uintptr_t addr_hdr;
    uintptr_t addr_end;
    uintptr_t size_old;
    uintptr_t size_new;
    uintptr_t size_used;
    uintptr_t chunk_hdr;
    uintptr_t chunk_end;
    uintptr_t number;       /* 单元块数量 */
    uintptr_t bitmap;       /* 块空闲链表标记位图 */
    uintptr_t bucket[0];    /* 块空闲链表(以2的指数次方分割) */
} app_sys_mem_olsf_t;

/* chunk的单元类型固定为uintptr_t */
#define app_sys_mem_olsf_loops     3

/*@brief 一级隔离策略分配堆内存使用(最大片段)
 *@param mem_olsf 一级隔离策略分配堆实例
 *@retval 内存大小
 */
uintptr_t app_sys_mem_olsf_frag(app_sys_mem_olsf_t *mem_olsf);

/*@brief 一级隔离策略分配堆内存使用
 *@param mem_olsf 一级隔离策略分配堆实例
 *@retval 内存大小
 */
uintptr_t app_sys_mem_olsf_used(app_sys_mem_olsf_t *mem_olsf);

/*@brief 一级隔离策略分配堆计算指定内存
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param pointer  内存地址
 *@retval 内存大小
 */
uintptr_t app_sys_mem_olsf_size(app_sys_mem_olsf_t *mem_olsf, void *pointer);

/*@brief 一级隔离策略分配堆计算指定内存所属
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param pointer  内存地址
 *@retval 包含与否
 */
bool app_sys_mem_olsf_inside(app_sys_mem_olsf_t *mem_olsf, void *pointer);

/*@brief 一级隔离策略分配堆释放内存
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param pointer  内存地址
 */
void app_sys_mem_olsf_free(app_sys_mem_olsf_t *mem_olsf, void *pointer);

/*@brief 一级隔离策略分配堆申请内存
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param size     字节大小
 *@retval 内存地址
 */
void * app_sys_mem_olsf_alloc(app_sys_mem_olsf_t *mem_olsf, uintptr_t size);

/*@brief 一级隔离策略分配堆申请内存
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param size     字节大小
 *@param align    指定字节对齐(sizeof(uintptr_t) * 2^k)
 *@retval 内存地址
 */
void * app_sys_mem_olsf_alloc_align(app_sys_mem_olsf_t *mem_olsf, uintptr_t size, uintptr_t align);

/*@brief 一级隔离策略分配堆实例初始化
 *@param addr 内存地址
 *@param size 字节大小
 *@retval 返回分配器(分配器在内存头部)
 */
app_sys_mem_olsf_t * app_sys_mem_olsf_ready(void *addr, uintptr_t size);

/*@brief 一级隔离策略分配堆内存布局使用
 *@param mem_olsf 一级隔离策略分配堆实例
 */
void app_sys_mem_olsf_check(app_sys_mem_olsf_t *mem_olsf);

/*@brief 一级隔离策略分配堆内存遍历检查
 *@param mem_olsf 一级隔离策略分配堆实例
 *@param invoke   回调实例
 */
void app_sys_mem_olsf_walk(app_sys_mem_olsf_t *mem_olsf, void (*invoke)(void *pointer, bool used));

#endif
