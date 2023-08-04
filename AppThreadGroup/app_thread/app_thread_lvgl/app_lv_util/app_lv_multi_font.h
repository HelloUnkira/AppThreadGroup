#ifndef APP_LV_MULTI_FONT_H
#define APP_LV_MULTI_FONT_H

/* 字库尺寸表 */
/* 在该表中的字号才有对应的动态字库字体 */
/* 否则使用系统默认的字体 */
typedef enum {
    app_lv_multi_font_16,
    app_lv_multi_font_20,
    app_lv_multi_font_24,
    app_lv_multi_font_28,
    app_lv_multi_font_num,
} app_lv_multi_font_size_t;

typedef enum {
    app_lv_multi_font_default,
    app_lv_multi_font_english,
    app_lv_multi_font_chinese,
} app_lv_multi_font_type_t;

/* 默认多字库表标号 */
#define APP_LV_MULTI_FONT_DEFAULT   app_lv_multi_font_16

/*@brief     设置当前使用的多字库
 *@param[in] multi_font_type 多字库类型
 */
void app_lv_multi_font_update(uint32_t multi_font_type);

/*@brief     获取当前使用的多字库的指定尺寸
 *@param[in] multi_font_size 多字库尺寸
 */
lv_font_t * app_lv_multi_font(uint32_t multi_font_size);

/*@brief     获取内部静态字库
 *@param[in] font_size 字库尺寸
 */
lv_font_t * app_lv_ui_font(uint32_t font_size);

#endif
