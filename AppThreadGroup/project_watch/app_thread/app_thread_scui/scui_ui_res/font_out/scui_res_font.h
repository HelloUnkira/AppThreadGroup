#ifndef SCUI_FONT_PACKAGE_H
#define SCUI_FONT_PACKAGE_H

/* 本文件由 scui_pack_tools.exe 生成 */

typedef enum {
	scui_font_lang_ascii,
	scui_font_lang_multi,
	scui_font_lang_symbol,
	scui_font_lang_en,
	scui_font_lang_eu,
	scui_font_lang_cjk,
} scui_font_lang_t;

typedef enum {
	SCUI_HANDLE_OFFSET_FONT = 0x4000 - 1,
	scui_font_ascii_8bin, // 0x4000
	scui_font_ascii_12bin, // 0x4001
	scui_font_ascii_16bin, // 0x4002
	scui_font_ascii_20bin, // 0x4003
	scui_font_ascii_24bin, // 0x4004
	scui_font_ascii_32bin, // 0x4005
	scui_font_ascii_40bin, // 0x4006
	scui_font_ascii_48bin, // 0x4007
	scui_font_ascii_56bin, // 0x4008
	scui_font_ascii_64bin, // 0x4009
	scui_font_ascii_72bin, // 0x400a
	scui_font_ascii_80bin, // 0x400b
	scui_font_ascii_88bin, // 0x400c
	scui_font_symbol_8bin, // 0x400d
	scui_font_symbol_12bin, // 0x400e
	scui_font_symbol_16bin, // 0x400f
	scui_font_symbol_20bin, // 0x4010
	scui_font_symbol_24bin, // 0x4011
	scui_font_symbol_32bin, // 0x4012
	scui_font_symbol_40bin, // 0x4013
	scui_font_symbol_48bin, // 0x4014
	scui_font_symbol_56bin, // 0x4015
	scui_font_symbol_64bin, // 0x4016
	scui_font_symbol_72bin, // 0x4017
	scui_font_symbol_80bin, // 0x4018
	scui_font_symbol_88bin, // 0x4019
	scui_font_en_32bin, // 0x401a
	scui_font_en_36bin, // 0x401b
	scui_font_eu_32bin, // 0x401c
	scui_font_eu_36bin, // 0x401d
	scui_font_cjk_32bin, // 0x401e
	scui_font_cjk_36bin, // 0x401f
	scui_font_tinyTTFttf, // 0x4020
} scui_font_type_t;

extern const void * const scui_font_package_table[33];

/*@brief 字库语言字号匹配
 *@param lang 字库语言类型
 *@param size 字库字号
 *@retval 字库句柄
 */
scui_handle_t scui_font_match(scui_font_lang_t lang, scui_handle_t size);

#endif
