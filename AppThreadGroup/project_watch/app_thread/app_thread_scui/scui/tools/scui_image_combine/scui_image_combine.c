/*一个通过scui_image_combine脚本生成的image表
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

static const scui_image_t test___image_src_repeat_01_bluetooth_bmp = {
	.format			 = scui_image_format_bmp565,
	.pixel.data		 = 0x0,
	.pixel.width	 = 0xd2,
	.pixel.height	 = 0xd2,
	.pixel.size_mem	 = 0x15888,
	.pixel.size_raw	 = 0xfe9,
	.status			 = scui_image_status_lz4,
	.from			 = scui_image_from_ext,
};

static const scui_image_t test___image_src_repeat_02_window_png = {
	.format			 = scui_image_format_png8565,
	.pixel.data		 = 0xfe9,
	.pixel.width	 = 0x146,
	.pixel.height	 = 0x7c,
	.pixel.size_mem	 = 0x1d9b8,
	.pixel.size_raw	 = 0x1892,
	.status			 = scui_image_status_lz4,
	.from			 = scui_image_from_ext,
};

static const scui_image_t test___image_src_repeat_03_bar_bmp = {
	.format			 = scui_image_format_a4,
	.pixel.data		 = 0x287b,
	.pixel.width	 = 0x6,
	.pixel.height	 = 0x98,
	.pixel.size_mem	 = 0x1c8,
	.pixel.size_raw	 = 0x3f,
	.status			 = scui_image_status_lz4,
	.from			 = scui_image_from_ext,
};

static const scui_image_t test___image_src_repeat_04_bar_bmp = {
	.format			 = scui_image_format_a4,
	.pixel.data		 = 0x28ba,
	.pixel.width	 = 0x10,
	.pixel.height	 = 0x90,
	.pixel.size_mem	 = 0x480,
	.pixel.size_raw	 = 0x56,
	.status			 = scui_image_status_lz4,
	.from			 = scui_image_from_ext,
};

static const scui_image_t test___image_src_repeat_05_dot_bmp = {
	.format			 = scui_image_format_a4,
	.pixel.data		 = 0x2910,
	.pixel.width	 = 0x6,
	.pixel.height	 = 0x6,
	.pixel.size_mem	 = 0x12,
	.pixel.size_raw	 = 0x29,
	.status			 = scui_image_status_lz4,
	.from			 = scui_image_from_ext,
};

const void * scui_image_combine_table[5] = {
	(void *)&test___image_src_repeat_01_bluetooth_bmp,
	(void *)&test___image_src_repeat_02_window_png,
	(void *)&test___image_src_repeat_03_bar_bmp,
	(void *)&test___image_src_repeat_04_bar_bmp,
	(void *)&test___image_src_repeat_05_dot_bmp,
};
