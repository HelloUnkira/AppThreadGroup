#ifndef SCUI_FONT_TYPE_H
#define SCUI_FONT_TYPE_H

typedef enum {
    SCUI_FONT_SIZE_32 = 0,
    SCUI_FONT_SIZE_36,
} scui_font_type_size_t;

typedef enum {
    SCUI_HANDLE_OFFSET_FONT = 0x3000 - 1,
    SCUI_FONT_TYPE_EN,      // 系列
    SCUI_FONT_TYPE_32_EN    = SCUI_FONT_TYPE_EN,
    SCUI_FONT_TYPE_36_EN,
    SCUI_FONT_TYPE_ZH,      // 系列
    SCUI_FONT_TYPE_32_ZH    = SCUI_FONT_TYPE_ZH,
    SCUI_FONT_TYPE_36_ZH,
    SCUI_FONT_TYPE_ASCII,   // 系列
    SCUI_FONT_TYPE_8_ASCII  = SCUI_FONT_TYPE_ASCII,
    SCUI_FONT_TYPE_16_ASCII,
    SCUI_FONT_TYPE_24_ASCII,
    SCUI_FONT_TYPE_32_ASCII,
    SCUI_FONT_TYPE_40_ASCII,
    SCUI_FONT_TYPE_48_ASCII,
    SCUI_FONT_TYPE_56_ASCII,
    SCUI_FONT_TYPE_64_ASCII,
    SCUI_FONT_TYPE_72_ASCII,
    SCUI_FONT_TYPE_80_ASCII,
    SCUI_FONT_TYPE_88_ASCII,
    SCUI_FONT_TYPE_NUM,
} scui_font_type_table_t;

extern const char * scui_font_type_table[SCUI_FONT_TYPE_NUM - SCUI_HANDLE_OFFSET_FONT];

/*@brief 切换字库句柄到多国语语言编号
 *@param handle 字库句柄
 *@retval 语言编号(scui_multi_lang_type_t)
 */
scui_multi_lang_type_t scui_font_type_switch(scui_handle_t handle);

/*@brief 多国语字符串转换
 *@param font_idx   字库索引
 *@param multi_lang 多国语句柄
 *@retval 字符串
 */
uint8_t * scui_str_by_multi_lang(scui_handle_t font_idx, scui_handle_t multi_lang);

#endif
