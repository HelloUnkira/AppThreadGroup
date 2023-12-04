#ifndef APP_SYS_EXT_MEM_SRC_H
#define APP_SYS_EXT_MEM_SRC_H

typedef struct {
    char     *data_name;
    uintptr_t data_base;
    size_t    data_size;
} app_sys_ext_mem_src_t;

/*@brief 读取资源从指定chunk的指定data段
 *@param chunk_name chunk名字
 *@param data_name  chunk内data名字
 *@param buffer     数据空间
 *@param size       数据大小
 *@retval 实际读写的数据长度
 */
size_t app_sys_ext_mem_src_read(const char *chunk_name, const char *data_name, uint8_t *buffer, size_t size);

/*@brief 写入资源到指定chunk的指定data段
 *@param chunk_name chunk名字
 *@param data_name  chunk内data名字
 *@param buffer     数据空间
 *@param size       数据大小
 *@retval 实际读写的数据长度
 */
size_t app_sys_ext_mem_src_write(const char *chunk_name, const char *data_name, uint8_t *buffer, size_t size);

#endif
