/*一个代替注册的,脚本自动生成的,本地静态的外存chunk表
 *通过app_module_ext_mem_table.py生成
 *参考app_module_ext_mem_table.json中的模式生成源
 */

#include "app_std_lib.h"
#include "app_module_ext_mem.h"

static const app_module_ext_mem_t app_module_ext_mem_table[] = {
	{
		/* large mix data chunk, Suitable RAW management */
		.chunk_name = "mix_chunk_large",
		.chunk_size = 4096 * 50,
		.chunk_base = 0x0,
	},
	{
		/* small mix data chunk, Suitable FS management */
		.chunk_name = "mix_chunk_small",
		.chunk_size = 4096 * 10,
		.chunk_base = 0x32000,
	},
};

static const uint32_t app_module_ext_mem_table_size =
               sizeof(app_module_ext_mem_table) /
               sizeof(app_module_ext_mem_table[0]);

/*@brief通过名字索引chunk静态实例
 *@param[in] name名字
 *@retval    chunk静态实例
 */
const app_module_ext_mem_t * app_module_ext_mem_find_by_name(const char *name)
{
	for (uint32_t idx = 0; idx < app_module_ext_mem_table_size; idx++)
		if (!strcmp(app_module_ext_mem_table[idx].chunk_name, name))
			return &app_module_ext_mem_table[idx];
		return NULL;
}
