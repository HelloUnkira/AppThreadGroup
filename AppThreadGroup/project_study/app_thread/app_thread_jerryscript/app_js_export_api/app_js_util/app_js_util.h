#ifndef APP_JS_UTIL_H
#define APP_JS_UTIL_H

/* JS响应C函数回调(定义:提供给JS调用) */
#define APP_JS_DECL_FUNC(name)                                  \
    jerry_value_t name(const jerry_call_info_t *call_info_p,    \
                       const jerry_value_t      args_p[],       \
                       const jerry_length_t     args_c)

/* JS响应C函数回调(引用:内部调用) */
#define APP_JS_CHAIN_FUNC(name)                                 \
    name(call_info_p, args_p, args_c)

/* JS响应C函数回调(定义:泛型,内部调用) */
#define APP_JS_DECL_FUNC_ARGS(name, ...)                        \
    jerry_value_t name(const jerry_call_info_t *call_info_p,    \
                       const jerry_value_t      args_p[],       \
                       const jerry_length_t     args_c, __VA_ARGS__)

/* JS响应C函数回调(引用:泛型,内部调用) */
#define APP_JS_CHAIN_FUNC_ARGS(name, ...)                       \
    name(call_info_p, args_p, args_c, __VA_ARGS__)

#ifndef JERRY_UNUSED
#define JERRY_UNUSED(x) ((void)(x))
#endif

/*@brief 对象数量计数回调
 *       搭配jerry_object_foreach使用
 *@prarm user_data_p 一个对象数量计数器地址(jerry_size_t类型)
 */
bool app_js_log_object_count_cb(const jerry_value_t property_name, const jerry_value_t property_value, void *user_data_p);

#endif
