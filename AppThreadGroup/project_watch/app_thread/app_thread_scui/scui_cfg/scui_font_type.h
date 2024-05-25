#ifndef SCUI_FONT_TYPE_H
#define SCUI_FONT_TYPE_H

typedef enum {
	SCUI_HANDLE_OFFSET_FONT = 0x3000 - 1,
    SCUI_FONT_TYPE_32_EN,
    SCUI_FONT_TYPE_36_EN,
    SCUI_FONT_TYPE_32_ZH,
    SCUI_FONT_TYPE_36_ZH,
    SCUI_FONT_TYPE_NUM,
} scui_font_type_table_t;

extern const char * scui_font_type_table[SCUI_FONT_TYPE_NUM - SCUI_HANDLE_OFFSET_FONT];

/*@brief 切换字库句柄到多国语语言编号
 *@param handle 字库句柄
 *@retval 语言编号(scui_multi_lang_type_t)
 */
scui_multi_lang_type_t scui_font_type_switch(scui_handle_t handle);

#endif
