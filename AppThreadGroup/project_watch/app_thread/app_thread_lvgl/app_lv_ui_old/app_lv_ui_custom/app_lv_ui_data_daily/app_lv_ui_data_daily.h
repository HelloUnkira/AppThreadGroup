#ifndef APP_LV_UI_DATA_DAILY_H
#define APP_LV_UI_DATA_DAILY_H

typedef struct {
    /* 今日三环目标 */
    uint32_t (*get_tar_cal)(void);          /* 今日目标卡路里 */
    uint32_t (*get_cur_cal)(void);          /* 今日当前卡路里 */
    uint32_t (*get_tar_step)(void);         /* 今日目标步数 */
    uint32_t (*get_cur_step)(void);         /* 今日当前步数 */
    uint32_t (*get_tar_walk_cnt)(void);     /* 今日目标走动达标次数 */
    uint32_t (*get_cur_walk_cnt)(void);     /* 今日当前走动达标次数 */
    /* 步数图表 */
    uint32_t (*get_tab_step_max)(void);             /* 步数图表最大值 */
    void     (*get_tab_step)(uint32_t table[24]);   /* 获取今日[0~23]每小时的步数记录 */
    /* 卡路里图表 */
    uint32_t (*get_tab_cal_max)(void);              /* 卡路里图表最大值 */
    void     (*get_tab_cal)(uint32_t table[24]);    /* 获取今日[0~23]每小时的卡路里记录 */
    /* 运动时长表 */
    uint32_t (*get_tab_act_max)(void);              /* 运动时长图表最大值 */
    void     (*get_tab_act)(uint32_t table[24]);    /* 获取今日[0~23]每小时的运动时长记录 */
    /* 距离 */
    float    (*get_dist)(void);
    /* 运动后恢复时间(休息时间)(高8位为时,低8位为分) */
    uint16_t (*get_rest)(void);
    /* 最大摄氧量 */
    uint8_t  (*get_o2_max)(void);
    /* 继续补充... */
} app_lv_ui_data_daily_presenter_t;

extern app_lv_ui_data_daily_presenter_t app_lv_ui_data_daily_presenter;

#endif
