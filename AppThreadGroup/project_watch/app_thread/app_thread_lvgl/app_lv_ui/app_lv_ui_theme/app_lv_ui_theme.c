
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_theme_show(void *scene)
{
    /* 常规列表 */
    app_lv_ui_list_show(scene);
    /* 弧形列表 */
    // app_lv_ui_list_arc_show(scene);
    /* 星轨 */
    // app_lv_ui_star_robit_show(scene);
    /* 波轮 */
    // app_lv_ui_thumb_wheel_show(scene);
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_theme_hide(void *scene)
{
    /* 常规列表 */
    app_lv_ui_list_hide(scene);
    /* 弧形列表 */
    // app_lv_ui_list_arc_hide(scene);
    /* 星轨 */
    // app_lv_ui_star_robit_hide(scene);
    /* 波轮 */
    // app_lv_ui_thumb_wheel_hide(scene);
}

app_lv_scene_t app_lv_ui_theme = {
    /* 场景资源节点 */
    .show = app_lv_ui_theme_show,
    .hide = app_lv_ui_theme_hide,
};

/*@brief      获得主题列表
 *@param[out] list 主题列表
 *@param[out] num  主题列表数量
 */
void app_lv_ui_theme_list_alloc(app_lv_ui_theme_item_t **list, uint32_t *num)
{
    /* 转移到堆空间,不做静态常驻 */
    app_lv_ui_theme_item_t theme_list[] = {
        #if APP_LV_DEVELOPER_MODEL
        {APP_LV_LANG_NUM, APP_LV_PIC_NUM, &app_lv_ui_test_list,},
        #endif
        {APP_LV_LANG_0X0019, APP_LV_PIC_00_THEME_00_HEART_00_PNG,        &app_lv_ui_null,},
        {APP_LV_LANG_0X0040, APP_LV_PIC_00_THEME_01_SPO2_00_PNG,         &app_lv_ui_null,},
        {APP_LV_LANG_0X00c7, APP_LV_PIC_00_THEME_02_MESSAGE_00_PNG,      &app_lv_ui_null,},
        {APP_LV_LANG_0X007b, APP_LV_PIC_00_THEME_04_CALL_00_PNG,         &app_lv_ui_null,},
        {APP_LV_LANG_0X01c5, APP_LV_PIC_00_THEME_05_SPORT_RECORD_00_PNG, &app_lv_ui_null,},
        {APP_LV_LANG_0X0009, APP_LV_PIC_00_THEME_06_ACTIVITY_00_PNG,     &app_lv_ui_null,},
        {APP_LV_LANG_0X0080, APP_LV_PIC_00_THEME_07_DIAL_00_PNG,         &app_lv_ui_null,},
        {APP_LV_LANG_0X0133, APP_LV_PIC_00_THEME_09_FIND_PHONE_00_PNG,   &app_lv_ui_null,},
        {APP_LV_LANG_0X0140, APP_LV_PIC_00_THEME_10_WORD_COLOCK_00_PNG,  &app_lv_ui_null,},
        {APP_LV_LANG_0X0092, APP_LV_PIC_00_THEME_11_WEATHER_00_PNG,      &app_lv_ui_null,},
        {APP_LV_LANG_0X010b, APP_LV_PIC_00_THEME_12_MUSIC_00_PNG,        &app_lv_ui_null,},
        {APP_LV_LANG_0X012a, APP_LV_PIC_00_THEME_13_ALTITUDE_00_PNG,     &app_lv_ui_null,},
        {APP_LV_LANG_0X0151, APP_LV_PIC_00_THEME_14_SETTINGS_00_PNG,     &app_lv_ui_null,},
        {APP_LV_LANG_0X0136, APP_LV_PIC_00_THEME_15_TAKE_PHOTO_00_PNG,   &app_lv_ui_null,},
        {APP_LV_LANG_0X013f, APP_LV_PIC_00_THEME_16_STOPWATCH_00_PNG,    &app_lv_ui_stopwatch,},
        {APP_LV_LANG_0X00e6, APP_LV_PIC_00_THEME_17_ALARMS_00_PNG,       &app_lv_ui_null,},
        {APP_LV_LANG_0X0139, APP_LV_PIC_00_THEME_18_TIMER_00_PNG,        &app_lv_ui_countdown,},
        {APP_LV_LANG_0X012f, APP_LV_PIC_00_THEME_20_VOICE_00_PNG,        &app_lv_ui_null,},
        {APP_LV_LANG_0X0134, APP_LV_PIC_00_THEME_21_FLASHLIGHT_00_PNG,   &app_lv_ui_null,},
        {APP_LV_LANG_0X0138, APP_LV_PIC_00_THEME_22_CALCULATOR_00_PNG,   &app_lv_ui_calculator,},
        {APP_LV_LANG_0X011c, APP_LV_PIC_00_THEME_23_COMPASS_00_PNG,      &app_lv_ui_null,},
        {APP_LV_LANG_0X0049, APP_LV_PIC_00_THEME_24_STRESS_00_PNG,       &app_lv_ui_null,},
        {APP_LV_LANG_0X00fe, APP_LV_PIC_00_THEME_25_BREATHE_00_PNG,      &app_lv_ui_null,},
        {APP_LV_LANG_0X01c5, APP_LV_PIC_00_THEME_26_SOPRT_00_PNG,        &app_lv_ui_null,},
        {APP_LV_LANG_0X004f, APP_LV_PIC_00_THEME_27_SLEEP_00_PNG,        &app_lv_ui_null,},
        {APP_LV_LANG_0X0110, APP_LV_PIC_00_THEME_28_PERIOD_00_PNG,       &app_lv_ui_women_health,},
        {APP_LV_LANG_0X017c, APP_LV_PIC_00_THEME_29_DISCOVER_00_PNG,     &app_lv_ui_null,},
        {APP_LV_LANG_0X0163, APP_LV_PIC_00_THEME_30_THEME_00_PNG,        &app_lv_ui_null,},
        {APP_LV_LANG_0X01a2, APP_LV_PIC_00_THEME_31_ABOUT_00_PNG,        &app_lv_ui_null,},
        {APP_LV_LANG_0X015d, APP_LV_PIC_00_THEME_32_BRIGHTNESS_00_PNG,   &app_lv_ui_null,},
        {APP_LV_LANG_0X0185, APP_LV_PIC_00_THEME_33_PASSWOED_00_PNG,     &app_lv_ui_null,},
        {APP_LV_LANG_0X016e, APP_LV_PIC_00_THEME_34_SOUND_00_PNG,        &app_lv_ui_null,},
        {APP_LV_LANG_0X02f0, APP_LV_PIC_00_THEME_35_TIME_00_PNG,         &app_lv_ui_null,},
        {APP_LV_LANG_0X0177, APP_LV_PIC_00_THEME_37_DND_00_PNG,          &app_lv_ui_null,},
        {APP_LV_LANG_0X0154, APP_LV_PIC_00_THEME_38_AOD_00_PNG,          &app_lv_ui_null,},
        {APP_LV_LANG_0X019b, APP_LV_PIC_00_THEME_39_SYSTEM_00_PNG,       &app_lv_ui_null,},
        {APP_LV_LANG_0X017a, APP_LV_PIC_00_THEME_40_DOWN_KEY_00_PNG,     &app_lv_ui_null,},
    };
    
    APP_SYS_ASSERT(list != NULL && num != NULL);
    
    *num  = app_sys_arr_len(theme_list);
    *list = lv_mem_alloc(sizeof(app_lv_ui_theme_item_t) * app_sys_arr_len(theme_list));
    memcpy(*list, theme_list, sizeof(app_lv_ui_theme_item_t) * app_sys_arr_len(theme_list));
}

/*@brief     归还主题列表
 *@param[in] list 主题列表
 */
void app_lv_ui_theme_list_free(app_lv_ui_theme_item_t *list)
{
    lv_mem_free(list);
}
