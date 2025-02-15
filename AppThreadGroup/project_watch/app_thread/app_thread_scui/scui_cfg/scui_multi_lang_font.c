/*实现目标:
 *    本地静态的字库表
 *    一些无法入库却重要的接口
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_multi_lang_type_t scui_multi_lang_type = 0;

const char * scui_font_type_table[SCUI_FONT_TYPE_NUM - SCUI_HANDLE_OFFSET_FONT] = {
    [SCUI_FONT_TYPE_32_EN    - SCUI_HANDLE_OFFSET_FONT - 1] = "font_en_32.bin",
    [SCUI_FONT_TYPE_36_EN    - SCUI_HANDLE_OFFSET_FONT - 1] = "font_en_36.bin",
    [SCUI_FONT_TYPE_32_ZH    - SCUI_HANDLE_OFFSET_FONT - 1] = "font_zh_32.bin",
    [SCUI_FONT_TYPE_36_ZH    - SCUI_HANDLE_OFFSET_FONT - 1] = "font_zh_36.bin",
    
    [SCUI_FONT_TYPE_MZ       - SCUI_HANDLE_OFFSET_FONT - 1] = "ttf_tiny_file.ttf",
    
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

/*@brief 字库名称转换
 *@param font_idx  字库名称编号
 *@param lang_type 语言类型
 */
scui_handle_t scui_font_name_match(scui_handle_t font_idx, scui_handle_t lang_type)
{
    switch (lang_type) {
    case SCUI_MULTI_LANG_TYPE_MZ: {
        SCUI_ASSERT(font_idx == SCUI_FONT_IDX_MZ);
        return SCUI_FONT_TYPE_MZ;
    }
    case SCUI_MULTI_LANG_TYPE_A: {
        switch (font_idx) {
        case SCUI_FONT_IDX_A8:  return SCUI_FONT_TYPE_8_ASCII;
        case SCUI_FONT_IDX_A16: return SCUI_FONT_TYPE_16_ASCII;
        case SCUI_FONT_IDX_A24: return SCUI_FONT_TYPE_24_ASCII;
        case SCUI_FONT_IDX_A32: return SCUI_FONT_TYPE_32_ASCII;
        case SCUI_FONT_IDX_A40: return SCUI_FONT_TYPE_40_ASCII;
        case SCUI_FONT_IDX_A48: return SCUI_FONT_TYPE_48_ASCII;
        case SCUI_FONT_IDX_A56: return SCUI_FONT_TYPE_56_ASCII;
        case SCUI_FONT_IDX_A64: return SCUI_FONT_TYPE_64_ASCII;
        case SCUI_FONT_IDX_A72: return SCUI_FONT_TYPE_72_ASCII;
        case SCUI_FONT_IDX_A80: return SCUI_FONT_TYPE_80_ASCII;
        case SCUI_FONT_IDX_A88: return SCUI_FONT_TYPE_88_ASCII;
        default: SCUI_ASSERT(false);
        }
    }
    case scui_multi_lang_type_zh: {
        switch (font_idx) {
        case SCUI_FONT_IDX_32: return SCUI_FONT_TYPE_32_ZH;
        case SCUI_FONT_IDX_36: return SCUI_FONT_TYPE_36_ZH;
        case SCUI_FONT_IDX_MZ: return SCUI_FONT_TYPE_MZ;
        default: SCUI_ASSERT(false);
        }
    }
    case scui_multi_lang_type_en: {
        switch (font_idx) {
        case SCUI_FONT_IDX_32: return SCUI_FONT_TYPE_32_EN;
        case SCUI_FONT_IDX_36: return SCUI_FONT_TYPE_36_EN;
        case SCUI_FONT_IDX_MZ: return SCUI_FONT_TYPE_MZ;
        default: SCUI_ASSERT(false);
        }
    }
    default:
        SCUI_ASSERT(false);
    }
    
    return SCUI_HANDLE_INVALID;
}

/*@brief 字库尺寸转换
 *@param font_idx  字库名称编号
 *@param font_size 字库尺寸
 */
scui_handle_t scui_font_size_match(scui_handle_t font_idx, scui_handle_t font_size)
{
    // 这里无需转换, 做一个检查即可
    if (font_idx == SCUI_FONT_IDX_MZ)
        SCUI_ASSERT(font_size != 0);
    
    return font_size;
}

/*@brief 设置多国语语言类型
 *@param type 语言类型编号(scui_multi_lang_type_t)
 */
void scui_multi_lang_set(scui_handle_t *type)
{
    SCUI_ASSERT(type != NULL);
    scui_multi_lang_type = *type;
    
    scui_event_t event = {
        .object = SCUI_HANDLE_SYSTEM,
        .type   = scui_event_change_lang,
        .absorb = scui_event_absorb_none,
    };
    scui_event_notify(&event);
}

/*@brief 获取多国语语言类型
 *@param type 语言类型编号(scui_multi_lang_type_t)
 */
void scui_multi_lang_get(scui_handle_t *type)
{
    SCUI_ASSERT(type != NULL);
    *type = scui_multi_lang_type;
}

/*@brief 多国语字符串转换
 *@param handle 字符串句柄
 *@param type   语言类型编号(scui_multi_lang_type_t)
 *@retval 字符串
 */
const char * scui_multi_lang_str(scui_handle_t handle, scui_handle_t type)
{
    scui_handle_t string = handle;
    string += SCUI_MULTI_LANG_TYPE_MZ == type ? scui_multi_lang_type : type;
    string -= SCUI_MULTI_LANG_NUM_OFS;
    return scui_handle_source(string);
}
