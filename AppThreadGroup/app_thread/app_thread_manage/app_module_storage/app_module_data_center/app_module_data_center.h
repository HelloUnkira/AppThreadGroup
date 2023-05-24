#ifndef APP_MODULE_DATA_CENTER_H
#define APP_MODULE_DATA_CENTER_H

typedef enum {
    app_module_data_center_default = 0,
    app_module_data_center_system_profile,
    app_module_data_center_user_profile,
    app_module_data_center_number,
} app_module_data_center_type_t;

typedef union {
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源:系统配置字段 */
    #define APP_MODULE_DATA_CENTER_SYSTEM_PROFILE   "system profile"
    /* 数据中心管理资源:系统配置结构 */
    struct {
        uint8_t system_mode:4;  /* 设备工作模式 */
    } system_profile;
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源:用户配置字段 */
    #define APP_MODULE_DATA_CENTER_USER_PROFILE     "user profile"
    /* 数据中心管理资源:用户配置结构 */
    struct {
        uint8_t reserve;
    } user_profile;
    /* --------------------------------------------------------------------- */
    /* 数据中心管理资源:keep adding */
} app_module_data_center_t;

/*@brief      获得数据中心的数据源,直接本地修改
 *@param[out] data_center数据中心索引地址
 */
void app_module_data_center_source(app_module_data_center_t **data_center);

/*@brief 更换目标数据类型并锁定数据中心,静止更换其他类型数据
 */
void app_module_data_center_load(uint32_t type);

/*@brief 更新目标数据类型并锁定数据中心,允许换其他类型数据
 */
void app_module_data_center_dump(void);

/*@brief 数据中心模组初始化
 *       内部使用: 被manage线程使用
 */
void app_module_data_center_ready(void);

#endif
