#ifndef APP_SYS_EXT_MEM_H
#define APP_SYS_EXT_MEM_H

typedef struct {
    const char *chunk_name;     /* 读写chunk使用 */
    uintptr_t   chunk_base;     /* 当前chunk落在指定物理实体基地址映射(保留内存,闪存,卡,其他介质...) */
    uintptr_t   chunk_size;     /* 当前chunk大小 */
    uintptr_t   chunk_offset;   /* 相对chunk_base的基址偏移量 */
} app_sys_ext_mem_t;

/*@brief             读取数据从指定的文件中
 *@param[in] ext_mem chunk静态实例
 *@param[in] offset  数据偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            成功或者失败
 */
bool app_sys_ext_mem_read(const app_sys_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, uintptr_t size);

/*@brief             写入数据到指定的文件中
 *@param[in] ext_mem chunk静态实例
 *@param[in] offset  数据偏移
 *@param[in] buffer  数据缓存
 *@param[in] size    数据缓存大小
 *@retval            成功或者失败
 */
bool app_sys_ext_mem_write(const app_sys_ext_mem_t *ext_mem, uintptr_t offset, uint8_t *buffer, uintptr_t size);

/*@brief     使用数据元刷新文件
 *@param[in] ext_mem chunk静态实例
 *@param[in] value   指定数据元
 */
void app_sys_ext_mem_reflush(const app_sys_ext_mem_t *ext_mem, uint8_t value);

/*@brief 初始化原生外存
 */
void app_sys_ext_mem_ready(void);

/*@brief 生成物理外存映射,将其都刷为0xFF
 *       注意:仅在PC上构建
 */
void app_sys_ext_mem_remake(void);

#endif
