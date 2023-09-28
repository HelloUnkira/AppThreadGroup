#ifndef APP_LV_UI_STOPWATCH_H
#define APP_LV_UI_STOPWATCH_H

typedef struct {
    uint8_t  hour;       /* 时 */
    uint8_t  minute;     /* 分 */
    uint8_t  second;     /* 秒 */
    uint16_t msec;       /* 毫秒 */
} app_lv_ui_stopwatch_t;

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
} app_lv_ui_stopwatch_presenter_t;

extern app_lv_ui_stopwatch_presenter_t app_lv_ui_stopwatch_presenter;

#endif
