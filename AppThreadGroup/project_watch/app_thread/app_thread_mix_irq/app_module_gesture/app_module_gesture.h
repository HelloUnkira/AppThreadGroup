#ifndef APP_MODULE_GESTURE_H
#define APP_MODULE_GESTURE_H

#define APP_MODULE_GESTURE_XMSEC    200     /* 一次主动测量时间 */

typedef struct {
    /* 运动方向数据统计 */
    void *data;
    /*  */
} app_module_gesture_t;

/*@brief 启动手势
 *       此时手势模组工作在轮询模式,中断响应关闭
 */
void app_module_gesture_start(void);

/*@brief 关闭手势
 *       此时手势模组工作在中断模式,轮询响应关闭
 */
void app_module_gesture_stop(void);

/*@brief 手势模组事件处理
 *       内部使用: 被mix custom线程使用
 */
void app_module_gesture_xmsec_update(void);

/*@brief 手势模组事件处理
 *       内部使用: 被mix custom线程使用
 */
void app_module_gesture_event_update(void);

/*@brief 初始化手势模组
 */
void app_module_gesture_ready(void);

#endif
