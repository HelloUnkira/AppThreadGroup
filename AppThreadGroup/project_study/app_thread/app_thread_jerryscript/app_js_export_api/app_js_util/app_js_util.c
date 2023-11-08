/*实现目标:
 *对jerry的库进行封装
 *注意:
 *约定俗成
 *根据高级语言的特性,所有类型都是对象
 *下述中的指定类型的值变量都统一叫做xxx值对象
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_js_lib.h"

/*@brief     对象数量计数回调
 *           搭配jerry_object_foreach使用
 *@prarm[in] user_data_p 一个对象数量计数器地址(jerry_size_t类型)
 */
bool app_js_log_object_count_cb(const jerry_value_t property_name, const jerry_value_t property_value, void *user_data_p)
{
    JERRY_UNUSED(property_name);
    JERRY_UNUSED(property_value);
    (*(jerry_size_t *)user_data_p)++;
    return true;
}

bool jmem_is_heap_pointer(const void *pointer)
{
    APP_SYS_LOG_WARN("");
    JERRY_UNUSED(pointer);
    return true;
}
