/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*****************************************************************************/
/* 主题:蜂窝使用 */

void scui_ui_honeycomb_list_layout(scui_point_t *list_layout, scui_handle_t ofs_cur)
{
    #define HB_DIFX     scui_ui_honeycomb_dif_x(ofs_cur)
    #define HB_DIFY     scui_ui_honeycomb_dif_y(ofs_cur)
    #define HB_MIDX     scui_ui_honeycomb_mid_x(ofs_cur)
    #define HB_MIDY     scui_ui_honeycomb_mid_y(ofs_cur)
    
    scui_point_t hb_layout[SCUI_UI_HONEYCOMB_LIST_NUM] =
    {
        /* -3线 */
        {HB_MIDX - HB_DIFX * 1 / 2  - HB_DIFX * 1, HB_MIDY - HB_DIFY * 3},
        {HB_MIDX - HB_DIFX * 1 / 2               , HB_MIDY - HB_DIFY * 3},
        {HB_MIDX + HB_DIFX * 1 / 2               , HB_MIDY - HB_DIFY * 3},
        {HB_MIDX + HB_DIFX * 1 / 2  + HB_DIFX * 1, HB_MIDY - HB_DIFY * 3},

        /* -2线 */
        {HB_MIDX                    - HB_DIFX * 2, HB_MIDY - HB_DIFY * 2},
        {HB_MIDX                    - HB_DIFX * 1, HB_MIDY - HB_DIFY * 2},
        {HB_MIDX                                 , HB_MIDY - HB_DIFY * 2},
        {HB_MIDX                    + HB_DIFX * 1, HB_MIDY - HB_DIFY * 2},
        {HB_MIDX                    + HB_DIFX * 2, HB_MIDY - HB_DIFY * 2},

        /* -1线 */
        {HB_MIDX - HB_DIFX * 1 / 2  - HB_DIFX * 2, HB_MIDY - HB_DIFY * 1},
        {HB_MIDX - HB_DIFX * 1 / 2  - HB_DIFX * 1, HB_MIDY - HB_DIFY * 1},
        {HB_MIDX - HB_DIFX * 1 / 2               , HB_MIDY - HB_DIFY * 1},
        {HB_MIDX + HB_DIFX * 1 / 2               , HB_MIDY - HB_DIFY * 1},
        {HB_MIDX + HB_DIFX * 1 / 2  + HB_DIFX * 1, HB_MIDY - HB_DIFY * 1},
        {HB_MIDX + HB_DIFX * 1 / 2  + HB_DIFX * 2, HB_MIDY - HB_DIFY * 1},

        /* 中心线: */
        {HB_MIDX                    - HB_DIFX * 3, HB_MIDY},
        {HB_MIDX                    - HB_DIFX * 2, HB_MIDY},
        {HB_MIDX                    - HB_DIFX * 1, HB_MIDY},
        {HB_MIDX                                 , HB_MIDY},    // 中心图标
        {HB_MIDX                    + HB_DIFX * 1, HB_MIDY},
        {HB_MIDX                    + HB_DIFX * 2, HB_MIDY},
        {HB_MIDX                    + HB_DIFX * 3, HB_MIDY},

        /* +1线 */
        {HB_MIDX - HB_DIFX * 1 / 2  - HB_DIFX * 2, HB_MIDY + HB_DIFY * 1},
        {HB_MIDX - HB_DIFX * 1 / 2  - HB_DIFX * 1, HB_MIDY + HB_DIFY * 1},
        {HB_MIDX - HB_DIFX * 1 / 2               , HB_MIDY + HB_DIFY * 1},
        {HB_MIDX + HB_DIFX * 1 / 2               , HB_MIDY + HB_DIFY * 1},
        {HB_MIDX + HB_DIFX * 1 / 2  + HB_DIFX * 1, HB_MIDY + HB_DIFY * 1},
        {HB_MIDX + HB_DIFX * 1 / 2  + HB_DIFX * 2, HB_MIDY + HB_DIFY * 1},

        /* +2线 */
        {HB_MIDX                    - HB_DIFX * 2, HB_MIDY + HB_DIFY * 2},
        {HB_MIDX                    - HB_DIFX * 1, HB_MIDY + HB_DIFY * 2},
        {HB_MIDX                                 , HB_MIDY + HB_DIFY * 2},
        {HB_MIDX                    + HB_DIFX * 1, HB_MIDY + HB_DIFY * 2},
        {HB_MIDX                    + HB_DIFX * 2, HB_MIDY + HB_DIFY * 2},

        /* +3线 */
        {HB_MIDX - HB_DIFX * 1 / 2  - HB_DIFX * 1, HB_MIDY + HB_DIFY * 3},
        {HB_MIDX - HB_DIFX * 1 / 2               , HB_MIDY + HB_DIFY * 3},
        {HB_MIDX + HB_DIFX * 1 / 2               , HB_MIDY + HB_DIFY * 3},
        {HB_MIDX + HB_DIFX * 1 / 2  + HB_DIFX * 1, HB_MIDY + HB_DIFY * 3},
    };
    
    uintptr_t size = SCUI_UI_HONEYCOMB_LIST_NUM * sizeof(scui_point_t);
    memcpy(list_layout, hb_layout, size);
}

/*****************************************************************************/
/* 应用列表,设置列表等使用 */
scui_handle_t  scui_ui_scene_list_num   = 0;
scui_handle_t *scui_ui_scene_list_image = NULL;
scui_handle_t *scui_ui_scene_list_text  = NULL;
scui_handle_t *scui_ui_scene_list_jump  = NULL;

void scui_ui_scene_list_cfg(scui_ui_scene_list_type_t type)
{
    switch (type) {
    case scui_ui_scene_list_type_list_scale:
    case scui_ui_scene_list_type_list_arc:
    case scui_ui_scene_list_type_waterfall:
    case scui_ui_scene_list_type_themewheel:
    case scui_ui_scene_list_type_spread: {
        
        static const scui_handle_t list_image[] = {
            scui_image_prj_image_src_00_theme_icon_00_heart_01_00png,
            scui_image_prj_image_src_00_theme_icon_01_spo2_01_00png,
            scui_image_prj_image_src_00_theme_icon_02_message_01_00png,
            scui_image_prj_image_src_00_theme_icon_04_call_01_00png,
            scui_image_prj_image_src_00_theme_icon_05_sport_record_01_00png,
            scui_image_prj_image_src_00_theme_icon_06_activity_01_00png,
            scui_image_prj_image_src_00_theme_icon_07_dial_01_00png,
            scui_image_prj_image_src_00_theme_icon_09_find_phone_01_00png,
            scui_image_prj_image_src_00_theme_icon_10_word_colock_01_00png,
            scui_image_prj_image_src_00_theme_icon_11_weather_01_00png,
            scui_image_prj_image_src_00_theme_icon_12_music_01_00png,
            scui_image_prj_image_src_00_theme_icon_13_altitude_01_00png,
            scui_image_prj_image_src_00_theme_icon_14_settings_01_00png,
            scui_image_prj_image_src_00_theme_icon_15_take_photo_01_00png,
            scui_image_prj_image_src_00_theme_icon_16_stopwatch_01_00png,
            scui_image_prj_image_src_00_theme_icon_17_alarms_01_00png,
            scui_image_prj_image_src_00_theme_icon_18_timer_01_00png,
            scui_image_prj_image_src_00_theme_icon_20_voice_01_00png,
            scui_image_prj_image_src_00_theme_icon_21_flashlight_01_00png,
            scui_image_prj_image_src_00_theme_icon_22_calculator_01_00png,
            scui_image_prj_image_src_00_theme_icon_23_compass_01_00png,
            scui_image_prj_image_src_00_theme_icon_24_stress_01_00png,
            scui_image_prj_image_src_00_theme_icon_25_breathe_01_00png,
            scui_image_prj_image_src_00_theme_icon_26_soprt_01_00png,
            scui_image_prj_image_src_00_theme_icon_27_sleep_01_00png,
            scui_image_prj_image_src_00_theme_icon_28_period_01_00png,
            scui_image_prj_image_src_00_theme_icon_29_discover_01_00png,
            scui_image_prj_image_src_00_theme_icon_30_theme_01_00png,
            scui_image_prj_image_src_00_theme_icon_31_about_01_00png,
            scui_image_prj_image_src_00_theme_icon_32_brightness_01_00png,
            scui_image_prj_image_src_00_theme_icon_33_passwoed_01_00png,
            scui_image_prj_image_src_00_theme_icon_34_sound_01_00png,
            scui_image_prj_image_src_00_theme_icon_35_time_01_00png,
            scui_image_prj_image_src_00_theme_icon_37_dnd_01_00png,
            scui_image_prj_image_src_00_theme_icon_38_aod_01_00png,
            scui_image_prj_image_src_00_theme_icon_39_system_01_00png,
            scui_image_prj_image_src_00_theme_icon_40_down_key_01_00png,
            // scui_image_prj_image_src_00_theme_icon_41_blood_pressure_01_00png,
            // scui_image_prj_image_src_00_theme_icon_42_measurement_01_00png,
        };
        static const scui_handle_t list_text[] = {
            SCUI_MULTI_LANG_0X0019,
            SCUI_MULTI_LANG_0X0040,
            SCUI_MULTI_LANG_0X00c9,
            SCUI_MULTI_LANG_0X008b,
            SCUI_MULTI_LANG_0X01c5,
            SCUI_MULTI_LANG_0X0009,
            SCUI_MULTI_LANG_0X008b,
            SCUI_MULTI_LANG_0X0133,
            SCUI_MULTI_LANG_0X0140,
            SCUI_MULTI_LANG_0X0092,
            SCUI_MULTI_LANG_0X010b,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X0151,
            SCUI_MULTI_LANG_0X0136,
            SCUI_MULTI_LANG_0X013f,
            SCUI_MULTI_LANG_0X00e6,
            SCUI_MULTI_LANG_0X0139,
            SCUI_MULTI_LANG_0X016e,
            SCUI_MULTI_LANG_0X0134,
            SCUI_MULTI_LANG_0X0138,
            SCUI_MULTI_LANG_0X011c,
            SCUI_MULTI_LANG_0X0049,
            SCUI_MULTI_LANG_0X00fe,
            SCUI_MULTI_LANG_0X01c7,
            SCUI_MULTI_LANG_0X004f,
            SCUI_MULTI_LANG_0X0110,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X0163,
            SCUI_MULTI_LANG_0X01a2,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X0185,
            SCUI_MULTI_LANG_0X016e,
            SCUI_MULTI_LANG_0X0181,
            SCUI_MULTI_LANG_0X0177,
            SCUI_MULTI_LANG_0X0154,
            SCUI_MULTI_LANG_0X019b,
            SCUI_MULTI_LANG_0X017a,
            // SCUI_MULTI_LANG_0X0030,
            // SCUI_MULTI_LANG_0X0030,
        };
        static const scui_handle_t list_jump[] = {
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            // SCUI_UI_SCENE_6,
            // SCUI_UI_SCENE_6,
        };
        
        scui_ui_scene_list_num   = scui_arr_len(list_image);
        scui_ui_scene_list_image = list_image;
        scui_ui_scene_list_text  = list_text;
        scui_ui_scene_list_jump  = list_jump;
        SCUI_ASSERT(scui_arr_len(list_image) == scui_arr_len(list_text));
        SCUI_ASSERT(scui_arr_len(list_image) == scui_arr_len(list_jump));
        break;
    }
    case scui_ui_scene_list_type_honeycomb: {
        
        static const scui_handle_t list_image[SCUI_UI_HONEYCOMB_LIST_NUM] = {
            scui_image_prj_image_src_00_theme_icon_00_heart_01_00png,
            scui_image_prj_image_src_00_theme_icon_01_spo2_01_00png,
            scui_image_prj_image_src_00_theme_icon_02_message_01_00png,
            scui_image_prj_image_src_00_theme_icon_04_call_01_00png,
            scui_image_prj_image_src_00_theme_icon_05_sport_record_01_00png,
            scui_image_prj_image_src_00_theme_icon_06_activity_01_00png,
            scui_image_prj_image_src_00_theme_icon_07_dial_01_00png,
            scui_image_prj_image_src_00_theme_icon_09_find_phone_01_00png,
            scui_image_prj_image_src_00_theme_icon_10_word_colock_01_00png,
            scui_image_prj_image_src_00_theme_icon_11_weather_01_00png,
            scui_image_prj_image_src_00_theme_icon_12_music_01_00png,
            scui_image_prj_image_src_00_theme_icon_13_altitude_01_00png,
            scui_image_prj_image_src_00_theme_icon_14_settings_01_00png,
            scui_image_prj_image_src_00_theme_icon_15_take_photo_01_00png,
            scui_image_prj_image_src_00_theme_icon_16_stopwatch_01_00png,
            scui_image_prj_image_src_00_theme_icon_17_alarms_01_00png,
            scui_image_prj_image_src_00_theme_icon_18_timer_01_00png,
            scui_image_prj_image_src_00_theme_icon_20_voice_01_00png,
            scui_image_prj_image_src_00_theme_icon_21_flashlight_01_00png,
            scui_image_prj_image_src_00_theme_icon_22_calculator_01_00png,
            scui_image_prj_image_src_00_theme_icon_23_compass_01_00png,
            scui_image_prj_image_src_00_theme_icon_24_stress_01_00png,
            scui_image_prj_image_src_00_theme_icon_25_breathe_01_00png,
            scui_image_prj_image_src_00_theme_icon_26_soprt_01_00png,
            scui_image_prj_image_src_00_theme_icon_27_sleep_01_00png,
            scui_image_prj_image_src_00_theme_icon_28_period_01_00png,
            scui_image_prj_image_src_00_theme_icon_29_discover_01_00png,
            scui_image_prj_image_src_00_theme_icon_30_theme_01_00png,
            scui_image_prj_image_src_00_theme_icon_31_about_01_00png,
            scui_image_prj_image_src_00_theme_icon_32_brightness_01_00png,
            scui_image_prj_image_src_00_theme_icon_33_passwoed_01_00png,
            scui_image_prj_image_src_00_theme_icon_34_sound_01_00png,
            scui_image_prj_image_src_00_theme_icon_35_time_01_00png,
            scui_image_prj_image_src_00_theme_icon_37_dnd_01_00png,
            scui_image_prj_image_src_00_theme_icon_38_aod_01_00png,
            scui_image_prj_image_src_00_theme_icon_39_system_01_00png,
            scui_image_prj_image_src_00_theme_icon_40_down_key_01_00png,
            // scui_image_prj_image_src_00_theme_icon_41_blood_pressure_01_00png,
            // scui_image_prj_image_src_00_theme_icon_42_measurement_01_00png,
        };
        
        static const scui_handle_t list_jump[SCUI_UI_HONEYCOMB_LIST_NUM] = {
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
        };
        
        scui_ui_scene_list_num   = SCUI_UI_HONEYCOMB_LIST_NUM;
        scui_ui_scene_list_image = list_image;
        scui_ui_scene_list_text  = NULL;
        scui_ui_scene_list_jump  = list_jump;
        SCUI_ASSERT(scui_arr_len(list_image) == scui_arr_len(list_jump));
        break;
    }
    default:
        SCUI_ASSERT(false);
        break;
    }
}

/*****************************************************************************/
/*@brief 窗口关联配置
 *@param event 事件
 */
void scui_ui_scene_link_cfg(scui_event_t *event)
{
    // 仅焦点切换时才可进行
    if (event->type != scui_event_focus_get ||
       !scui_widget_event_check_prepare(event))
        return;
    
    /* 窗口属性参数配置(浮动窗口) */
    scui_window_float_t float_cfg = {0};
    scui_window_float_cfg_get(&float_cfg);
    float_cfg.main = event->object;
    float_cfg.list[0] = SCUI_HANDLE_INVALID;
    float_cfg.list[1] = SCUI_HANDLE_INVALID;
    float_cfg.list[2] = SCUI_HANDLE_INVALID;
    float_cfg.list[3] = SCUI_HANDLE_INVALID;
    
    switch (event->object) {
    case SCUI_UI_SCENE_HOME:
        float_cfg.list[0] = SCUI_UI_SCENE_FLOAT_1;
        float_cfg.list[1] = SCUI_UI_SCENE_FLOAT_2;
        float_cfg.list[2] = SCUI_UI_SCENE_FLOAT_3;
        float_cfg.list[3] = SCUI_UI_SCENE_FLOAT_4;
        break;
    default:
        break;
    }
    scui_window_float_cfg_set(&float_cfg);
    
    /* 窗口属性参数配置(场景管理) */
    scui_window_cfg_t window_cfg = {0};
    scui_window_cfg_get(event->object, &window_cfg);
    window_cfg.sibling[0] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[1] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[2] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[3] = SCUI_HANDLE_INVALID;
    
    switch (event->object) {
    case SCUI_UI_SCENE_HOME:
        window_cfg.sibling[2] = SCUI_UI_SCENE_6;
        window_cfg.sibling[3] = SCUI_UI_SCENE_1;
        break;
    case SCUI_UI_SCENE_1:
        window_cfg.sibling[0] = SCUI_UI_SCENE_TEST;
        window_cfg.sibling[1] = SCUI_UI_SCENE_CUBE;
        window_cfg.sibling[2] = SCUI_UI_SCENE_HOME;
        window_cfg.sibling[3] = SCUI_UI_SCENE_2;
        break;
    case SCUI_UI_SCENE_2:
        window_cfg.sibling[0] = SCUI_UI_SCENE_LANTERN;
        window_cfg.sibling[1] = SCUI_UI_SCENE_SOCCER;
        window_cfg.sibling[2] = SCUI_UI_SCENE_1;
        window_cfg.sibling[3] = SCUI_UI_SCENE_3;
        break;
    case SCUI_UI_SCENE_3:
        window_cfg.sibling[0] = SCUI_UI_SCENE_BUTTERFLY;
        window_cfg.sibling[2] = SCUI_UI_SCENE_2;
        window_cfg.sibling[3] = SCUI_UI_SCENE_4;
        break;
    case SCUI_UI_SCENE_4:
        window_cfg.sibling[2] = SCUI_UI_SCENE_3;
        window_cfg.sibling[3] = SCUI_UI_SCENE_5;
        break;
    case SCUI_UI_SCENE_5:
        window_cfg.sibling[2] = SCUI_UI_SCENE_4;
        window_cfg.sibling[3] = SCUI_UI_SCENE_6;
        break;
    case SCUI_UI_SCENE_6:
        window_cfg.sibling[2] = SCUI_UI_SCENE_5;
        window_cfg.sibling[3] = SCUI_UI_SCENE_HOME;
        break;
    
    case SCUI_UI_SCENE_TEST:
        window_cfg.sibling[1] = SCUI_UI_SCENE_1;
        break;
    case SCUI_UI_SCENE_CUBE:
        window_cfg.sibling[0] = SCUI_UI_SCENE_1;
        break;
    
    case SCUI_UI_SCENE_LANTERN:
        window_cfg.sibling[1] = SCUI_UI_SCENE_2;
        break;
    case SCUI_UI_SCENE_SOCCER:
        window_cfg.sibling[0] = SCUI_UI_SCENE_2;
        break;
    
    case SCUI_UI_SCENE_BUTTERFLY:
        window_cfg.sibling[1] = SCUI_UI_SCENE_3;
        break;
    default:
        break;
    }
    scui_window_cfg_set(event->object, &window_cfg);
}