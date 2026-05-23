#ifndef APP_LV_SCENE_H
#define APP_LV_SCENE_H

typedef struct {
    lv_obj_t *root;             /* 场景根 */
    void (*show)(void *scene);  /* 载入场景 */
    void (*hide)(void *scene);  /* 移除场景 */
    void  *wheel;               /* 自定义轮盘(app_lv_wheel_t)(默认为NULL) */
} app_lv_scene_t;

typedef struct {
    app_lv_scene_t *scene;
    /* 场景触发事件 */
    enum {
        /* lvgl不需要额外的场景交互接口 */
        /* lvgl更新控件属性和参数后会自动更新布局 */
        app_lv_scene_event_none = 0,
        app_lv_scene_event_show,
        app_lv_scene_event_hide,
    } event;
} app_lv_scene_sched_t;

/*@brief 场景调度
 *       内部使用: 被中心轮盘调度使用
 *@param sched 场景调度实例
 */
void app_lv_scene_sched(app_lv_scene_sched_t *sched);

#endif
