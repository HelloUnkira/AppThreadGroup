/*实现目标:
 *    生成一个精简且实用的抽象布局
 *    将各种数据以chunk的形式组织起来
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
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
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_take);
    APP_SYS_LOG_INFO("read start");
    app_dev_ext_mem_data_t *ext_mem_data = app_dev_ext_mem_data_addr(&app_dev_ext_mem);
    ext_mem_data->ext_mem.chunk_base   = ext_mem->chunk_base;
    ext_mem_data->ext_mem.chunk_size   = ext_mem->chunk_size;
    ext_mem_data->ext_mem.chunk_offset = ext_mem->chunk_offset;
    ext_mem_data->rw_args.offset = offset;
    ext_mem_data->rw_args.buffer = buffer;
    ext_mem_data->rw_args.size   = size;
    size_t retval = app_dev_ext_mem_read(&app_dev_ext_mem);
    APP_SYS_LOG_INFO("read end");
    APP_SYS_LOG_INFO("retval:%d", retval);
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
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_take);
    APP_SYS_LOG_INFO("write start");
    app_dev_ext_mem_data_t *ext_mem_data = app_dev_ext_mem_data_addr(&app_dev_ext_mem);
    ext_mem_data->ext_mem.chunk_base   = ext_mem->chunk_base;
    ext_mem_data->ext_mem.chunk_size   = ext_mem->chunk_size;
    ext_mem_data->ext_mem.chunk_offset = ext_mem->chunk_offset;
    ext_mem_data->rw_args.offset = offset;
    ext_mem_data->rw_args.buffer = buffer;
    ext_mem_data->rw_args.size   = size;
    size_t retval = app_dev_ext_mem_write(&app_dev_ext_mem);
    APP_SYS_LOG_INFO("write end");
    APP_SYS_LOG_INFO("retval:%d", retval);
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
    int32_t retval = app_dev_ext_mem_ready(&app_dev_ext_mem);
    APP_SYS_ASSERT(retval == 0);
}
