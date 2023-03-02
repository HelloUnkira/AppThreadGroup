#ifndef APP_LV_UI_CLOCK_PRESENTER_H
#define APP_LV_UI_CLOCK_PRESENTER_H

typedef struct {
    uint64_t (*get_utc)(void);
    uint16_t (*get_year)(void);
    uint8_t  (*get_month)(void);
    uint8_t  (*get_day)(void);
    uint8_t  (*get_hour)(void);
    uint8_t  (*get_minute)(void);
    uint8_t  (*get_second)(void);
    uint8_t  (*get_week)(void);
} app_lv_ui_clock_presenter_t;

extern app_lv_ui_clock_presenter_t app_lv_ui_clock_presenter;

#endif
