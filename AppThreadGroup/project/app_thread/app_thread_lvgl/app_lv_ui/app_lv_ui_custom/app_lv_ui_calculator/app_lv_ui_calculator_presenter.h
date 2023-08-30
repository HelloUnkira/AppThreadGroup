#ifndef APP_LV_UI_CALCULATOR_PRESENTER_H
#define APP_LV_UI_CALCULATOR_PRESENTER_H

typedef struct {
    bool (*math_expression)(char *expression, double *result, char *error_string);
} app_lv_ui_calculator_presenter_t;

extern app_lv_ui_calculator_presenter_t app_lv_ui_calculator_presenter;

#endif
