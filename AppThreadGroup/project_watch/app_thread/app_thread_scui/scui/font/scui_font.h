#ifndef SCUI_FONT_H
#define SCUI_FONT_H

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

/*@brief 计算调色板值
 *@param bitmap 位图
 *@param bpp_x  偏移值
 *@retval 调色板值
 */
uint8_t scui_font_bpp_palette(uint8_t bitmap, uint8_t bpp, uint8_t bpp_x);

/*@brief 字库加载
 *@param name   字库名称
 *@param handle 字库句柄
 */
void scui_font_load(char *name, scui_handle_t *handle);

/*@brief 字库卸载
 *@param handle 字库句柄
 */
void scui_font_unload(scui_handle_t handle);

/*@brief 字库大小
 *@param handle 字库句柄
 *@retval 字库大小
 */
uint32_t scui_font_size(scui_handle_t handle);

/*@brief 字型加载
 *@param glyph 字形信息
 */
void scui_font_glyph_load(scui_font_glyph_t *glyph);

/*@brief 字型卸载
 *@param glyph 字形信息
 */
void scui_font_glyph_unload(scui_font_glyph_t *glyph);

#endif
