#ifndef APP_SYS_SLAB_H
#define APP_SYS_SLAB_H

typedef struct {
    app_sys_list_dln_t dl_node;
    uint8_t *mem_s;     /* 管理区起始地址 */
    uint8_t *mem_e;     /* 管理区结束地址 */
    uint8_t *blk_list;  /* 空闲块链表集 */
    uint32_t blk_size;  /* 块单元大小 */
    uint32_t blk_used;  /* 块使用数量 */
    uint32_t blk_num;   /* 块总计数量 */
} app_sys_slab_item_t;

typedef struct {
    app_sys_list_dll_t dl_list;
    app_mutex_t mutex;
    uint32_t blk_size;
    uint32_t blk_num;
    uint32_t debounce;
} app_sys_slab_t;

/*@brief     初始化slab分配器
 *@param[in] slab     slab分配器实例
 *@param[in] size     分配单元块大小
 *@param[in] num      分配单元块数量
 *@param[in] debounce 分配单元块数量抖动
 */
void app_sys_slab_ready(app_sys_slab_t *slab, uintptr_t size, uint32_t num, uint32_t debounce);

/*@brief     向slab分配器获取一个块
 *@param[in] slab slab分配器实例
 *@retval    新的块
 */
void * app_sys_slab_alloc(app_sys_slab_t *slab);

/*@brief     向slab分配器归还一个块
 *@param[in] ptr 旧的块
 */
void app_sys_slab_free(app_sys_slab_t *slab, void *ptr);

#endif
