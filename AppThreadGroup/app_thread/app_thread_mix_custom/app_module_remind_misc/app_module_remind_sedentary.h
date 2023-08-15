#ifndef APP_MODULE_REMIND_SEDENTARY_H
#define APP_MODULE_REMIND_SEDENTARY_H

/* 定制化策略(泛用策略见时间表设计) */

typedef struct {
    uint16_t reflush;       /* 刷新时间分,内部使用 */
    uint8_t  am_time_s[2];  /* 上午提醒起始[时,分] */
    uint8_t  am_time_e[2];  /* 上午提醒结束[时,分] */
    uint8_t  pm_time_s[2];  /* 下午提醒起始[时,分] */
    uint8_t  pm_time_e[2];  /* 下午提醒结束[时,分] */
    uint8_t  am_valid:1;    /* 启停状况 */
    uint8_t  pm_valid:1;    /* 启停状况 */
    uint8_t  interval:6;    /* 提醒间隔(分) */
    uint8_t  week:7;        /* 周:日一二三四五六:[0, 6] */
} app_module_remind_sedentary_t;

/*@brief     走动提醒设置
 *@param[in] remind_sedentary 走动提醒参数
 */
void app_module_remind_sedentary_set(app_module_remind_sedentary_t *remind_sedentary);

/*@brief      走动提醒获取
 *@param[out] remind_sedentary 走动提醒参数
 */
void app_module_remind_sedentary_get(app_module_remind_sedentary_t *remind_sedentary);

/*@brief      走动提醒默认设置
 *@param[out] remind_sedentary 走动提醒参数
 */
void app_module_remind_sedentary_reset(app_module_remind_sedentary_t *remind_sedentary);

/*@brief 走到提醒时长刷新
 *       如果产生了活动或走动,应该刷新它
 */
void app_module_remind_sedentary_reflush(void);

/*@brief 走动提醒更新事件
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_sedentary_xmin_update(void);

/*@brief 走动提醒转储到外存
 */
void app_module_remind_sedentary_dump(void);

/*@brief 走动提醒加载到内存
 */
void app_module_remind_sedentary_load(void);

/*@brief 走动提醒模组初始化
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_sedentary_ready(void);

#endif
