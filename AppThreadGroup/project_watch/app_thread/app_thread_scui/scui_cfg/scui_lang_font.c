/*实现目标:
 *    本地静态的字库表
 *    一些无法入库却重要的接口
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 字库名称转换
 *@param font_idx  字库名称编号
 *@param lang_type 语言类型
 */
scui_handle_t scui_font_name_match(scui_handle_t font_idx, scui_handle_t lang_type)
{
    switch (lang_type) {
    case scui_lang_type_multi: {
        SCUI_ASSERT(font_idx == SCUI_FONT_IDX_MZ);
        break;
    }
    case scui_lang_type_ascii: {
        switch (font_idx) {
        case SCUI_FONT_IDX_X8:  return scui_font_ascii_8bin;
        case SCUI_FONT_IDX_X12: return scui_font_ascii_12bin;
        case SCUI_FONT_IDX_X16: return scui_font_ascii_16bin;
        case SCUI_FONT_IDX_X20: return scui_font_ascii_20bin;
        case SCUI_FONT_IDX_X24: return scui_font_ascii_24bin;
        case SCUI_FONT_IDX_X32: return scui_font_ascii_32bin;
        case SCUI_FONT_IDX_X40: return scui_font_ascii_40bin;
        case SCUI_FONT_IDX_X48: return scui_font_ascii_48bin;
        case SCUI_FONT_IDX_X56: return scui_font_ascii_56bin;
        case SCUI_FONT_IDX_X64: return scui_font_ascii_64bin;
        case SCUI_FONT_IDX_X72: return scui_font_ascii_72bin;
        case SCUI_FONT_IDX_X80: return scui_font_ascii_80bin;
        case SCUI_FONT_IDX_X88: return scui_font_ascii_88bin;
        }
        break;
    }
    case scui_lang_type_symbol: {
        switch (font_idx) {
        case SCUI_FONT_IDX_X8:  return scui_font_symbol_8bin;
        case SCUI_FONT_IDX_X12: return scui_font_symbol_12bin;
        case SCUI_FONT_IDX_X16: return scui_font_symbol_16bin;
        case SCUI_FONT_IDX_X20: return scui_font_symbol_20bin;
        case SCUI_FONT_IDX_X24: return scui_font_symbol_24bin;
        case SCUI_FONT_IDX_X32: return scui_font_symbol_32bin;
        case SCUI_FONT_IDX_X40: return scui_font_symbol_40bin;
        case SCUI_FONT_IDX_X48: return scui_font_symbol_48bin;
        case SCUI_FONT_IDX_X56: return scui_font_symbol_56bin;
        case SCUI_FONT_IDX_X64: return scui_font_symbol_64bin;
        case SCUI_FONT_IDX_X72: return scui_font_symbol_72bin;
        case SCUI_FONT_IDX_X80: return scui_font_symbol_80bin;
        case SCUI_FONT_IDX_X88: return scui_font_symbol_88bin;
        }
        break;
    }
    case scui_lang_type_en: {
        switch (font_idx) {
        case SCUI_FONT_IDX_32: return scui_font_match(scui_font_lang_en, 32);
        case SCUI_FONT_IDX_36: return scui_font_match(scui_font_lang_en, 36);
        }
        break;
    }
    case scui_lang_type_fra:
    case scui_lang_type_de:
    case scui_lang_type_nl:
    case scui_lang_type_pt: {
        switch (font_idx) {
        case SCUI_FONT_IDX_32: return scui_font_match(scui_font_lang_eu, 32);
        case SCUI_FONT_IDX_36: return scui_font_match(scui_font_lang_eu, 36);
        }
        break;
    }
    case scui_lang_type_jp:
    case scui_lang_type_zh: {
        switch (font_idx) {
        case SCUI_FONT_IDX_32: return scui_font_match(scui_font_lang_cjk, 32);
        case SCUI_FONT_IDX_36: return scui_font_match(scui_font_lang_cjk, 36);
        }
        break;
    }
    }
    
    /* 如果有通配字库 */
    return scui_font_tinyTTFttf;
    
    /* 如果没有通配字库 */
    SCUI_ASSERT(false);
    return SCUI_HANDLE_INVALID;
}

/*@brief 字库尺寸转换
 *@param font_idx  字库名称编号
 *@param font_size 字库尺寸
 */
scui_handle_t scui_font_size_match(scui_handle_t font_idx, scui_handle_t font_size)
{
    /* 这里无需转换, 做一个检查即可 */
    if (font_idx == SCUI_FONT_IDX_MZ)
        SCUI_ASSERT(font_size != 0);
    
    return font_size;
}
