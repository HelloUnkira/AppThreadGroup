#ifndef APP_MODULE_REMIND_MENSTRUATION_H
#define APP_MODULE_REMIND_MENSTRUATION_H

#define APP_MODULE_REMIND_MENSTRUATION_MAX          (10)
#define APP_MODULE_REMIND_MENSTRUATION_ITEM_MAX     (10)

typedef struct {
    uint64_t menstrual_last;        // 最近经期时间
    uint8_t  menstrual_cycle;       // 经期周期
    uint8_t  menstrual_length;      // 经期长度
    
    uint8_t  ovulation_interval;    // 从下一个经期开始前到排卵日的间隔,一般为14天
    uint8_t  ovulation_before;      // 排卵日之前易孕期的天数,一般为5
    uint8_t  ovulation_after;       // 排卵日之后易孕期的天数,一般为4
    
    uint8_t  menstrual_start;       // 经期开始日提醒 提前天数
    uint8_t  menstrual_end;         // 经期结束日提醒 提前天数
    uint8_t  pregnancy_start;       // 易孕期开始日提醒 提前天数
    uint8_t  pregnancy_end;         // 易孕期结束日提醒 提前天数
    uint8_t  ovulation_start;       // 排卵日开始日提醒 提前天数
    
    struct {
        uint64_t utc;               // 经期开始(utc)
        uint8_t  cycle;             // 周期长度(天)
        uint8_t  menstrual;         // 经期长度(天)
    } history[APP_MODULE_REMIND_MENSTRUATION_ITEM_MAX];
    uint8_t  avg_menstrual;
    uint8_t  avg_cycle;
    
    uint8_t  onoff:1;
    uint8_t  onoff_menstrual:1;
    uint8_t  onoff_ovulation:1;
    uint8_t  onoff_pregnancy:1;
    
} app_module_remind_menstruation_info_t;

/*@brief 提醒组是否为提醒女性健康组
 *@param remind_group 提醒组
 *@retval 是否为女性健康组
 */
bool app_module_remind_menstruation_group_check(uint32_t remind_group);

/*@brief 锁定提醒女性健康列表
 *       此时提醒女性健康列表不再迭代,也不生成事件
 *       可以直接对提醒女性健康列表本地修改
 */
void app_module_remind_menstruation_array_lock(void);

/*@brief 解锁提醒女性健康列表
 *       此时提醒女性健康列表继续迭代,继续生成事件
 *       不可以直接对提醒女性健康列表本地修改
 */
void app_module_remind_menstruation_array_unlock(void);

/*@brief 提醒女性健康列表
 *@param menstruation_item 提醒女性健康列表
 *@param menstruation_info 提醒女性健康信息列表
 */
void app_module_remind_menstruation_array(app_module_remind_item_t **menstruation_item, app_module_remind_menstruation_info_t **menstruation_info);

/*@brief 提醒女性健康转储到外存
 */
void app_module_remind_menstruation_dump(void);

/*@brief 提醒女性健康加载到内存
 */
void app_module_remind_menstruation_load(void);

/*@brief 初始化提醒女性健康组
 */
void app_module_remind_menstruation_ready(void);

#endif
