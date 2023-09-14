/*实现目标:
 *    生成一个精简且实用的抽象布局
 *    将各种数据以chunk的形式组织起来
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

static app_mutex_t app_sys_ext_mem_mutex = {0};

/*@brief             读取数据从指定的文件中
 *@param[in] ext_mem chunk静态实例
 *@param[in] offset  数据偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            成功或者失败
 */
bool app_sys_ext_mem_read(const app_sys_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, uintptr_t size)
{
    int32_t retval = 0;
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_take);
    APP_SYS_LOG_INFO("read start");
    app_dev_ext_mem_data_t *ext_mem_data = app_dev_ext_mem_data_addr(&app_dev_ext_mem);
    ext_mem_data->ext_mem.chunk_base   = ext_mem->chunk_base;
    ext_mem_data->ext_mem.chunk_size   = ext_mem->chunk_size;
    ext_mem_data->ext_mem.chunk_offset = ext_mem->chunk_offset;
    ext_mem_data->rw_args.offset = offset;
    ext_mem_data->rw_args.buffer = buffer;
    ext_mem_data->rw_args.size   = size;
    if ((retval = app_dev_ext_mem_read(&app_dev_ext_mem)) != 0)
        APP_SYS_LOG_ERROR("retval:%d", retval);
    APP_SYS_LOG_INFO("read end");
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_give);
    return retval == 0;
}

/*@brief             写入数据到指定的文件中
 *@param[in] ext_mem chunk静态实例
 *@param[in] offset  数据偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            成功或者失败
 */
bool app_sys_ext_mem_write(const app_sys_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, uintptr_t size)
{
    int32_t retval = 0;
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_take);
    APP_SYS_LOG_INFO("write start");
    app_dev_ext_mem_data_t *ext_mem_data = app_dev_ext_mem_data_addr(&app_dev_ext_mem);
    ext_mem_data->ext_mem.chunk_base   = ext_mem->chunk_base;
    ext_mem_data->ext_mem.chunk_size   = ext_mem->chunk_size;
    ext_mem_data->ext_mem.chunk_offset = ext_mem->chunk_offset;
    ext_mem_data->rw_args.offset = offset;
    ext_mem_data->rw_args.buffer = buffer;
    ext_mem_data->rw_args.size   = size;
    if ((retval = app_dev_ext_mem_write(&app_dev_ext_mem)) != 0)
        APP_SYS_LOG_ERROR("retval:%d", retval);
    APP_SYS_LOG_INFO("write end");
    app_mutex_process(&app_sys_ext_mem_mutex, app_mutex_give);
    return retval == 0;
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
    for (uint32_t idx = 0; idx < ext_mem->chunk_size; idx += sizeof(block))
        app_sys_ext_mem_write(ext_mem, idx, block, sizeof(block));
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

/*@brief 输出原生外存布局
 */
void app_sys_ext_mem_layout(void)
{
    app_sys_ext_mem_t     *ext_mem = NULL;
    app_sys_ext_mem_src_t *ext_src = NULL;
    
    for (uint32_t idx = 0; true; idx++) {
        /* 读取一级分区 */
        ext_mem = (void *)app_sys_ext_mem_find_by_index(idx);
        if (ext_mem == NULL)
            break;
        if (idx == 0)
        APP_SYS_LOG_INFO("ext mem table:");
        APP_SYS_LOG_INFO("---base:%08x, size:%08x, offset:%08x, name:%s",
                          ext_mem->chunk_base,
                          ext_mem->chunk_size,
                          ext_mem->chunk_offset,
                          ext_mem->chunk_name);
        for (uint32_t idx1 = 0; true; idx1++) {
            ext_src = app_sys_ext_mem_src_find_by_index(ext_mem->chunk_name, idx1);
            if (ext_src == NULL)
                break;
            if (idx1 == 0)
            APP_SYS_LOG_INFO("\text mem src table:");
            APP_SYS_LOG_INFO("\t---base:%08x, size:%08x, name:%s",
                              ext_src->data_base,
                              ext_src->data_size,
                              ext_src->data_name);
        }
    }
}

/*@brief 生成物理外存映射,将其都刷为0xFF
 *       注意:仅在PC上构建
 */
void app_sys_ext_mem_remake(void)
{
    #if APP_ARCH_IS_PC && (APP_OS_IS_WINDOWS || APP_OS_IS_LINUX)
    
    uint8_t buffer[1024] = {0};
    for (uint32_t idx = 0; idx < 1024; buffer[idx] = 0xff, idx++);
    fclose(fopen("ext_mem_static",  "wb")); // 64K
    fclose(fopen("ext_mem_flash",   "wb")); // 8M
    fclose(fopen("ext_mem_sd_card", "wb")); // 64M
    FILE *file1 = fopen("ext_mem_static",  "rb+");
    FILE *file2 = fopen("ext_mem_flash",   "rb+");
    FILE *file3 = fopen("ext_mem_sd_card", "rb+");
    fseek(file1, 0, SEEK_SET);
    fseek(file2, 0, SEEK_SET);
    fseek(file3, 0, SEEK_SET);
    for (uint32_t idx = 0; idx < 64; idx++)
        fwrite(buffer, 1024, 1, file1);
    for (uint32_t idx = 0; idx < 1024 * 8; idx++)
        fwrite(buffer, 1024, 1, file2);
    for (uint32_t idx = 0; idx < 1024 * 64; idx++)
        fwrite(buffer, 1024, 1, file3);
    fclose(file1);
    fclose(file2);
    fclose(file3);
    
    #else
    #error "unknown"
    #endif
}
