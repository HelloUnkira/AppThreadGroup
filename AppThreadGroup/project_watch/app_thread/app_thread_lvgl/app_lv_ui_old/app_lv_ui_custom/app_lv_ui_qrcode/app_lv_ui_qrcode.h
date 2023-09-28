#ifndef APP_LV_UI_QRCODE_H
#define APP_LV_UI_QRCODE_H

typedef struct {
    const char * (*qrcode_str)(void);
} app_lv_ui_qrcode_presenter_t;

extern app_lv_ui_qrcode_presenter_t app_lv_ui_qrcode_presenter;

#endif
