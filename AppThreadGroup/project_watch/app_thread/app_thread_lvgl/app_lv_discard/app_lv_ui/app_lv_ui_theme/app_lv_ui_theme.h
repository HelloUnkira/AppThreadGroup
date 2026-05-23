#ifndef APP_LV_UI_THEME_H
#define APP_LV_UI_THEME_H

typedef struct {
    uint32_t        idx_str;
    uint32_t        idx_pic;
    app_lv_scene_t *scene;
} app_lv_ui_theme_item_t;

/*@brief 获得主题列表
 *@param list 主题列表
 *@param num  主题列表数量
 */
void app_lv_ui_theme_list_alloc(app_lv_ui_theme_item_t **list, uint32_t *num);

/*@brief 归还主题列表
 *@param list 主题列表
 */
void app_lv_ui_theme_list_free(app_lv_ui_theme_item_t *list);

#endif
