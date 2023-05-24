#if 0
#ifndef APP_MODULE_ALARM_H
#define APP_MODULE_ALARM_H

typedef enum {
    app_module_alarm_custom,
    app_module_alarm_repeat,
} app_module_alarm_type_t;

typedef struct {
    app_module_clock_t clock_base;  /* 设置时的时钟 */
    union {
        struct {    /* 常规式闹钟 */
            app_module_clock_t clock_month;     /* 月更新提醒时钟(内部维护) */
            app_module_clock_t clock_week;      /* 周更新提醒时钟(内部维护) */
            uint32_t field_month:12;            /* 一月到十二月 */
            uint32_t field_week:7;              /* 日一二三四五六 */
        };
        struct {    /* 轮转式闹钟 */
            app_module_clock_t clock_repeat;    /* 轮转更新提醒时钟(内部维护) */
            uint32_t repeat;                    /* 轮转时间(秒),0为不轮转 */
        };
    };
    uint8_t type:3;                 /* 闹钟模式 */
    uint8_t valid:1;                /* 有效性 */
    uint8_t onoff:1;                /* 启停状态 */
    /* 使用者自行维护字段 */
    uint8_t *data;                  /* 扩充数据包 */
    uint32_t size;                  /* 扩充数据包长度 */
                                    /* 例如:日程提醒,事项提醒, */
                                    /*      闹钟名字等信息时使用到 */
} app_module_alarm_t;

/*扩充设计:
 *    因为闹钟提供闹钟组功能
 *    如果需要实现更为复杂的如:
 *    贪睡闹钟,提前或延后提醒功能
 *    只需要组合多个闹钟生成闹钟组即可
 *    开辟一个新闹钟临时的完成该功能即可,完成后直接丢弃
 *    因为这些功能的策略变动性随外界影响过大
 *    故它不应该加入到基础闹钟结构中
 */

/* 本地闹钟组限制 */
#define APP_MODULE_ALARM_GROUP_MAX  5

/*@brief 初始化闹钟组
 */
void app_module_alarm_group_ready(void);

/*@brief        注册闹钟组
 *@param[in]    array  闹钟数组
 *@param[in]    number 闹钟数量
 *@retval       闹钟组id(错误为-1)
 */
uint32_t app_module_alarm_group_register(app_module_alarm_t *array, uint32_t number);

/*@brief        闹钟组添加闹钟
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm          闹钟实例
 *@retval       闹钟id(错误为-1)
 */
uint32_t app_module_alarm_add(uint32_t alarm_group_id, app_module_alarm_t *alarm);

/*@brief        闹钟组启动闹钟
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm          闹钟实例
 */
void app_module_alarm_start(uint32_t alarm_group_id, uint32_t alarm_id);

/*@brief        闹钟组停止闹钟
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm          闹钟实例
 */
void app_module_alarm_stop(uint32_t alarm_group_id, uint32_t alarm_id);

/*@brief        闹钟组移除闹钟
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm_id       闹钟实例
 */
void app_module_alarm_del(uint32_t alarm_group_id, uint32_t alarm_id);

/*@brief        设置闹钟组中的闹钟实例
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm_id       闹钟id
 *@param[out]   alarm          闹钟实例
 */
void app_module_alarm_set(uint32_t alarm_group_id, uint32_t alarm_id, app_module_alarm_t *alarm);

/*@brief        获得闹钟组中的闹钟实例
 *@param[in]    alarm_group_id 闹钟组id
 *@param[in]    alarm_id       闹钟id
 *@param[out]   alarm          闹钟实例
 */
void app_module_alarm_get(uint32_t alarm_group_id, uint32_t alarm_id, app_module_alarm_t *alarm);

#endif
#endif
