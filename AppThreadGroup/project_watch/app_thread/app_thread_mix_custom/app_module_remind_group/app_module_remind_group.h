#ifndef APP_MODULE_REMIND_GROUP_H
#define APP_MODULE_REMIND_GROUP_H

typedef enum {
    app_module_remind_item_none = 0,
    app_module_remind_item_custom,
    app_module_remind_item_repeat,
} app_module_remind_item_type_t;

typedef enum {
    app_module_remind_group_alarm,
    app_module_remind_group_calendar,
    app_module_remind_group_matter,
    app_module_remind_group_number,
    /* 添加其他类型... */
} app_module_remind_group_type_t;

typedef struct {
    app_module_clock_t clock;   /* 提醒点(所有字段均需有效) */
    uint64_t offset_utc;        /* 提前或者迟延提醒(秒) */
                                /* 该字段是相对clock.utc做的左右偏移 */
                                /* 内部不做转换而由上层同步维护 */
                                /* 动态修改它产生动态提醒的效果 */
    /* 轮转提醒字段: */
    uint32_t repeat;            /* 轮转时间(秒),0为不轮转 */
    /* 常规提醒字段: */
    uint32_t month:12;          /* 一月到十二月 */
    uint32_t week:7;            /* 日一二三四五六 */
    /* 继续添加... */
    uint32_t type:4;            /* 提醒模式 */
    uint32_t valid:1;           /* 有效性 */
    uint32_t onoff:1;           /* 启停状态 */
} app_module_remind_item_t;

typedef struct {
    void       *array;      /* 提醒项数组 */
    uint32_t    number;     /* 提醒项数量 */
} app_module_remind_group_t;

typedef struct {
    uint32_t remind_group;
    uint32_t remind_item;
    uint32_t remind_type;
} app_module_remind_package_t;

/*为什么需要组?
 *总有一些奇奇怪怪的提醒流程不可以被预见
 *组的目标就是将可能繁杂的提醒流程化繁为简
 *对于需要使用提醒组的每一个模组而言
 *它将自己完全管理数据的整个内存驻留
 *这对于存储化管理而言是直观且操作效率明显的
 *因为一个复杂的提醒序列可以被一个组给合理的组织起来
 *简化后的整套逻辑对于每一个组的操作均是固定的
 *注意:
 *    需要更新组内的提醒项
 *    无论是更新一个还是多个
 *    先将提醒组移除然后更新提醒项
 *    最后将其重新加入回组管理
 *注意:提醒组是访问组,并不修改组中提醒项的数据内容
 *     为了使访问的准确性,对其修改严格遵守组使用流程
 */

/*@brief 添加提醒组
 *@param array  提醒数组
 *@param number 提醒数量
 *@retval 提醒组id(错误为-1)
 */
uint32_t app_module_remind_group_add(app_module_remind_item_t *array, uint32_t number);

/*@brief 移除提醒组
 *@param remind_group_id 提醒组id
 */
void app_module_remind_group_del(uint32_t remind_group_id);

/*@brief 更新提醒组
 *       内部使用: 被mix custom线程使用
 */
void app_module_remind_group_reflush(void);

/*@brief 初始化提醒组
 */
void app_module_remind_group_ready(void);

#endif
