#ifndef APP_LV_UI_COUNTDOWN_H
#define APP_LV_UI_COUNTDOWN_H

typedef struct {
    uint8_t  hour;       /* 时 */
    uint8_t  minute;     /* 分 */
    uint8_t  second;     /* 秒 */
    uint16_t msec;       /* 毫秒 */
} app_lv_ui_countdown_t;

#endif
