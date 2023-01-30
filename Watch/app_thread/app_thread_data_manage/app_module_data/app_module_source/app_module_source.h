#ifndef APP_MODULE_SOURCE_H
#define APP_MODULE_SOURCE_H

typedef struct {
    char     *data_name;
    uintptr_t data_base;
    uintptr_t data_size;
} app_module_source_t;

/*@brief     读取资源从指定chunk的指定data段
 *@param[in] chunk_name chunk名字
 *@param[in] data_name  chunk内data名字
 *@param[in] buffer     数据空间
 *@param[in] size       数据大小
 *@retval    实际读写的数据长度
 */
ssize_t app_module_source_read(const char *chunk_name, const char *data_name, uint8_t *buffer, ssize_t size);

/*@brief     写入资源到指定chunk的指定data段
 *@param[in] chunk_name chunk名字
 *@param[in] data_name  chunk内data名字
 *@param[in] buffer     数据空间
 *@param[in] size       数据大小
 *@retval    实际读写的数据长度
 */
ssize_t app_module_source_write(const char *chunk_name, const char *data_name, uint8_t *buffer, ssize_t size);

#endif
