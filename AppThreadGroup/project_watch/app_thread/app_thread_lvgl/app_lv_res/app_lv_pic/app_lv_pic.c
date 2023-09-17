/*本地静态的图片路径宏表
 *通过app_lv_pic.py生成
 *脚本会递归遍历..\lvgl_pic下所有文件
 *并读取相对路径与名字合成宏
 */

#include "app_ext_lib.h"
#include "app_lv_lib.h"

static const char * app_lv_pic_table[] = {
	"S:lvgl_pic/LVGL.ico",
	"S:lvgl_pic/test1/LVGL.ico",
	"S:lvgl_pic/test1/test1/LVGL.jpg",
	"S:lvgl_pic/test1/test2/LVGL.png",
	"S:lvgl_pic/test1/test3/LVGL.bmp",
	"S:lvgl_pic/test2/LVGL.ico",
	"S:lvgl_pic/test2/test1/LVGL.jpg",
	"S:lvgl_pic/test2/test2/LVGL.png",
	"S:lvgl_pic/test2/test3/LVGL.bmp",
	"S:lvgl_pic/test3/LVGL.ico",
	"S:lvgl_pic/test3/test1/LVGL.jpg",
	"S:lvgl_pic/test3/test2/LVGL.png",
	"S:lvgl_pic/test3/test3/LVGL.bmp",
};

/*@brief     获得图片资源字符串
 *@param[in] index图片资源索引(0~n-1)
 */
const char * app_lv_pic_str_find(uint32_t index)
{
	if (index < APP_LV_PIC_NUM)
		return app_lv_pic_table[index];
	return NULL;
}
