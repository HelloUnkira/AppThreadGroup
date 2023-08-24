#ifndef APP_SYS_ERR_CODE_H
#define APP_SYS_ERR_CODE_H

/* 可选择使用的错误码 */
typedef enum {
    app_sys_error_code_success,             ///< success
    /*  */
    //operation
    app_sys_error_code_internal,            ///< internal error
    app_sys_error_code_forbidden,           ///< forbidden operation
    app_sys_error_code_unsupported,         ///< unsupported operation
    app_sys_error_code_timeout,             ///< operation timed out
    app_sys_error_code_busy,                ///< operation busy
    app_sys_error_code_leak_resources,      ///< operation leak enough resources
    app_sys_error_code_no_space,            ///< not enough space for operation
    //operation argument
    app_sys_error_code_invalid_args,        ///< invalid argumenter
    app_sys_error_code_invalid_addr,        ///< bad memory address
    app_sys_error_code_invalid_stat,        ///< invalid state
    app_sys_error_code_invalid_flag,        ///< invalid flag
    app_sys_error_code_invalid_data_size,   ///< invalid data size
    app_sys_error_code_invalid_data,        ///< invalid data
    app_sys_error_code_invalid_size,        ///< invalid size
    app_sys_error_code_invalid_file_name,   ///< invalid file name
    app_sys_error_code_null,                ///< null pointer
    app_sys_error_code_not_found,           ///< not found
    app_sys_error_code_no_mem,              ///< no memory for operation
    app_sys_error_code_disabled_soft_dev,   ///< soft device has been disabled
    //system status
    app_sys_error_code_svc_handler_miss,    ///< svc handler is miss
    app_sys_error_code_connect_count,       ///< maximum connection count exceeded.
    app_sys_error_code_low_power,           ///< battery percent is too low
    app_sys_error_code_bt_ota,              ///< not enough resources for operation
    /*  */
    app_sys_error_code_num,
} app_sys_error_code_type_t;

#endif

