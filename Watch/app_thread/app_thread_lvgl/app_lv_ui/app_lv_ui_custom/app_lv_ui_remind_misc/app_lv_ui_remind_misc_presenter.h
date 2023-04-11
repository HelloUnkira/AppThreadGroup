#ifndef APP_LV_UI_REMIND_MISC_PRESENTER_H
#define APP_LV_UI_REMIND_MISC_PRESENTER_H

typedef struct {
    void (*remind_custom_start)(void);
    void (*remind_custom_end)(void);
    /* 特殊提醒单独补充即可 */
} app_lv_ui_remind_misc_presenter_t;

extern app_lv_ui_remind_misc_presenter_t app_lv_ui_remind_misc_presenter;

#endif
