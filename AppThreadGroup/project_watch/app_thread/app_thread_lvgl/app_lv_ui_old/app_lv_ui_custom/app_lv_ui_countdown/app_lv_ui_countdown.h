#ifndef APP_LV_UI_COUNTDOWN_H
#define APP_LV_UI_COUNTDOWN_H

typedef struct {
    uint8_t  hour;       /* 时 */
    uint8_t  minute;     /* 分 */
    uint8_t  second;     /* 秒 */
    uint16_t msec;       /* 毫秒 */
} app_lv_ui_countdown_t;

typedef struct {
    void (*get_tick)(app_lv_ui_countdown_t *countdown);
    bool (*get_onoff)(void);
    void (*set)(app_lv_ui_countdown_t *countdown);
    void (*get)(app_lv_ui_countdown_t *countdown);
    void (*reset)(void);
    void (*start)(void);
    void (*stop)(void);
} app_lv_ui_countdown_presenter_t;

extern app_lv_ui_countdown_presenter_t app_lv_ui_countdown_presenter;

#endif
