/*实现目标:
 *    扩展封装
 *    将对外存的读写接口进一步封装成一体化资源
 */

#include "app_std_lib.h"
#include "app_sys_log.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_table.h"
#include "app_sys_ext_src.h"
#include "app_sys_ext_src_table.h"

/*@brief     读取资源从指定chunk的指定data段
 *@param[in] chunk_name chunk名字
 *@param[in] data_name  chunk内data名字
 *@param[in] buffer     数据空间
 *@param[in] size       数据大小
 *@retval    实际读写的数据长度
 */
size_t app_sys_ext_src_read(const char *chunk_name, const char *data_name, uint8_t *buffer, size_t size)
{
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name(chunk_name);
    const app_sys_ext_src_t *ext_src = app_sys_ext_src_find_by_name(chunk_name, data_name);
    APP_SYS_ASSERT(ext_mem == NULL);
    APP_SYS_ASSERT(ext_src == NULL);
    APP_SYS_ASSERT(ext_src->data_size < size);
    return app_sys_ext_mem_read(ext_mem, ext_src->data_base, buffer, size);
}

/*@brief     写入资源到指定chunk的指定data段
 *@param[in] chunk_name chunk名字
 *@param[in] data_name  chunk内data名字
 *@param[in] buffer     数据空间
 *@param[in] size       数据大小
 *@retval    实际读写的数据长度
 */
size_t app_sys_ext_src_write(const char *chunk_name, const char *data_name, uint8_t *buffer, size_t size)
{
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name(chunk_name);
    const app_sys_ext_src_t *ext_src = app_sys_ext_src_find_by_name(chunk_name, data_name);
    APP_SYS_ASSERT(ext_mem == NULL);
    APP_SYS_ASSERT(ext_src == NULL);
    APP_SYS_ASSERT(ext_src->data_size < size);
    return app_sys_ext_mem_write(ext_mem, ext_src->data_base, buffer, size);
}
