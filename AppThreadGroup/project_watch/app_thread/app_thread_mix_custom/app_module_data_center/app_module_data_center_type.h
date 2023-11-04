#ifndef APP_MODULE_DATA_CENTER_TYPE_H
#define APP_MODULE_DATA_CENTER_TYPE_H

typedef enum {
    app_module_data_center_system_mode_normal,
    app_module_data_center_system_mode_shutdown,
    app_module_data_center_system_mode_low_power,
    app_module_data_center_system_mode_ota_update,
    app_module_data_center_system_mode_num,
} app_module_data_center_system_mode_t;

typedef enum {
    app_module_data_center_backlight_level_1,
    app_module_data_center_backlight_level_2,
    app_module_data_center_backlight_level_3,
    app_module_data_center_backlight_level_4,
    app_module_data_center_backlight_level_5,
} app_module_data_center_backlight_level_t;

typedef enum {
    app_module_data_center_lang_none = 0,   // 无效
    /* europe: */
    app_module_data_center_lang_en,         // 英语
    app_module_data_center_lang_fra,        // 法语
    app_module_data_center_lang_de,         // 德语
    app_module_data_center_lang_it,         // 意大利
    app_module_data_center_lang_pt,         // 葡萄牙
    app_module_data_center_lang_spa,        // 西班牙
    app_module_data_center_lang_pl,         // 波兰语
    app_module_data_center_lang_cs,         // 捷克语
    app_module_data_center_lang_nl,         // 荷兰语
    app_module_data_center_lang_lit,        // 立陶宛
    app_module_data_center_lang_swe,        // 瑞典语
    app_module_data_center_lang_fin,        // 芬兰语
    app_module_data_center_lang_per,        // 波斯语
    app_module_data_center_lang_nor,        // 挪威语
    app_module_data_center_lang_sk,         // 斯洛伐克语
    app_module_data_center_lang_hu,         // 匈牙利
    app_module_data_center_lang_dan,        // 丹麦语
    app_module_data_center_lang_ukr,        // 乌克兰语
    app_module_data_center_lang_tr,         // 土耳其语
    app_module_data_center_lang_rom,        // 罗马尼亚
    app_module_data_center_lang_slo,        // 斯洛文尼亚
    app_module_data_center_lang_hrv,        // 克罗地亚语
    app_module_data_center_lang_el,         // 希腊语
    app_module_data_center_lang_hi,         // 印地语
    /* asia */
    app_module_data_center_lang_zh,         // 中文
    app_module_data_center_lang_cht,        // 繁体中文
    app_module_data_center_lang_jp,         // 日语
    app_module_data_center_lang_kor,        // 韩语
    app_module_data_center_lang_ru,         // 俄语
    app_module_data_center_lang_vie,        // 越南
    app_module_data_center_lang_th,         // 泰国
    app_module_data_center_lang_fli,        // 菲律宾
    app_module_data_center_lang_bur,        // 缅甸
    app_module_data_center_lang_id,         // 印尼语
    /* other */
    app_module_data_center_lang_ara,        // 阿拉伯语
    /*  */
    app_module_data_center_lang_num,
} app_module_data_center_lang_t;

typedef enum {
    app_module_data_center_motion_none = 0,
    app_module_data_center_motion_num,
} app_module_data_center_motion_t;

typedef enum {
    app_module_data_center_ui_theme_level2_is_list,
    app_module_data_center_ui_theme_level2_is_grid,
    app_module_data_center_ui_theme_level2_is_chessboard,
    app_module_data_center_ui_theme_level2_num,
} app_module_data_center_ui_theme_level2_t;

typedef enum {
    /* health: */
    app_module_data_center_usage_spo2,
    app_module_data_center_usage_sleep,
    app_module_data_center_usage_noise,
    app_module_data_center_usage_stress,
    app_module_data_center_usage_breath,
    app_module_data_center_usage_heartrate,
    app_module_data_center_usage_body_energy,
    app_module_data_center_usage_click_measure,
    app_module_data_center_usage_female_health,
    app_module_data_center_usage_activity,
    /* plug */
    app_module_data_center_usage_alexa,
    app_module_data_center_usage_music,
    app_module_data_center_usage_phone,
    app_module_data_center_usage_clock,
    app_module_data_center_usage_camera,
    app_module_data_center_usage_weather,
    app_module_data_center_usage_reminder,
    app_module_data_center_usage_findphone,
    app_module_data_center_usage_flashlight,
    /* sport */
    app_module_data_center_usage_sports,
    app_module_data_center_usage_sports_records,
    app_module_data_center_usage_running_courses,
    /* num */
    app_module_data_center_usage_num,
    app_module_data_center_usage_sports_list = 5,
} app_module_data_center_usage_t;

/* --------------------------------------------------------------------- */
/* 转换单位(系数) */
#define APP_MODULE_DATA_CENTER_M_TO_MI              (0.00062137)    /* 米转英里 */
#define APP_MODULE_DATA_CENTER_KM_TO_MI             (0.62137120)    /* 公里转英里 */
#define APP_MODULE_DATA_CENTER_MI_TO_KM             (1.60934400)    /* 英里转公里 */
#define APP_MODULE_DATA_CENTER_CM_TO_IN             (0.03280000)    /* 厘米转英尺 */ 
#define APP_MODULE_DATA_CENTER_CM_TO_INCH           (0.39360000)    /* 厘米转英寸 */
#define APP_MODULE_DATA_CENTER_M_TO_YD              (1.09360000)    /* 米转码 */
#define APP_MODULE_DATA_CENTER_YD_TO_M              (0.91440000)    /* 码转米 */
/* 最大数据(Activity) */
#define APP_MODULE_DATA_CENTER_STEP_MAX             (99999)         /* 步数数据最大值 */
#define APP_MODULE_DATA_CENTER_DIST_MAX             (999999)        /* 距离数据的最大值(m) */
#define APP_MODULE_DATA_CENTER_CALORIE_MAX          (9999)          /* 燃烧热量数据的最大值(cal) */
#define APP_MODULE_DATA_CENTER_DURATION_MAX         (1440)          /* 锻炼时长数据的最大值(min) */
#define APP_MODULE_DATA_CENTER_WALK_TIME_MAX        (24)            /* 走动时长最大值(h) */
#define APP_MODULE_DATA_CENTER_WALK_TIME_GOAL_MAX   (14)            /* 走动时长目标最大值(h) */
/* 最大数据(Sport) */
#define APP_MODULE_DATA_CENTER_SETP_FREQ_MAX        (399)           /* 步频 SPM */
#define APP_MODULE_DATA_CENTER_HEART_RATE_MAX       (220)           /* 心率 BPM */
#define APP_MODULE_DATA_CENTER_SPEED_MAX            (9999)          /* 最大速度((1/100)*km/h) */
#define APP_MODULE_DATA_CENTER_LAPS_MAX             (10)            /* 最大游泳躺数 */
#define APP_MODULE_DATA_CENTER_VO2_MAX_MAX          (99)            /* 最大摄氧量(ml/kg/min) */
#define APP_MODULE_DATA_CENTER_TE_MAX               (49)            /* 有氧训练效果(x10) */
/* --------------------------------------------------------------------- */

#endif
