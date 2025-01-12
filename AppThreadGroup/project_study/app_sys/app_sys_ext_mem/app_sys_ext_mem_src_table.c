/*一个代替注册的,脚本自动生成的,本地静态的外存data表
 *通过app_sys_ext_mem_src_table.py生成
 *参考本目录下的.json中的模式生成源
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

static const app_sys_ext_mem_src_t app_sys_ext_mem_src_mix_chunk_large_table[] = {
	{
		/* unknown */
		.data_name = "unknown",
		.data_size = 512 * 1,
		.data_base = 0x0,
	},
	{
		/* unknown */
		.data_name = "unknown",
		.data_size = 512 * 1,
		.data_base = 0x200,
	},
};

static const app_sys_ext_mem_src_t app_sys_ext_mem_src_mix_chunk_small_table[] = {
	{
		/* trace log text */
		.data_name = "trace log text",
		.data_size = 4096 * 1,
		.data_base = 0x0,
	},
	{
		/* module source */
		.data_name = "module source",
		.data_size = 512 * 1,
		.data_base = 0x1000,
	},
	{
		/* unknown */
		.data_name = "unknown",
		.data_size = 512 * 1,
		.data_base = 0x1200,
	},
};

static const app_sys_ext_mem_src_t * app_sys_ext_mem_src_set_table[] = {
	app_sys_ext_mem_src_mix_chunk_large_table,
	app_sys_ext_mem_src_mix_chunk_small_table,
};

static const uint32_t app_sys_ext_mem_src_set_size_table[] = {
	app_sys_arr_len(app_sys_ext_mem_src_mix_chunk_large_table),
	app_sys_arr_len(app_sys_ext_mem_src_mix_chunk_small_table),
};

static const char * app_sys_ext_mem_src_string_table[] = {
	"mix_chunk_large",
	"mix_chunk_small",
};

static const uint32_t app_sys_ext_mem_src_table_size = 2;

/*@brief 通过chunk名字以及data名字索引data静态实例
 *@param chunk_name 名字
 *@param data_name  名字
 *@retval 静态实例
 */
const app_sys_ext_mem_src_t * app_sys_ext_mem_src_find_by_name(const char *chunk_name, const char *data_name)
{
	for (uint32_t idx = 0; idx < app_sys_ext_mem_src_table_size; idx++)
		if (!strcmp(app_sys_ext_mem_src_string_table[idx], chunk_name))
			for (uint32_t idx1 = 0; idx1 < app_sys_ext_mem_src_set_size_table[idx]; idx1++)
				if (!strcmp(app_sys_ext_mem_src_set_table[idx][idx1].data_name, data_name))
					return &app_sys_ext_mem_src_set_table[idx][idx1];
		return NULL;
}
/*@brief 通过chunk名字以及索引data静态实例
 *@param chunk_name 名字
 *@param index      索引
 *@retval 静态实例
 */
const app_sys_ext_mem_src_t * app_sys_ext_mem_src_find_by_index(const char *chunk_name, uint32_t index)
{
	for (uint32_t idx = 0; idx < app_sys_ext_mem_src_table_size; idx++)
		if (!strcmp(app_sys_ext_mem_src_string_table[idx], chunk_name))
			if (index < app_sys_ext_mem_src_set_size_table[idx])
				return &app_sys_ext_mem_src_set_table[idx][index];
			else
				return NULL;
		return NULL;
}
