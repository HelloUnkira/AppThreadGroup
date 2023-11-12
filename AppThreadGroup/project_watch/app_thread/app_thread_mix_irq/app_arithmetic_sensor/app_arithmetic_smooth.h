#ifndef APP_ARITHMETIC_SMOOTH_H
#define APP_ARITHMETIC_SMOOTH_H

/* 平滑滤波器: */
typedef struct {
    #define APP_ARITHMETIC_SMOOTH_SIZE  8
	int32_t buf[APP_ARITHMETIC_SMOOTH_SIZE];
	int32_t sum;
    int32_t avg;
	uint8_t pos:7;
	uint8_t sta:1;
} app_arithmetic_smooth_t;

/*@brief     平滑滤波器重置
 *@param[in] smooth 实例
 */
void app_arithmetic_smooth_reset(app_arithmetic_smooth_t *smooth);

/*@brief     平滑滤波器重置
 *@param[in] smooth 实例
 *@param[in] data   新数据
 *@retval    滤波后数据
 */
int32_t app_arithmetic_smooth_execute(app_arithmetic_smooth_t *smooth, int32_t data);

#endif
