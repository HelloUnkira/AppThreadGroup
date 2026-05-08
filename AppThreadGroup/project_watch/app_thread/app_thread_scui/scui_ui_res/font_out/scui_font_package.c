/*一个通过scui_font_package脚本生成的font表
 */

#include "scui.h"

static const scui_font_t font_ascii_8_bin = {
	.font_name	 = "font_ascii_8.bin",
	.font_lang	 = scui_font_lang_ascii,
	.font_size	 = 8,
	.data_bin	 = 0x0,
	.size_bin	 = 0x59c,
};

static const scui_font_t font_ascii_16_bin = {
	.font_name	 = "font_ascii_16.bin",
	.font_lang	 = scui_font_lang_ascii,
	.font_size	 = 16,
	.data_bin	 = 0x59c,
	.size_bin	 = 0xc80,
};

static const scui_font_t font_ascii_24_bin = {
	.font_name	 = "font_ascii_24.bin",
	.font_lang	 = scui_font_lang_ascii,
	.font_size	 = 24,
	.data_bin	 = 0x121c,
	.size_bin	 = 0x1710,
};

static const scui_font_t font_ascii_32_bin = {
	.font_name	 = "font_ascii_32.bin",
	.font_lang	 = scui_font_lang_ascii,
	.font_size	 = 32,
	.data_bin	 = 0x292c,
	.size_bin	 = 0x22e8,
};

static const scui_font_t font_ascii_40_bin = {
	.font_name	 = "font_ascii_40.bin",
	.font_lang	 = scui_font_lang_ascii,
	.font_size	 = 40,
	.data_bin	 = 0x4c14,
	.size_bin	 = 0x3380,
};

static const scui_font_t font_ascii_48_bin = {
	.font_name	 = "font_ascii_48.bin",
	.font_lang	 = scui_font_lang_ascii,
	.font_size	 = 48,
	.data_bin	 = 0x7f94,
	.size_bin	 = 0x42e0,
};

static const scui_font_t font_ascii_56_bin = {
	.font_name	 = "font_ascii_56.bin",
	.font_lang	 = scui_font_lang_ascii,
	.font_size	 = 56,
	.data_bin	 = 0xc274,
	.size_bin	 = 0x56b0,
};

static const scui_font_t font_ascii_64_bin = {
	.font_name	 = "font_ascii_64.bin",
	.font_lang	 = scui_font_lang_ascii,
	.font_size	 = 64,
	.data_bin	 = 0x11924,
	.size_bin	 = 0x6a3c,
};

static const scui_font_t font_ascii_72_bin = {
	.font_name	 = "font_ascii_72.bin",
	.font_lang	 = scui_font_lang_ascii,
	.font_size	 = 72,
	.data_bin	 = 0x18360,
	.size_bin	 = 0x82a0,
};

static const scui_font_t font_ascii_80_bin = {
	.font_name	 = "font_ascii_80.bin",
	.font_lang	 = scui_font_lang_ascii,
	.font_size	 = 80,
	.data_bin	 = 0x20600,
	.size_bin	 = 0x9668,
};

static const scui_font_t font_ascii_88_bin = {
	.font_name	 = "font_ascii_88.bin",
	.font_lang	 = scui_font_lang_ascii,
	.font_size	 = 88,
	.data_bin	 = 0x29c68,
	.size_bin	 = 0xaf40,
};

static const scui_font_t font_en_32_bin = {
	.font_name	 = "font_en_32.bin",
	.font_lang	 = scui_font_lang_en,
	.font_size	 = 32,
	.data_bin	 = 0x34ba8,
	.size_bin	 = 0x22e8,
};

static const scui_font_t font_en_36_bin = {
	.font_name	 = "font_en_36.bin",
	.font_lang	 = scui_font_lang_en,
	.font_size	 = 36,
	.data_bin	 = 0x36e90,
	.size_bin	 = 0x2c10,
};

static const scui_font_t font_eu_32_bin = {
	.font_name	 = "font_eu_32.bin",
	.font_lang	 = scui_font_lang_eu,
	.font_size	 = 32,
	.data_bin	 = 0x39aa0,
	.size_bin	 = 0x661c,
};

static const scui_font_t font_eu_36_bin = {
	.font_name	 = "font_eu_36.bin",
	.font_lang	 = scui_font_lang_eu,
	.font_size	 = 36,
	.data_bin	 = 0x400bc,
	.size_bin	 = 0x75d8,
};

static const scui_font_t font_jp_32_bin = {
	.font_name	 = "font_jp_32.bin",
	.font_lang	 = scui_font_lang_jp,
	.font_size	 = 32,
	.data_bin	 = 0x47694,
	.size_bin	 = 0x352bf8,
};

static const scui_font_t font_jp_36_bin = {
	.font_name	 = "font_jp_36.bin",
	.font_lang	 = scui_font_lang_jp,
	.font_size	 = 36,
	.data_bin	 = 0x39a28c,
	.size_bin	 = 0x3fd810,
};

static const scui_font_t font_zh_32_bin = {
	.font_name	 = "font_zh_32.bin",
	.font_lang	 = scui_font_lang_zh,
	.font_size	 = 32,
	.data_bin	 = 0x797a9c,
	.size_bin	 = 0x325dac,
};

static const scui_font_t font_zh_36_bin = {
	.font_name	 = "font_zh_36.bin",
	.font_lang	 = scui_font_lang_zh,
	.font_size	 = 36,
	.data_bin	 = 0xabd848,
	.size_bin	 = 0x3c6104,
};

static const scui_font_t font_tinyTTF_ttf = {
	.font_name	 = "font_tinyTTF.ttf",
	.font_lang	 = scui_font_lang_multi,
	.font_size	 = 0,
	.data_bin	 = 0xe8394c,
	.size_bin	 = 0xc256c4,
};

const void * scui_font_package_table[20] = {
	(void *)&font_ascii_8_bin,
	(void *)&font_ascii_16_bin,
	(void *)&font_ascii_24_bin,
	(void *)&font_ascii_32_bin,
	(void *)&font_ascii_40_bin,
	(void *)&font_ascii_48_bin,
	(void *)&font_ascii_56_bin,
	(void *)&font_ascii_64_bin,
	(void *)&font_ascii_72_bin,
	(void *)&font_ascii_80_bin,
	(void *)&font_ascii_88_bin,
	(void *)&font_en_32_bin,
	(void *)&font_en_36_bin,
	(void *)&font_eu_32_bin,
	(void *)&font_eu_36_bin,
	(void *)&font_jp_32_bin,
	(void *)&font_jp_36_bin,
	(void *)&font_zh_32_bin,
	(void *)&font_zh_36_bin,
	(void *)&font_tinyTTF_ttf,
};

/*@brief 字库语言字号匹配
 *@param lang 字库语言类型
 *@param size 字库字号
 *@retval 字库句柄
 */
scui_handle_t scui_font_match(scui_font_lang_t lang, scui_handle_t size)
{
	if (size ==  32 && lang == scui_font_lang_en)
		return scui_font_en_32bin;
	if (size ==  36 && lang == scui_font_lang_en)
		return scui_font_en_36bin;
	if (size ==  32 && lang == scui_font_lang_eu)
		return scui_font_eu_32bin;
	if (size ==  36 && lang == scui_font_lang_eu)
		return scui_font_eu_36bin;
	if (size ==  32 && lang == scui_font_lang_jp)
		return scui_font_jp_32bin;
	if (size ==  36 && lang == scui_font_lang_jp)
		return scui_font_jp_36bin;
	if (size ==  32 && lang == scui_font_lang_zh)
		return scui_font_zh_32bin;
	if (size ==  36 && lang == scui_font_lang_zh)
		return scui_font_zh_36bin;
	
	return SCUI_HANDLE_INVALID;
}
