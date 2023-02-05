#ifndef APP_LV_UI_SCENE_H
#define APP_LV_UI_SCENE_H

typedef enum {
    app_lv_ui_scene_default = 0,
    app_lv_ui_scene_need_show,
    app_lv_ui_scene_need_hide,
} app_lv_ui_scene_event_t;

typedef struct {
    /* 场景资源节点 */
    const void  *presenter;         /* 场景交互回调集 */
    /* 临近场景(adjacent scene) */
    uint8_t      cross_lock:1;      /* 临近场景锁 */
    uint8_t      cross_dire:1;      /* 临近场景方向 */
    uint16_t     cross_vlist[2];    /* 临近场景(landscape,portrait) */
    const void  *cross_plist[2];    /* 临近场景交互资源 */
    /* 场景组 */
    uint8_t      set_num;           /* 场景集合数 */
    uint16_t    *set_vlist;         /* 场景集合 */
    const void **set_plist;         /* 场景集合交互资源 */
    /* 场景触发事件及回调 */
    uint8_t event;                  /* 触发事件 */
    void  (*show)(void *scene);     /* 载入场景 */
    void  (*hide)(void *scene);     /* 移除场景 */
    /* ... */
} app_lv_ui_scene_t;

/* 场景最大嵌套数量 */
#define APP_LV_UI_SCENE_NEST        10

/*@brief 场景调度
 *       内部使用: 被lvgl线程使用
 */
void app_lv_ui_scene_sched(app_lv_ui_scene_t *scene);

/*@brief      获取最上层显示场景
 *@param[out] 场景(不可为空)
 */
void app_lv_ui_scene_get_top(app_lv_ui_scene_t **scene);

/*@brief     场景复位
 *@param[in] 场景
 */
void app_lv_ui_scene_reset(app_lv_ui_scene_t *scene);

/*@brief     场景覆盖显示场景
 *@param[in] 场景(不可为空)
 */
void app_lv_ui_scene_cover(app_lv_ui_scene_t *scene);

/*@brief     场景添加新显示场景
 *@param[in] 场景(不可为空)
 */
void app_lv_ui_scene_add(app_lv_ui_scene_t *scene);

/*@brief      场景移除当前显示场景
 *@param[out] 场景(不可为空)
 */
void app_lv_ui_scene_del(app_lv_ui_scene_t *scene);

/*@brief  当前场景嵌套层级
 *@retval 场景数量
 */
uint8_t app_lv_ui_scene_get_nest(void);

#endif
