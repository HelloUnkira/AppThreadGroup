#ifndef APP_LV_UI_TRACE_TEXT_PRESENTER_H
#define APP_LV_UI_TRACE_TEXT_PRESENTER_H

typedef struct {
    const char * (*peek_one)(void);
    void (*reset)(void);
} app_lv_ui_trace_text_presenter_t;

extern app_lv_ui_trace_text_presenter_t app_lv_ui_trace_text_presenter;

#endif
