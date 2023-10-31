#ifndef APP_SYS_EXT_MEM_SRC_TABLE_H
#define APP_SYS_EXT_MEM_SRC_TABLE_H

/*@brief     通过chunk名字以及data名字索引data静态实例
通过chunk名字以及data名字索引data静态实例
 *@param[in] chunk_name 名字
 *@param[in] data_name  名字
 *@retval    静态实例
 */
const app_sys_ext_mem_src_t * app_sys_ext_mem_src_find_by_name(const char *chunk_name, const char *data_name);

/*@brief     通过chunk名字以及索引data静态实例
 *@param[in] chunk_name 名字
 *@param[in] index      索引
 *@retval    静态实例
 */
const app_sys_ext_mem_src_t * app_sys_ext_mem_src_find_by_index(const char *chunk_name, uint32_t index);

#endif
