#ifndef APP_LV_UI_PRESENTER_COUNTDOWN_H
#define APP_LV_UI_PRESENTER_COUNTDOWN_H

typedef struct {
    void (*get_tick)(app_lv_ui_countdown_t *countdown);
    bool (*get_onoff)(void);
    void (*set)(app_lv_ui_countdown_t *countdown);
    void (*get)(app_lv_ui_countdown_t *countdown);
    void (*reset)(void);
    void (*start)(void);
    void (*stop)(void);
} app_lv_ui_presenter_countdown_t;

extern app_lv_ui_presenter_countdown_t app_lv_ui_presenter_countdown;

#endif
