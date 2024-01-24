#ifndef SCUI_WIDGET_PARSER_H
#define SCUI_WIDGET_PARSER_H

/*一个通过scui_widget_parser脚本生成的widget表
 */

typedef enum {
	SCUI_HANDLE_OFFSET_WIDGET = 0x1000 - 1,
	SCUI_UI_SCENE_FLOAT_1,
	SCUI_UI_SCENE_FLOAT_2,
	SCUI_UI_SCENE_FLOAT_3,
	SCUI_UI_SCENE_FLOAT_4,
	SCUI_UI_SCENE_HOME,
	SCUI_UI_SCENE_HOME_CENTER,
	SCUI_UI_SCENE_1,
	SCUI_UI_SCENE_2,
	SCUI_UI_SCENE_3,
	SCUI_UI_SCENE_4,
	SCUI_UI_SCENE_5,
	SCUI_UI_SCENE_6,
} scui_widget_parser_handle_t;

extern const void * scui_widget_parser_table[12];

#endif
