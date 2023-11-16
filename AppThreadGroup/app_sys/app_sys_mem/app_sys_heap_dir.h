#ifndef APP_SYS_HEAP_DIR_H
#define APP_SYS_HEAP_DIR_H

/* 金丝雀检查,不敏感,能力有限 */
#define APP_SYS_HEAP_DIR_CANARY     ((uintptr_t)0xA55A5AA5A55A5AA5)

typedef struct {
    uintptr_t canary;
    app_sys_list_dln_t dl_node;
    uintptr_t size;
    uintptr_t offset[0];
} app_sys_heap_dir_item_t;

typedef struct {
    app_mutex_t mutex;
    app_sys_list_dll_t dl_list_alloc;
    app_sys_list_dll_t dl_list_free;
    uintptr_t addr;     /* 内存地址 */
    uintptr_t size;     /* 内存大小(字节) */
    uintptr_t used;     /* 内存使用(字节) */
} app_sys_heap_dir_t;

/*@brief         初始化双端分配堆
 *@param[in,out] heap_dir 双端分配堆实例
 *@param[in]     addr     内存地址(平台字节对齐)
 *@param[in]     size     内存大小(字节)
 */
void app_sys_heap_dir_ready(app_sys_heap_dir_t *heap_dir, uintptr_t addr, uintptr_t size);

/*@brief         双端分配堆获取内存
 *@param[in,out] heap_dir 双端分配堆实例
 *@param[in]     size     内存大小
 *@param[in]     way      分配方向(true:正向;false:逆向)
 *@retval        内存地址
 */
void * app_sys_heap_dir_alloc(app_sys_heap_dir_t *heap_dir, uintptr_t size, bool way);

/*@brief         双端分配堆获取内存
 *@param[in,out] heap_dir 双端分配堆实例
 *@param[in]     size     内存大小
 *@param[in]     align    指定字节对齐(不小于平台字节对齐, 2的指数)
 *@param[in]     way      分配方向(true:正向;false:逆向)
 *@retval        内存地址
 */
void * app_sys_heap_dir_alloc_align(app_sys_heap_dir_t *heap_dir, uintptr_t size, uintptr_t align, bool way);

/*@brief         双端分配堆归还内存
 *@param[in,out] heap_dir 双端分配堆实例
 *@param[in]     pointer  内存地址
 */
void app_sys_heap_dir_free(app_sys_heap_dir_t *heap_dir, void *pointer);

/*@brief         双端分配堆获取内存
 *@param[in,out] heap_dir 双端分配堆实例
 */
bool app_sys_heap_dir_check(app_sys_heap_dir_t *heap_dir);

#endif
