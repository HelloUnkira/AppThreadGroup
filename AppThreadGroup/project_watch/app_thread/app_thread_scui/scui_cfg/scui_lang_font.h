#ifndef SCUI_LANG_FONT_H
#define SCUI_LANG_FONT_H

typedef enum {
    SCUI_FONT_IDX_32,
    SCUI_FONT_IDX_36,
    SCUI_FONT_IDX_MZ,
    SCUI_FONT_IDX_X8,
    SCUI_FONT_IDX_X12,
    SCUI_FONT_IDX_X16,
    SCUI_FONT_IDX_X20,
    SCUI_FONT_IDX_X24,
    SCUI_FONT_IDX_X32,
    SCUI_FONT_IDX_X40,
    SCUI_FONT_IDX_X48,
    SCUI_FONT_IDX_X56,
    SCUI_FONT_IDX_X64,
    SCUI_FONT_IDX_X72,
    SCUI_FONT_IDX_X80,
    SCUI_FONT_IDX_X88,
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
