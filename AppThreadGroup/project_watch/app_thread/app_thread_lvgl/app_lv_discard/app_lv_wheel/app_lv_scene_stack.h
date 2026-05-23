#ifndef APP_LV_SCENE_STACK_H
#define APP_LV_SCENE_STACK_H

/* 场景栈最大嵌套数量 */
#define APP_LV_SCENE_NEST   10

/*@brief 默认轮盘场景
 *@param current 轮盘场景
 *@param parent  轮盘场景
 *@retval 轮盘场景
 */
app_lv_wheel_t app_lv_scene_wheel_def(app_lv_scene_t *current, app_lv_scene_t *parent);

/*@brief 当前场景嵌套层级
 *@retval 场景数量
 */
uint8_t app_lv_scene_get_nest(void);

/*@brief 获取次上层显示场景
 *@param scene 场景(不可为空)
 */
void app_lv_scene_get_last(app_lv_scene_t **scene);

/*@brief 获取最上层显示场景
 *@param scene 场景(不可为空)
 */
void app_lv_scene_get_top(app_lv_scene_t **scene);

/*@brief 场景复位
 *@param scene 场景
 *@param reserve 保留当前场景
 */
void app_lv_scene_reset(app_lv_scene_t *scene, bool reserve);

/*@brief 场景覆盖显示场景
 *@param scene 场景(不可为空)
 */
void app_lv_scene_cover(app_lv_scene_t *scene);

/*@brief 场景添加新场景
 *@param scene 场景
 *@param reserve 保留当前场景
 */
void app_lv_scene_add(app_lv_scene_t *scene, bool reserve);

/*@brief 场景移除当前显示场景
 *@param scene 场景(不可为空)
 */
void app_lv_scene_del(app_lv_scene_t **scene);

/*@brief 场景为轮盘更新接口
 *@param scene 场景
 *@param state 0:add;1:cover;2:del;
 */
void app_lv_scene_wheel_update(app_lv_scene_t **scene, int8_t state);

#endif
