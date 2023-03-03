
#include "Calculator.h"
#include "app_third_calculator.h"

/*@brief     数学表达式解析
 *param[in]  expression 数学表达式
 *param[out] result     数学表达式解析结果
 *param[out] print      错误日志信息
 #retval     解析是否有效
 */
bool app_third_calculator_math_expression(char *expression, double *result, app_third_calculator_error_print print)
{
    return CalculatorMathExpression(expression, result, print);
}
