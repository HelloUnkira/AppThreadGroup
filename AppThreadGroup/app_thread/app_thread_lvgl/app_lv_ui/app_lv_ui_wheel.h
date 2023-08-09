#ifndef APP_LV_UI_WHEEL_H
#define APP_LV_UI_WHEEL_H

#define APP_LV_UI_WHEEL_SCROLL_MAX  4

typedef struct {
    /* scene:   轮盘窗口本身 */
    /* way:     0:垂直浮窗;水平滚窗; */
    /*          1:水平浮窗;垂直浮窗; */
    /* scene_f: 垂直浮窗[左浮窗,右浮窗]; */
    /*          水平浮窗[上浮窗,下浮窗]; */
    /* scene_s: 垂直滚窗链表[滚窗数量]; */
    /*          水平滚窗链表[滚窗数量]; */
    /* num_s:   滚窗链表数量 */
    /* idx_s:   主显示滚窗 */
    app_lv_scene_t *scene;
    uint8_t         way;
    app_lv_scene_t *scene_f[2];
    app_lv_scene_t *scene_s[APP_LV_UI_WHEEL_SCROLL_MAX];
    uint8_t         num_s;
    uint8_t         idx_s;
} app_lv_ui_wheel_t;

/*@brief    配置轮盘子窗口信息
 *param[in] wheel 轮盘参数
 */
void app_lv_ui_wheel_event(app_lv_ui_wheel_t *wheel);

/*@brief    配置轮盘子窗口信息
 *param[in] wheel 轮盘参数
 */
void app_lv_ui_wheel_sched(void *data, uint32_t size);

// #include "app_lv_ui_wheel_float.h"

#endif
