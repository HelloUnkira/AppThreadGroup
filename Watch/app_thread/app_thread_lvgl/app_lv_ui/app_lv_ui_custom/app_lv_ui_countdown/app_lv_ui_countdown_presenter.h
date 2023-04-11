#ifndef APP_LV_UI_COUNTDOWN_PRESENTER_H
#define APP_LV_UI_COUNTDOWN_PRESENTER_H

typedef struct {
    void (*get_tick)(app_lv_ui_countdown_t *countdown);
    bool (*get_onoff)(void);
    void (*set)(app_lv_ui_countdown_t *countdown);
    void (*get)(app_lv_ui_countdown_t *countdown);
    void (*reset)(void);
    void (*start)(void);
    void (*stop)(void);
    void (*remind_start)(void);
    void (*remind_end)(void);
} app_lv_ui_countdown_presenter_t;

extern app_lv_ui_countdown_presenter_t app_lv_ui_countdown_presenter;

#endif
