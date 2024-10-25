/*本地静态的字库表
 */

#include "scui.h"

const char * scui_font_type_table[SCUI_FONT_TYPE_NUM - SCUI_HANDLE_OFFSET_FONT] = {
    [SCUI_FONT_TYPE_32_EN    - SCUI_HANDLE_OFFSET_FONT - 1] = "font_en_32.bin",
    [SCUI_FONT_TYPE_36_EN    - SCUI_HANDLE_OFFSET_FONT - 1] = "font_en_36.bin",
    [SCUI_FONT_TYPE_32_ZH    - SCUI_HANDLE_OFFSET_FONT - 1] = "font_zh_32.bin",
    [SCUI_FONT_TYPE_36_ZH    - SCUI_HANDLE_OFFSET_FONT - 1] = "font_zh_36.bin",
    [SCUI_FONT_TYPE_8_ASCII  - SCUI_HANDLE_OFFSET_FONT - 1] = "font_ascii_8.bin",
    [SCUI_FONT_TYPE_16_ASCII - SCUI_HANDLE_OFFSET_FONT - 1] = "font_ascii_16.bin",
    [SCUI_FONT_TYPE_24_ASCII - SCUI_HANDLE_OFFSET_FONT - 1] = "font_ascii_24.bin",
    [SCUI_FONT_TYPE_32_ASCII - SCUI_HANDLE_OFFSET_FONT - 1] = "font_ascii_32.bin",
    [SCUI_FONT_TYPE_40_ASCII - SCUI_HANDLE_OFFSET_FONT - 1] = "font_ascii_40.bin",
    [SCUI_FONT_TYPE_48_ASCII - SCUI_HANDLE_OFFSET_FONT - 1] = "font_ascii_48.bin",
    [SCUI_FONT_TYPE_56_ASCII - SCUI_HANDLE_OFFSET_FONT - 1] = "font_ascii_56.bin",
    [SCUI_FONT_TYPE_64_ASCII - SCUI_HANDLE_OFFSET_FONT - 1] = "font_ascii_64.bin",
    [SCUI_FONT_TYPE_72_ASCII - SCUI_HANDLE_OFFSET_FONT - 1] = "font_ascii_72.bin",
    [SCUI_FONT_TYPE_80_ASCII - SCUI_HANDLE_OFFSET_FONT - 1] = "font_ascii_80.bin",
    [SCUI_FONT_TYPE_88_ASCII - SCUI_HANDLE_OFFSET_FONT - 1] = "font_ascii_88.bin",
};

/*@brief 切换字库句柄到多国语语言编号
 *@param handle 字库句柄
 *@retval 语言编号(scui_multi_lang_type_t)
 */
scui_multi_lang_type_t scui_font_type_switch(scui_handle_t handle)
{
    switch (handle) {
    case SCUI_FONT_TYPE_32_EN:
    case SCUI_FONT_TYPE_36_EN:
        return scui_multi_lang_type_en;
    case SCUI_FONT_TYPE_32_ZH:
    case SCUI_FONT_TYPE_36_ZH:
        return scui_multi_lang_type_zh;
    default:
        SCUI_ASSERT(false);
        return SCUI_HANDLE_INVALID;
    }
}
