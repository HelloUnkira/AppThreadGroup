#ifndef APP_MODULE_DATA_CENTER_TYPE_H
#define APP_MODULE_DATA_CENTER_TYPE_H

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

/* 混合多运动 */
typedef enum {
    app_module_data_center_sport_mix_null = 0,
    app_module_data_center_sport_mix_breathe,                         // 呼吸
    app_module_data_center_sport_mix_cycling,                         // 骑行
    app_module_data_center_sport_mix_indoor_bicycle,                  // 室内自行车
    app_module_data_center_sport_mix_treadmill,                       // 跑步机
    app_module_data_center_sport_mix_treadmill_1,                     // 走步机
    app_module_data_center_sport_mix_run,                             // 跑步
    app_module_data_center_sport_mix_swimming,                        // 游泳
    app_module_data_center_sport_mix_brisk_walking,                   // 健走
    app_module_data_center_sport_mix_weightlifting,                   // 举重
    app_module_data_center_sport_mix_yoga,                            // 瑜伽
    app_module_data_center_sport_mix_badminton,                       // 羽毛球
    app_module_data_center_sport_mix_basketball,                      // 篮球
    app_module_data_center_sport_mix_rope_skipping,                   // 跳绳
    app_module_data_center_sport_mix_free_training,                   // 自由训练
    app_module_data_center_sport_mix_football,                        // 足球
    app_module_data_center_sport_mix_mountaineering,                  // 登山
    app_module_data_center_sport_mix_ping_pong,                       // 乒乓球
    app_module_data_center_sport_mix_bowling,                         // 保龄球
    app_module_data_center_sport_mix_openwater,                       // 开放水域
    app_module_data_center_sport_mix_dance,                           // 舞蹈
    app_module_data_center_sport_mix_dumbbell,                        // 哑铃
    app_module_data_center_sport_mix_hula_hoop,                       // 呼啦圈
    app_module_data_center_sport_mix_climb_stairs,                    // 爬楼梯
    app_module_data_center_sport_mix_mark_time,                       // 踏步
    app_module_data_center_sport_mix_triathlon,                       // 铁人三项
    app_module_data_center_sport_mix_do_sit_ups,                      // 仰卧起坐
    app_module_data_center_sport_mix_skiing,                          // 滑雪
    app_module_data_center_sport_mix_billiards,                       // 台球
    app_module_data_center_sport_mix_elliptical_trainers,             // 椭圆机
    app_module_data_center_sport_mix_cross_country_race,              // 越野跑
    app_module_data_center_sport_mix_aerobics,                        // 健美操
    app_module_data_center_sport_mix_pilates,                         // 普拉提
    app_module_data_center_sport_mix_shuttlecock,                     // 毽球
    app_module_data_center_sport_mix_spin,                            // 旋转
    app_module_data_center_sport_mix_tennis,                          // 网球
    app_module_data_center_sport_mix_leg_training,                    // 下肢训练
    app_module_data_center_sport_mix_body_training,                   // 上肢训练
    app_module_data_center_sport_mix_fencing,                         // 击剑
    app_module_data_center_sport_mix_archery,                         // 射箭
    app_module_data_center_sport_mix_barbell,                         // 杠铃
    app_module_data_center_sport_mix_thai_boxing,                     // 泰拳
    app_module_data_center_sport_mix_parkour,                         // 跑酷
    app_module_data_center_sport_mix_rowing_machine,                  // 划船机
    app_module_data_center_sport_mix_handball,                        // 手球
    app_module_data_center_sport_mix_cricket,                         // 板球
    app_module_data_center_sport_mix_belly_dance,                     // 肚皮舞
    app_module_data_center_sport_mix_sword_fight,                     // 剑术
    app_module_data_center_sport_mix_ducks_and_drakes,                // 打水漂
    app_module_data_center_sport_mix_judo,                            // 柔道
    app_module_data_center_sport_mix_skating,                         // 溜冰
    app_module_data_center_sport_mix_lumbar_abdominal_training,       // 腰腹训练
    app_module_data_center_sport_mix_latin_dance,                     // 拉丁舞
    app_module_data_center_sport_mix_flexibility,                     // 柔韧度
    app_module_data_center_sport_mix_skateboard,                      // 滑板
    app_module_data_center_sport_mix_abdomen_back_stretch,            // 腹背部训练
    app_module_data_center_sport_mix_roller_skating,                  // 轮滑
    app_module_data_center_sport_mix_cross_training,                  // 交叉训练
    app_module_data_center_sport_mix_functional_training,             // 功能性训练
    app_module_data_center_sport_mix_stretching,                      // 拉伸
    app_module_data_center_sport_mix_core_strength_training,          // 核心力量
    app_module_data_center_sport_mix_paraglider,                      // 滑翔伞
    app_module_data_center_sport_mix_free_combat,                     // 自由搏击
    app_module_data_center_sport_mix_strength_training,               // 力量训练
    app_module_data_center_sport_mix_soft_medicine_ball,              // 软式药球
    app_module_data_center_sport_mix_kabati,                          // 卡巴蒂运动
    app_module_data_center_sport_mix_tug_of_war,                      // 拔河
    app_module_data_center_sport_mix_baseball,                        // 棒球
    app_module_data_center_sport_mix_fishing,                         // 钓鱼
    app_module_data_center_sport_mix_gymnastics,                      // 体操
    app_module_data_center_sport_mix_snowboarding,                    // 双板滑雪
    app_module_data_center_sport_mix_boxing,                          // 拳击
    app_module_data_center_sport_mix_fat_burn,                        // 燃脂
    app_module_data_center_sport_mix_physical_culture,                // 体育运动
    app_module_data_center_sport_mix_volleyball,                      // 排球
    app_module_data_center_sport_mix_rugby,                           // 橄榄球
    app_module_data_center_sport_mix_ballet,                          // 芭蕾舞
    app_module_data_center_sport_mix_toboggan,                        // 长雪橇
    app_module_data_center_sport_mix_wrestling,                       // 摔跤
    app_module_data_center_sport_mix_walk,                            // 步行
    app_module_data_center_sport_mix_jazz,                            // 爵士舞
    app_module_data_center_sport_mix_sepaktakraw,                     // 藤球
    app_module_data_center_sport_mix_frisbee,                         // 飞盘
    app_module_data_center_sport_mix_snowmobile,                      // 摩托雪橇
    app_module_data_center_sport_mix_martial_arts,                    // 武术
    app_module_data_center_sport_mix_physical_training,               // 物理训练
    app_module_data_center_sport_mix_hip_hop,                         // 街舞
    app_module_data_center_sport_mix_darts,                           // 飞镖
    app_module_data_center_sport_mix_sledding,                        // 坐雪橇
    app_module_data_center_sport_mix_rock_climb,                      // 攀岩
    app_module_data_center_sport_mix_marathon,                        // 马拉松
    app_module_data_center_sport_mix_bodybuilding,                    // 健身
    app_module_data_center_sport_mix_softball,                        // 垒球
    app_module_data_center_sport_mix_climb,                           // 攀爬
    app_module_data_center_sport_mix_folk_dance,                      // 民间舞
    app_module_data_center_sport_mix_thriller,                        // 僵尸舞
    app_module_data_center_sport_mix_rackets,                         // 墙网球
    app_module_data_center_sport_mix_kiteflying,                      // 放风筝
    app_module_data_center_sport_mix_motor_racing,                    // 汽车比赛
    app_module_data_center_sport_mix_weight_training,                 // 硬拉负重训练
    app_module_data_center_sport_mix_horseback_riding,                // 骑马
    app_module_data_center_sport_mix_all_terrain,                     // 全地形车
    app_module_data_center_sport_mix_taiji_boxing,                    // 太极拳
    app_module_data_center_sport_mix_square_dance,                    // 方块舞
    app_module_data_center_sport_mix_beach_volleyball,                // 沙滩排球
    app_module_data_center_sport_mix_karate,                          // 空手道
    app_module_data_center_sport_mix_taekwondo,                       // 跆拳道
    app_module_data_center_sport_mix_golf,                            // 高尔夫
    app_module_data_center_sport_mix_curling,                         // 冰壶
    app_module_data_center_sport_mix_indoor_walking,                  // 室内健走
    app_module_data_center_sport_mix_beach_soccer,                    // 沙滩足球
    app_module_data_center_sport_mix_ice_hockey,                      // 冰球
    app_module_data_center_sport_mix_hockey,                          // 曲棍球
    app_module_data_center_sport_mix_poby,                            // 波比运动
    app_module_data_center_sport_mix_num,
} app_module_data_center_sport_mix_type_t;

typedef enum {
    app_module_data_center_sport_null = 0,
    app_module_data_center_sport_walk,
    app_module_data_center_sport_run,
    app_module_data_center_sport_num,
} app_module_data_center_sport_mode_t;

typedef enum {
    app_module_data_center_sport_stop,
    app_module_data_center_sport_stop_force,
    app_module_data_center_sport_start,
    app_module_data_center_sport_pause,
    app_module_data_center_sport_continue,
    app_module_data_center_sport_sync,
} app_module_data_center_sport_state_t;

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
