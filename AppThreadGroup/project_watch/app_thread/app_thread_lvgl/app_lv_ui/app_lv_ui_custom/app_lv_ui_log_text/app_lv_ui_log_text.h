#ifndef APP_LV_UI_LOG_TEXT_H
#define APP_LV_UI_LOG_TEXT_H

typedef struct {
    void (*peek_reset)(void);
    const char * (*peek_one)(void);
    void (*reset)(void);
} app_lv_ui_log_text_presenter_t;

extern app_lv_ui_log_text_presenter_t app_lv_ui_log_text_presenter;

#endif
