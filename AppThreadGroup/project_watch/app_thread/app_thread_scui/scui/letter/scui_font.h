#ifndef SCUI_FONT_H
#define SCUI_FONT_H

typedef struct {
    
    uint16_t size;
    // size == 0: 使用固定字号字体
    // size != 0: 使用ttf_tiny字体
    union {
    void *bmp_fixed;
    void *ttf_tiny;
    };
    
} scui_font_t;

typedef struct {
    
    /* 字形加载时动态生成,卸载时动态销毁 */
    uint8_t *bitmap;
    uint32_t bitmap_size;
    
    /* lvgl式: */
    uint16_t adv_w;
    uint16_t box_w;
    uint16_t box_h;
    int16_t ofs_x;
    int16_t ofs_y;
    uint8_t bpp;
    
    /* 字形信息附加信息: */
    
    uint32_t unicode_letter;
    uint32_t unicode_letter_next;
    uint32_t space_width;
    
    /* 字体句柄: */
    scui_handle_t handle;
    
} scui_font_glyph_t;

/*@brief 字库加载
 *@param name   字库名称
 *@param size   字库字号
 *@param handle 字库句柄
 */
void scui_font_load(char *name, uint32_t size, scui_handle_t *handle);

/*@brief 字库卸载
 *@param handle 字库句柄
 */
void scui_font_unload(scui_handle_t handle);

/*@brief 字库大小
 *@param handle 字库句柄
 *@retval 字库大小
 */
uint32_t scui_font_size(scui_handle_t handle);

/*@brief 字库参数信息
 *@param handle 字库句柄
 *@retval 字库参数信息
 */
scui_coord_t scui_font_base_line(scui_handle_t handle);

/*@brief 字库参数信息
 *@param handle 字库句柄
 *@retval 字库参数信息
 */
scui_coord_t scui_font_line_height(scui_handle_t handle);

/*@brief 字库参数信息
 *@param handle 字库句柄
 *@retval 字库参数信息
 */
scui_coord_t scui_font_underline(scui_handle_t handle);

/*@brief 字型加载
 *@param glyph 字形信息
 */
void scui_font_glyph_load(scui_font_glyph_t *glyph);

/*@brief 字型卸载
 *@param glyph 字形信息
 */
void scui_font_glyph_unload(scui_font_glyph_t *glyph);

#endif
