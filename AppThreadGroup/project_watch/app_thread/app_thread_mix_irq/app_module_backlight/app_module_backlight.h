#ifndef APP_MODULE_BACKLIGHT_H
#define APP_MODULE_BACKLIGHT_H

typedef struct {
    uint8_t duty_cycle;
} app_module_backlight_t;

/*@brief 设置背光模组
 *@param backlight 背光实例
 */
void app_module_backlight_set(app_module_backlight_t *backlight);

/*@brief 获取背光模组
 *@param backlight 背光实例
 */
void app_module_backlight_get(app_module_backlight_t *backlight);

/*@brief 背光模组开启
 */
void app_module_backlight_open(void);

/*@brief 背光模组关闭
 */
void app_module_backlight_close(void);

/*@brief 背光模组初始化
 */
void app_module_backlight_ready(void);

#endif
