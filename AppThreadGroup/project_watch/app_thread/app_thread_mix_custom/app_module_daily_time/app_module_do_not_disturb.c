/*实现目标:
 *    勿扰模式相关的功能组件
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/* 时间表检查函数:app_module_daily_time_do_not_disturb_f */
APP_MODULE_DAILY_TIME_F(app_module_daily_time_do_not_disturb_t, app_module_daily_time_do_not_disturb_f, 2);

/*@brief     勿扰模式设置
 *@param[in] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_set(app_module_do_not_disturb_t *do_not_disturb)
{
    /* 更新数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(&data_center_src->module_source.do_not_disturb, do_not_disturb, sizeof(app_module_do_not_disturb_t));
    app_module_data_center_give();
}

/*@brief      勿扰模式获取
 *@param[out] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_get(app_module_do_not_disturb_t *do_not_disturb)
{
    /* 提取数据中心资源 */
    app_module_data_center_src_t *data_center_src = app_module_data_center_take(app_module_data_center_src_module_source);
    memcpy(do_not_disturb, &data_center_src->module_source.do_not_disturb, sizeof(app_module_do_not_disturb_t));
    app_module_data_center_give();
}

/*@brief  勿扰模式状态检查
 *@retval 是否为勿扰态
 */
bool app_module_do_not_disturb_status(void)
{
    app_module_do_not_disturb_t do_not_disturb;
    app_module_do_not_disturb_get(&do_not_disturb);
    return app_module_daily_time_do_not_disturb_f(&do_not_disturb.daily_time);
}

/*@brief      勿扰模式默认设置
 *@param[out] do_not_disturb 勿扰模式参数
 */
void app_module_do_not_disturb_reset(app_module_do_not_disturb_t *do_not_disturb)
{
    /* 9:00~11:30;13:00~18:00;workday */
    do_not_disturb->daily_time.table[0].time_s[0] = 9;
    do_not_disturb->daily_time.table[0].time_s[1] = 0;
    do_not_disturb->daily_time.table[0].time_s[2] = 0;
    do_not_disturb->daily_time.table[0].time_e[0] = 11;
    do_not_disturb->daily_time.table[0].time_e[1] = 30;
    do_not_disturb->daily_time.table[0].time_e[2] = 0;
    do_not_disturb->daily_time.table[1].time_s[0] = 13;
    do_not_disturb->daily_time.table[1].time_s[1] = 0;
    do_not_disturb->daily_time.table[1].time_s[2] = 0;
    do_not_disturb->daily_time.table[1].time_e[0] = 18;
    do_not_disturb->daily_time.table[1].time_e[1] = 0;
    do_not_disturb->daily_time.table[1].time_e[2] = 0;
    do_not_disturb->daily_time.week = 0b0111110;
    do_not_disturb->daily_time.bits = 0b11;
}
