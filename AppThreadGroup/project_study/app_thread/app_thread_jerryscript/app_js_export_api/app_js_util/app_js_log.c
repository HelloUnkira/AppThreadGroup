/*实现目标:
 *    JS定时器
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_js_lib.h"

static uint32_t app_js_log_level = 0;

/*@brief JS日志递归格式化输出
 */
static void app_js_log_recursion(const jerry_value_t value, uint32_t level)
{
    /* 这里只监控几个必要的基础对象和复合对象的日志输出,其他类型的默认未知 */
    
    /* bool类型:打印true或者false */
    if (jerry_value_is_boolean(value)) {
        APP_SYS_LOG_INFO_RAW("%s", jerry_value_is_true(value)  ? "true" :
                                   jerry_value_is_false(value) ? "false" : "unknown");
        return;
    }
    /* number类型 */
    if (jerry_value_is_number(value)) {
        APP_SYS_LOG_INFO_RAW("%.5f", jerry_value_as_number(value));
        return;
    }
    /* string类型 */
    if (jerry_value_is_string(value)) {
        jerry_size_t  buffer_s = jerry_string_size(value, JERRY_ENCODING_UTF8);
        jerry_char_t *buffer_p = jerry_heap_alloc(buffer_s + 1);
        memset(buffer_p, 0, sizeof(jerry_char_t) * (buffer_s + 1));
        jerry_string_to_buffer(value, JERRY_ENCODING_UTF8, buffer_p, buffer_s);
        APP_SYS_LOG_INFO_RAW("%s", buffer_p);
        jerry_heap_free(buffer_p, buffer_s + 1);
        return;
    }
    /* object类型 */
    if (jerry_value_is_object(value)) {
        jerry_size_t  key_cnt  = 0;
        jerry_value_t key_list = jerry_object_keys(value);
        jerry_object_foreach(value, app_js_log_object_count_cb, &key_cnt);
        APP_SYS_LOG_INFO_RAW("{");
        for (jerry_size_t idx = 0; idx < key_cnt; idx++) {
            jerry_value_t key_value = jerry_object_get_index(key_list, idx);
            if (jerry_value_is_string(key_value)) {
                jerry_size_t  buffer_s = jerry_string_size(key_value, JERRY_ENCODING_UTF8);
                jerry_char_t *buffer_p = jerry_heap_alloc(buffer_s + 1);
                memset(buffer_p, 0, sizeof(jerry_char_t) * (buffer_s + 1));
                jerry_string_to_buffer(key_value, JERRY_ENCODING_UTF8, buffer_p, buffer_s);
                APP_SYS_LOG_INFO_RAW("%s", buffer_p);
                jerry_heap_free(buffer_p, buffer_s + 1);
                return;
            }
            APP_SYS_LOG_INFO_RAW(":");
            jerry_value_t sub_value = jerry_object_get(value, key_value);
            app_js_log_recursion(sub_value, level + 1);
            jerry_value_free(sub_value);
            jerry_value_free(key_value);
            if (idx < key_cnt - 1)
                APP_SYS_LOG_INFO_RAW(",");
        }
        APP_SYS_LOG_INFO_RAW("}");
        jerry_value_free(key_list);
        return;
    }
    /* array类型 */
    if (jerry_value_is_array(value)) {
        jerry_size_t  key_cnt  = 0;
        jerry_value_t key_list = jerry_object_keys(value);
        jerry_object_foreach(value, app_js_log_object_count_cb, &key_cnt);
        APP_SYS_LOG_INFO_RAW("[");
        for (jerry_size_t idx = 0; idx < key_cnt; idx++) {
            jerry_value_t key_value = jerry_object_get_index(key_list, idx);
            jerry_value_t sub_value = jerry_object_get(value, key_value);
            app_js_log_recursion(sub_value, level + 1);
            jerry_value_free(sub_value);
            jerry_value_free(key_value);
            if (idx < key_cnt - 1)
                APP_SYS_LOG_INFO_RAW(",");
        }
        APP_SYS_LOG_INFO_RAW("]");
        jerry_value_free(key_list);
        return;
    }
    /* function类型 */
    if (jerry_value_is_function(value)) {
        APP_SYS_LOG_INFO_RAW("(function)");
    }
    /* not adapter yet */
    {
        APP_SYS_LOG_INFO_RAW("(unknown)");
    }
}

/*@brief JS日志等级设置
 */
static APP_JS_DECL_FUNC(app_js_log_level_set)
{
    JERRY_UNUSED(call_info_p);
    JERRY_UNUSED(args_p);
    JERRY_UNUSED(args_c);
    
    app_js_log_level = jerry_value_as_uint32(args_p[0]);
    return jerry_null();
}

/*@brief JS日志等级获取
 */
static APP_JS_DECL_FUNC(app_js_log_level_get)
{
    JERRY_UNUSED(call_info_p);
    JERRY_UNUSED(args_p);
    JERRY_UNUSED(args_c);
    
    return jerry_number(app_js_log_level);
}

/*@brief JS日志输出:DEBUG
 */
static APP_JS_DECL_FUNC(app_js_log_debug)
{
    if (app_js_log_level <= 0) {
        for (jerry_length_t idx = 0; idx < args_c; idx++)
            app_js_log_recursion(args_p[idx], 0);
    }
}

/*@brief JS日志输出:INFO
 */
static APP_JS_DECL_FUNC(app_js_log_info)
{
    if (app_js_log_level <= 1) {
        for (jerry_length_t idx = 0; idx < args_c; idx++)
            app_js_log_recursion(args_p[idx], 0);
    }
}

/*@brief JS日志输出:WARN
 */
static APP_JS_DECL_FUNC(app_js_log_warn)
{
    if (app_js_log_level <= 2) {
        for (jerry_length_t idx = 0; idx < args_c; idx++)
            app_js_log_recursion(args_p[idx], 0);
    }
}

/*@brief JS日志输出:ERROR
 */
static APP_JS_DECL_FUNC(app_js_log_error)
{
    if (app_js_log_level <= 3) {
        for (jerry_length_t idx = 0; idx < args_c; idx++)
            app_js_log_recursion(args_p[idx], 0);
    }
}

/*@brief JS日志
 */
void app_js_log_ready(void)
{
    jerry_value_t func_value, prop_name_val, ret_val;
    jerry_value_t global_obj_val = jerry_current_realm();
    func_value = jerry_function_external(app_js_log_level_set);
    prop_name_val = jerry_string_sz("app_js_log_level_set");
    ret_val = jerry_object_set (global_obj_val, prop_name_val, func_value);
    jerry_value_free(func_value);
    jerry_value_free (prop_name_val);
    jerry_value_free (ret_val);
    func_value = jerry_function_external(app_js_log_level_get);
    prop_name_val = jerry_string_sz("app_js_log_level_get");
    ret_val = jerry_object_set (global_obj_val, prop_name_val, func_value);
    jerry_value_free(func_value);
    jerry_value_free (prop_name_val);
    jerry_value_free (ret_val);
    func_value = jerry_function_external(app_js_log_debug);
    prop_name_val = jerry_string_sz("app_js_log_debug");
    ret_val = jerry_object_set (global_obj_val, prop_name_val, func_value);
    jerry_value_free(func_value);
    jerry_value_free (prop_name_val);
    jerry_value_free (ret_val);
    func_value = jerry_function_external(app_js_log_info);
    prop_name_val = jerry_string_sz("app_js_log_info");
    ret_val = jerry_object_set (global_obj_val, prop_name_val, func_value);
    jerry_value_free(func_value);
    jerry_value_free (prop_name_val);
    jerry_value_free (ret_val);
    func_value = jerry_function_external(app_js_log_warn);
    prop_name_val = jerry_string_sz("app_js_log_warn");
    ret_val = jerry_object_set (global_obj_val, prop_name_val, func_value);
    jerry_value_free(func_value);
    jerry_value_free (prop_name_val);
    jerry_value_free (ret_val);
    func_value = jerry_function_external(app_js_log_error);
    prop_name_val = jerry_string_sz("app_js_log_error");
    ret_val = jerry_object_set (global_obj_val, prop_name_val, func_value);
    jerry_value_free(func_value);
    jerry_value_free (prop_name_val);
    jerry_value_free (ret_val);
}

