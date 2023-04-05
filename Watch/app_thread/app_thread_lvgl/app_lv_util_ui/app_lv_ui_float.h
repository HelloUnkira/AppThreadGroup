#ifndef APP_LV_UI_FLOAT_H
#define APP_LV_UI_FLOAT_H

typedef struct {
    lv_obj_t  *scene;
    lv_coord_t click_x;
    lv_coord_t click_y;
    lv_coord_t resume_x;
    lv_coord_t resume_y;
    lv_coord_t scroll_x;
    lv_coord_t scroll_y;
    lv_dir_t   scroll_w;
    uint32_t   percent;
    uint32_t   valid:1;
    uint32_t   cover:1;
    lv_anim_t  anim_pos;
    lv_anim_t  anim_opa;
    uint32_t   anim_period;
} app_lv_ui_float_t;

/*@brief  浮动子窗口禁用手势
 *@retval 浮动子窗口需要禁用指定手势
 */
bool app_lv_ui_float_cannot_gestrue(lv_dir_t direct);

/*@brief  浮动子窗口复位(如果未复位的话)
 *@retval 是否进行复位
 */
bool app_lv_ui_float_reset(void);

/*@brief     浮窗激活
 *@param[in] scene 场景
 */
void app_lv_ui_float_active(app_lv_scene_t *scene);

/*@brief     浮窗失活
 *@param[in] scene 场景
 */
void app_lv_ui_float_hidden(app_lv_scene_t *scene);

#endif
