#ifndef APP_MODULE_DATA_CENTER_H
#define APP_MODULE_DATA_CENTER_H

typedef enum {
    app_module_data_center_none = 0,
    app_module_data_center_system_profile,
    app_module_data_center_user_profile,
    app_module_data_center_user_goal,
    app_module_data_center_user_dev_info,
    app_module_data_center_num,
} app_module_data_center_type_t;

#pragma pack(push, 1)
typedef union {
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源(system profile) */
    struct {
        uint8_t system_mode:4;      // 设备工作模式
    } system_profile;
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
    } user_profile;
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源(user goal) */
    struct {
        uint64_t step:32;               // 步数
        uint64_t step_week:32;          // 步数(周目标)
        uint64_t calories:16;           // 活动卡路里(kcal)
        uint64_t calories_min:16;       // 活动卡路里最小值(kcal)
        uint64_t calories_max:16;       // 活动卡路里最大值(kcal)
        uint64_t distance:32;           // 距离
        uint64_t duration:32;           // 中高时长(分钟)
        uint64_t walk_goal:8;           // 走动目标
        uint64_t sleep_time:16;         // 睡眠时长(min)
        uint64_t onoff:1;               // 0 as open
                                        // 1 as close
    } user_goal;
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源(user dev info) */
    struct {
        uint64_t id:16;
        uint64_t version:8;
        uint64_t energy:8;
        uint64_t status:8;
        uint64_t pair:8;
    } user_dev_info;
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
    /* 数据中心管理资源:keep adding */
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
 */
void app_module_data_center_source(app_module_data_center_t **data_center);

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
