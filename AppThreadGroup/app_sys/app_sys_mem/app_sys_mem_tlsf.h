#ifndef APP_SYS_MEM_TLSF_H
#define APP_SYS_MEM_TLSF_H

/*
 *(动态内存管理)二级隔离策略分配堆(Two Level Segregated Fit)
 *该实现参考于lvgl\src\misc\lv_tlsf
 *使用目标:泛用(建议分堆使用)
 */

typedef enum {
    /* 字节对齐(仅支持32或64位) */
    app_sys_mem_tlsf_byte = (uintptr_t)(sizeof(uintptr_t) == 8 ? 3 : sizeof(uintptr_t) == 4 ? 2 : 0),
    /* 一级块长度限制(Log2,修改它以支持更大的块尺寸) */
    app_sys_mem_tlsf_fl_index = (uintptr_t)(sizeof(uintptr_t) == 8 ? 32 : sizeof(uintptr_t) == 4 ? 30 : 0),
    /* 二级块长度限制(Log2,线性细分值,4~5是典型值) */
    app_sys_mem_tlsf_sl_index = (uintptr_t)((uintptr_t)5),
    /* 一级块偏移量 */
    app_sys_mem_tlsf_fl_shift = (uintptr_t)(app_sys_mem_tlsf_sl_index + app_sys_mem_tlsf_byte),
    /* 一级块数量 */
    app_sys_mem_tlsf_fl_count = (uintptr_t)(app_sys_mem_tlsf_fl_index - app_sys_mem_tlsf_fl_shift + 1),
    /* 二级块数量 */
    app_sys_mem_tlsf_sl_count = (uintptr_t)((uintptr_t)1 << app_sys_mem_tlsf_sl_index),
    /* 一级块大小(最小) */
    app_sys_mem_tlsf_fl_limit = (uintptr_t)((uintptr_t)1 << app_sys_mem_tlsf_fl_shift),
    /*  */
} app_sys_mem_tlsf_status_t;

typedef struct app_sys_mem_tlsf_block {
    /* 该字段前一块空闲时有效 */
    /* 且该字段有效时,嵌入到前一个块的末尾 */
    struct app_sys_mem_tlsf_block *prev_tail;
    /* 包括本身以及后面跟随的整体块的大小(低位二次复用) */
    /* 次低位:前一块是忙碌还是空闲态 */
    /* 最低位:当前块是忙碌还是空闲态 */
    uintptr_t size_free;
    /* 下一块与前一块 */
    struct app_sys_mem_tlsf_block *prev;
    struct app_sys_mem_tlsf_block *next;
    /*  */
} app_sys_mem_tlsf_block_t;

typedef struct {
    /* 空列表指向这个块以表示它们是空闲的 */
    app_sys_mem_tlsf_block_t block_none;
    /* 空闲列表使用的位图 */
    uintptr_t bitmap_fl;
    uintptr_t bitmap_sl[app_sys_mem_tlsf_fl_count];
    /* 空闲列表 */
    app_sys_mem_tlsf_block_t *block[app_sys_mem_tlsf_fl_count][app_sys_mem_tlsf_sl_count];
    /*  */
} app_sys_mem_tlsf_control_t;

typedef app_sys_mem_tlsf_control_t app_sys_mem_tlsf_t;

/*@brief     二级隔离策略分配堆释放内存
 *@param[in] mem_tlsf 二级隔离策略分配堆实例
 *@param[in] pointer  内存地址
 */
void app_sys_mem_tlsf_free(app_sys_mem_tlsf_t *mem_tlsf, void *pointer);

/*@brief     二级隔离策略分配堆申请内存
 *@param[in] mem_tlsf 二级隔离策略分配堆实例
 *@param[in] size     字节大小
 *@retval    内存地址
 */
void * app_sys_mem_tlsf_alloc(app_sys_mem_tlsf_t *mem_tlsf, uintptr_t size);

/*@brief     二级隔离策略分配堆申请内存
 *@param[in] mem_olsf 一级隔离策略分配堆实例
 *@param[in] size     字节大小
 *@param[in] align    指定字节对齐(不小于平台字节对齐, 2的指数)
 *@retval    内存地址
 */
void * app_sys_mem_tlsf_alloc_align(app_sys_mem_tlsf_t *mem_tlsf, uintptr_t size, uintptr_t align);

/*@brief 二级隔离策略分配堆重获取内存
 *       realloc语义不提供,应该规避这种情况的使用
 *       它存在造成内存峰值过大的情况,使用应该规避掉对这种逻辑的产生
 */

/*@brief     二级隔离策略分配堆实例初始化
 *@param[in] addr     内存地址
 *@param[in] size     字节大小
 *@retval    返回分配器(分配器在内存头部)
 */
app_sys_mem_tlsf_t * app_sys_mem_tlsf_ready(void *addr, uintptr_t size);

/*@brief     二级隔离策略分配堆内存布局使用
 *@param[in] mem_olsf 二级隔离策略分配堆实例
 */
void app_sys_mem_tlsf_check(app_sys_mem_tlsf_t *mem_tlsf);

#endif
