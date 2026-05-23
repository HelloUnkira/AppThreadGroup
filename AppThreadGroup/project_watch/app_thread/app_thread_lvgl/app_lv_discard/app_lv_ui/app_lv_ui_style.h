#ifndef APP_LV_UI_STYLE_H
#define APP_LV_UI_STYLE_H

/*@brief 默认控件风格
 *       黑色背景,无边框,无阴影,无间隙,无外部线
 *@param obj 控件
 */
void app_lv_style_object(lv_obj_t *obj);

/*@brief 默认界面风格
 *       无主题风格,全屏幕,黑色背景全覆盖,无边界,无内部间隙
 *retval 返回场景(窗口,界面)
 */
lv_obj_t * app_lv_style_scene(void);

#endif
