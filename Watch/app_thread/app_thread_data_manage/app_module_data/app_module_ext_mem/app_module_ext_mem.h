#ifndef APP_MODULE_EXT_MEM_H
#define APP_MODULE_EXT_MEM_H

typedef struct {
    char     *chunk_name;   /* 读写chunk使用 */
    uintptr_t chunk_base;   /* 当前chunk落在指定物理实体基地址映射(保留内存,闪存,卡,其他介质...) */
    ssize_t   chunk_size;   /* 当前chunk大小 */
    uintptr_t chunk_offset; /* 相对chunk_base的基址偏移量 */
} app_module_ext_mem_t;

/*@brief             读取数据从指定的文件中
 *@param[in] ext_mem chunk静态实例
 *@param[in] offset  数据偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            读取数据实际大小(失败返回负数)
 */
ssize_t app_module_ext_mem_read(const app_module_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, size_t size);

/*@brief             写入数据到指定的文件中
 *@param[in] ext_mem chunk静态实例
 *@param[in] offset  数据偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            写入数据实际大小(失败返回负数)
 */
ssize_t app_module_ext_mem_write(const app_module_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, size_t size);

/*@brief 初始化原生外存
 */
void app_module_ext_mem_ready(void);

#endif
