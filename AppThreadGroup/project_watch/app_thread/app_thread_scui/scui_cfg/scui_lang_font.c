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
        case SCUI_FONT_IDX_A8:  return scui_font_ascii_8bin;
        case SCUI_FONT_IDX_A16: return scui_font_ascii_16bin;
        case SCUI_FONT_IDX_A24: return scui_font_ascii_24bin;
        case SCUI_FONT_IDX_A32: return scui_font_ascii_32bin;
        case SCUI_FONT_IDX_A40: return scui_font_ascii_40bin;
        case SCUI_FONT_IDX_A48: return scui_font_ascii_48bin;
        case SCUI_FONT_IDX_A56: return scui_font_ascii_56bin;
        case SCUI_FONT_IDX_A64: return scui_font_ascii_64bin;
        case SCUI_FONT_IDX_A72: return scui_font_ascii_72bin;
        case SCUI_FONT_IDX_A80: return scui_font_ascii_80bin;
        case SCUI_FONT_IDX_A88: return scui_font_ascii_88bin;
        }
    }
    case scui_lang_type_zh: {
        switch (font_idx) {
        case SCUI_FONT_IDX_32: return scui_font_match(scui_font_lang_zh, 32);
        case SCUI_FONT_IDX_36: return scui_font_match(scui_font_lang_zh, 36);
        }
    }
    case scui_lang_type_en: {
        switch (font_idx) {
        case SCUI_FONT_IDX_32: return scui_font_match(scui_font_lang_en, 32);
        case SCUI_FONT_IDX_36: return scui_font_match(scui_font_lang_en, 36);
        }
    }
    }
    
    /* 如果有通配字库 */
    return scui_font_testttf;
    
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
