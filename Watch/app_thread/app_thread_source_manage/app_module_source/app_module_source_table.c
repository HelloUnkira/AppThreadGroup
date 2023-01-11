/*一个代替注册的,脚本自动生成的,本地静态的外存data表
 *通过app_module_source_table.py生成
 *参考本目录下的.json中的模式生成源
 */

#include "app_thread_interface.h"

static const app_module_source_t app_module_source_thread_master_table[] = {
	{
		/* unknown */
		.data_name = "unknown",
		.data_size = 20 * 10,
		.data_base = 0x0,
	},
	{
		/* unknown */
		.data_name = "unknown",
		.data_size = 20 * 10,
		.data_base = 0xc8,
	},
};

static const app_module_source_t app_module_source_thread_mix_custom_table[] = {
	{
		/* system clock + crc32 + checksum32 */
		.data_name = "system clock",
		.data_size = 20 + 4 + 4,
		.data_base = 0x0,
	},
	{
		/* system stopwatch + crc32 + checksum32 */
		.data_name = "system stopwatch",
		.data_size = 30 + 4 + 4,
		.data_base = 0x1c,
	},
	{
		/* system countdown + crc32 + checksum32 */
		.data_name = "system countdown",
		.data_size = 30 + 4 + 4,
		.data_base = 0x42,
	},
	{
		/* unknown */
		.data_name = "unknown",
		.data_size = 20 * 10,
		.data_base = 0x68,
	},
};

static const app_module_source_t app_module_source_thread_mix_irq_table[] = {
	{
		/* unknown */
		.data_name = "unknown",
		.data_size = 20 * 10,
		.data_base = 0x0,
	},
};

static const app_module_source_t * app_module_source_set_table[] = {
	app_module_source_thread_master_table,
	app_module_source_thread_mix_custom_table,
	app_module_source_thread_mix_irq_table,
};

static const uint32_t app_module_source_set_size_table[] = {
	sizeof(app_module_source_thread_master_table) /
	sizeof(app_module_source_thread_master_table[0]),
	sizeof(app_module_source_thread_mix_custom_table) /
	sizeof(app_module_source_thread_mix_custom_table[0]),
	sizeof(app_module_source_thread_mix_irq_table) /
	sizeof(app_module_source_thread_mix_irq_table[0]),
};

static const char * app_module_source_string_table[] = {
	"thread_master",
	"thread_mix_custom",
	"thread_mix_irq",
};

static const uint32_t app_module_source_table_size = 3;

/*@通过chunk名字以及data名字索引data静态实例
 *@param[in] chunk_name名字
 *@param[in] data_name名字
 *@retval    data静态实例
 */
const app_module_source_t * app_module_source_find_by_name(const char *chunk_name, const char *data_name)
{
	for (uint32_t idx = 0; idx < app_module_source_table_size; idx++)
		if (!strcmp(app_module_source_string_table[idx], chunk_name))
			for (uint32_t idx1 = 0; idx1 < app_module_source_set_size_table[idx]; idx1++)
				if (!strcmp(app_module_source_set_table[idx][idx1].data_name, data_name))
					return &app_module_source_set_table[idx][idx1];
		return NULL;
}
