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

/* 补充收集:misc */

/*@brief     将一个索引下标数组随机打乱
 *@param[in] arr 索引下标数组
 *@param[in] len 索引下标数组长度
 */
void app_sys_idx_shuffle(uint32_t *arr, uint32_t len);

/*@brief     移动指定字符到尾部
 *@param[in] str 字符串
 *@param[in] c   指定字符
 *@param[in] rev rev时移动到头部
 */
void app_sys_str_move(char *str, char c, bool rev);

/*@brief     循环左旋转字符串
 *@param[in] str 字符串
 *@param[in] ofs 旋转点
 *@param[in] rev rev时循环右旋转
 */
void app_sys_str_rotate(char *str, uint32_t ofs, bool rev);

/*@brief     指定字符为分割点逆序字符串
 *@param[in] str 字符串
 *@param[in] c   指定字符
 */
void app_sys_str_reverse(char *str, char c);

#endif
