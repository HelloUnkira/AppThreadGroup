#ifndef SCUI_UI_H
#define SCUI_UI_H

/* 应用列表,设置列表等使用 */
extern scui_handle_t  scui_ui_scene_list_num;
extern scui_handle_t *scui_ui_scene_list_image;
extern scui_handle_t *scui_ui_scene_list_text;
extern scui_handle_t *scui_ui_scene_list_jump;
void scui_ui_scene_list_cfg(uint8_t type);

/*@brief 窗口关联配置
 *@param event 事件
 */
void scui_ui_scene_link_cfg(scui_event_t *event);

/* 全局弹窗效果 */
void scui_ui_scene_popup_exec(scui_handle_t text, uint8_t *string);

#endif
