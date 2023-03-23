#ifdef APP_MODULE_CLOCK_H
#ifdef APP_MODULE_CLOCK_CB_H

/*实现目标:
 *一个代替动态注册的,静态本地导入的时钟秒时分天回调集
 *它会被内置到mix irq线程中作为一个辅助功能使用
 *秒时分天回调会在到点时执行下面注册的回调,按顺序执行
 *更新回调则当本地时钟被更新时调用,辅助上层更新对应的事项
 *因为这些回调都是编码时注册,本地对应回调的实际空间占用
 *最终编译器都会将其转化为函数代码段函数表,不占用内存
 *注意:这个文件不可以以任何方式被其他地方引用
 *     它的唯一使用者为app_module_clock.c
 *     !!!这只是一个定时事件产生的模组
 *     !!!不应该在里面直接执行业务流程
 *     !!!取而替代的是回调中产生事件包发送
 *     !!!不要越过主线程的管控
 */

/* 函数声明及其引用(注意:不要导入头文件,直接提供函数名即可): */
extern void app_module_clock_cb1_empty(app_module_clock_t clock[1]);
extern void app_module_system_ctrl_check(app_module_clock_t clock[1]);
extern void app_module_watchdog_ctrl_check(app_module_clock_t clock[1]);
extern void app_module_alarm_group_update(app_module_clock_t clock[1]);
extern void app_module_remind_drink_update(app_module_clock_t clock[1]);

/* 函数声明及其引用(注意:不要导入头文件,直接提供函数名即可): */
void app_module_clock_cb2_empty(app_module_clock_t clock[2], uint32_t event);

/* 时钟回调组集合: */

/* 秒回调 */
static const app_module_clock_cb1 app_module_clock_second_cb[] = {
    app_module_clock_cb1_empty,
    /* 补充中...... */
    app_module_system_ctrl_check,   /* 系统控制更新检查 */
    app_module_watchdog_ctrl_check, /* 子线程软件看门狗更新检查 */
    app_module_alarm_group_update,  /* 闹钟更新 */
};

/* 分回调 */
static const app_module_clock_cb1 app_module_clock_minute_cb[] = {
    app_module_clock_cb1_empty,
    /* 补充中...... */
    app_module_remind_drink_update,     /* 喝水提醒更新 */
};

/* 时回调 */
static const app_module_clock_cb1 app_module_clock_hour_cb[] = {
    app_module_clock_cb1_empty,
    /* 补充中...... */
};

/* 天回调 */
static const app_module_clock_cb1 app_module_clock_day_cb[] = {
    app_module_clock_cb1_empty,
    /* 补充中...... */
};

/* 更新回调 */
static const app_module_clock_cb2 app_module_clock_update_cb[] = {
    app_module_clock_cb2_empty,
    /* 补充中...... */
};

/* 其他类型有待补充... */

static const uint32_t app_module_clock_second_cb_size = 
               sizeof(app_module_clock_second_cb) /
               sizeof(app_module_clock_second_cb[0]);
static const uint32_t app_module_clock_minute_cb_size = 
               sizeof(app_module_clock_minute_cb) /
               sizeof(app_module_clock_minute_cb[0]);
static const uint32_t app_module_clock_hour_cb_size = 
               sizeof(app_module_clock_hour_cb) /
               sizeof(app_module_clock_hour_cb[0]);
static const uint32_t app_module_clock_day_cb_size = 
               sizeof(app_module_clock_day_cb) /
               sizeof(app_module_clock_day_cb[0]);
static const uint32_t app_module_clock_update_cb_size = 
               sizeof(app_module_clock_update_cb) /
               sizeof(app_module_clock_update_cb[0]);

/*  */

/*@brief     一类时钟空回调(占位回调,防止优化禁用)
 *@param[in] clock 时钟实例
 */
void app_module_clock_cb1_empty(app_module_clock_t clock[1])
{
}

/*@brief     一类时钟空回调(占位回调,防止优化禁用)
 *@param[in] last_clock 时钟实例
 *@param[in] last_clock 时钟实例
 *@param[in] event      时钟事件
 */
void app_module_clock_cb2_empty(app_module_clock_t clock[2], uint32_t event)
{
}

#endif
#endif
