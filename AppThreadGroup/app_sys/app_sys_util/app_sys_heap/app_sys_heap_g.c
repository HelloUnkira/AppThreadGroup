/*实现目标:
 *    泛型大根堆和小根堆
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

//Root:   ((Index = 0))
//child_l:   ((Index * 2 + 1))
//child_r:  ((Index * 2 + 2))
//parent: ((Index - 1) / 2)

/*@brief     重置一个堆
 *@param[in] heap    堆实例
 *@param[in] arr     堆空间
 *@param[in] len     堆空间大小
 *@param[in] compare 比较语义
 */
void app_sys_heap_g_reset(app_sys_heap_g_t   *heap, void **arr, uint32_t len,
                          app_sys_heap_g_fc_t compare)
{
    heap->arr = arr;
    heap->len = len;
    heap->num = 0;
    heap->compare = compare;
}

/*@brief     节点入堆
 *@param[in] heap 堆实例
 *@param[in] ptr  堆节点地址
 *@retval    成功或失败
 */
bool app_sys_heap_g_push(app_sys_heap_g_t *heap, void *ptr)
{
    if (heap->num == heap->len)
        return false;
    /* 节点入堆 */
    heap->arr[heap->num++] = ptr;
    /*  */
    uint32_t node   = heap->num - 1;
    uint32_t parent = 0;
    /* 逆向调整至根 */
    while (node != 0) {
        /* 计算父节点 */
        parent = (node - 1) / 2;
        /* 比较子节点与父节点,无需交换退出 */
        if (heap->compare(heap->arr[parent], heap->arr[node]) == true)
            return true;
        /* 比较子节点与父节点,交换 */
        void *tmp1 = heap->arr[parent];
        void *tmp2 = heap->arr[node];
        heap->arr[parent] = tmp2;
        heap->arr[node]   = tmp1;
        /* 指向上一层 */
        node = parent;
    }
    return true;
}

/*@brief      节点出堆
 *@param[in]  heap 堆实例
 *@param[out] ptr  堆节点地址
 *@retval     成功或失败
 */
bool app_sys_heap_g_pop(app_sys_heap_g_t *heap, void **ptr)
{
    if (heap->num == 0)
        return false;
    /* 节点出堆 */
    *ptr = heap->arr[0];
    /* 最后一个节点迁移到根 */
    heap->arr[0] = heap->arr[--heap->num];
    /*  */
    uint32_t node    = 0;
    uint32_t parent  = 0;
    uint32_t child_l = 0;
    uint32_t child_r = 0;
    /* 顺向调整至叶子 */
    while (parent < heap->num) {
        /* 计算左右子节点 */
        child_l = parent * 2 + 1;
        child_r = parent * 2 + 2;
        /* 如果左孩子溢出,结束 */
        if (child_l >= heap->num)
            return true;
        /* 如果右孩子溢出 */
        if (child_r >= heap->num) {
            node = child_l;
            /* 比较子节点与父节点,交换 */
            if (heap->compare(heap->arr[parent], heap->arr[node]) == false) {
                void *tmp1 = heap->arr[parent];
                void *tmp2 = heap->arr[node];
                heap->arr[parent] = tmp2;
                heap->arr[node]   = tmp1;
            }
            return true;
        }
        /* 如果都没溢出 */
        node = (heap->compare(heap->arr[child_l], heap->arr[child_r]) == true) ? child_l : child_r;
        /* 比较子节点与父节点,无需交换退出 */
        if (heap->compare(heap->arr[parent], heap->arr[node]) == true)
            return true;
        /* 比较子节点与父节点,交换 */
        void *tmp1 = heap->arr[parent];
        void *tmp2 = heap->arr[node];
        heap->arr[parent] = tmp2;
        heap->arr[node]   = tmp1;
        /* 指向下一层 */
        parent = node;
    }
}
