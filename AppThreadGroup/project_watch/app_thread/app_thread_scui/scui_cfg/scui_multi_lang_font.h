#ifndef SCUI_MULTI_LANG_FONT_H
#define SCUI_MULTI_LANG_FONT_H

// 默认语言类型, 跟随系统配置
#define SCUI_MULTI_LANG_TYPE_MZ     (SCUI_HANDLE_SYSTEM)
#define SCUI_MULTI_LANG_TYPE_A      (SCUI_MULTI_LANG_TYPE_MZ - 1)

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

/*@brief 设置多国语语言类型
 *@param type 语言类型编号(scui_multi_lang_type_t)
 */
void scui_multi_lang_set(scui_handle_t *type);

/*@brief 获取多国语语言类型
 *@param type 语言类型编号(scui_multi_lang_type_t)
 */
void scui_multi_lang_get(scui_handle_t *type);

/*@brief 多国语字符串转换
 *@param handle 字符串句柄
 *@param type   语言类型编号(scui_multi_lang_type_t)
 *@retval 字符串
 */
const char * scui_multi_lang_str(scui_handle_t handle, scui_handle_t type);

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

typedef enum {
    SCUI_HANDLE_OFFSET_FONT = 0x3000 - 1,
    
    SCUI_FONT_TYPE_EN,      // 系列
    SCUI_FONT_TYPE_32_EN    = SCUI_FONT_TYPE_EN,
    SCUI_FONT_TYPE_36_EN,
    SCUI_FONT_TYPE_ZH,      // 系列
    SCUI_FONT_TYPE_32_ZH    = SCUI_FONT_TYPE_ZH,
    SCUI_FONT_TYPE_36_ZH,
    
    SCUI_FONT_TYPE_MZ,      // 系列
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

#endif
