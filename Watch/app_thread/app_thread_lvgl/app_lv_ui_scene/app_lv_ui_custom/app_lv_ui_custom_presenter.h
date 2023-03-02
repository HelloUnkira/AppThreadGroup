#ifndef APP_LV_UI_CUSTOM_PRESENTER_H
#define APP_LV_UI_CUSTOM_PRESENTER_H

typedef struct {
    bool    (*is_am)(void);
    bool    (*is_pm)(void);
    bool    (*is_24_mode)(void);
    uint8_t (*get_user_hour)(void);
    uint8_t (*get_user_minute)(void);
} app_lv_ui_custom_presenter_t;

extern app_lv_ui_custom_presenter_t app_lv_ui_custom_presenter;

#endif
