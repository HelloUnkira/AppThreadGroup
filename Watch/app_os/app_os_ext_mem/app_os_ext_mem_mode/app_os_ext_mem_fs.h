#ifndef APP_OS_EXT_MEM_FS_H
#define APP_OS_EXT_MEM_FS_H

#if APP_OS_EXT_MEM_FS

/*@brief            读取数据从指定的文件中
 *@param[in] name   文件名
 *@param[in] offset 文件偏移
 *@param[in] buffer 数据缓存
 *@param[in] size   数据缓存大小
 *@retval           读取数据实际大小(失败返回负数)
 */
ssize_t app_os_ext_mem_fs_read(char *name, uintptr_t offset, uint8_t *buffer, size_t size);

/*@brief            写入数据到指定的文件中
 *@param[in] name   文件名
 *@param[in] offset 文件偏移
 *@param[in] buffer 数据缓存
 *@param[in] size   数据缓存大小
 *@retval           写入数据实际大小(失败返回负数)
 */
ssize_t app_os_ext_mem_fs_write(char *name, uintptr_t offset, uint8_t *buffer, size_t size);

/*@brief  初始化外存及文件系统
 *@retval 失败返回负数
 */
int32_t app_os_ext_mem_fs_ready(void);

#endif

#endif
