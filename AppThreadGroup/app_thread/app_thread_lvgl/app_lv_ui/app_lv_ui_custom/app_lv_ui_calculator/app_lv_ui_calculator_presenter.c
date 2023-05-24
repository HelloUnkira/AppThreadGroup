
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "app_lv_ui_calculator.h"
#include "app_lv_ui_calculator_presenter.h"

#include "Calculator.h"

static char *app_lv_ui_calculator_error_string = NULL;

void app_lv_ui_error_print(const char *error_string)
{
    uint32_t len = strlen(error_string);
    if (app_lv_ui_calculator_error_string != NULL)
        app_mem_free(app_lv_ui_calculator_error_string);
    app_lv_ui_calculator_error_string = app_mem_alloc(len + 1);
    memset(app_lv_ui_calculator_error_string, 0, len + 1);
    strcpy(app_lv_ui_calculator_error_string, error_string);
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_math_expression(char *expression, double *result, char *error_string)
{
    /* 外部引用 */
    bool retval = CalculatorMathExpression(expression, result, app_lv_ui_error_print);
    
     if (!retval && app_lv_ui_calculator_error_string != NULL) {
        strcpy(error_string, app_lv_ui_calculator_error_string);
        app_mem_free(app_lv_ui_calculator_error_string);
        app_lv_ui_calculator_error_string = NULL;
    }
    return retval;
}

app_lv_ui_calculator_presenter_t app_lv_ui_calculator_presenter = {
    .math_expression = app_lv_ui_math_expression,
};
