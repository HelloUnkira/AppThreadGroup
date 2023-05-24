/*一个代替注册的,脚本自动生成的,本地静态的外存chunk表
 *通过app_sys_ext_mem_table.py生成
 *参考app_sys_ext_mem_table.json中的模式生成源
 */

#include "app_ext_lib.h"
#include "app_sys_ext_mem.h"

static const app_sys_ext_mem_t app_sys_ext_mem_table[] = {
	{
		/* static memory, suitable thermal data */
		.chunk_name 	= "static_memory",
		.chunk_base 	=  0x02000000,
		.chunk_size 	=  1024 * 10,
		.chunk_offset 	=  0x0,
	},
	{
		/* flash memory: large mix data chunk, suitable RAW management */
		.chunk_name 	= "mix_chunk_large",
		.chunk_base 	=  0x08000000,
		.chunk_size 	=  1024 * 200,
		.chunk_offset 	=  0x0,
	},
	{
		/* flash memory: small mix data chunk, suitable FS management */
		.chunk_name 	= "mix_chunk_small",
		.chunk_base 	=  0x08000000,
		.chunk_size 	=  1024 * 40,
		.chunk_offset 	=  0x32000,
	},
	{
		/* secure digital memory card, suitable permanently data */
		.chunk_name 	= "sd_mem_card",
		.chunk_base 	=  0x10000000,
		.chunk_size 	=  1024 * 1024 * 8,
		.chunk_offset 	=  0x0,
	},
};

static const uint32_t app_sys_ext_mem_table_size =
               sizeof(app_sys_ext_mem_table) /
               sizeof(app_sys_ext_mem_table[0]);

/*@brief通过名字索引chunk静态实例
 *@param[in] name名字
 *@retval    chunk静态实例
 */
const app_sys_ext_mem_t * app_sys_ext_mem_find_by_name(const char *name)
{
	for (uint32_t idx = 0; idx < app_sys_ext_mem_table_size; idx++)
		if (!strcmp(app_sys_ext_mem_table[idx].chunk_name, name))
			return &app_sys_ext_mem_table[idx];
		return NULL;
}
