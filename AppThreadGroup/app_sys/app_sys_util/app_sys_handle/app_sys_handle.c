/*实现目标:
 *    泛用系统句柄模组
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/* 句柄计算规则:
 * idx1 = handle / unit->len;
 * idx2 = handle % unit->len;
 * handle = idx1 * unit->len + idx2;
 */

/* 句柄管理集合 */
static app_sys_handle_t app_sys_handle = {0};

/*@brief     释放一个句柄
 *@param[in] handle 句柄
 */
void app_sys_handle_give(uint32_t handle)
{
    int32_t  idx  = 0;
    uint32_t idx1 = handle / APP_SYS_HANDLE_UINT_LENGTH;
    uint32_t idx2 = handle % APP_SYS_HANDLE_UINT_LENGTH;
    /* 1.这是一个非法句柄 */
    if (idx1 >= app_sys_handle.len || idx2 >= app_sys_handle.unit[idx1].len)
        return;
    /* 2.释放这个合法的句柄 */
    app_sys_handle.unit[idx1].src[idx2] = NULL;
    app_sys_handle.unit[idx1].num++;
    /* 3.句柄管理单元非空时,不释放它 */
    if (app_sys_handle.unit[idx1].len != app_sys_handle.unit[idx1].num)
        return;
    /* 4.释放这个句柄管理单元 */
    app_mem_free(app_sys_handle.unit[idx1].src);
    /* 5.更新管理器的情景 */
    app_sys_handle.unit[idx1].src = NULL;
    app_sys_handle.num++;
    /* 6.如果句柄管理单元空闲过多,尝试压缩 */
    while (app_sys_handle.num >= APP_SYS_HANDLE_UINT_FACTOR) {
        /* 1.一级寻址依赖下标,所以只能逆向检查,中间空余部不可压缩 */
        for (idx = app_sys_handle.len - 1; idx >= 0; idx--)
            if (app_sys_handle.unit[idx].src != NULL)
                break;
        /* 2.如果句柄管理单元全空闲,释放本身 */
        if (idx < 0) {
            /* 释放这个句柄管理单元集合 */
            app_mem_free(app_sys_handle.unit);
            app_sys_handle.num = 0;
            app_sys_handle.len = 0;
            app_sys_handle.unit   = NULL;
            break;
        }
        /* 3.如果最后的几个连续为空闲态单元未达到伸缩标准时,放弃 */
        if (idx + APP_SYS_HANDLE_UINT_FACTOR >= app_sys_handle.len)
            break;
        /* 4.如果最后的几个连续为空闲态单元达到伸缩标准时,压缩它 */
        if (idx + APP_SYS_HANDLE_UINT_FACTOR <  app_sys_handle.len) {
            /* 1.生成一个新的资源管理单元集合 */
            uint32_t num = app_sys_handle.num - APP_SYS_HANDLE_UINT_FACTOR;
            uint32_t len = app_sys_handle.len - APP_SYS_HANDLE_UINT_FACTOR;
            app_sys_handle_unit_t *unit = app_mem_alloc(sizeof(app_sys_handle_unit_t) * len);
            /* 2.拷贝原生的资源管理单元集合 */
            for (idx = 0; idx < len; idx++)
                unit[idx] = app_sys_handle.unit[idx];
            /* 3.释放原生的资源管理单元集合 */
            app_mem_free(app_sys_handle.unit);
            /* 4.设置新生成的资源管理单元集合 */
            app_sys_handle.num  = num;
            app_sys_handle.len  = len;
            app_sys_handle.unit = unit;
        }
    }
}

/*@brief      获取一个句柄
 *@param[out] handle 句柄
 */
void app_sys_handle_take(uint32_t *handle)
{
    *handle = APP_SYS_HANDLE_INVALID;
    
    uint32_t idx1 = 0;
    uint32_t idx2 = 0;
    /* 1.遍历资源管理单元,查找一个有空闲句柄的单元 */
    for (idx1 = 0; idx1 < app_sys_handle.len; idx1++) {
        if (app_sys_handle.unit[idx1].src == NULL)
            break;
        if (app_sys_handle.unit[idx1].num > 0)
            break;
    }
    /* 2.如果资源管理单元不足,扩张它 */
    if (idx1 == app_sys_handle.len) {
        /* 生成一个新的资源管理单元集合 */
        uint32_t num = app_sys_handle.num + APP_SYS_HANDLE_UINT_FACTOR;
        uint32_t len = app_sys_handle.len + APP_SYS_HANDLE_UINT_FACTOR;
        app_sys_handle_unit_t *unit = app_mem_alloc(sizeof(app_sys_handle_unit_t) * len);
        /* 拷贝原生的资源管理单元集合 */
        for (uint32_t idx = 0; idx < app_sys_handle.len; idx++)
            unit[idx] = app_sys_handle.unit[idx];
        /* 初始化新生成的资源管理单元集合 */
        for (uint32_t idx = app_sys_handle.len; idx < len; idx++)
            unit[idx].src = NULL;
        /* 释放原生的资源管理单元集合 */
        app_mem_free(app_sys_handle.unit);
        /* 设置新生成的资源管理单元集合 */
        app_sys_handle.num  = num;
        app_sys_handle.len  = len;
        app_sys_handle.unit = unit;
    }
    /* 3.如果资源管理单元不存在,创建它 */
    if (app_sys_handle.unit[idx1].src == NULL) {
        /* 生成单元句柄集合 */
        uint32_t num = APP_SYS_HANDLE_UINT_LENGTH;
        uint32_t len = APP_SYS_HANDLE_UINT_LENGTH;
        void   **src = app_mem_alloc(sizeof(void *) * len);
        /* 初始化新生成的单元句柄集合 */
        for (uint32_t idx = 0; idx < len; idx++)
            src[idx] = NULL;
        /* 设置新生成的资源管理单元集合 */
        app_sys_handle.unit[idx1].num = num;
        app_sys_handle.unit[idx1].len = len;
        app_sys_handle.unit[idx1].src = src;
        app_sys_handle.num--;
    }
    /* 4.寻找一个空闲句柄 */
    if (app_sys_handle.unit[idx1].num > 0) {
        /* 遍历单元句柄集合,寻找一个空闲句柄 */
        for (idx2 = 0; idx2 < app_sys_handle.unit[idx1].len; idx2++)
            if (app_sys_handle.unit[idx1].src[idx2] == NULL) {
                app_sys_handle.unit[idx1].src[idx2]  = (void *)(~0);
                app_sys_handle.unit[idx1].num--;
                break;
            }
    }
    /* 5.最终检查 */
    if (idx1 < app_sys_handle.len && idx2 < app_sys_handle.unit[idx1].len) {
        /* 计算句柄 */
        *handle = idx1 * APP_SYS_HANDLE_UINT_LENGTH + idx2;
    }
}

/*@brief     句柄绑定或更新资源
 *@param[in] handle 句柄
 *@param[in] src 资源
 */
void app_sys_handle_src_set(uint32_t handle, void *src)
{
    uint32_t idx1 = handle / APP_SYS_HANDLE_UINT_LENGTH;
    uint32_t idx2 = handle % APP_SYS_HANDLE_UINT_LENGTH;
    /* 这是一个合法句柄 */
    if (idx1 < app_sys_handle.len && idx2 < app_sys_handle.unit[idx1].len)
        app_sys_handle.unit[idx1].src[idx2] = src;
}

/*@brief      句柄获取资源
 *@param[in]  handle 句柄
 *@param[out] src 资源
 */
void app_sys_handle_src_get(uint32_t handle, void **src)
{
    *src = NULL;
    uint32_t idx1 = handle / APP_SYS_HANDLE_UINT_LENGTH;
    uint32_t idx2 = handle % APP_SYS_HANDLE_UINT_LENGTH;
    /* 这是一个合法句柄 */
    if (idx1 < app_sys_handle.len && idx2 < app_sys_handle.unit[idx1].len)
        *src = app_sys_handle.unit[idx1].src[idx2];
}
