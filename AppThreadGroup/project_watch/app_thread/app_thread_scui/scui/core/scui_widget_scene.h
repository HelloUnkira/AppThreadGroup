#ifndef SCUI_WIDGET_SCENE_H
#define SCUI_WIDGET_SCENE_H

/*@brief 加载一个场景
 *       从根控件开始到它的所有布局子控件
 *       动态子控件在show之前生成
 *@param handle 场景控件句柄
 */
void scui_widget_scene_load(scui_handle_t handle);

/*@brief 卸载一个场景
 *       从根控件开始到它的所有子控件
 *@param handle 控件句柄
 */
void scui_widget_scene_unload(scui_handle_t handle);

#endif
