
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

static struct {
    lv_obj_t  *scene;
    lv_font_t *font36;
    struct {
        uint32_t        idx_str;
        uint32_t        idx_pic;
        app_lv_scene_t *scene;
    } *list;
} *app_lv_ui_res_local = NULL;

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_list_btn_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        app_lv_scene_t *scene = lv_event_get_user_data(e);
        app_lv_scene_add(scene, false);
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_list_show(void *scene)
{
    /* 栈空间转移到堆空间,不做静态常驻 */
    struct {
        uint32_t        idx_str;
        uint32_t        idx_pic;
        app_lv_scene_t *scene;
    } app_lv_ui_res_local_list[] = {
        #if APP_LV_DEVELOPER_MODEL
        {APP_LV_LANG_NUM, APP_LV_PIC_LVGL_PNG, &app_lv_ui_test_list,},
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
    
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(*app_lv_ui_res_local));
        /* 静态资源构建 */
        uint32_t size = sizeof(*app_lv_ui_res_local_list) * app_sys_arr_len(app_lv_ui_res_local_list);
        app_lv_ui_res_local->list = lv_mem_alloc(size);
        memcpy(app_lv_ui_res_local->list, app_lv_ui_res_local_list, size);
        /* 获取字体 */
        app_lv_ui_res_local->font36 = app_lv_multi_font_load(app_lv_multi_font_size_36);
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        /* 初始化列表 */
        lv_obj_t *list = lv_list_create(app_lv_ui_res_local->scene);
        app_lv_style_object(list);
        lv_obj_set_style_pad_row(list, app_lv_style_ver_pct(2), 0);
        lv_obj_set_style_pad_ver(list, app_lv_style_ver_pct(2), 0);
        lv_obj_set_style_pad_hor(list, app_lv_style_hor_pct(5), 0);
        lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES);
        lv_obj_center(list);
        /* 为列表批量追加按钮 */
        for (uint32_t idx = 0; idx < app_sys_arr_len(app_lv_ui_res_local_list) + 1; idx++) {
            if (idx == 0) {
                /* 条目按钮 */
                lv_obj_t *btn = lv_btn_create(list);
                app_lv_style_object(btn);
                lv_obj_set_width(btn, app_lv_style_hor_pct(90));
                lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 0);
                /* 条目文本 */
                lv_obj_t *lab = lv_label_create(btn);
                app_lv_style_object(lab);
                lv_obj_set_style_text_font(lab, app_lv_ui_res_local->font36, 0);
                lv_obj_set_width(lab, app_lv_style_hor_pct(60));
                lv_label_set_long_mode(lab, LV_LABEL_LONG_SCROLL_CIRCULAR);
                lv_label_set_text(lab, app_lv_lang_str_find(APP_LV_LANG_0X01b1));
                lv_obj_center(lab);
            } else {
                /* 条目按钮 */
                lv_obj_t *btn = lv_btn_create(list);
                app_lv_style_object(btn);
                lv_obj_add_event_cb(btn, app_lv_ui_list_btn_cb, LV_EVENT_CLICKED, app_lv_ui_res_local->list[idx - 1].scene);
                /* 条目按钮添加图片 */
                lv_obj_t *img = lv_img_create(btn);
                app_lv_style_object(img);
                const char *img_str = app_lv_pic_str_find(app_lv_ui_res_local->list[idx - 1].idx_pic + 5);
                lv_img_set_src(img, img_str != NULL ? img_str : app_lv_pic_str_find(app_lv_ui_res_local->list[idx - 1].idx_pic));
                lv_obj_align(img, LV_ALIGN_LEFT_MID, 0, 0);
                /* 条目按钮添加文本 */
                lv_obj_t *lab = lv_label_create(btn);
                app_lv_style_object(lab);
                lv_obj_set_style_text_font(lab, app_lv_ui_res_local->font36, 0);
                lv_obj_set_style_text_align(lab, LV_TEXT_ALIGN_LEFT, 0);
                lv_obj_set_width(lab, app_lv_style_hor_pct(65));
                lv_label_set_long_mode(lab, LV_LABEL_LONG_SCROLL_CIRCULAR);
                const char *lab_str = app_lv_lang_str_find(app_lv_ui_res_local->list[idx - 1].idx_str);
                lv_label_set_text(lab, lab_str != NULL ? lab_str : "Internal Test");
                lv_obj_align_to(lab, img, LV_ALIGN_OUT_RIGHT_MID, app_lv_style_hor_pct(1), 0);
            }
        }
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_list_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        ((app_lv_scene_t *)scene)->root = NULL;
        lv_mem_free(app_lv_ui_res_local->list);
        /* 释放字体 */
        app_lv_multi_font_free(app_lv_multi_font_size_36);
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_list = {
    /* 场景资源节点 */
    .show = app_lv_ui_list_show,
    .hide = app_lv_ui_list_hide,
};
