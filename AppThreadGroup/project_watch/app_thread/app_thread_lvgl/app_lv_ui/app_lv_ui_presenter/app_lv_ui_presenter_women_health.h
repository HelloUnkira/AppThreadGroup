#ifndef APP_LV_UI_PRESENTER_WOMEN_HEALTH_H
#define APP_LV_UI_PRESENTER_WOMEN_HEALTH_H

typedef struct {
    bool (*is_valid)(void);
    void (*get_dtime)(uint16_t *year, uint8_t *month);
    void (*get_menstrual)(uint8_t *day_s, uint8_t *day_e);
    void (*get_pregnancy)(uint8_t *day_s, uint8_t *day_e);
} app_lv_ui_presenter_women_health_t;

extern app_lv_ui_presenter_women_health_t app_lv_ui_presenter_women_health;

#endif
