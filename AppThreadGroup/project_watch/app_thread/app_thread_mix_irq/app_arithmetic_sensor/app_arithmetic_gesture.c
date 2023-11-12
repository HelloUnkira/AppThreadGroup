/*实现目标:
 *    算法组件:其余手势
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief     抬手亮屏,放下息屏等
 *@param[in] frame  帧数据
 *@param[in] length 帧数据实际数量
 */
void app_arithmetic_gesture_wrist(int32_t frame[][3], uint8_t length)
{
    app_module_data_center_src_t *data_center_src = NULL;
    data_center_src = app_module_data_center_take(app_module_data_center_src_user_profile);
    bool wrist     = data_center_src->user_profile.gesture.wrist;
    uint8_t hour_s = data_center_src->user_profile.gesture.time_s[0];
    uint8_t hour_e = data_center_src->user_profile.gesture.time_e[0];
    uint8_t min_s  = data_center_src->user_profile.gesture.time_s[1];
    uint8_t min_e  = data_center_src->user_profile.gesture.time_e[1];
    app_module_data_center_give();
    
    if (!wrist)
         return;
    
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    
    uint16_t mins_s = hour_s * 60 + min_s;
    uint16_t mins_e = hour_e * 60 + min_e;
    uint16_t mins_c = clock.hour * 60 + clock.minute;
    
    if (mins_c < mins_s || mins_c > mins_e)
        return;
    
    for (uint8_t idx = 0; idx < length; idx++) {
        
        int32_t x = frame[idx][0];
        int32_t y = frame[idx][1];
        int32_t z = frame[idx][2];
        
        static uint8_t onoff_cnt = false;
        /* 放下息屏 */
        if (app_module_system_dlps_get())
            onoff_cnt = 0;
        else {
            /* 手放下时:y平行重力线,成为主值,而x,z的值均垂直重力线,相对接近0 */
            bool dir_flag = z > (-1000) && (app_sys_abs(y) > (600) || x < (-400) || z > (+500));
            /* 当次条件未达成,休眠技术值清空 */
            if (z < (-1500))
                onoff_cnt = 0;
            /* 当次条件达成,且达到休眠计数值 */
            if (onoff_cnt > 0 && dir_flag)
            if (onoff_cnt++ > 10) {
                onoff_cnt = 0;
                /* 休眠 */
                app_module_system_dlps_set(true);
                APP_SYS_LOG_WARN("sleep");
            }
        }
        /*调节行走手方向, 行走时调节X值 */
        
        bool keep_sleep = false;
        static uint8_t type[5] = {0};
        static uint32_t num = 0;
        /* 不同类型,计数值达到不同标准触发亮屏 */
        /* 根据手表方向布局,重力线在yz平面的四象限分布 */
        if (0) ;
        /* 种类1:抬腕亮屏 */
        else if (app_sys_abs(y) > (600) && z > (-200))
             if (type[0]++ > 3)
                 type[0] = num = keep_sleep = 0;
        /* 种类2:翻腕亮屏 */
        else if (app_sys_abs(y) < (500) && x < (-500) && z > (+200))
             if (type[1]++ > 3)
                 type[1] = num = keep_sleep = 0;
        /* 种类3:横屏小幅度倾斜亮屏 */
        else if (app_sys_abs(y) > (600) && z < (-500))
             if (type[2]++ > 3)
                 type[2] = num = keep_sleep = 0;
        /* 种类4:坚屏小幅度倾斜亮屏 */
        else if (x < (-400) && z < (-500))
             if (type[3]++ > 3)
                 type[3] = num = keep_sleep = 0;
        /* 种类5:抬手亮屏(跑步模式) */
        else if (z < (-500))
             if (type[4]++ > 20)
                 type[4] = num = keep_sleep = 0;
        else {
            memset(type, 0, sizeof(type));
            num++;
        }
        /* 抬手过程:y轴离开重力线,x轴靠近重力线 */
        if (app_sys_abs(y) < (310) && x > (-170) && z < (-300) && num < (60)) {
            if (!keep_sleep) {
                 keep_sleep = false;
                 onoff_cnt = 1;
                /* 唤醒 */
                app_module_system_dlps_set(false);
                APP_SYS_LOG_WARN("wake up");
            }
        }
    }
}

/*@brief     摇手
 *@param[in] frame  帧数据
 *@param[in] length 帧数据实际数量
 */
void app_arithmetic_gesture_shake(int32_t frame[][3], uint8_t length)
{
    app_module_data_center_src_t *data_center_src = NULL;
    data_center_src = app_module_data_center_take(app_module_data_center_src_user_profile);
    bool shake = data_center_src->user_profile.gesture.shake;
    app_module_data_center_give();
    
    if (!shake)
         return;
    
    static app_execute_us_t execute_us = {0};
    
    for (uint8_t idx = 0; idx < length; idx++) {
        
        int32_t x = frame[idx][0];
        int32_t y = frame[idx][1];
        int32_t z = frame[idx][2];
        
        int32_t sg = x * x + y * y + z * z;
        
        static bool shake_onoff = 0;
        static uint8_t shake_cnt = 0;
        
        if (sg > 1100 * 1100)
            shake_cnt++;
        else if (x < 900 * 900)
            shake_cnt = 0;
        
        if (shake_cnt > 3 && !shake_onoff) {
            shake_onoff = true;
            /* 间隔时间(摇手事件) */
            if (app_execute_us(&execute_us, false) / 1000.0 < 1200) {
                /* 发送摇手事件 */
                APP_SYS_LOG_WARN("shake hand");
            }
            break;
        }
    }
    app_execute_us(&execute_us, true);
}
