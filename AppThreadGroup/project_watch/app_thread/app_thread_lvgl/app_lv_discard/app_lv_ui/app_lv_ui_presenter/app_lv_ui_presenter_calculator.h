#ifndef APP_LV_UI_PRESENTER_CALCULATOR_H
#define APP_LV_UI_PRESENTER_CALCULATOR_H

typedef struct {
    bool (*math_expression)(char *expression, double *result, char *error_string);
} app_lv_ui_presenter_calculator_t;

extern app_lv_ui_presenter_calculator_t app_lv_ui_presenter_calculator;

#endif
