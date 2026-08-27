#ifndef APP_MODULE_DATA_CENTER_SRC_H
#define APP_MODULE_DATA_CENTER_SRC_H

/*备注:
 *    将各种各样的零碎数据统一打包到此结构中
 *    通过不同的结构块区分不同类型的数据区域
 *    并以一个完全统一的接口对需要持久化的资源统一访问
 *    因为数据中心使用了缓存机制,为了提升缓存性能
 *    尽可能的将大数据块切碎成各种各样类型的小集合
 *    那么缓存的热缓冲机制能充分被发挥出效果
 */
typedef enum {
    app_module_data_center_src_none = 0,
    app_module_data_center_src_module_source,            /* 模组资源 */
    app_module_data_center_src_module_weather,           /* 模组资源 */
    app_module_data_center_src_module_world_time,        /* 模组资源 */
    app_module_data_center_src_remind_alarm,             /* 提醒闹钟 */
    app_module_data_center_src_remind_calendar,          /* 提醒日历 */
    app_module_data_center_src_remind_matter,            /* 提醒事项 */
    app_module_data_center_src_system_profile,           /* 系统配置 */
    app_module_data_center_src_system_data,              /* 系统数据(绑定/省电/影院/运动识别) */
    app_module_data_center_src_user_profile,             /* 用户体征+运动目标+手势 */
    app_module_data_center_src_system_time,              /* 系统时间与显示格式 */
    app_module_data_center_src_display_info,             /* 显示与菜单(背光/主题/菜单) */
    app_module_data_center_src_sleep_setting,            /* 睡眠设置 */
    app_module_data_center_src_heart_health,             /* 心率/血氧/血压/压力/情绪 */
    app_module_data_center_src_watch_config,             /* 表盘配置 */
    app_module_data_center_src_protocol_device_info,     /* 协议存储:设备信息 */
    app_module_data_center_src_protocol_elec_card,       /* 协议存储:电子保卡 */
    app_module_data_center_src_protocol_music,           /* 协议存储:音乐 */
    app_module_data_center_src_protocol_msg_info,        /* 协议存储:消息通知 */
    app_module_data_center_src_protocol_contact,         /* 协议存储:联系人 */
    app_module_data_center_src_protocol_account,         /* 协议存储:账户 */
    app_module_data_center_src_protocol_sport_state,     /* 协议存储:运动状态 */
    app_module_data_center_src_protocol_fem_cycle,       /* 协议存储:生理周期 */
    app_module_data_center_src_protocol_sport_mng,       /* 协议存储:运动管理 */
    app_module_data_center_src_protocol_sport_rcd,       /* 协议存储:运动记录 */
    app_module_data_center_src_protocol_position,        /* 协议存储:地理位置 */
    app_module_data_center_src_num,
} app_module_data_center_src_type_t;

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
        } module_remind_alarm;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(remind calendar) */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        struct {
            app_module_remind_item_t          calendar_item[APP_MODULE_REMIND_CALENDAR_MAX];
            app_module_remind_calendar_info_t calendar_info[APP_MODULE_REMIND_CALENDAR_MAX];
        } module_remind_calendar;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(remind matter) */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        struct {
            app_module_remind_item_t        matter_item[APP_MODULE_REMIND_MATTER_MAX];
            app_module_remind_matter_info_t matter_info[APP_MODULE_REMIND_MATTER_MAX];
        } module_remind_matter;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(system profile) */
        struct {
            uint8_t system_mode;            // 设备工作模式
        } system_profile;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(system data) */
        struct {
            /* system data: app bind */
            struct {
                uint32_t new_bind_uid;      // 新绑定用户ID
                uint32_t save_bind_uid;     // 已保存用户ID
                uint32_t qr_random;         // 二维码随机码
                uint8_t  conn_before:1;     // 曾经连接过
                uint8_t  bind_notify:1;     // 有绑定通知
                uint8_t  our_app_conn:1;    // 已连自家APP
                uint8_t  bind_start:1;      // 正在绑定
                uint8_t  disp_screen:1;     // 已显示二维码界面
                uint8_t  conn_sec_cnt;      // 连接秒计数
            } app_binding;
            /* system data: power save */
            struct {
                uint8_t power_save:1;       // 省电模式
                uint8_t everbright:1;       // 屏幕常亮
                uint8_t hr_auto:1;          // 省电下心率自动
                uint8_t spo2_auto:1;        // 省电下血氧自动
                uint8_t pressure_auto:1;    // 省电下压力自动
                uint8_t emotion_auto:1;     // 省电下情绪自动
                uint8_t theater:1;          // 影院模式
                uint8_t motion_reco:1;      // 运动识别
            } power;
        } system_data;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(user profile) */
        struct {
            uint32_t birthday;              // 生日
            uint8_t  age;                   // 年龄
            uint8_t  gender;                // 性别
            uint16_t height;                // 身高(cm)
            uint16_t weight;                // 体重(kg)
            uint8_t  stride_run;            // 跑步步长(cm)
            uint8_t  stride_walk;           // 走路步长(cm)
            struct {
                uint32_t step;              // 目标步数
                uint32_t kcal;              // 目标卡路里(kcal)
                uint32_t meter;             // 目标距离(m)
                uint32_t duration;          // 目标时长(s)
                uint32_t mh_duration;       // 目标中高强度时长(s)
            } goal;
            struct {
                uint8_t shake:1;            // shake
                uint8_t wrist:1;            // wrist
                uint8_t time_s[2];          // wrist:监控起始[时,分]
                uint8_t time_e[2];          // wrist:监控结束[时,分]
            } gesture;
        } user_profile;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(system time) */
        struct {
            uint32_t zone:16;               // 时区偏移(小时)
            uint32_t date_fmt:1;            // 日期格式
            uint32_t is_12h:1;              // 12/24小时制
            uint32_t is_metric:1;           // 公/英制
            uint32_t lang;                  // 语言ID
            uint32_t sync_on:1;             // 时间同步开关
        } system_time;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(display info) */
        struct {
            uint8_t display_off_sec;        // 熄屏时间
            uint8_t backlight_percent;      // 背光亮度
            uint8_t last_brightness;        // 上次背光
            uint8_t theme_screen;           // 菜单主题
            uint8_t effect_style;           // 切换特效
            uint8_t interface_style;        // 一级菜单特效
            uint8_t jump_home;              // 跳回首页
            uint8_t dial_lock:1;            // 表盘切换锁
        } display_info;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(sleep setting) */
        struct {
            struct {
                uint8_t day_of_week;        // 有效星期
                uint8_t fall_hour;          // 入睡时
                uint8_t fall_min;           // 入睡分
                uint8_t wake_hour;          // 起床时
                uint8_t wake_min;           // 起床分
                uint8_t is_alarm:1;         // 是否到闹钟
            } schedule[7];
            uint8_t  mode_switch:1;         // 睡眠模式开关
            uint8_t  mode_status:1;         // 睡眠模式生效中
            uint16_t target_duration;       // 睡眠目标(min)
            uint8_t  remind_switch:1;       // 睡眠提醒开关
            uint16_t remind;                // 提前提醒(min)
            uint8_t  snooze_on:1;           // 贪睡开关
            uint32_t snooze_time;           // 贪睡时间
            uint8_t  snooze_cnt;            // 贪睡次数
        } sleep_setting;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(heart health) */
        struct {
            uint8_t  hr_value;              // 最近心率
            uint32_t hr_ts;                 // 心率时间戳
            uint8_t  rest_hr;               // 静息心率
            uint32_t rest_hr_ts;            // 静息心率时间戳
            uint8_t  spo2;                  // 血氧
            uint32_t spo2_ts;               // 血氧时间戳
            uint8_t  pressure;              // 压力
            uint32_t pressure_ts;           // 压力时间戳
            uint8_t  emotion;               // 情绪
            uint32_t emotion_ts;            // 情绪时间戳
            uint8_t  sbp;                   // 收缩压
            uint8_t  dbp;                   // 舒张压
            uint32_t bp_ts;                 // 血压时间戳
            struct {
                uint8_t hr_auto:1;          // 心率自动测试
                uint8_t spo2_auto:1;        // 血氧自动测试
                uint8_t bp_auto:1;          // 血压自动测试
                uint8_t pressure_auto:1;    // 压力自动测试
                uint8_t emotion_auto:1;     // 情绪自动测试
                uint8_t hr_interval;        // 心率间隔(min)
                uint8_t spo2_interval;      // 血氧间隔(min)
                uint8_t bp_interval;        // 血压间隔(min)
            } auto_test;
            struct {
                uint8_t lwarn_on:1;         // 低心率提醒
                uint8_t hwarn_on:1;         // 高心率提醒
                uint8_t lwarn;              // 低提醒值
                uint8_t hwarn;              // 高提醒值
                uint8_t spo2_warn:1;        // 血氧下限提醒
                uint8_t spo2_limit;         // 血氧下限值
            } warn;
        } heart_health;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(watch config) */
        struct {
            uint16_t pixel_width;           // 表盘宽
            uint16_t pixel_height;          // 表盘高
            uint32_t file_crc32;            // 表盘文件CRC32
            uint8_t  cur_index;             // 当前表盘索引
            uint8_t  aod_index;             // AOD表盘索引
            uint8_t  sort[8];               // 表盘排序
        } watch_config;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(protocol device info) */
        struct {
            char    model[16];              // 设备型号
            char    hw_ver[16];             // 硬件版本
            char    sw_ver[16];             // 软件版本
            char    sn[32];                 // 序列号
            char    bt_addr[32];            // 蓝牙地址
            char    pid[16];                // 产品ID
            uint8_t battery;                // 电量
        } protocol_device_info;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(protocol elec card) */
        struct {
            uint8_t is_activate;            // 是否激活
            uint8_t is_reported;            // 是否上报
        } protocol_elec_card;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(protocol music) */
        struct {
            char    singer[64];             // 歌手
            char    song_name[64];          // 曲名
            uint8_t play_st;                // 播放状态
            uint8_t max_vol;                // 最大音量
            uint8_t cur_vol;                // 当前音量
            uint8_t app_st;                 // app状态
        } protocol_music;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(protocol msg info) */
        struct {
            char    app_name[64];           // 应用名
            char    contact[64];            // 联系人
            char    content[128];           // 内容
            char    phone[32];              // 电话
            uint8_t msg_id;                 // 消息ID
            uint8_t msg_type;               // 消息类型
            uint8_t vibrate;                // 震动开关
        } protocol_msg_info;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(protocol contact) */
        struct {
            char     name[128];             // 联系人名
            uint16_t name_len;              // 名字长度
            char     phone[32];             // 号码
            uint16_t phone_len;             // 号码长度
            uint8_t  state;                 // 接听/挂断
        } protocol_contact;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(protocol account) */
        struct {
            char    account[255];           // 账号
            uint8_t acc_len;                // 账号长度
            uint8_t pair_state;             // 配对状态
            uint8_t app_role;               // app角色
        } protocol_account;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(protocol sport state) */
        struct {
            uint8_t  monitor_st;            // 监测占用
            uint8_t  operator_t;            // 执行类型
            uint8_t  sport_type;            // 运动分类
            uint32_t start_time;            // 开始时间
            uint8_t  workout_type;          // 单次运动类型
            uint32_t op_time;               // 操作时间
        } protocol_sport_state;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(protocol fem cycle) */
        struct {
            uint8_t  remind_sw;             // 总开关
            uint8_t  menstr_remind;         // 经期开始提醒
            uint8_t  menstr_end_remind;     // 经期结束提醒
            uint8_t  ovulat_remind;         // 易孕开始提醒
            uint8_t  ovulat_end_remind;     // 易孕结束提醒
            uint32_t cycle_start;           // 经期开始时间
            uint32_t cycle_end;             // 经期结束时间
            uint8_t  keep_days;             // 持续天数
            uint16_t cycle_days;            // 周期天数
        } protocol_fem_cycle;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(protocol sport mng) */
        struct {
            uint8_t max_add_num;            // 最大可添加数
            uint8_t min_add_num;            // 最小可添加数
            uint8_t sport_type_count;       // 运动类型数量
            uint8_t sport_type[32];         // 运动类型列表
        } protocol_sport_mng;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(protocol sport rcd) */
        struct {
            uint16_t id;                    // 记录ID
            uint8_t  status;                // 状态
            uint32_t start_time;            // 开始时间
            uint32_t end_time;              // 结束时间
            uint32_t calorie;               // 卡路里
            uint32_t distance;              // 距离(米)
            uint32_t step;                  // 步数
            uint32_t duration;              // 时长(秒)
            uint16_t speed;                 // 速度
            uint8_t  type;                  // 运动类型
        } protocol_sport_rcd;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(protocol position) */
        struct {
            uint8_t  speed;                 // 速度(dm/s)
            uint16_t distance;              // 距离(分米)
            int16_t  altitude;              // 海拔(米)
            uint32_t total_dist;            // 总距离(分米)
            uint32_t start_time;            // 开始时间
            uint32_t end_time;              // 结束时间
            int32_t  latitude;              // 纬度
            int32_t  longitude;             // 经度
            uint16_t bearing;               // 方向
            uint16_t accuracy;              // 精度
        } protocol_position;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源:keep adding */
        /* --------------------------------------------------------------------- */
    };
} app_module_data_center_src_t;

typedef struct {
    app_module_data_center_src_type_t type;
    const char *data_name;
    uint32_t data_size;
} app_module_data_center_src_table_t;

#endif
