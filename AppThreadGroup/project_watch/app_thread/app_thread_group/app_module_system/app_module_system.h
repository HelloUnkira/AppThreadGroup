#ifndef APP_MODULE_SYSTEM_H
#define APP_MODULE_SYSTEM_H

typedef struct {
    uint8_t mode;           /* 当前工作模式 */
    uint8_t mode_bak;       /* 待切换工作模式 */
    uint8_t valid:1;        /* 当前工作模式运行 */
    uint8_t dlps_exec:1;    /* 当前工作模式的DLPS */
    uint8_t dlps_status:1;  /* 当前工作模式的DLPS */
    /* normal模式调度参数 */
    struct {
        struct {
            uint8_t not_srand_yet:1;
            uint8_t not_load_yet:1;
            uint8_t not_dump_yet:1;
            uint8_t not_start_yet:1;
            uint8_t not_stop_yet:1;
        } normal;
        struct {
            uint8_t not_shutdown_yet:1;
        } shutdown;
    } ctrl;
} app_module_system_t;

/*@brief 系统进出DLPS
 *@param dlps true:进入dlps;false:退出dlps
 */
void app_module_system_dlps_set(bool dlps);

/*@brief 系统进出DLPS
 *@retval true:进入dlps;false:退出dlps
 */
bool app_module_system_dlps_get(void);

/*@brief 设置系统运行状态
 *@param valid 系统运行
 */
void app_module_system_valid_set(bool valid);

/*@brief 获取系统运行状态
 *@retval 系统运行
 */
bool app_module_system_valid_get(void);

/*@brief 设置系统工作模式
 *@param status 系统工作模式枚举量
 */
void app_module_system_mode_set(uint8_t mode);

/*@brief 获取系统工作模式
 *@retval 系统工作模式枚举量
 */
uint8_t app_module_system_mode_get(void);

/*@brief 初始化系统模组
 */
void app_module_system_ready(void);

#endif
