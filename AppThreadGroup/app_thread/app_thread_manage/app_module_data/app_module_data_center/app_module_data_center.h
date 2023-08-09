#ifndef APP_MODULE_DATA_CENTER_H
#define APP_MODULE_DATA_CENTER_H

#include "app_module_clock.h"
#include "app_module_remind_drink.h"
#include "app_module_remind_sedentary.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"
#include "app_module_do_not_disturb.h"

/*备注:
 *    将各种各样的零碎数据统一打包到此结构中
 *    通过不同的结构块区分不同类型的数据区域
 *    并以一个完全统一的接口对需要持久化的资源统一访问
 */

#pragma pack(push, 1)
typedef union {
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源占位标识,本字段无意义(type) */
    enum {
        app_module_data_center_none = 0,
        app_module_data_center_system_profile,
        app_module_data_center_system_data,
        app_module_data_center_user_profile,
        app_module_data_center_user_data,
        app_module_data_center_num,
    } type;
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源(system profile) */
    struct {
        enum {
            app_module_data_center_system_mode_normal,
            app_module_data_center_system_mode_shutdown,
            app_module_data_center_system_mode_low_power,
            app_module_data_center_system_mode_ota_update,
            app_module_data_center_system_mode_num,
        } system_mode;              // 设备工作模式
        uint64_t id:16;             // 设备号
        uint64_t is_pair:1;         // 配对状态
        uint64_t ver_major:8;       // 主版本号
        uint64_t ver_minor:8;       // 次版本号
        uint64_t ver_patch:8;       // 补版本号
        /* system profile: backlight */
        struct {
            enum {
                app_module_data_center_backlight_level_1,
                app_module_data_center_backlight_level_2,
                app_module_data_center_backlight_level_3,
                app_module_data_center_backlight_level_4,
                app_module_data_center_backlight_level_5,
            } level;
            uint8_t cover_sleep:1;      // 覆盖息屏开关
            uint8_t dtouch_awake:1;     // 双击亮屏开关
            uint8_t opera:1;            // 0 as auto mode
                                        // 1 as manual mode
            uint8_t use_sensor:1;       // 使用环境光传感器
            uint8_t app_control:1;      // 手动模式下远程控制使能
            uint8_t auto_onoff:1;       // 自动模式下背光调节使能
            uint8_t auto_time_s[2];     // 自动监控开始时间[时,分]
            uint8_t auto_time_e[2];     // 自动监控结束时间[时,分]
        } backlight;
    } system_profile;
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源(system data) */
    struct {
        /* system data: app_module_clock */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        app_module_clock_t system_clock;
        /*  */
        uint64_t batt_chg_s_utc:36;     // 设备开始充电utc
        uint64_t batt_chg_e_utc:36;     // 设备结束充电utc
        uint64_t ble_conn_utc:36;       // BLE断连时utc
        uint64_t ble_conn_onoff:1;      // 1 as open
                                        // 0 as close
        /* system data: bind authorization */
        struct {
            uint8_t disp_code[12];      // 显示的授权码
            uint8_t auth_code[12];      // 保存的授权码
            uint8_t auth_status:1;      // 授权状态
            uint8_t bind_status:1;      // 绑定状态
            uint8_t bind_request:1;     // 请求绑定
        } bind_auth;
        /* system data: power save */
        struct {
            uint64_t onoff:1;                // 省电模式开关
            uint64_t gesture:1;              // 手势亮屏开关
            uint64_t cover_sleep:1;          // 覆盖息屏开关
            uint64_t dtouch_awake:1;         // 双击亮屏开关
            uint64_t backlight_auto:1;       // 背光自动模式开关
            uint64_t backlight_level:1;      // 背光等级
            uint64_t backlight_duration:1;   // 背光持续时间
            /* 省电模式主要针对屏幕亮灭做限制,可以继续添加其他内容 */
        } save_power;
        /* system data: device usage statistics */
        struct {
            uint64_t record_utc:36;         // clock fisrt update
            struct {
                uint64_t utc:36;            // 记录时间
                uint64_t voltage:16;        // 电压值
                uint64_t percent:8;         // 电量
            } charge[3];                    // [0:开始;1:结束;2:充满]
            uint64_t battery_level:8;       // 电量
            uint64_t detect_level:8;        // 检测电量
            uint64_t detect_count:8;        // 检测计数
            struct {
                uint64_t bright:24;         // 亮屏时长(s)
                uint64_t vibrate:24;        // 震动时长(s)
                uint64_t accelerate:24;     // 加速度计时长(s)
                uint64_t geomagnetism:24;   // 地磁计时长(s)
                uint64_t gyroscope:24;      // 陀螺仪时长(s)
                uint64_t barometer:24;      // 气压计时长(s)
                uint64_t gnss:24;           // GNSS计时长(s)
                uint64_t light_green:24;    // 绿灯时长(s)
                uint64_t light_red:24;      // 红灯时长(s)
                uint64_t infrared:24;       // 红外时长(s)
                uint64_t ble_time:24;       // BLE连接时长
                uint64_t ble_cnt:24;        // BLE连接次数
                uint64_t bt_time:24;        // BT连接时长
                uint64_t bt_cnt:24;         // BT连接次数
                uint64_t key:16;            // 按键次数
            } duration;
        } device_usage;
    } system_data;
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源(user profile) */
    struct {
        uint64_t brithday_utc:36;   // brithday
        uint64_t height:8;          // cm
        uint64_t weight:8;          // cm
        uint64_t stride:8;          // stride cm
        uint64_t stride_run:8;      // stride run cm
        uint64_t stride_walk:8;     // stride walk cm
        uint64_t gender:1;          // 0 as boy
                                    // 1 as girl
        uint64_t metric:1;          // 0 as km
                                    // 1 as mi
        uint64_t mode_time:1;       // 0 as 24 mode
                                    // 1 as 12 mode
        uint64_t mode_temp:1;       // 0 as C" mode
                                    // 1 as F" mode
        uint64_t mode_voice:1;      // 0 as disable voice
                                    // 1 as enabble voice
        uint64_t mode_vibrate:1;    // 0 as disable vibrate
                                    // 1 as enabble vibrate
        uint64_t state_bind:1;      // 0 as unbind state
                                    // 1 as bind   state
        uint64_t state_hand:1;      // 0 as left   hand
                                    // 1 as right  hand
        uint64_t state_week:2;      // 0 as start in monday
                                    // 1 as start in sunday
                                    // 2 as start in saturday
        uint64_t phone_version:9;   // major:minor:patch(3:3:3)
        uint64_t phone_type:2;      // 0 as Android
                                    // 1 as IOS
                                    // 2 as Harmony
        uint64_t find_phone:1;      // 1 as open
                                    // 0 as close
        uint64_t find_band:1;       // 1 as open
                                    // 0 as close
        /* user profile: language */
        enum {
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
        } lang;
        /* user profile: user goal */
        struct {
            uint64_t step:32;               // 步数
            uint64_t step_week:32;          // 步数(周目标)
            uint64_t calories:16;           // 活动卡路里(kcal)
            uint64_t calories_min:16;       // 活动卡路里最小值(kcal)
            uint64_t calories_max:16;       // 活动卡路里最大值(kcal)
            uint64_t distance:32;           // 距离
            uint64_t time_walk:8;           // 走动时长(hour)
            uint64_t time_sleep:16;         // 睡眠时长(min)
            uint64_t time_duration:32;      // 中高时长(min)
            uint64_t onoff:1;               // 1 as open
                                            // 0 as close
        } goal;
        /* user profile: aod watchface */
        struct {
            uint8_t time_s[2];              // 提醒起始[时,分]
            uint8_t time_e[2];              // 提醒结束[时,分]
            uint8_t index:8;                // 表盘索引号
            uint8_t onoff:1;                // 1 as open
                                            // 0 as close
        } aod_watchface;
        /* user profile: ui level 2 theme */
        enum {
            app_module_data_center_ui_theme_level2_is_list,
            app_module_data_center_ui_theme_level2_is_grid,
            app_module_data_center_ui_theme_level2_is_chessboard,
            app_module_data_center_ui_theme_level2_num,
        } ui_theme_level2;
        /* user profile: app_module_remind_drink */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        app_module_remind_drink_t remind_drink;
        /* user profile: app_module_remind_sedentary */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        app_module_remind_sedentary_t remind_sedentary;
        /* user profile: app_module_do_not_disturb */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        app_module_do_not_disturb_t do_not_disturb;
    } user_profile;
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源(user data) */
    struct {
        /* user data: function usage statistics(buried data) */
        struct {
            enum {
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
            } usage_type;
            /* use count:(use type is enum re) */
            uint16_t usage_count[app_module_data_center_usage_num];
            uint16_t usage_queue[3];        // [0:curr;1:prev;2:prev 1]
            uint16_t sports_list_queue[3];  // [0:curr;1:prev;2:prev 1]
            uint16_t sports_list_count[app_module_data_center_usage_sports_list];
            uint16_t sports_list_type[app_module_data_center_usage_sports_list];
        } func_usage;
        /* user data: app_module_remind_alarm */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        struct {
            app_module_remind_item_t       remind_item[APP_MODULE_REMIND_ALARM_MAX];
            app_module_remind_alarm_info_t alarm_info[APP_MODULE_REMIND_ALARM_MAX];
        } remind_alarm;
    } user_data;
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源:keep adding */
    /* --------------------------------------------------------------------- */
    /* 转换单位(系数) */
    #define APP_MODULE_DATA_CENTER_M_TO_MI      0.00062137      /* 米转英里 */
    #define APP_MODULE_DATA_CENTER_KM_TO_MI     0.62137120      /* 公里转英里 */
    #define APP_MODULE_DATA_CENTER_MI_TO_KM     1.60934400      /* 英里转公里 */
    #define APP_MODULE_DATA_CENTER_CM_TO_IN     0.03280000      /* 厘米转英尺 */ 
    #define APP_MODULE_DATA_CENTER_CM_TO_INCH   0.39360000      /* 厘米转英寸 */
    #define APP_MODULE_DATA_CENTER_M_TO_YD      1.09360000      /* 米转码 */
    #define APP_MODULE_DATA_CENTER_YD_TO_M      0.91440000      /* 码转米 */
    /* 最大数据(Activity) */
    #define APP_MODULE_DATA_CENTER_STEP_MAX            (99999)     // 步数数据最大值
    #define APP_MODULE_DATA_CENTER_DIST_MAX            (999999)    // 距离数据的最大值(m)
    #define APP_MODULE_DATA_CENTER_CALORIE_MAX         (9999)      // 燃烧热量数据的最大值(cal)
    #define APP_MODULE_DATA_CENTER_DURATION_MAX        (1440)      // 锻炼时长数据的最大值(min)
    #define APP_MODULE_DATA_CENTER_WALK_TIME_MAX       (24)        // 走动时长最大值(h)
    #define APP_MODULE_DATA_CENTER_WALK_TIME_GOAL_MAX  (14)        // 走动时长目标最大值(h)
    /* 最大数据(Sport) */
    #define APP_MODULE_DATA_CENTER_SETP_FREQ_MAX       (399)       // 步频 SPM
    #define APP_MODULE_DATA_CENTER_HEART_RATE_MAX      (220)       // 心率 BPM
    #define APP_MODULE_DATA_CENTER_SPEED_MAX           (9999)      // 最大速度((1/100)*km/h)
    #define APP_MODULE_DATA_CENTER_LAPS_MAX            (10)        // 最大游泳躺数
    #define APP_MODULE_DATA_CENTER_VO2_MAX_MAX         (99)        // 最大摄氧量(ml/kg/min)
    #define APP_MODULE_DATA_CENTER_TE_MAX              (49)        // 有氧训练效果(x10)
    /* --------------------------------------------------------------------- */
} app_module_data_center_t;
#pragma pack(pop)

/*@brief 重置数据中心的数据源
 *       执行时间(load -...-reset-...-dump)
 *       当数据load失败时会自动调用reset
 */
void app_module_data_center_reset(void);

/*@brief      获得数据中心的数据源,直接本地修改
 *@param[out] data_center数据中心索引地址
 *@retval     数据中心的数据是否无效
 */
bool app_module_data_center_source(app_module_data_center_t **data_center);

/*@brief 更换目标数据类型并锁定数据中心,静止更换其他类型数据
 */
void app_module_data_center_load(uint32_t type);

/*@brief 更新目标数据类型并锁定数据中心,允许换其他类型数据
 */
void app_module_data_center_dump(void);

/*@brief 数据中心模组初始化
 *       内部使用: 被manage线程使用
 */
void app_module_data_center_ready(void);

#endif
