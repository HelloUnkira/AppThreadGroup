/*实现目标:
 *    系统提供的文件系统相关的功能组件
 */

#include "app_ext_lib.h"

#if APP_ARCH_EXT_MEM && APP_ARCH_IS_PC

/*@brief             读取数据从指定的文件中
 *@param[in] ext_mem 文件静态实例
 *@param[in] offset  文件偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            读取数据实际大小(失败返回负数)
 */
size_t app_arch_ext_mem_read(const app_arch_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, size_t size)
{
    FILE *file = fopen(ext_mem->chunk_name, "rb+");
    fseek(file, offset, SEEK_SET);
    size_t ret = fread(buffer, size, 1, file);
    fclose(file);
    return ret;
}

/*@brief             写入数据到指定的文件中
 *@param[in] ext_mem 文件静态实例
 *@param[in] offset  文件偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            写入数据实际大小(失败返回负数)
 */
size_t app_arch_ext_mem_write(const app_arch_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, size_t size)
{
    FILE *file = fopen(ext_mem->chunk_name, "rb+");
    fseek(file, offset, SEEK_SET);
    size_t ret = fwrite(buffer, size, 1, file);
    fclose(file);
    return ret;
}

/*@brief  初始化外存及文件系统
 *@retval 失败返回负数
 */
int32_t app_arch_ext_mem_ready(void)
{
    return 0;
}

#endif
