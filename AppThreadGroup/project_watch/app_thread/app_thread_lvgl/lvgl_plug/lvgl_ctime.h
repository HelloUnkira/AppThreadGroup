#ifndef LVGL_CTIME_H
#define LVGL_CTIME_H

#define LVGL_CTIME_OVER_MAX   0xFF        /* 一直界面常亮 */
#define LVGL_CTIME_IDLE_MAX   0xFF        /* 一直空闲等待(息屏后不自动返回主界面) */
#define LVGL_CTIME_IDLE       5           /* 息屏后返回主界面空闲等待时间 */
#define LVGL_CTIME_OVER       10          /* 界面空闲亮屏等待时间 */
#define LVGL_CTIME_USE        1           /* 模组使用标记 */



typedef struct
{
    uint32_t over_time;
    uint32_t idle_time;
    uint32_t over_time_once;
    uint32_t idle_time_once;
    uint32_t over_time_bak;
    uint32_t idle_time_bak;
    uint32_t over_time_def;     // 默认息屏时间
    uint32_t idle_time_def;     // 默认息屏后回到主界面时间
    uint32_t over: 1;           /* 超时执行标记 */
    uint32_t idle: 1;           /* 空闲执行标记 */
    uint32_t once: 1;           /* 单次时间设置 */
    uint32_t sleep_tag: 1;      // 低功耗标记
    uint32_t dlps_tag: 1;       // 低功耗标记
    
    lv_timer_t *timer;
} lvgl_ctime_t;

void lvgl_ctime_ready(void);
void lvgl_ctime_pause(void);
void lvgl_ctime_resume(void);

void lvgl_ctime_reset(uint32_t over_time, uint32_t idle_time);
void lvgl_ctime_def_reset(uint32_t over_time, uint32_t idle_time);
void lvgl_ctime_once_reset(uint32_t over_time, uint32_t idle_time);

// 界面常亮:恢复默认设置:界面息屏后不自动返回主界面:
// lvgl_ctime_reset(LVGL_CTIME_OVER_MAX, 0);
// lvgl_ctime_reset(0, LVGL_CTIME_IDLE_MAX);
// lvgl_ctime_reset(0, 0);

bool lvgl_ctime_sleep_tag(void);
void lvgl_ctime_sleep_quit(void);

#endif
