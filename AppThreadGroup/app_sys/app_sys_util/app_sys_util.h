#ifndef APP_SYS_UTIL_H
#define APP_SYS_UTIL_H

/*@brief     计算数据流的checksum32校验
 *@param[in] data 数据流
 *@param[in] size 数据大小
 */
uint32_t app_sys_checksum32(void *data, uint32_t size);

/*@brief     计算数据流的crc32校验
 *@param[in] data 数据流
 #@param[in] size 数据大小
 */
uint32_t app_sys_crc32(uint8_t *data, uint32_t size);

/*@brief     计算数据流的crc8校验
 *@param[in] data 数据流
 #@param[in] size 数据大小
 */
uint8_t app_sys_crc8(uint8_t *data, uint32_t size);

#endif
