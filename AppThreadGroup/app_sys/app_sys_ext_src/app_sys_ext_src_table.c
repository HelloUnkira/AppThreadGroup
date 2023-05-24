/*一个代替注册的,脚本自动生成的,本地静态的外存data表
 *通过app_sys_ext_src_table.py生成
 *参考本目录下的.json中的模式生成源
 */

#include "app_ext_lib.h"
#include "app_sys_ext_src.h"

static const app_sys_ext_src_t app_sys_ext_src_mix_chunk_large_table[] = {
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

static const app_sys_ext_src_t app_sys_ext_src_mix_chunk_small_table[] = {
	{
		/* trace log text */
		.data_name = "trace log text",
		.data_size = 4096 * 1,
		.data_base = 0x0,
	},
	{
		/* system profile */
		.data_name = "system profile",
		.data_size = 512 * 1,
		.data_base = 0x1000,
	},
	{
		/* user profile */
		.data_name = "user profile",
		.data_size = 512 * 1,
		.data_base = 0x1200,
	},
	{
		/* system clock + crc32 */
		.data_name = "system clock",
		.data_size = 40,
		.data_base = 0x1400,
	},
	{
		/* remind alarm + crc32 */
		.data_name = "remind alarm",
		.data_size = 65 * 10 + 10,
		.data_base = 0x1428,
	},
	{
		/* remind sedentary + crc32 */
		.data_name = "remind sedentary",
		.data_size = 20,
		.data_base = 0x16bc,
	},
	{
		/* remind drink + crc32 */
		.data_name = "remind drink",
		.data_size = 20,
		.data_base = 0x16d0,
	},
	{
		/* do not disturb + crc32 */
		.data_name = "do not disturb",
		.data_size = 30,
		.data_base = 0x16e4,
	},
	{
		/* unknown */
		.data_name = "unknown",
		.data_size = 512 * 1,
		.data_base = 0x1702,
	},
};

static const app_sys_ext_src_t * app_sys_ext_src_set_table[] = {
	app_sys_ext_src_mix_chunk_large_table,
	app_sys_ext_src_mix_chunk_small_table,
};

static const uint32_t app_sys_ext_src_set_size_table[] = {
	sizeof(app_sys_ext_src_mix_chunk_large_table) /
	sizeof(app_sys_ext_src_mix_chunk_large_table[0]),
	sizeof(app_sys_ext_src_mix_chunk_small_table) /
	sizeof(app_sys_ext_src_mix_chunk_small_table[0]),
};

static const char * app_sys_ext_src_string_table[] = {
	"mix_chunk_large",
	"mix_chunk_small",
};

static const uint32_t app_sys_ext_src_table_size = 2;

/*@通过chunk名字以及data名字索引data静态实例
 *@param[in] chunk_name名字
 *@param[in] data_name名字
 *@retval    data静态实例
 */
const app_sys_ext_src_t * app_sys_ext_src_find_by_name(const char *chunk_name, const char *data_name)
{
	for (uint32_t idx = 0; idx < app_sys_ext_src_table_size; idx++)
		if (!strcmp(app_sys_ext_src_string_table[idx], chunk_name))
			for (uint32_t idx1 = 0; idx1 < app_sys_ext_src_set_size_table[idx]; idx1++)
				if (!strcmp(app_sys_ext_src_set_table[idx][idx1].data_name, data_name))
					return &app_sys_ext_src_set_table[idx][idx1];
		return NULL;
}
