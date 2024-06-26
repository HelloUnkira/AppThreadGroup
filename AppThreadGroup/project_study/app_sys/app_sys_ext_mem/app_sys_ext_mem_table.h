#ifndef APP_SYS_EXT_MEM_TABLE_H
#define APP_SYS_EXT_MEM_TABLE_H

/*一个代替注册的,脚本自动生成的,本地静态的外存chunk表
 *通过app_sys_ext_mem_table.py生成
 *参考app_sys_ext_mem_table.json中的模式生成源
 */

/*@brief 通过名字索引chunk静态实例
 *@param name 名字
 *@retval 静态实例
 */
const app_sys_ext_mem_t * app_sys_ext_mem_find_by_name(const char *name);

/*@brief 通过索引chunk静态实例
 *@param index 索引
 *@retval 静态实例
 */
const app_sys_ext_mem_t * app_sys_ext_mem_find_by_index(uint32_t index);

#endif
