#ifndef SCUI_UI_PRESENTER_H
#define SCUI_UI_PRESENTER_H

typedef struct {
    /* date time: */
    bool     (*is_am)(void);
    bool     (*is_pm)(void);
    bool     (*is_24)(void);
    uint64_t (*get_utc)(void);
    uint16_t (*get_year)(void);
    uint8_t  (*get_month)(void);
    uint8_t  (*get_day)(void);
    uint8_t  (*get_hour)(void);
    uint8_t  (*get_minute)(void);
    uint8_t  (*get_second)(void);
    uint8_t  (*get_week)(void);
    /* dev data: */
    uint32_t (*get_hr_min)(void);
    uint32_t (*get_hr_max)(void);
    uint32_t (*get_hr_cur)(void);
    uint32_t (*get_spo2_min)(void);
    uint32_t (*get_spo2_max)(void);
    uint32_t (*get_spo2_cur)(void);
    uint32_t (*get_kcal_min)(void);
    uint32_t (*get_kcal_max)(void);
    uint32_t (*get_kcal_cur)(void);
    uint32_t (*get_step_min)(void);
    uint32_t (*get_step_max)(void);
    uint32_t (*get_step_cur)(void);
    uint32_t (*get_batt_min)(void);
    uint32_t (*get_batt_max)(void);
    uint32_t (*get_batt_cur)(void);
    uint32_t (*get_dist_min)(void);
    uint32_t (*get_dist_max)(void);
    uint32_t (*get_dist_cur)(void);
    int16_t  (*get_temp_cur)(void);
    bool     (*get_temp_unit)(void);
    
    /* keep adding... */
} scui_ui_presenter_t;

/* scui 唯一交互实例回调集合 */
extern scui_ui_presenter_t scui_ui_presenter;

#endif
