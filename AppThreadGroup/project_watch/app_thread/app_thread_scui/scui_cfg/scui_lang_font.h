#ifndef SCUI_LANG_FONT_H
#define SCUI_LANG_FONT_H

typedef enum {
    SCUI_FONT_IDX_32,
    SCUI_FONT_IDX_36,
    SCUI_FONT_IDX_MZ,
    SCUI_FONT_IDX_A8,
    SCUI_FONT_IDX_A16,
    SCUI_FONT_IDX_A24,
    SCUI_FONT_IDX_A32,
    SCUI_FONT_IDX_A40,
    SCUI_FONT_IDX_A48,
    SCUI_FONT_IDX_A56,
    SCUI_FONT_IDX_A64,
    SCUI_FONT_IDX_A72,
    SCUI_FONT_IDX_A80,
    SCUI_FONT_IDX_A88,
} scui_font_type_idx_t;

/*@brief 字库名称转换
 *@param font_idx  字库名称编号
 *@param lang_type 语言类型
 */
scui_handle_t scui_font_name_match(scui_handle_t font_idx, scui_handle_t lang_type);

/*@brief 字库尺寸转换
 *@param font_idx  字库名称编号
 *@param font_size 字库尺寸
 */
scui_handle_t scui_font_size_match(scui_handle_t font_idx, scui_handle_t font_size);

#endif
