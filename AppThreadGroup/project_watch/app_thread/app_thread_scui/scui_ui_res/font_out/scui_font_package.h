#ifndef SCUI_FONT_PACKAGE_H
#define SCUI_FONT_PACKAGE_H

/*一个通过scui_font_package脚本生成的font表
 */

typedef enum {
	scui_font_lang_ascii,
	scui_font_lang_multi,
	scui_font_lang_symbol,
	scui_font_lang_en,
	scui_font_lang_eu,
	scui_font_lang_cjk,
} scui_font_lang_t;

typedef enum {
	SCUI_HANDLE_OFFSET_FONT = 0x3000 - 1,
	scui_font_ascii_8bin, // 0x3000
	scui_font_ascii_12bin, // 0x3001
	scui_font_ascii_16bin, // 0x3002
	scui_font_ascii_20bin, // 0x3003
	scui_font_ascii_24bin, // 0x3004
	scui_font_ascii_32bin, // 0x3005
	scui_font_ascii_40bin, // 0x3006
	scui_font_ascii_48bin, // 0x3007
	scui_font_ascii_56bin, // 0x3008
	scui_font_ascii_64bin, // 0x3009
	scui_font_ascii_72bin, // 0x300a
	scui_font_ascii_80bin, // 0x300b
	scui_font_ascii_88bin, // 0x300c
	scui_font_symbol_8bin, // 0x300d
	scui_font_symbol_12bin, // 0x300e
	scui_font_symbol_16bin, // 0x300f
	scui_font_symbol_20bin, // 0x3010
	scui_font_symbol_24bin, // 0x3011
	scui_font_symbol_32bin, // 0x3012
	scui_font_symbol_40bin, // 0x3013
	scui_font_symbol_48bin, // 0x3014
	scui_font_symbol_56bin, // 0x3015
	scui_font_symbol_64bin, // 0x3016
	scui_font_symbol_72bin, // 0x3017
	scui_font_symbol_80bin, // 0x3018
	scui_font_symbol_88bin, // 0x3019
	scui_font_en_32bin, // 0x301a
	scui_font_en_36bin, // 0x301b
	scui_font_eu_32bin, // 0x301c
	scui_font_eu_36bin, // 0x301d
	scui_font_cjk_32bin, // 0x301e
	scui_font_cjk_36bin, // 0x301f
	scui_font_tinyTTFttf, // 0x3020
} scui_font_type_t;

extern const void * const scui_font_package_table[33];

/*@brief 字库语言字号匹配
 *@param lang 字库语言类型
 *@param size 字库字号
 *@retval 字库句柄
 */
scui_handle_t scui_font_match(scui_font_lang_t lang, scui_handle_t size);

#endif
