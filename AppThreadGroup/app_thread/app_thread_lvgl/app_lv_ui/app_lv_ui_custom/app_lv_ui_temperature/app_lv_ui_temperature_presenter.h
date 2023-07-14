#ifndef APP_LV_UI_TEMPERATURE_PRESENTER_H
#define APP_LV_UI_TEMPERATURE_PRESENTER_H

typedef struct {
    void  (*measure)(void);
    bool  (*measure_status)(void);
    bool  (*measure_result)(void);
    float (*get_value)(void);
} app_lv_ui_temperature_presenter_t;

extern app_lv_ui_temperature_presenter_t app_lv_ui_temperature_presenter;

#endif
