#ifndef APP_LV_UI_COUNTDOWN_PRESENTER_H
#define APP_LV_UI_COUNTDOWN_PRESENTER_H

typedef struct {
    bool    (*is_am)(void);
    bool    (*is_pm)(void);
    bool    (*is_24_mode)(void);
    uint8_t (*get_user_hour)(void);
    uint8_t (*get_user_minute)(void);
    void    (*get_tick)(app_lv_ui_countdown_t *countdown);
    bool    (*get_onoff)(void);
    void    (*set)(app_lv_ui_countdown_t *countdown);
    void    (*get)(app_lv_ui_countdown_t *countdown);
    void    (*reset)(void);
    void    (*start)(void);
    void    (*stop)(void);
} app_lv_ui_countdown_presenter_t;

extern app_lv_ui_countdown_presenter_t app_lv_ui_countdown_presenter;

#endif
