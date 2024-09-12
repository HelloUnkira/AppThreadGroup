/*实现目标:
 *    输入设备事件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"
#include "app_thread_group.h"

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_notify(scui_indev_data_t *data)
{
    if (data->type == scui_indev_type_ptr)
        scui_indev_ptr_notify(data);
    if (data->type == scui_indev_type_enc)
        scui_indev_enc_notify(data);
    if (data->type == scui_indev_type_key)
        scui_indev_key_notify(data);
}

/*@brief 输入设备初始化
 */
void scui_indev_ready(void)
{
    scui_indev_ptr_ready();
    scui_indev_enc_ready();
    scui_indev_key_ready();
}

/*@brief 输入设备获取数据集合实例
 *@param 数据集合实例
 */
void scui_indev_data_set(scui_indev_data_set_t **data_set)
{
    static scui_indev_data_set_t scui_indev_data_set = {0};
    
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    scui_indev_data_set.sys_time_h      = clock.hour;
    scui_indev_data_set.sys_time_m      = clock.minute;
    scui_indev_data_set.sys_time_s      = clock.second;
    
    *data_set = &scui_indev_data_set;
}
