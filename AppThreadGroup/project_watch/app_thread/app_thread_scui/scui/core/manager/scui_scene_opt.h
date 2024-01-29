#ifndef SCUI_SCENE_OPT_H
#define SCUI_SCENE_OPT_H

/*@brief 场景切换风格(配置)
 *@param switch_type 场景切换风格
 */
void scui_scene_switch_type_cfg(scui_scene_switch_type_t switch_type);

/*@brief 场景切换方向(配置)
 *@param switch_dir 场景切换方向
 */
void scui_scene_switch_dir_cfg(scui_event_dir_t switch_dir);

/*@brief 场景列表添加场景
 *@param handle 场景句柄
 */
void scui_scene_list_add(scui_handle_t handle);

/*@brief 场景列表移除场景
 *@param handle 场景句柄
 */
void scui_scene_list_del(scui_handle_t handle);

/*@brief 场景隐藏
 *@param handle 场景句柄
 */
void scui_scene_hide_without(scui_handle_t handle);

/*@brief 获得活跃场景句柄
 *@retval 场景句柄
 */
scui_handle_t scui_scene_active_last(void);

/*@brief 获得活跃场景句柄
 *@retval 场景句柄
 */
scui_handle_t scui_scene_active_curr(void);

/*@brief 场景激活
 *@param handle 场景句柄
 */
void scui_scene_active(scui_handle_t handle);

/*@brief 场景跳转
 *@param handle 场景句柄
 *@param type   场景切换风格
 *@param dir    场景切换方向
 */
void scui_scene_jump(scui_handle_t handle, scui_scene_switch_type_t type, scui_event_dir_t dir);

#endif
