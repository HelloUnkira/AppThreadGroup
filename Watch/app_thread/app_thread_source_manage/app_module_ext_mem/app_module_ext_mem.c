/*实现目标:
 *    生成一个精简且实用的抽象布局
 *    将各种数据以chunk的形式组织起来
 */

#include "app_thread_interface.h"

static app_mutex_t app_module_ext_mem_mutex = {0};

/*@brief             读取数据从指定的文件中
 *@param[in] ext_mem chunk静态实例
 *@param[in] offset  数据偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            读取数据实际大小(失败返回负数)
 */
ssize_t app_module_ext_mem_read(const app_module_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, size_t size)
{
    app_mutex_take(&app_module_ext_mem_mutex);
#if 0
#elif APP_OS_EXT_MEM
    ssize_t retval = app_os_ext_mem_read((const app_os_ext_mem_t *)ext_mem, offset, buffer, size);
#elif APP_ARCH_EXT_MEM
    ssize_t retval = app_arch_ext_mem_read((const app_arch_ext_mem_t *)ext_mem, offset, buffer, size);
#else
#error "unknown ext mem adaptor"
#endif
    app_mutex_give(&app_module_ext_mem_mutex);
    return retval;
}

/*@brief             写入数据到指定的文件中
 *@param[in] ext_mem chunk静态实例
 *@param[in] offset  数据偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            写入数据实际大小(失败返回负数)
 */
ssize_t app_module_ext_mem_write(const app_module_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, size_t size)
{
    app_mutex_take(&app_module_ext_mem_mutex);
#if 0
#elif APP_OS_EXT_MEM
    ssize_t retval = app_os_ext_mem_write((const app_os_ext_mem_t *)ext_mem, offset, buffer, size);
#elif APP_ARCH_EXT_MEM
    ssize_t retval = app_arch_ext_mem_write((const app_arch_ext_mem_t *)ext_mem, offset, buffer, size);
#else
#error "unknown ext mem adaptor"
#endif
    app_mutex_give(&app_module_ext_mem_mutex);
    return retval;
}

/*@brief  初始化原生外存
 *@retval 失败返回负数
 */
int32_t app_module_ext_mem_ready(void)
{
    app_mutex_process(&app_module_ext_mem_mutex);
#if 0
#elif APP_OS_EXT_MEM
    return app_os_ext_mem_ready();
#elif APP_ARCH_EXT_MEM
    return app_arch_ext_mem_ready();
#else
#error "unknown ext mem adaptor"
#endif
}
