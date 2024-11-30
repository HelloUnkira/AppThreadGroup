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
    uint32_t (*get_hr_passby)(void);
    uint32_t (*get_spo2_min)(void);
    uint32_t (*get_spo2_max)(void);
    uint32_t (*get_spo2_cur)(void);
    uint32_t (*get_spo2_passby)(void);
    uint32_t (*get_stress_min)(void);
    uint32_t (*get_stress_max)(void);
    uint32_t (*get_stress_cur)(void);
    uint32_t (*get_stress_passby)(void);
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
    uint32_t (*get_temp_type)(void);
    uint32_t (*get_temp_icon)(void);
    int16_t  (*get_temp_min)(void);
    int16_t  (*get_temp_max)(void);
    int16_t  (*get_temp_cur)(void);
    bool     (*get_temp_unit)(void);
    /* dev data: */
    bool     (*alarm_none)(void);
    bool     (*alarm_near_state)(void);
    uint16_t (*alarm_near_date)(void);
    uint8_t  (*alarm_near_hour)(void);
    uint8_t  (*alarm_near_minute)(void);
    bool     (*sleep_none)(void);
    uint8_t  (*sleep_hour)(void);
    uint8_t  (*sleep_minute)(void);
    void     (*sleep_pct4)(uint8_t pct[4]);
    bool     (*sport_record_none)(void);
    uint32_t (*sport_record_type)(void);
    uint32_t (*sport_record_icon)(void);
    uint8_t  (*sport_record_hour)(void);
    uint8_t  (*sport_record_minute)(void);
    bool     (*women_health_none)(void);
    uint32_t (*women_health_type)(void);
    uint32_t (*women_health_icon)(void);
    uint8_t  (*women_health_hour)(void);
    uint8_t  (*women_health_minute)(void);
    bool     (*compass_invalid)(void);
    uint16_t (*compass_angle)(void);
    float    (*altimeter_pressure)(void);
    float    (*altimeter_altitude)(void);
    
    /* drv func: */
    void (*vibrate_shot)(void);
    /* keep adding... */
} scui_ui_presenter_t;

/* scui 唯一交互实例回调集合 */
extern scui_ui_presenter_t scui_ui_presenter;

#endif
