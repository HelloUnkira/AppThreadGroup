#ifndef APP_MODULE_DO_NOT_DISTURB_H
#define APP_MODULE_DO_NOT_DISTURB_H

/* DND: Do Not Disturb, 勿扰 */

/* 勿扰时间表数量,一般俩个就够了,上午一个下午一个 */
#define APP_MODULE_DO_NOT_DISTURB_TIME_TABLE_NUM    2

typedef struct {
    struct {
        uint8_t time_s[3];      /* 勿扰开始时间[时,分,秒] */
        uint8_t time_e[3];      /* 勿扰结束时间[时,分,秒] */
    } table[APP_MODULE_DO_NOT_DISTURB_TIME_TABLE_NUM];              /* 勿扰时间表 */
    uint32_t valid_bits:APP_MODULE_DO_NOT_DISTURB_TIME_TABLE_NUM;   /* 勿扰时间表位域 */
    uint32_t week:7;            /* 周:日一二三四五六:[0, 6] */
} app_module_do_not_disturb_t;

/*@brief     勿扰模式设置
 *@param[in] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_set(app_module_do_not_disturb_t *do_not_disturb);

/*@brief     勿扰模式获取
 *@param[in] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_get(app_module_do_not_disturb_t *do_not_disturb);

/*@brief  勿扰模式状态检查
 *@retval 是否为勿扰态
 */
bool app_module_do_not_disturb_status(void);

/*@brief 勿扰模式默认设置
 */
void app_module_do_not_disturb_reset(void);

/*@brief 勿扰状态转储到外存
 */
void app_module_do_not_disturb_dump(void);

/*@brief 勿扰状态加载到内存
 */
void app_module_do_not_disturb_load(void);

/*@brief 勿扰模组初始化
 *       内部使用: 被mix custom线程使用
 */
void app_module_do_not_disturb_ready(void);

#endif
