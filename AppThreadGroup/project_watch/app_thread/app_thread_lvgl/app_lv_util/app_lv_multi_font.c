/*实现目标:
 *    lvgl多字体库接口(当前暂时不做其他字体库)
 *    通常设备中的文本信息一共分为俩类
 *    第一类是兼容多国的语言字体
 *        这一类字体通常用于主字体以及默认字体
 *        它主要书写用于表示变化的文本
 *    第二类是以特殊书写形成的文本
 *        这一类字体通常统一使用英语字体
 *    剩下特殊的如符号字体等
 *    统一使用图形切片制作
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/* 字体类型,所有尺寸,计数器用于检测如果非默认字体尺寸下何时卸载 */
static uint32_t   app_lv_multi_font_cnt  = 0;
static uint32_t   app_lv_multi_font_size = 0;
static uint32_t   app_lv_multi_font_type = 0;
/* 地址绑定到inst而不是obj上,以达到动态切换的效果 */
static lv_font_t *app_lv_multi_font_obj[app_lv_multi_font_size_num] = {0};
static lv_font_t  app_lv_multi_font_inst[app_lv_multi_font_size_num] = {0};

/*@brief 更新主题多字库
 *@param font 字库对象
 */
static void app_lv_multi_font_config_theme(lv_font_t *font)
{
    APP_SYS_ASSERT(font != NULL);
    /* 获得默认显示器及其默认主题,更新它 */
    lv_disp_t *disp = lv_disp_get_default();
    lv_theme_t *theme = lv_disp_get_theme(disp);
    /* 重置显示器主题(它会自动更新风格) */
    lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE),
                                lv_palette_main(LV_PALETTE_RED),
                                LV_THEME_DEFAULT_DARK, font);
    lv_disp_set_theme(disp, theme);
}

/*@brief 加载动态多字库表
 *@param type 字库尺寸
 */
static void app_lv_multi_font_obj_load(uint32_t size)
{
    if (app_lv_multi_font_obj[size] != NULL)
        return;
    /*  */
    if (size == app_lv_multi_font_size_24) {
        if (app_lv_multi_font_type == app_lv_multi_font_type_chinese)
            app_lv_multi_font_obj[size] = lv_font_load("S:lvgl_font/font_zh_24.bin");
        if (app_lv_multi_font_type == app_lv_multi_font_type_english)
            app_lv_multi_font_obj[size] = lv_font_load("S:lvgl_font/font_en_24.bin");
    }
    /*  */
    if (size == app_lv_multi_font_size_36) {
        if (app_lv_multi_font_type == app_lv_multi_font_type_chinese)
            app_lv_multi_font_obj[size] = lv_font_load("S:lvgl_font/font_zh_36.bin");
        if (app_lv_multi_font_type == app_lv_multi_font_type_english)
            app_lv_multi_font_obj[size] = lv_font_load("S:lvgl_font/font_en_36.bin");
    }
    memcpy(&app_lv_multi_font_inst[size], app_lv_multi_font_obj[size], sizeof(lv_font_t));
    /*  */
}

/*@brief 卸载动态多字库表
 *@param type 字库尺寸
 */
static void app_lv_multi_font_obj_free(uint32_t size)
{
    if (app_lv_multi_font_obj[size] == NULL)
        return;
    /*  */
    if (size == app_lv_multi_font_size_24) {
        if (app_lv_multi_font_type == app_lv_multi_font_type_chinese)
            lv_font_free(app_lv_multi_font_obj[size]);
        if (app_lv_multi_font_type == app_lv_multi_font_type_english)
            lv_font_free(app_lv_multi_font_obj[size]);
    }
    /*  */
    if (size == app_lv_multi_font_size_36) {
        if (app_lv_multi_font_type == app_lv_multi_font_type_chinese)
            lv_font_free(app_lv_multi_font_obj[size]);
        if (app_lv_multi_font_type == app_lv_multi_font_type_english)
            lv_font_free(app_lv_multi_font_obj[size]);
    }
    memcpy(&app_lv_multi_font_inst[size], LV_FONT_DEFAULT, sizeof(lv_font_t));
    app_lv_multi_font_obj[size] = NULL;
    /*  */
}

/*@brief 配置指定类型的多字库表
 *       会先卸载之前指定类型的所有尺寸字库
 *       但不会批量加载本类型的所有尺寸字库(迟延到获取时加载)
 *@param type 字库类型
 */
void app_lv_multi_font_type_config(uint32_t type)
{
    APP_SYS_ASSERT(app_lv_multi_font_type_num >= type && 0 != type);
    
    if (app_lv_multi_font_type == type)
        return;
    
    /* 先更新主题的字体为默认字体(解锁对原字体的使用) */
    app_lv_multi_font_config_theme(LV_FONT_DEFAULT);
    for (uint8_t idx = 0; idx < app_lv_multi_font_size_num; idx++)
         app_lv_multi_font_obj_free(idx);
    /*  */
    app_lv_multi_font_type = type;
    /*  */
    switch (app_lv_multi_font_type)
    {
    case app_lv_multi_font_type_chinese:
        app_lv_lang_type_config(0);
        break;
    case app_lv_multi_font_type_english:
        app_lv_lang_type_config(1);
        break;
    default:
        break;
    }
    
    if (app_lv_multi_font_size_num >= app_lv_multi_font_size && 0 != app_lv_multi_font_size)
    {
        app_lv_multi_font_obj_load(app_lv_multi_font_size);
        app_lv_multi_font_config_theme(&app_lv_multi_font_inst[app_lv_multi_font_size]);
    }
}

/*@brief 配置指定类型的多字库表
 *       会先卸载之前指定类型的所有尺寸字库
 *       但不会批量加载本类型的所有尺寸字库(迟延到获取时加载)
 *@param type 字库类型
 */
void app_lv_multi_font_size_config(uint32_t size)
{
    APP_SYS_ASSERT(app_lv_multi_font_size_num >= size && 0 != size);
    
    if (app_lv_multi_font_size == size)
        return;
    
    app_lv_multi_font_obj_free(app_lv_multi_font_size);
    app_lv_multi_font_obj_load(size);
    app_lv_multi_font_size = size;
    
    app_lv_multi_font_config_theme(&app_lv_multi_font_inst[size]);
}

/*@brief 获得默认尺寸动态多字库表
 */
lv_font_t * app_lv_multi_font_def(void)
{
    APP_SYS_ASSERT(app_lv_multi_font_size_num >= app_lv_multi_font_size && 0 != app_lv_multi_font_size);
    app_lv_multi_font_load(app_lv_multi_font_size);
    return &app_lv_multi_font_inst[app_lv_multi_font_size];
}

/*@brief 加载动态多字库表
 *@param type 字库尺寸
 */
lv_font_t * app_lv_multi_font_load(uint32_t size)
{
    APP_SYS_ASSERT(app_lv_multi_font_size_num >= size && 0 != size);
    
    app_lv_multi_font_obj_load(size);
    if (app_lv_multi_font_size == size)
        app_lv_multi_font_cnt++;
    
    return &app_lv_multi_font_inst[size];
}

/*@brief 卸载动态多字库表
 *@param type 字库尺寸
 */
void app_lv_multi_font_free(uint32_t size)
{
    APP_SYS_ASSERT(app_lv_multi_font_size_num >= size && 0 != size);
    
    if (app_lv_multi_font_size == size)
    if (app_lv_multi_font_cnt  != 0) {
        app_lv_multi_font_cnt--;
    if (app_lv_multi_font_cnt  != 0)
        return;
    }
    
    app_lv_multi_font_obj_free(size);
}

/*@brief 获取内部静态字库
 *@param font_size 字库尺寸
 */
lv_font_t * app_lv_ui_font(uint32_t size)
{
    lv_font_t *font = lv_font_default();
    /* 太多了,挤在一起 */
    switch (size) {
    case  8: if (LV_FONT_MONTSERRAT_8)      font = &lv_font_montserrat_8;   break;
    case 10: if (LV_FONT_MONTSERRAT_10)     font = &lv_font_montserrat_10;  break;
    case 12: if (LV_FONT_MONTSERRAT_12)     font = &lv_font_montserrat_12;  break;
    case 14: if (LV_FONT_MONTSERRAT_14)     font = &lv_font_montserrat_14;  break;
    case 16: if (LV_FONT_MONTSERRAT_16)     font = &lv_font_montserrat_16;  break;
    case 18: if (LV_FONT_MONTSERRAT_18)     font = &lv_font_montserrat_18;  break;
    case 20: if (LV_FONT_MONTSERRAT_20)     font = &lv_font_montserrat_20;  break;
    case 22: if (LV_FONT_MONTSERRAT_22)     font = &lv_font_montserrat_22;  break;
    case 24: if (LV_FONT_MONTSERRAT_24)     font = &lv_font_montserrat_24;  break;
    case 26: if (LV_FONT_MONTSERRAT_26)     font = &lv_font_montserrat_26;  break;
    case 28: if (LV_FONT_MONTSERRAT_28)     font = &lv_font_montserrat_28;  break;
    case 30: if (LV_FONT_MONTSERRAT_30)     font = &lv_font_montserrat_30;  break;
    case 32: if (LV_FONT_MONTSERRAT_32)     font = &lv_font_montserrat_32;  break;
    case 34: if (LV_FONT_MONTSERRAT_34)     font = &lv_font_montserrat_34;  break;
    case 36: if (LV_FONT_MONTSERRAT_36)     font = &lv_font_montserrat_36;  break;
    case 38: if (LV_FONT_MONTSERRAT_38)     font = &lv_font_montserrat_38;  break;
    case 40: if (LV_FONT_MONTSERRAT_40)     font = &lv_font_montserrat_40;  break;
    case 42: if (LV_FONT_MONTSERRAT_42)     font = &lv_font_montserrat_42;  break;
    case 44: if (LV_FONT_MONTSERRAT_44)     font = &lv_font_montserrat_44;  break;
    case 46: if (LV_FONT_MONTSERRAT_46)     font = &lv_font_montserrat_46;  break;
    case 48: if (LV_FONT_MONTSERRAT_48)     font = &lv_font_montserrat_48;  break;
    default: break;
    }
    APP_SYS_ASSERT(font != NULL);
    return font;
}
