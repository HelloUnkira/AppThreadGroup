#ifndef SCUI_IMAGE_COMBINE_H
#define SCUI_IMAGE_COMBINE_H

/*一个通过scui_image_combine脚本生成的image表
 */

typedef enum {
	scui_image_test___image_src_10_weather_icon_bmp,
	scui_image_test___image_src_17_Compass_bj_en_bmp,
	scui_image_test___image_src_17_Compass_bj_zh_bmp,
	scui_image_test___image_src_17_Compass_faxiang_png,
	scui_image_test___image_src_17_Compass_fenjiexian_bmp,
	scui_image_test___image_src_17_Compass_gaodu_bmp,
	scui_image_test___image_src_17_Compass_icon_01_bmp,
	scui_image_test___image_src_17_Compass_icon_du_bmp,
	scui_image_test___image_src_17_Compass_qiya_bmp,
	scui_image_test___image_src_24_temperature_icon_bar_00_bmp,
	scui_image_test___image_src_24_temperature_icon_bar_01_bmp,
	scui_image_test___image_src_24_temperature_icon_bar_bj_bmp,
	scui_image_test___image_src_24_temperature_icon_dian_bmp,
	scui_image_test___image_src_24_temperature_icon_tubiao_da_bmp,
	scui_image_test___image_src_24_temperature_line_bmp,
	scui_image_test___image_src_system_I308001_system_p0001_sos_png,
	scui_image_test___image_src_system_I308001_system_p0002_turn_off_png,
	scui_image_test___image_src_system_I308001_system_p0003_restart_png,
} scui_image_combine_handle_t;

extern const void * scui_image_combine_table[18];

//<         w,         h,  size_raw,  size_mem,     lz4_pct> handle
//<      0x8c,      0x68,      2042,    0x71c0,        0.07> scui_image_test___image_src_10_weather_icon_bmp
//<     0x1d2,     0x1d2,     28770,   0x6a088,        0.07> scui_image_test___image_src_17_Compass_bj_en_bmp
//<     0x1d2,     0x1d2,     29152,   0x6a088,        0.07> scui_image_test___image_src_17_Compass_bj_zh_bmp
//<       0x6,      0x3c,        71,     0x2d0,        0.10> scui_image_test___image_src_17_Compass_faxiang_png
//<       0x2,      0x50,        36,     0x140,        0.11> scui_image_test___image_src_17_Compass_fenjiexian_bmp
//<      0x24,      0x20,       503,     0x900,        0.22> scui_image_test___image_src_17_Compass_gaodu_bmp
//<      0xfe,      0x81,      8792,    0xfffc,        0.13> scui_image_test___image_src_17_Compass_icon_01_bmp
//<      0x16,      0x16,       268,     0x3c8,        0.28> scui_image_test___image_src_17_Compass_icon_du_bmp
//<      0x24,      0x20,       446,     0x900,        0.19> scui_image_test___image_src_17_Compass_qiya_bmp
//<     0x108,     0x108,      2133,    0x8820,        0.06> scui_image_test___image_src_24_temperature_icon_bar_00_bmp
//<       0xe,       0xe,        90,      0x62,        0.92> scui_image_test___image_src_24_temperature_icon_bar_01_bmp
//<     0x108,     0x108,      4488,   0x22080,        0.03> scui_image_test___image_src_24_temperature_icon_bar_bj_bmp
//<      0x10,      0x80,       388,    0x1000,        0.09> scui_image_test___image_src_24_temperature_icon_dian_bmp
//<      0x8c,      0x8c,      7856,    0x9920,        0.20> scui_image_test___image_src_24_temperature_icon_tubiao_da_bmp
//<       0x2,      0x50,        36,     0x140,        0.11> scui_image_test___image_src_24_temperature_line_bmp
//<      0x50,      0x50,      1881,    0x4b00,        0.10> scui_image_test___image_src_system_I308001_system_p0001_sos_png
//<      0x50,      0x50,      1195,    0x4b00,        0.06> scui_image_test___image_src_system_I308001_system_p0002_turn_off_png
//<      0x50,      0x50,      1189,    0x4b00,        0.06> scui_image_test___image_src_system_I308001_system_p0003_restart_png

#endif
