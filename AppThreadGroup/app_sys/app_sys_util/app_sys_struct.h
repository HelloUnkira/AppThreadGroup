#ifndef APP_SYS_STRUCT_H
#define APP_SYS_STRUCT_H

/*一个简单的结构体方法重载器
 *辅助工具,一般不使用
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint8_t        	argument1;               //comment1
	uint16_t       	argument2;               //comment2
	uint32_t       	argument3;               //comment3
	uint64_t       	argument4;               //comment4
	float          	argument5;               //comment5
	double         	argument6;               //comment6
} app_sys_test_t;

/*@brief 设置语义
 *@param 设置 argument1
 */
static inline void app_sys_test_set_argument1(app_sys_test_t *instance, uint8_t argument1)
{
	instance->argument1 = argument1;
}

/*@brief 设置语义
 *@param 设置 argument2
 */
static inline void app_sys_test_set_argument2(app_sys_test_t *instance, uint16_t argument2)
{
	instance->argument2 = argument2;
}

/*@brief 设置语义
 *@param 设置 argument3
 */
static inline void app_sys_test_set_argument3(app_sys_test_t *instance, uint32_t argument3)
{
	instance->argument3 = argument3;
}

/*@brief 设置语义
 *@param 设置 argument4
 */
static inline void app_sys_test_set_argument4(app_sys_test_t *instance, uint64_t argument4)
{
	instance->argument4 = argument4;
}

/*@brief 设置语义
 *@param 设置 argument5
 */
static inline void app_sys_test_set_argument5(app_sys_test_t *instance, float argument5)
{
	instance->argument5 = argument5;
}

/*@brief 设置语义
 *@param 设置 argument6
 */
static inline void app_sys_test_set_argument6(app_sys_test_t *instance, double argument6)
{
	instance->argument6 = argument6;
}

/*@brief  获取语义
 *@retval 获取 argument1
 */
static inline uint8_t app_sys_test_get_argument1(app_sys_test_t *instance)
{
	return instance->argument1;
}

/*@brief  获取语义
 *@retval 获取 argument2
 */
static inline uint16_t app_sys_test_get_argument2(app_sys_test_t *instance)
{
	return instance->argument2;
}

/*@brief  获取语义
 *@retval 获取 argument3
 */
static inline uint32_t app_sys_test_get_argument3(app_sys_test_t *instance)
{
	return instance->argument3;
}

/*@brief  获取语义
 *@retval 获取 argument4
 */
static inline uint64_t app_sys_test_get_argument4(app_sys_test_t *instance)
{
	return instance->argument4;
}

/*@brief  获取语义
 *@retval 获取 argument5
 */
static inline float app_sys_test_get_argument5(app_sys_test_t *instance)
{
	return instance->argument5;
}

/*@brief  获取语义
 *@retval 获取 argument6
 */
static inline double app_sys_test_get_argument6(app_sys_test_t *instance)
{
	return instance->argument6;
}

#endif
