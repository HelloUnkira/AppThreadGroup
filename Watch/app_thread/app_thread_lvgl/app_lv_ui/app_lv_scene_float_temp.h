#ifndef APP_LV_SCENE_FLOAT_H
#define APP_LV_SCENE_FLOAT_H

/* 浮窗在主界面的触发边界 */
#define APP_LV_SCENE_FLOAT_PAD      20
#define APP_LV_SCENE_FLOAT_OPA      LV_OPA_50


/*@brief     准备并就绪浮动窗口
 *           注意:它调用了敏感资源
 *           对它的调用不能递归到lv_timer_handler
 *@param[in] t 上浮动窗口
 *@param[in] b 下浮动窗口
 *@param[in] l 左浮动窗口
 *@param[in] r 右浮动窗口
 */
void app_lv_scene_float_show(app_lv_scene_t *t, app_lv_scene_t *b,
                             app_lv_scene_t *l, app_lv_scene_t *r);

/*@brief 准备并就绪浮动窗口
 *       注意:它调用了敏感资源
 *       对它的调用不能递归到lv_timer_handler
 */
void app_lv_scene_float_hide(void);

#endif
