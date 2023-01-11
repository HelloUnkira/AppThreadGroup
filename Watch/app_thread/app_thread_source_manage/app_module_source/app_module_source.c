/*实现目标:
 *    扩展封装
 *    将对外存的读写接口进一步封装成一体化资源
 */

#include "app_thread_interface.h"

/*@brief     读取资源从指定chunk的指定data段
 *@param[in] chunk_name chunk名字
 *@param[in] data_name  chunk内data名字
 *@param[in] buffer     数据空间
 *@param[in] size       数据大小
 *@retval    实际读写的数据长度
 */
ssize_t app_module_source_read(const char *chunk_name, const char *data_name, uint8_t *buffer, ssize_t size)
{
    const app_module_ext_mem_t *ext_mem = app_module_ext_mem_find_by_name(chunk_name);
    const app_module_source_t  *source  = app_module_source_find_by_name(chunk_name, data_name);
    if (ext_mem == NULL || source == NULL || source->data_size < size)
        return 0;
    return app_module_ext_mem_read(ext_mem, source->data_base, buffer, size);
}

/*@brief     写入资源到指定chunk的指定data段
 *@param[in] chunk_name chunk名字
 *@param[in] data_name  chunk内data名字
 *@param[in] buffer     数据空间
 *@param[in] size       数据大小
 *@retval    实际读写的数据长度
 */
ssize_t app_module_source_write(const char *chunk_name, const char *data_name, uint8_t *buffer, ssize_t size)
{
    const app_module_ext_mem_t *ext_mem = app_module_ext_mem_find_by_name(chunk_name);
    const app_module_source_t  *source  = app_module_source_find_by_name(chunk_name, data_name);
    if (ext_mem == NULL || source == NULL || source->data_size < size)
        return 0;
    return app_module_ext_mem_write(ext_mem, source->data_base, buffer, size);
}
