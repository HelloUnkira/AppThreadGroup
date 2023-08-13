#ifndef APP_SYS_EXT_SRC_TABLE_H
#define APP_SYS_EXT_SRC_TABLE_H

/*@通过chunk名字以及data名字索引data静态实例
 *@param[in] chunk_name名字
 *@param[in] data_name名字
 *@retval    data静态实例
 */
const app_sys_ext_src_t * app_sys_ext_src_find_by_name(const char *chunk_name, const char *data_name);

#endif
