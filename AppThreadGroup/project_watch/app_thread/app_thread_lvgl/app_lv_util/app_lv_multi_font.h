#ifndef APP_LV_MULTI_FONT_H
#define APP_LV_MULTI_FONT_H

typedef enum {
    app_lv_multi_font_type_none,
    app_lv_multi_font_type_chinese,
    app_lv_multi_font_type_english,
    app_lv_multi_font_type_num,
} app_lv_multi_font_type_t;
    
typedef enum {
    app_lv_multi_font_size_none,
    app_lv_multi_font_size_24,
    app_lv_multi_font_size_36,
    app_lv_multi_font_size_num,
} app_lv_multi_font_size_t;

/* 默认多字库表标号 */
#define APP_LV_MULTI_FONT_TYPE_DEFAULT  app_lv_multi_font_type_english
#define APP_LV_MULTI_FONT_SIZE_DEFAULT  app_lv_multi_font_size_24

/*@brief     配置指定类型的多字库表
 *           会先卸载之前指定类型的所有尺寸字库
 *           但不会批量加载本类型的所有尺寸字库(迟延到获取时加载)
 *@param[in] type 字库类型
 */
void app_lv_multi_font_type_config(uint32_t type);

/*@brief     配置指定类型的多字库表
 *           会先卸载之前指定类型的所有尺寸字库
 *           但不会批量加载本类型的所有尺寸字库(迟延到获取时加载)
 *@param[in] type 字库类型
 */
void app_lv_multi_font_size_config(uint32_t size);

/*@brief 获得默认尺寸动态多字库表
 */
lv_font_t * app_lv_multi_font_def(void);

/*@brief     加载动态多字库表
 *@param[in] type 字库尺寸
 */
lv_font_t * app_lv_multi_font_load(uint32_t size);

/*@brief     卸载动态多字库表
 *@param[in] type 字库尺寸
 */
void app_lv_multi_font_free(uint32_t size);

/*@brief     获取内部静态字库
 *@param[in] font_size 字库尺寸
 */
lv_font_t * app_lv_ui_font(uint32_t size);

#endif
