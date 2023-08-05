#ifndef APP_LV_SCENE_H
#define APP_LV_SCENE_H

typedef enum {
    app_lv_scene_default = 0,
    app_lv_scene_need_show,
    app_lv_scene_need_hide,
} app_lv_scene_event_t;

typedef struct {
    /* 场景触发事件及回调 */
    uint8_t event;                  /* 触发事件 */
    void  (*show)(void *scene);     /* 载入场景 */
    void  (*hide)(void *scene);     /* 移除场景 */
    /* 场景指示的根场景(lv_obj_t *) */
    void   *root;
} app_lv_scene_t;

/* 场景最大嵌套数量 */
#define APP_LV_SCENE_NEST        10

/*@brief     场景调度
 *           内部使用: 被lvgl线程使用,或者扩展使用
 *@param[in] scene 场景
 */
void app_lv_scene_sched(app_lv_scene_t *scene);

/*@brief      获取次上层显示场景
 *@param[out] scene 场景(不可为空)
 */
void app_lv_scene_get_last(app_lv_scene_t **scene);

/*@brief      获取最上层显示场景
 *@param[out] scene 场景(不可为空)
 */
void app_lv_scene_get_top(app_lv_scene_t **scene);

/*@brief     场景复位
 *@param[in] scene 场景
 *@param[in] reserve 保留当前场景
 */
void app_lv_scene_reset(app_lv_scene_t *scene, bool reserve);

/*@brief     场景覆盖显示场景
 *@param[in] scene 场景(不可为空)
 */
void app_lv_scene_cover(app_lv_scene_t *scene);

/*@brief     场景添加新场景
 *@param[in] scene 场景
 *@param[in] reserve 保留当前场景
 */
void app_lv_scene_add(app_lv_scene_t *scene, bool reserve);

/*@brief      场景移除当前显示场景
 *@param[out] scene 场景(不可为空)
 */
void app_lv_scene_del(app_lv_scene_t *scene);

/*@brief  当前场景嵌套层级
 *@retval 场景数量
 */
uint8_t app_lv_scene_get_nest(void);

/*@brief    激活一个游离窗口,该窗口不受场景栈管理
 *          注意:它调用了敏感资源,对它的递归调用,不能到lv_timer_handler
 *          此外:在它的调用之前,scene内部资源应该以lv_scr_act()为根对象
 *param[in] scene     游离窗口
 *param[in] right_now 就地执行
 */
void app_lv_scene_active(app_lv_scene_t *scene, bool right_now);

/*@brief    失活一个游离窗口,该窗口不受场景栈管理
 *          注意:它调用了敏感资源,对它的递归调用,不能到lv_timer_handler
 *          此外:在它的调用之前,scene内部资源应该以lv_scr_act()为根对象
 *param[in] scene     游离窗口
 *param[in] right_now 就地执行
 */
void app_lv_scene_hidden(app_lv_scene_t *scene, bool right_now);

#endif
