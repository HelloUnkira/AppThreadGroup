/*实现目标:
 *    生成一个精简且实用的抽象布局
 *    将各种数据以chunk的形式组织起来
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_ext_mem.h"

static app_mutex_t app_sys_ext_mem_mutex = {0};

/*@brief             读取数据从指定的文件中
 *@param[in] ext_mem chunk静态实例
 *@param[in] offset  数据偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            读取数据实际大小(失败返回负数)
 */
size_t app_sys_ext_mem_read(const app_sys_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, size_t size)
{
    size_t retval = 0;
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_take);
    /* 根据chunk_base的不同使用不同的底层读写接口,取决于平台分布情况,这里忽略 */
    APP_SYS_LOG_INFO("read start");
    /* chunk_base == 0x02000000, 保留内存 */
    if (ext_mem->chunk_base == 0x02000000) {
        #if 0
        #elif APP_OS_EXT_MEM
        retval = app_os_ext_mem_read((const app_os_ext_mem_t *)ext_mem, offset, buffer, size);
        #elif APP_ARCH_EXT_MEM
        retval = app_arch_ext_mem_read((const app_arch_ext_mem_t *)ext_mem, offset, buffer, size);
        #else
        #error "unknown ext mem adaptor"
        #endif
    }
    /* chunk_base == 0x08000000, 闪存 */
    if (ext_mem->chunk_base == 0x08000000) {
        #if 0
        #elif APP_OS_EXT_MEM
        retval = app_os_ext_mem_read((const app_os_ext_mem_t *)ext_mem, offset, buffer, size);
        #elif APP_ARCH_EXT_MEM
        retval = app_arch_ext_mem_read((const app_arch_ext_mem_t *)ext_mem, offset, buffer, size);
        #else
        #error "unknown ext mem adaptor"
        #endif
    }
    /* chunk_base == 0x10000000, SD卡 */
    if (ext_mem->chunk_base == 0x10000000) {
        #if 0
        #elif APP_OS_EXT_MEM
        retval = app_os_ext_mem_read((const app_os_ext_mem_t *)ext_mem, offset, buffer, size);
        #elif APP_ARCH_EXT_MEM
        retval = app_arch_ext_mem_read((const app_arch_ext_mem_t *)ext_mem, offset, buffer, size);
        #else
        #error "unknown ext mem adaptor"
        #endif
    }
    /* 继续添加其他类型外存介质的映射空间 */
    APP_SYS_LOG_INFO("read end");
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_give);
    return retval;
}

/*@brief             写入数据到指定的文件中
 *@param[in] ext_mem chunk静态实例
 *@param[in] offset  数据偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            写入数据实际大小(失败返回负数)
 */
size_t app_sys_ext_mem_write(const app_sys_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, size_t size)
{
    size_t retval = 0;
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_take);
    /* 根据chunk_base的不同使用不同的底层读写接口,取决于平台分布情况,这里忽略 */
    APP_SYS_LOG_INFO("write start");
    /* chunk_base == 0x02000000, 保留内存 */
    if (ext_mem->chunk_base == 0x02000000) {
        #if 0
        #elif APP_OS_EXT_MEM
        retval = app_os_ext_mem_write((const app_os_ext_mem_t *)ext_mem, offset, buffer, size);
        #elif APP_ARCH_EXT_MEM
        retval = app_arch_ext_mem_write((const app_arch_ext_mem_t *)ext_mem, offset, buffer, size);
        #else
        #error "unknown ext mem adaptor"
        #endif
    }
    /* ext_mem->chunk_base == 0x08000000, 闪存 */
    if (ext_mem->chunk_base == 0x08000000) {
        #if 0
        #elif APP_OS_EXT_MEM
        retval = app_os_ext_mem_write((const app_os_ext_mem_t *)ext_mem, offset, buffer, size);
        #elif APP_ARCH_EXT_MEM
        retval = app_arch_ext_mem_write((const app_arch_ext_mem_t *)ext_mem, offset, buffer, size);
        #else
        #error "unknown ext mem adaptor"
        #endif
    }
    /* chunk_base == 0x10000000, SD卡 */
    if (ext_mem->chunk_base == 0x10000000) {
        #if 0
        #elif APP_OS_EXT_MEM
        retval = app_os_ext_mem_write((const app_os_ext_mem_t *)ext_mem, offset, buffer, size);
        #elif APP_ARCH_EXT_MEM
        retval = app_arch_ext_mem_write((const app_arch_ext_mem_t *)ext_mem, offset, buffer, size);
        #else
        #error "unknown ext mem adaptor"
        #endif
    }
    /* 继续添加其他类型外存介质的映射空间 */
    APP_SYS_LOG_INFO("write end");
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_give);
    return retval;
}

/*@brief     使用数据元刷新文件
 *@param[in] ext_mem chunk静态实例
 *@param[in] value   指定数据元
 */
void app_sys_ext_mem_reflush(const app_sys_ext_mem_t *ext_mem, uint8_t value)
{
    uint8_t block[4096] = {0};
    for (uint32_t idx = 0; idx < sizeof(block); block[idx] = value, idx++);
    APP_SYS_LOG_INFO("reflush start");
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_take);
    for (uint32_t idx = 0; idx < ext_mem->chunk_size; idx += sizeof(block))
        app_sys_ext_mem_write(ext_mem, idx, block, sizeof(block));
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_give);
    APP_SYS_LOG_INFO("reflush end");
}

/*@brief 初始化原生外存
 */
void app_sys_ext_mem_ready(void)
{
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_static);
    
    #if APP_OS_EXT_MEM
    int32_t retval_1 = app_os_ext_mem_ready();
    if (retval_1 != 0) {
        APP_SYS_LOG_ERROR("retval_1:%d", retval_1);
        APP_SYS_ASSERT(retval_1 == 0);
    }
    #endif
    
    #if APP_ARCH_EXT_MEM
    int32_t retval_2 = app_arch_ext_mem_ready();
    if (retval_2 != 0) {
        APP_SYS_LOG_ERROR("retval_2:%d", retval_2);
        APP_SYS_ASSERT(retval_2 == 0);
    }
    #endif
}
