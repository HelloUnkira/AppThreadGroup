/*一个代替注册的,脚本自动生成的,本地静态的外存chunk表
 *通过app_module_ext_mem_table.py生成
 *参考app_module_ext_mem_table.json中的模式生成源
 */

#include "app_std_lib.h"
#include "app_module_ext_mem.h"

static const app_module_ext_mem_t app_module_ext_mem_table[] = {
	{
		/* chunk0 */
		.chunk_name = "thread_master",
		.chunk_size = 4096 * 10,
		.chunk_base = 0x0,
	},
	{
		/* chunk1 */
		.chunk_name = "thread_mix_irq",
		.chunk_size = 4096 * 20,
		.chunk_base = 0xa000,
	},
	{
		/* chunk2 */
		.chunk_name = "thread_mix_custom",
		.chunk_size = 4096 * 30,
		.chunk_base = 0x1e000,
	},
	{
		/* chunk2 */
		.chunk_name = "thread_source_manage",
		.chunk_size = 4096 * 30,
		.chunk_base = 0x3c000,
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
