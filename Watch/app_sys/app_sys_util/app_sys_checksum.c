/*实现目标:
 *    摘要于外界的chunksum32校验
 *    Airoha Technology Corp
 */

#include "app_ext_lib.h"

/*@brief     计算数据流的checksum32校验
 *@param[in] data 数据流
 #@param[in] size 数据大小
 */
uint32_t app_sys_checksum32(void *data, uint32_t size)
{
	uint32_t *data4 = (uint32_t *)data, checksum = 0;
	for(uint32_t idx = 0; idx < size / 4; idx++)
		checksum += data4[idx];
	return checksum;
}
