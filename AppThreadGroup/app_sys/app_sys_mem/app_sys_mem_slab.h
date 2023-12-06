#ifndef APP_SYS_MEM_SLAB_H
#define APP_SYS_MEM_SLAB_H

typedef struct {
    uintptr_t addr;     /* 内存地址 */
    uintptr_t size;     /* 内存大小(字节) */
    uint8_t  *blk_list; /* 空闲块链表集 */
    uint32_t  blk_size; /* 块单元大小 */
    uint32_t  blk_used; /* 块使用数量 */
    uint32_t  blk_num;  /* 块总计数量 */
} app_sys_mem_slab_t;

typedef struct {
    app_sys_list_dln_t dl_node;
    uint8_t *mem_s;     /* 管理区起始地址 */
    uint8_t *mem_e;     /* 管理区结束地址 */
    uint8_t *blk_list;  /* 空闲块链表集 */
    uint32_t blk_size;  /* 块单元大小 */
    uint32_t blk_used;  /* 块使用数量 */
    uint32_t blk_num;   /* 块总计数量 */
} app_sys_mem_slab_item_t;

typedef struct {
    app_sys_list_dll_t dl_list;
    app_mutex_t mutex;
    uint32_t blk_size;
    uint32_t blk_num;
    uint32_t debounce;
} app_sys_mem_slab_set_t;

/*@brief 初始化slab分配器
 *@param mem_slab slab分配器实例
 *@param addr     内存地址
 *@param size     内存大小(字节)
 *@param blk_size 块单元大小
 */
void app_sys_mem_slab_ready(app_sys_mem_slab_t *mem_slab, uintptr_t addr, uintptr_t size, uintptr_t blk_size);

/*@brief 向slab分配器获取一个块
 *@param mem_slab slab分配器实例
 *@retval 新的块
 */
void * app_sys_mem_slab_alloc(app_sys_mem_slab_t *mem_slab);

/*@brief 向slab分配器归还一个块
 *@param ptr 旧的块
 */
void app_sys_mem_slab_free(app_sys_mem_slab_t *mem_slab, void *ptr);

/*@brief 初始化slab分配器
 *@param mem_slab_set slab分配器实例
 *@param size         分配单元块大小
 *@param num          分配单元块数量
 *@param debounce     分配单元块数量抖动
 */
void app_sys_mem_slab_set_ready(app_sys_mem_slab_set_t *mem_slab_set, uintptr_t size, uint32_t num, uint32_t debounce);

/*@brief 向slab分配器获取一个块
 *@param mem_slab_set slab分配器实例
 *@retval 新的块
 */
void * app_sys_mem_slab_set_alloc(app_sys_mem_slab_set_t *mem_slab_set);

/*@brief 向slab分配器归还一个块
 *@param mem_slab_set slab分配器实例
 *@param ptr 旧的块
 */
void app_sys_mem_slab_set_free(app_sys_mem_slab_set_t *mem_slab_set, void *ptr);

#endif
