#ifndef SCUI_WIDGET_PARSER_H
#define SCUI_WIDGET_PARSER_H

/*一个通过scui_widget_parser脚本生成的widget表
 */

typedef enum {
	SCUI_HANDLE_OFFSET_WIDGET = 0x1000 - 1,
	SCUI_UI_SCENE_BUTTERFLY, // 0x1000
	SCUI_UI_SCENE_BUTTERFLY_CUSTOM, // 0x1001
	SCUI_UI_SCENE_CUBE, // 0x1002
	SCUI_UI_SCENE_CUBE_CUSTOM, // 0x1003
	SCUI_UI_SCENE_LANTERN, // 0x1004
	SCUI_UI_SCENE_LANTERN_CUSTOM, // 0x1005
	SCUI_UI_SCENE_SOCCER, // 0x1006
	SCUI_UI_SCENE_SOCCER_CUSTOM, // 0x1007
	SCUI_UI_SCENE_HONEYCOMB, // 0x1008
	SCUI_UI_SCENE_HONEYCOMB_SCROLL, // 0x1009
	SCUI_UI_SCENE_LIST_ARC, // 0x100a
	SCUI_UI_SCENE_LIST_ARC_SCROLL, // 0x100b
	SCUI_UI_SCENE_LIST_ARC_BAR_ARC, // 0x100c
	SCUI_UI_SCENE_LIST_ARC_MASK, // 0x100d
	SCUI_UI_SCENE_LIST_SCALE, // 0x100e
	SCUI_UI_SCENE_LIST_SCALE_BAR_ARC, // 0x100f
	SCUI_UI_SCENE_LIST_SCALE_MASK, // 0x1010
	SCUI_UI_SCENE_SPREAD, // 0x1011
	SCUI_UI_SCENE_SPREAD_CUSTOM, // 0x1012
	SCUI_UI_SCENE_THUMBWHEEL, // 0x1013
	SCUI_UI_SCENE_THUMBWHEEL_CUSTOM, // 0x1014
	SCUI_UI_SCENE_WATERFALL, // 0x1015
	SCUI_UI_SCENE_WATERFALL_SCROLL, // 0x1016
	SCUI_UI_SCENE_WATERFALL_BAR_ARC, // 0x1017
	SCUI_UI_SCENE_MONITOR, // 0x1018
	SCUI_UI_SCENE_POPUP, // 0x1019
	SCUI_UI_SCENE_POPUP_SCALE, // 0x101a
	SCUI_UI_SCENE_POPUP_BG, // 0x101b
	SCUI_UI_SCENE_ACTIVITY, // 0x101c
	SCUI_UI_SCENE_ACTIVITY_SCROLL, // 0x101d
	SCUI_UI_SCENE_ACTIVITY_BAR_ARC, // 0x101e
	SCUI_UI_SCENE_ACTIVITY_SCROLL_RING_BIG_3, // 0x101f
	SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_TITLE, // 0x1020
	SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_SUM, // 0x1021
	SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_KCAL, // 0x1022
	SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_STEP, // 0x1023
	SCUI_UI_SCENE_ACTIVITY_SCROLL_DITAIL_DIST, // 0x1024
	SCUI_UI_SCENE_ACTIVITY_SCROLL_BLANK, // 0x1025
	SCUI_UI_SCENE_HOME, // 0x1026
	SCUI_UI_SCENE_MINI_CARD, // 0x1027
	SCUI_UI_SCENE_MINI_CARD_BAR_ARC, // 0x1028
	SCUI_UI_SCENE_MINI_CARD_MASK, // 0x1029
	SCUI_UI_SCENE_NONE, // 0x102a
	SCUI_UI_SCENE_NOTIFY, // 0x102b
	SCUI_UI_SCENE_QUICK_CARD, // 0x102c
	SCUI_UI_SCENE_STANDBY, // 0x102d
	SCUI_UI_SCENE_FLOAT_1, // 0x102e
	SCUI_UI_SCENE_FLOAT_1_1, // 0x102f
	SCUI_UI_SCENE_FLOAT_1_2, // 0x1030
	SCUI_UI_SCENE_FLOAT_2, // 0x1031
	SCUI_UI_SCENE_FLOAT_2_C, // 0x1032
	SCUI_UI_SCENE_FLOAT_2_1, // 0x1033
	SCUI_UI_SCENE_FLOAT_2_2, // 0x1034
	SCUI_UI_SCENE_FLOAT_2_3, // 0x1035
	SCUI_UI_SCENE_FLOAT_2_4, // 0x1036
	SCUI_UI_SCENE_FLOAT_2_5, // 0x1037
	SCUI_UI_SCENE_FLOAT_3, // 0x1038
	SCUI_UI_SCENE_FLOAT_3_RING, // 0x1039
	SCUI_UI_SCENE_FLOAT_3_BTN, // 0x103a
	SCUI_UI_SCENE_FLOAT_4, // 0x103b
	SCUI_UI_SCENE_1, // 0x103c
	SCUI_UI_SCENE_1_VEDIO, // 0x103d
	SCUI_UI_SCENE_2, // 0x103e
	SCUI_UI_SCENE_2_BAR_ARC, // 0x103f
	SCUI_UI_SCENE_2_RING, // 0x1040
	SCUI_UI_SCENE_3, // 0x1041
	SCUI_UI_SCENE_6, // 0x1042
	SCUI_UI_SCENE_6_1, // 0x1043
	SCUI_UI_SCENE_6_2, // 0x1044
	SCUI_UI_SCENE_6_3, // 0x1045
	SCUI_UI_SCENE_6_4, // 0x1046
	SCUI_UI_SCENE_6_5, // 0x1047
	SCUI_UI_SCENE_6_6, // 0x1048
	SCUI_UI_SCENE_6_7, // 0x1049
	SCUI_UI_SCENE_6_8, // 0x104a
	SCUI_UI_SCENE_6_9, // 0x104b
	SCUI_UI_SCENE_TEST, // 0x104c
} scui_widget_parser_handle_t;

extern const void * scui_widget_parser_table[77];

#endif
