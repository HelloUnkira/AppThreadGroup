#ifndef APP_LV_UI_STOPWATCH_PRESENTER_H
#define APP_LV_UI_STOPWATCH_PRESENTER_H

typedef struct {
    bool    (*is_am)(void);
    bool    (*is_pm)(void);
    bool    (*is_24_mode)(void);
    uint8_t (*get_user_hour)(void);
    uint8_t (*get_user_minute)(void);
    void    (*get_tick_main)(app_lv_ui_stopwatch_t *stopwatch);
    void    (*get_tick_list)(app_lv_ui_stopwatch_t **stopwatch);
    uint8_t (*get_tick_num)(void);
    void    (*refr_tick_list)(void);
    void    (*clean_tick_list)(void);
    bool    (*get_onoff)(void);
    void    (*reset)(void);
    void    (*start)(void);
    void    (*stop)(void);
} app_lv_ui_stopwatch_presenter_t;

extern app_lv_ui_stopwatch_presenter_t app_lv_ui_stopwatch_presenter;

#endif

