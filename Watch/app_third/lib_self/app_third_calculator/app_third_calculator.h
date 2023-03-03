#ifndef APP_THIRD_CALCULATOR_H
#define APP_THIRD_CALCULATOR_H

/* 解析时产生的错误信息输出重定向接口 */
typedef void (*app_third_calculator_error_print)(const char *error_string);

/*@brief     数学表达式解析
 *param[in]  expression 数学表达式
 *param[out] result     数学表达式解析结果
 *param[out] print      错误日志信息
 #retval     解析是否有效
 */
bool app_third_calculator_math_expression(char *expression, double *result, app_third_calculator_error_print print);

#endif
