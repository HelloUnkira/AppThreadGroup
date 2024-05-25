/*实现目标:
 *    heap分配器(双向块式分配器)
 */

#define APP_SYS_LOG_RECORD_LIMIT    1
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 双端分配堆节点按地址排序入队列比较回调
 */
static bool app_sys_mem_dir_sort(app_sys_queue_dlpn_t * node1, app_sys_queue_dlpn_t * node2)
{
    app_sys_mem_dir_item_t *mem_dir_item_1 = app_sys_own_ofs(app_sys_mem_dir_item_t, dl_node, node1);
    app_sys_mem_dir_item_t *mem_dir_item_2 = app_sys_own_ofs(app_sys_mem_dir_item_t, dl_node, node2);
    uintptr_t addr1 = (uintptr_t)mem_dir_item_1->offset;
    uintptr_t addr2 = (uintptr_t)mem_dir_item_2->offset;
    return addr1 <= addr2;
}

/*@brief 初始化双端分配堆
 *@param mem_dir 双端分配堆实例
 *@param addr    内存地址(平台字节对齐)
 *@param size    内存大小(字节)
 */
void app_sys_mem_dir_ready(app_sys_mem_dir_t *mem_dir, uintptr_t addr, uintptr_t size)
{
    APP_SYS_ASSERT(size > sizeof(app_sys_mem_dir_item_t));
    APP_SYS_ASSERT(app_sys_align_check((void *)addr, sizeof(uintptr_t)));
    app_mutex_process(&mem_dir->mutex, app_mutex_static);
    app_sys_list_dll_reset(&mem_dir->dl_list_alloc);
    app_sys_list_dll_reset(&mem_dir->dl_list_free);
    mem_dir->addr = addr;
    mem_dir->size = size;
    mem_dir->used = 0;
    /* 初始化分配器 */
    app_sys_mem_dir_item_t *mem_dir_item = (void *)mem_dir->addr;
    mem_dir_item->size = mem_dir->size;
    mem_dir_item->canary = APP_SYS_MEM_DIR_CANARY;
    app_sys_list_dln_reset(&mem_dir_item->dl_node);
    app_sys_list_dll_ainsert(&mem_dir->dl_list_free, NULL, &mem_dir_item->dl_node);
}

/*@brief 双端分配堆获取内存
 *@param mem_dir 双端分配堆实例
 *@param size    内存大小
 *@param way     分配方向(true:正向;false:逆向)
 *@retval 内存地址
 */
static void * app_sys_mem_dir_alloc_raw(app_sys_mem_dir_t *mem_dir, uintptr_t size, bool way)
{
    void *pointer = NULL;
    app_mutex_process(&mem_dir->mutex, app_mutex_take);
    /* 正向分配,逆向分配: */
    if (way) {
        app_sys_mem_dir_item_t *mem_dir_item = NULL;
        /* 正向递归找到第一个块,并且分配 */
        app_sys_list_dll_btra(&mem_dir->dl_list_free, item) {
            mem_dir_item = app_sys_own_ofs(app_sys_mem_dir_item_t, dl_node, item);
            if (mem_dir_item->size > sizeof(app_sys_mem_dir_item_t) + size)
                break;
            mem_dir_item = NULL;
        }
        if (mem_dir_item != NULL) {
            /* 如果节点适合分裂,将节点分开成俩个,后一个块归放入分配堆 */
            if (mem_dir_item->size - sizeof(app_sys_mem_dir_item_t) - size > sizeof(app_sys_mem_dir_item_t)) {
                app_sys_mem_dir_item_t *mem_dir_item_near = NULL;
                mem_dir_item_near = (void *)((uintptr_t)mem_dir_item + mem_dir_item->size - sizeof(app_sys_mem_dir_item_t) - size);
                mem_dir_item_near->size = sizeof(app_sys_mem_dir_item_t) + size;
                mem_dir_item_near->canary = APP_SYS_MEM_DIR_CANARY;
                mem_dir_item->size -= mem_dir_item_near->size;
                app_sys_list_dln_reset(&mem_dir_item_near->dl_node);
                app_sys_queue_dlpq_enqueue(&mem_dir->dl_list_alloc, &mem_dir_item_near->dl_node, app_sys_mem_dir_sort);
                mem_dir->used += mem_dir_item_near->size;
                pointer = mem_dir_item_near->offset;
            } else {
                app_sys_list_dll_remove(&mem_dir->dl_list_free, &mem_dir_item->dl_node);
                app_sys_queue_dlpq_enqueue(&mem_dir->dl_list_alloc, &mem_dir_item->dl_node, app_sys_mem_dir_sort);
                mem_dir->used += mem_dir_item->size;
                pointer = mem_dir_item->offset;
            }
        }
    } else {
        app_sys_mem_dir_item_t *mem_dir_item = NULL;
        /* 逆向递归找到第一个块,并且分配 */
        app_sys_list_dll_ftra(&mem_dir->dl_list_free, item) {
            mem_dir_item = app_sys_own_ofs(app_sys_mem_dir_item_t, dl_node, item);
            if (mem_dir_item->size > sizeof(app_sys_mem_dir_item_t) + size)
                break;
            mem_dir_item = NULL;
        }
        if (mem_dir_item != NULL) {
            /* 如果节点适合分裂,将节点分开成俩个,前一个块归放入分配堆,后一个块归放入空闲堆 */
            if (mem_dir_item->size - sizeof(app_sys_mem_dir_item_t) - size > sizeof(app_sys_mem_dir_item_t)) {
                app_sys_mem_dir_item_t *mem_dir_item_near = NULL;
                mem_dir_item_near = (void *)((uintptr_t)mem_dir_item + sizeof(app_sys_mem_dir_item_t) + size);
                mem_dir_item_near->size = mem_dir_item->size - sizeof(app_sys_mem_dir_item_t) - size;
                mem_dir_item_near->canary = APP_SYS_MEM_DIR_CANARY;
                mem_dir_item->size = sizeof(app_sys_mem_dir_item_t) + size;
                app_sys_list_dln_reset(&mem_dir_item_near->dl_node);
                app_sys_list_dll_remove(&mem_dir->dl_list_free, &mem_dir_item->dl_node);
                app_sys_queue_dlpq_enqueue(&mem_dir->dl_list_alloc, &mem_dir_item->dl_node, app_sys_mem_dir_sort);
                app_sys_queue_dlpq_enqueue(&mem_dir->dl_list_free, &mem_dir_item_near->dl_node, app_sys_mem_dir_sort);
                mem_dir->used += mem_dir_item->size;
                pointer = mem_dir_item->offset;
            } else {
                app_sys_list_dll_remove(&mem_dir->dl_list_free, &mem_dir_item->dl_node);
                app_sys_queue_dlpq_enqueue(&mem_dir->dl_list_alloc, &mem_dir_item->dl_node, app_sys_mem_dir_sort);
                mem_dir->used += mem_dir_item->size;
                pointer = mem_dir_item->offset;
            }
        }
    }
    /*  */
    app_mutex_process(&mem_dir->mutex, app_mutex_give);
    return pointer;
}

/*@brief 双端分配堆归还内存
 *@param mem_dir 双端分配堆实例
 *@param pointer 内存地址
 */
static void app_sys_mem_dir_free_raw(app_sys_mem_dir_t *mem_dir, void *pointer)
{
    if (pointer == NULL)
        return;
    if ((uintptr_t)pointer < mem_dir->addr || (uintptr_t)pointer > mem_dir->addr + mem_dir->size) {
        APP_SYS_LOG_WARN("ont in this heap");
        return;
    }
    app_mutex_process(&mem_dir->mutex, app_mutex_take);
    app_sys_mem_dir_item_t *mem_dir_item = (uintptr_t)pointer - sizeof(app_sys_mem_dir_item_t);
    /* canary检查 */
    bool canary_check = true;
    if (mem_dir_item->canary != APP_SYS_MEM_DIR_CANARY) {
        APP_SYS_LOG_ERROR("block %p canary check fail, heap is broken", pointer);
        canary_check = false;
    }
    /* 俩次释放检查 */
    bool single_free = true;
    app_sys_list_dll_btra(&mem_dir->dl_list_free, item) {
        app_sys_mem_dir_item_t *mem_dir_item_curr = app_sys_own_ofs(app_sys_mem_dir_item_t, dl_node, item);
        if (mem_dir_item_curr == mem_dir_item) {
            APP_SYS_LOG_ERROR("double free");
            single_free = false;
            break;
        }
    }
    if (canary_check && single_free) {
        /* 先移除分配堆,然后加入空闲堆 */
        app_sys_list_dll_remove(&mem_dir->dl_list_alloc, &mem_dir_item->dl_node);
        app_sys_queue_dlpq_enqueue(&mem_dir->dl_list_free, &mem_dir_item->dl_node, app_sys_mem_dir_sort);
        mem_dir->used -= mem_dir_item->size;
        /* 左右节点检查,尝试合并 */
        app_sys_list_dln_t *dl_node_next = app_sys_list_dln_next(&mem_dir_item->dl_node);
        app_sys_mem_dir_item_t *mem_dir_item_next = app_sys_own_ofs(app_sys_mem_dir_item_t, dl_node, dl_node_next);
        app_sys_list_dln_t *dl_node_prev = app_sys_list_dln_prev(&mem_dir_item->dl_node);
        app_sys_mem_dir_item_t *mem_dir_item_prev = app_sys_own_ofs(app_sys_mem_dir_item_t, dl_node, dl_node_prev);
        /* 先尝试和右兄弟合并 */
        if (dl_node_next != NULL && mem_dir_item_next != NULL) {
            APP_SYS_ASSERT((uintptr_t)mem_dir_item + mem_dir_item->size <= (uintptr_t)mem_dir_item_next);
            if ((uintptr_t)mem_dir_item + mem_dir_item->size == (uintptr_t)mem_dir_item_next) {
                app_sys_list_dll_remove(&mem_dir->dl_list_free, &mem_dir_item_next->dl_node);
                mem_dir_item->size += mem_dir_item_next->size;
            }
        }
        /* 后尝试和左兄弟合并 */
        if (dl_node_prev != NULL && mem_dir_item_prev != NULL) {
            APP_SYS_ASSERT((uintptr_t)mem_dir_item_prev + mem_dir_item_prev->size <= (uintptr_t)mem_dir_item);
            if ((uintptr_t)mem_dir_item_prev + mem_dir_item_prev->size == (uintptr_t)mem_dir_item) {
                app_sys_list_dll_remove(&mem_dir->dl_list_free, &mem_dir_item->dl_node);
                mem_dir_item_prev->size += mem_dir_item->size;
            }
        }
        /*  */
    }
    /*  */
    app_mutex_process(&mem_dir->mutex, app_mutex_give);
}

/*@brief 双端分配堆获取内存
 *@param mem_dir 双端分配堆实例
 *@param size    内存大小
 *@param align   指定字节对齐(不小于平台字节对齐, 2的指数)
 *@param way     分配方向(true:正向;false:逆向)
 *@retval 内存地址
 */
void * app_sys_mem_dir_alloc_align(app_sys_mem_dir_t *mem_dir, uintptr_t size, uintptr_t align, bool way)
{
    if (!app_sys_pow2_check(align) || align < sizeof(uintptr_t)) {
         APP_SYS_LOG_WARN("align size fail:%d", align);
         return NULL;
    }
    void *pointer_raw = NULL;
    void *pointer_ofs = NULL;
    size += align * 2;
    if ((pointer_raw = app_sys_mem_dir_alloc_raw(mem_dir, size, way)) == NULL)
         return NULL;
    /*  */
    pointer_ofs = (void *)((uintptr_t)app_sys_align_high(pointer_raw, align) + align);
    *(uintptr_t *)((uintptr_t)pointer_ofs - sizeof(uintptr_t)) = (uintptr_t)pointer_ofs - (uintptr_t)pointer_raw;
    APP_SYS_LOG_DEBUG("pointer_ofs:%p, pointer_raw:%p, align:%d",
                       pointer_ofs,    pointer_raw,   (uintptr_t)pointer_ofs - (uintptr_t)pointer_raw);
    return pointer_ofs;
}

/*@brief 双端分配堆获取内存
 *@param mem_dir 双端分配堆实例
 *@param size    内存大小
 *@param way     分配方向(true:正向;false:逆向)
 *@retval 内存地址
 */
void * app_sys_mem_dir_alloc(app_sys_mem_dir_t *mem_dir, uintptr_t size, bool way)
{
    return app_sys_mem_dir_alloc_align(mem_dir, size, sizeof(uintptr_t), way);
}

/*@brief 双端分配堆归还内存
 *@param mem_dir 双端分配堆实例
 *@param pointer 内存地址
 */
void app_sys_mem_dir_free(app_sys_mem_dir_t *mem_dir, void *pointer)
{
    if (pointer == NULL)
        return;
    pointer = (void *)((uintptr_t)pointer - *(uintptr_t *)((uintptr_t)pointer - sizeof(uintptr_t)));
    APP_SYS_LOG_DEBUG("pointer:%p", pointer);
    app_sys_mem_dir_free_raw(mem_dir, pointer);
}

/*@brief 双端分配堆获取内存
 *@param mem_dir 双端分配堆实例
 */
bool app_sys_mem_dir_check(app_sys_mem_dir_t *mem_dir)
{
    bool mem_dir_is_valid = true;
    app_mutex_process(&mem_dir->mutex, app_mutex_take);
    APP_SYS_LOG_INFO("heap size:%d", mem_dir->size);
    APP_SYS_LOG_INFO("heap used:%d", mem_dir->used);
    /* 分配堆 */
    APP_SYS_LOG_INFO("heap list alloc:");
    app_sys_list_dll_btra(&mem_dir->dl_list_alloc, item) {
        app_sys_mem_dir_item_t *mem_dir_item = app_sys_own_ofs(app_sys_mem_dir_item_t, dl_node, item);
        if (mem_dir_item->canary != APP_SYS_MEM_DIR_CANARY) {
            APP_SYS_LOG_ERROR("block %p canary check fail, heap is broken", mem_dir_item->offset);
            mem_dir_is_valid = false;
            break;
        }
        APP_SYS_LOG_INFO("addr:%p total size:%d", mem_dir_item->offset, mem_dir_item->size);
    }
    /* 空闲堆 */
    APP_SYS_LOG_INFO("heap list free:");
    app_sys_list_dll_btra(&mem_dir->dl_list_free, item) {
        app_sys_mem_dir_item_t *mem_dir_item = app_sys_own_ofs(app_sys_mem_dir_item_t, dl_node, item);
        if (mem_dir_item->canary != APP_SYS_MEM_DIR_CANARY) {
            APP_SYS_LOG_ERROR("block %p canary check fail, heap is broken", mem_dir_item->offset);
            mem_dir_is_valid = false;
            break;
        }
        APP_SYS_LOG_INFO("addr:%p total size:%d", mem_dir_item->offset, mem_dir_item->size);
    }
    /*  */
    app_mutex_process(&mem_dir->mutex, app_mutex_give);
    return mem_dir_is_valid;
}
