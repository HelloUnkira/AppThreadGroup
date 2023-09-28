#ifndef APP_LV_UI_PRESENTER_CLOCK_H
#define APP_LV_UI_PRESENTER_CLOCK_H

typedef struct {
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
    /* extern */
    void     (*format_clock_1)(char str[20]);
} app_lv_ui_presenter_clock_t;

extern app_lv_ui_presenter_clock_t app_lv_ui_presenter_clock;

#endif
