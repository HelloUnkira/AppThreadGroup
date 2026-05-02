#ifndef SCUI_FONT_PACKAGE_H
#define SCUI_FONT_PACKAGE_H

/*一个通过scui_font_package脚本生成的font表
 */

typedef enum {
	scui_font_lang_ascii,
	scui_font_lang_multi,
	scui_font_lang_en,
	scui_font_lang_zh,
} scui_font_lang_t;

typedef enum {
	SCUI_HANDLE_OFFSET_FONT = 0x3000 - 1,
	scui_font_ascii_8bin, // 0x3000
	scui_font_ascii_16bin, // 0x3001
	scui_font_ascii_24bin, // 0x3002
	scui_font_ascii_32bin, // 0x3003
	scui_font_ascii_40bin, // 0x3004
	scui_font_ascii_48bin, // 0x3005
	scui_font_ascii_56bin, // 0x3006
	scui_font_ascii_64bin, // 0x3007
	scui_font_ascii_72bin, // 0x3008
	scui_font_ascii_80bin, // 0x3009
	scui_font_ascii_88bin, // 0x300a
	scui_font_en_32bin, // 0x300b
	scui_font_en_36bin, // 0x300c
	scui_font_zh_32bin, // 0x300d
	scui_font_zh_36bin, // 0x300e
	scui_font_testttf, // 0x300f
} scui_font_type_t;

extern const void * scui_font_package_table[16];

/*@brief 字库语言字号匹配
 *@param lang 字库语言类型
 *@param size 字库字号
 *@retval 字库句柄
 */
scui_handle_t scui_font_match(scui_font_lang_t lang, scui_handle_t size);

#endif
