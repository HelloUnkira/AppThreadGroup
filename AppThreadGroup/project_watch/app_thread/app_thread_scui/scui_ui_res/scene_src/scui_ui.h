#ifndef SCUI_UI_H
#define SCUI_UI_H

/* 全局弹窗效果(这个使用全局浮动窗口,会影响性能,不建议使用) */
void scui_ui_scene_popup_text(scui_handle_t text, uint8_t *string);
void scui_ui_scene_popup_exec(void);

#endif
