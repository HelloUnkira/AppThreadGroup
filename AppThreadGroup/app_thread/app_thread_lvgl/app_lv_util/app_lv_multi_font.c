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

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static lv_font_t *app_lv_multi_font_table[app_lv_multi_font_num] = {0};

/*@brief 加载动态多字库表
 */
static inline void app_lv_multi_font_free(void)
{
    for (uint32_t idx = 0; idx < app_lv_multi_font_num; idx++) {
        lv_font_free(app_lv_multi_font_table[idx]);
        app_lv_multi_font_table[idx] = NULL;
    }
}

/*@brief 加载动态多字库表
 */
static inline void app_lv_multi_font_load(uint32_t multi_font_type)
{
    switch (multi_font_type) {
    case app_lv_multi_font_chinese:
        app_lv_multi_font_table[app_lv_multi_font_16] =
            lv_font_load("S:app_lv_multi_font_chinese_16.bin");
        app_lv_multi_font_table[app_lv_multi_font_20] =
            lv_font_load("S:app_lv_multi_font_chinese_20.bin");
        app_lv_multi_font_table[app_lv_multi_font_24] =
            lv_font_load("S:app_lv_multi_font_chinese_24.bin");
        app_lv_multi_font_table[app_lv_multi_font_28] =
            lv_font_load("S:app_lv_multi_font_chinese_28.bin");
        break;
    case app_lv_multi_font_default:
    case app_lv_multi_font_english:
        /* 使用默认静态字库表 */
    default:
        break;
    }
}

/*@brief     设置当前使用的多字库
 *@param[in] multi_font_type 多字库类型
 */
void app_lv_multi_font_update(uint32_t multi_font_type)
{
    /* 清空动态字库表 */
    app_lv_multi_font_free();
    /* 加载动态字库表 */
    app_lv_multi_font_load(multi_font_type);
    /* 更新默认主题字体 */
    lv_font_t *font = lv_font_default();
    if (NULL != app_lv_multi_font_table[APP_LV_MULTI_FONT_DEFAULT])
        font  = app_lv_multi_font_table[APP_LV_MULTI_FONT_DEFAULT];
    /* 获得默认显示器及其默认主题,更新它 */
    lv_disp_t *disp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_get_from_obj(disp);
    /* 重置显示器主题(它会自动更新风格) */
    lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE),
                                lv_palette_main(LV_PALETTE_RED),
                                LV_THEME_DEFAULT_DARK,
                                font);
    lv_disp_set_theme(disp, theme);
}

/*@brief     获取当前使用的多字库的指定尺寸
 *@param[in] multi_font_size 多字库尺寸
 */
lv_font_t * app_lv_multi_font(uint32_t multi_font_size)
{
    APP_SYS_ASSERT(app_lv_multi_font_num >= multi_font_size);
    lv_font_t *font = app_lv_multi_font_table[multi_font_size];
    /* 多字库没有, */
    if (font == NULL) {
        font  = lv_font_default();
        /* 返回目标默认静态字体 */
        switch (multi_font_size) {
        case app_lv_multi_font_16: {
            #if LV_FONT_MONTSERRAT_16
            font = &lv_font_montserrat_16;
            #endif
            break;
        }
        case app_lv_multi_font_20: {
            #if LV_FONT_MONTSERRAT_20
            font = &lv_font_montserrat_20;
            #endif
            break;
        }
        case app_lv_multi_font_24: {
            #if LV_FONT_MONTSERRAT_24
            font = &lv_font_montserrat_24;
            #endif
            break;
        }
        case app_lv_multi_font_28: {
            #if LV_FONT_MONTSERRAT_28
            font = &lv_font_montserrat_28;
            #endif
            break;
        }
        default:
            break;
        }
    }
    return font;
}

/*@brief     获取内部静态字库
 *@param[in] font_size 字库尺寸
 */
lv_font_t * app_lv_ui_font(uint32_t font_size)
{
    lv_font_t *font = lv_font_default();
    
    switch (font_size) {
    case 8: {
        #if LV_FONT_MONTSERRAT_8
        font = &lv_font_montserrat_8;
        #endif
        break;
    }
    case 10: {
        #if LV_FONT_MONTSERRAT_10
        font = &lv_font_montserrat_10;
        #endif
        break;
    }
    case 12: {
        #if LV_FONT_MONTSERRAT_12
        font = &lv_font_montserrat_12;
        #endif
        break;
    }
    case 14: {
        #if LV_FONT_MONTSERRAT_14
        font = &lv_font_montserrat_14;
        #endif
        break;
    }
    case 16: {
        #if LV_FONT_MONTSERRAT_16
        font = &lv_font_montserrat_16;
        #endif
        break;
    }
    case 18: {
        #if LV_FONT_MONTSERRAT_18
        font = &lv_font_montserrat_18;
        #endif
        break;
    }
    case 20: {
        #if LV_FONT_MONTSERRAT_20
        font = &lv_font_montserrat_20;
        #endif
        break;
    }
    case 22: {
        #if LV_FONT_MONTSERRAT_22
        font = &lv_font_montserrat_22;
        #endif
        break;
    }
    case 24: {
        #if LV_FONT_MONTSERRAT_24
        font = &lv_font_montserrat_24;
        #endif
        break;
    }
    case 26: {
        #if LV_FONT_MONTSERRAT_26
        font = &lv_font_montserrat_26;
        #endif
        break;
    }
    case 28: {
        #if LV_FONT_MONTSERRAT_28
        font = &lv_font_montserrat_28;
        #endif
        break;
    }
    case 30: {
        #if LV_FONT_MONTSERRAT_30
        font = &lv_font_montserrat_30;
        #endif
        break;
    }
    case 32: {
        #if LV_FONT_MONTSERRAT_32
        font = &lv_font_montserrat_32;
        #endif
        break;
    }
    case 34: {
        #if LV_FONT_MONTSERRAT_34
        font = &lv_font_montserrat_34;
        #endif
        break;
    }
    case 36: {
        #if LV_FONT_MONTSERRAT_36
        font = &lv_font_montserrat_36;
        #endif
        break;
    }
    case 38: {
        #if LV_FONT_MONTSERRAT_38
        font = &lv_font_montserrat_38;
        #endif
        break;
    }
    case 40: {
        #if LV_FONT_MONTSERRAT_40
        font = &lv_font_montserrat_40;
        #endif
        break;
    }
    case 42: {
        #if LV_FONT_MONTSERRAT_42
        font = &lv_font_montserrat_42;
        #endif
        break;
    }
    case 44: {
        #if LV_FONT_MONTSERRAT_44
        font = &lv_font_montserrat_44;
        #endif
        break;
    }
    case 46: {
        #if LV_FONT_MONTSERRAT_46
        font = &lv_font_montserrat_46;
        #endif
        break;
    }
    case 48: {
        #if LV_FONT_MONTSERRAT_48
        font = &lv_font_montserrat_48;
        #endif
        break;
    }
    default:
        break;
    }
    return font;
}
