#ifndef APP_LV_UI_PRESENTER_STOPWATCH_H
#define APP_LV_UI_PRESENTER_STOPWATCH_H

typedef struct {
    void    (*get_tick_main)(app_lv_ui_stopwatch_t *stopwatch);
    void    (*get_tick_list)(app_lv_ui_stopwatch_t **stopwatch);
    uint8_t (*get_tick_num)(void);
    void    (*refr_tick_list)(void);
    void    (*clean_tick_list)(void);
    bool    (*get_onoff)(void);
    void    (*reset)(void);
    void    (*start)(void);
    void    (*stop)(void);
} app_lv_ui_presenter_stopwatch_t;

extern app_lv_ui_presenter_stopwatch_t app_lv_ui_presenter_stopwatch;

#endif
