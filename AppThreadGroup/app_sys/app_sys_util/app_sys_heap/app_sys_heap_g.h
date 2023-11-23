#ifndef APP_SYS_HEAP_G_H
#define APP_SYS_HEAP_G_H

/* 比较函数:严格大于或者严格小于返回true */
typedef bool (*app_sys_heap_g_fc_t)(void *node1, void *node2);

typedef struct {
    void   **arr;
    uint32_t len;
    uint32_t num;
    app_sys_heap_g_fc_t compare;
} app_sys_heap_g_t;

/*@brief     重置一个堆
 *@param[in] heap    堆实例
 *@param[in] arr     堆空间
 *@param[in] len     堆空间大小
 *@param[in] compare 比较语义
 */
void app_sys_heap_g_reset(app_sys_heap_g_t   *heap, void **arr, uint32_t len,
                          app_sys_heap_g_fc_t compare);

/*@brief     节点入堆
 *@param[in] heap 堆实例
 *@param[in] ptr  堆节点地址
 *@retval    成功或失败
 */
bool app_sys_heap_g_push(app_sys_heap_g_t *heap, void *ptr);

/*@brief      节点出堆
 *@param[in]  heap 堆实例
 *@param[out] ptr  堆节点地址
 *@retval     成功或失败
 */
bool app_sys_heap_g_pop(app_sys_heap_g_t *heap, void **ptr);

/*
 *大小根堆容器复用:优先队列(Priority Queue)
 *大小根堆容器本质上就是优先队列,无需额外扩展复用
 */

#endif
