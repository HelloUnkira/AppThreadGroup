#ifndef SCUI_SCENE_MGR_H
#define SCUI_SCENE_MGR_H

typedef struct {
    scui_handle_t scene[SCUI_SCENE_MGR_LIMIT];
    scui_handle_t scene_active;
} scui_scene_mgr_t;

/*@brief 场景管理器混合根控件列表
 *       将所有根控件画布混合到绘制画布上
 *       场景管理器会有特殊的处理
 *       用于处理画布级别特效流程
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_scene_mgr_mix_list(scui_widget_t **list, uint32_t num);

/*@brief 场景管理器排序根控件列表
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_scene_mgr_sort_list(scui_widget_t **list, uint32_t num);

/*@brief 场景管理器混合画布
 *       将所有独立画布混合到绘制画布上
 *       将所有无独立画布就地渲染
 *       将绘制画布生成刷新事件
 */
void scui_scene_mgr_mix_surface(void);

/*@brief 场景管理器获得活跃场景句柄
 *@retval 场景句柄
 */
scui_handle_t scui_scene_mgr_active_get(void);

/*@brief 场景管理器设置活跃场景句柄
 *@param handle 场景句柄
 */
void scui_scene_mgr_active_set(scui_handle_t handle);

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_scene_mgr_event_dispatch(scui_event_t *event);

#endif
