#ifndef SCUI_IMAGE_COMBINE_H
#define SCUI_IMAGE_COMBINE_H

/*一个通过scui_image_combine脚本生成的image表
 */

typedef enum {
	scui_image_test___image_src_repeat_01_bluetooth_bmp,
	scui_image_test___image_src_repeat_02_window_png,
	scui_image_test___image_src_repeat_03_bar_bmp,
	scui_image_test___image_src_repeat_04_bar_bmp,
	scui_image_test___image_src_repeat_05_dot_bmp,
} scui_image_combine_handle_t;

extern const void * scui_image_combine_table[5];

//<         w,         h,  size_raw,  size_mem,     lz4_pct> handle
//<      0xd2,      0xd2,      4073,   0x15888,        0.05> scui_image_test___image_src_repeat_01_bluetooth_bmp
//<     0x146,      0x7c,      6290,   0x1d9b8,        0.05> scui_image_test___image_src_repeat_02_window_png
//<       0x6,      0x98,        63,     0x1c8,        0.14> scui_image_test___image_src_repeat_03_bar_bmp
//<      0x10,      0x90,        86,     0x480,        0.07> scui_image_test___image_src_repeat_04_bar_bmp
//<       0x6,       0x6,        41,      0x12,        2.28> scui_image_test___image_src_repeat_05_dot_bmp

#endif
