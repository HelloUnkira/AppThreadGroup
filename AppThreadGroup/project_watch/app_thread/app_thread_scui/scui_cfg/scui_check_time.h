#ifndef SCUI_CHECK_TIME_H
#define SCUI_CHECK_TIME_H

#define SCUI_CHECK_TIME_DEF                         (10)
#define SCUI_CHECK_TIME_HOME_DEF                    (5)

typedef struct {
    scui_handle_t handle;
    uint32_t sleep_tick;
    uint32_t home_tick;
} scui_check_time_t;

/*@brief 超时/空闲配置表查找
 *@param handle 界面句柄值
 *@retval 配置项(未登记返回NULL)
 */
const scui_check_time_t * scui_check_time_find(scui_handle_t handle);

#endif
