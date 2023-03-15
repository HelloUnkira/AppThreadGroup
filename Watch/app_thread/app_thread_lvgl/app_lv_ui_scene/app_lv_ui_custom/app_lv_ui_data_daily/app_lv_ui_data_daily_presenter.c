
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"

#include "app_lv_ui_data_daily.h"
#include "app_lv_ui_data_daily_presenter.h"

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_get_tar_cal(void)
{
    /* 今日目标卡路里 */
    return 50;
}

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_get_cur_cal(void)
{
    /* 今日当前卡路里 */
    static uint32_t count = 0;
    return count++ % 50;
}

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_get_tar_step(void)
{
    /* 今日目标步数 */
    return 50;
}

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_get_cur_step(void)
{
    /* 今日当前步数 */
    static uint32_t count = 0;
    return count++ % 50;
}

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_get_tar_walk_cnt(void)
{
    /* 今日目标走动达标次数 */
    return 12;
}

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_get_cur_walk_cnt(void)
{
    /* 今日当前走动达标次数 */
    static uint32_t count = 0;
    return count++ % 12;
}

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_get_tab_step_max(void)
{
    /* 步数图表最大值 */
    return 100;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_tab_step(uint32_t table[24])
{
    /* 获取今日[0~23]每小时的步数记录 */
    for (uint8_t idx = 0; idx < 24; table[idx] = rand() % 100, idx++);
}

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_get_tab_cal_max(void)
{
    /* 卡路里图表最大值 */
    return 100;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_tab_cal(uint32_t table[24])
{
    /* 获取今日[0~23]每小时的卡路里记录 */
    for (uint8_t idx = 0; idx < 24; table[idx] = rand() % 100, idx++);
}

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_get_tab_act_max(void)
{
    /* 运动时长图表最大值 */
    return 60;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_tab_act(uint32_t table[24])
{
    /* 获取今日[0~23]每小时的卡路里记录 */
    for (uint8_t idx = 0; idx < 24; table[idx] = rand() % 60, idx++);
}

/*@brief lvgl ui数据交互回调
 */
static float app_lv_ui_get_dist(void)
{
    /* 距离 */
    return 114.514;
}

/*@brief lvgl ui数据交互回调
 */
static uint16_t app_lv_ui_get_rest(void)
{
    /* 运动后恢复时间(休息时间)(高8位为时,低8位为分) */
    union {
        uint8_t  data_u8[2];
        uint16_t data_u16;
    } data = {.data_u8[0] = 114, .data_u8[1] = 514};
    return 114.514;
}

/*@brief lvgl ui数据交互回调
 */
static uint8_t app_lv_ui_get_o2_max(void)
{
    /* 最大摄氧量 */
    return 13;
}

app_lv_ui_data_daily_presenter_t app_lv_ui_data_daily_presenter = {
    .get_tar_cal        = app_lv_ui_get_tar_cal,
    .get_cur_cal        = app_lv_ui_get_cur_cal,
    .get_tar_step       = app_lv_ui_get_tar_step,
    .get_cur_step       = app_lv_ui_get_cur_step,
    .get_tar_walk_cnt   = app_lv_ui_get_tar_walk_cnt,
    .get_cur_walk_cnt   = app_lv_ui_get_cur_walk_cnt,
    .get_tab_step_max   = app_lv_ui_get_tab_step_max,
    .get_tab_step       = app_lv_ui_get_tab_step,
    .get_tab_cal_max    = app_lv_ui_get_tab_cal_max,
    .get_tab_cal        = app_lv_ui_get_tab_cal,
    .get_tab_act_max    = app_lv_ui_get_tab_act_max,
    .get_tab_act        = app_lv_ui_get_tab_act,
    .get_dist           = app_lv_ui_get_dist,
    .get_rest           = app_lv_ui_get_rest,
    .get_o2_max         = app_lv_ui_get_o2_max,
};
