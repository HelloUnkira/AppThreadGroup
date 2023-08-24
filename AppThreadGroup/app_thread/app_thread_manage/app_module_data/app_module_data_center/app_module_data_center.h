#ifndef APP_MODULE_DATA_CENTER_H
#define APP_MODULE_DATA_CENTER_H

/*备注:
 *    将各种各样的零碎数据统一打包到此结构中
 *    通过不同的结构块区分不同类型的数据区域
 *    并以一个完全统一的接口对需要持久化的资源统一访问
 *    因为数据中心使用了缓存机制,为了提升缓存性能
 *    尽可能的将大数据块切碎成各种各样类型的小集合
 *    那么缓存的热缓冲机制能充分被发挥出效果
 */
typedef enum {
    app_module_data_center_none = 0,
    app_module_data_center_module_source,           /* 模组资源 */
    app_module_data_center_module_weather,          /* 模组资源 */
    app_module_data_center_module_world_time,       /* 模组资源 */
    app_module_data_center_remind_alarm,            /* 提醒闹钟 */
    app_module_data_center_remind_calendar,         /* 提醒日历 */
    app_module_data_center_remind_matter,           /* 提醒事项 */
    app_module_data_center_system_profile,          /* 系统配置 */
    app_module_data_center_system_data,             /* 系统数据 */
    app_module_data_center_user_profile,            /* 用户配置 */
    app_module_data_center_user_data,               /* 用户数据 */
    app_module_data_center_num,
} app_module_data_center_type_t;

typedef struct {
    uint32_t crc32; /* 公共字段,以做校验 */
    union {
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源占位标识(内部偏移计算使用) */
        uintptr_t offset;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(module source) */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        struct {
            /* 模组专用字段,模组内部使用:静止访问该字段 */
            app_module_clock_t                  clock;
            app_module_countdown_t              countdown;
            app_module_stopwatch_t              stopwatch;
            app_module_remind_drink_t           remind_drink;
            app_module_remind_sedentary_t       remind_sedentary;
            app_module_do_not_disturb_t         do_not_disturb;
            /* keep adding... */
        } module_source;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(module weather) */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        struct {
            app_module_weather_t weather[APP_MODULE_WEATHER_MAX];
        } module_weather;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(module world_time) */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        struct {
            app_module_world_time_t world_time[APP_MODULE_WORLD_TIME_MAX];
        } module_world_time;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(remind alarm) */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        struct {
            app_module_remind_item_t       alarm_item[APP_MODULE_REMIND_ALARM_MAX];
            app_module_remind_alarm_info_t alarm_info[APP_MODULE_REMIND_ALARM_MAX];
        } remind_alarm;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(remind calendar) */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        struct {
            app_module_remind_item_t          calendar_item[APP_MODULE_REMIND_CALENDAR_MAX];
            app_module_remind_calendar_info_t calendar_info[APP_MODULE_REMIND_CALENDAR_MAX];
        } remind_calendar;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(remind matter) */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        struct {
            app_module_remind_item_t        matter_item[APP_MODULE_REMIND_MATTER_MAX];
            app_module_remind_matter_info_t matter_info[APP_MODULE_REMIND_MATTER_MAX];
        } remind_matter;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(system profile) */
        struct {
            uint64_t id:16;             // 设备号
            uint64_t is_pair:1;         // 配对状态
            uint64_t ver_major:8;       // 主版本号
            uint64_t ver_minor:8;       // 次版本号
            uint64_t ver_patch:8;       // 补版本号
            uint8_t  system_mode;       // 设备工作模式
            /* system profile: backlight */
            struct {
                uint8_t backlight_level;
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
                uint8_t onoff:1;                // 省电模式开关
                uint8_t gesture:1;              // 手势亮屏开关
                uint8_t cover_sleep:1;          // 覆盖息屏开关
                uint8_t dtouch_awake:1;         // 双击亮屏开关
                uint8_t backlight_auto:1;       // 背光自动模式开关
                uint8_t backlight_level:1;      // 背光等级
                uint8_t backlight_duration:1;   // 背光持续时间
                /* 省电模式主要针对屏幕亮灭做限制,可以继续添加其他内容 */
            } save_power;
            /* system data: device usage statistics */
            struct {
                uint64_t record_utc:36;         // 第一次更新UTC
                uint8_t  detect_level;          // 检测电量
                uint8_t  detect_count;          // 检测计数
                uint8_t  battery_level;         // 电量
                struct {
                    uint64_t utc:36;            // 记录时间
                    uint64_t voltage:16;        // 电压值
                    uint64_t percent:8;         // 电量
                } charge[3];                    // [0:开始;1:结束;2:充满]
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
            /* keep adding */
        } system_data;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(user profile) */
        struct {
            uint64_t brithday_utc:36;   // brithday
            uint64_t version_app:9;     // major:minor:patch(3:3:3)
            uint8_t height;             // cm
            uint8_t weight;             // cm
            uint8_t stride;             // stride cm
            uint8_t stride_run;         // stride run cm
            uint8_t stride_walk;        // stride walk cm
            uint8_t gender:1;           // 0 as boy
                                        // 1 as girl
            uint8_t metric:1;           // 0 as km
                                        // 1 as mi
            uint8_t mode_time:1;        // 0 as 24 mode
                                        // 1 as 12 mode
            uint8_t mode_temp:1;        // 0 as C" mode
                                        // 1 as F" mode
            uint8_t mode_voice:1;       // 0 as disable voice
                                        // 1 as enabble voice
            uint8_t mode_vibrate:1;     // 0 as disable vibrate
                                        // 1 as enabble vibrate
            uint8_t state_bind:1;       // 0 as unbind state
                                        // 1 as bind   state
            uint8_t state_hand:1;       // 0 as left   hand
                                        // 1 as right  hand
            uint8_t state_week:2;       // 0 as start in monday
                                        // 1 as start in sunday
                                        // 2 as start in saturday
            uint8_t phone_type:2;       // 0 as Android
                                        // 1 as IOS
                                        // 2 as Harmony
            uint8_t find_phone:1;       // 1 as open
                                        // 0 as close
            uint8_t find_band:1;        // 1 as open
                                        // 0 as close
            /* user profile: language */
            uint8_t lang;
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
            /* user profile: user volume */
            struct {
                uint8_t volume_system_level;
                uint8_t volume_system_index;
                uint8_t volume_notify_level;
                uint8_t volume_notify_index;
                uint8_t volume_call_level;
                uint8_t volume_call_index;
                uint8_t volume_alarm_level;
                uint8_t volume_alarm_index;
                uint8_t volume_sport_level;
                uint8_t volume_sport_index;
            } volume;
            /* user profile: user fitness monitor */
            struct {
                uint8_t time_s[2];              // 监控起始[时,分]
                uint8_t time_e[2];              // 监控结束[时,分]
                uint8_t notify:2;               // 0 as close
                                                // 1 as mute
                                                // 2 as allow
                                                // 3 as disallow
                uint8_t onoff:1;                // 1 as open
                                                // 0 as close
            } fitness;
            /* user profile: user noise monitor */
            struct {
                uint8_t time_s[2];              // 监控起始[时,分]
                uint8_t time_e[2];              // 监控结束[时,分]
                uint8_t value;                  // 阈值
                uint8_t onoff:1;                // 1 as open
                                                // 0 as close
                uint8_t onoff_noise:1;          // 1 as open
                                                // 0 as close
            } noise;
            /* user profile: user temperature monitor */
            struct {
                uint8_t time_s[2];              // 监控起始[时,分]
                uint8_t time_e[2];              // 监控结束[时,分]
                uint8_t onoff:1;                // 1 as open
                                                // 0 as close
            } temperature;
            /* user profile: user motion recognition */
            struct {
                /* 添加各个模式的开关... */
                uint8_t onoff:1;                // 1 as open
                                                // 0 as close
            } motion_recognition;
            /* user profile: aod watchface */
            struct {
                uint8_t time_s[2];              // 提醒起始[时,分]
                uint8_t time_e[2];              // 提醒结束[时,分]
                uint8_t index:8;                // 表盘索引号
                uint8_t onoff:1;                // 1 as open
                                                // 0 as close
            } aod_watchface;
            /* user profile: ui theme level2 */
            uint8_t ui_theme_level2;
            /* keep adding */
        } user_profile;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(user data) */
        struct {
            /* user data: function usage statistics(buried data) */
            struct {
                uint8_t  usage_type;
                /* use count:(use type is enum re) */
                uint16_t usage_count[app_module_data_center_usage_num];
                uint16_t usage_queue[3];        // [0:curr;1:prev;2:prev 1]
                uint16_t sports_list_queue[3];  // [0:curr;1:prev;2:prev 1]
                uint16_t sports_list_count[app_module_data_center_usage_sports_list];
                uint16_t sports_list_type[app_module_data_center_usage_sports_list];
            } func_usage;
            /* keep adding */
        } user_data;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源:keep adding */
        /* --------------------------------------------------------------------- */
    };
} app_module_data_center_t;

/*@brief    更换目标数据类型并锁定数据中心
 *param[in] type 数据中心类型(app_module_data_center_t(type))
 *@retval   数据中心的缓存地址
 */
app_module_data_center_t * app_module_data_center_take(uint32_t type);

/*@brief 解锁数据中心
 */
void app_module_data_center_give(void);

/*@brief     刷新数据中心
 *@param[in] force 强制刷新数据中心
 *           不建议使用,这会导致非正常的逻辑
 */
void app_module_data_center_reflush(bool force);

/*@brief 数据中心模组初始化
 *       内部使用: 被manage线程使用
 */
void app_module_data_center_ready(void);

#endif
