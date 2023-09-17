#ifndef APP_LV_PIC_H
#define APP_LV_PIC_H

/*本地静态的图片路径宏表
 *通过app_lv_pic.py生成
 *脚本会递归遍历..\lvgl_pic下所有文件
 *并读取相对路径与名字合成宏
 */

typedef enum {
	APP_LV_PIC_LVGL_ICO,                             // S:lvgl_pic/LVGL.ico
	APP_LV_PIC_TEST1_LVGL_ICO,                       // S:lvgl_pic/test1/LVGL.ico
	APP_LV_PIC_TEST1_TEST1_LVGL_JPG,                 // S:lvgl_pic/test1/test1/LVGL.jpg
	APP_LV_PIC_TEST1_TEST2_LVGL_PNG,                 // S:lvgl_pic/test1/test2/LVGL.png
	APP_LV_PIC_TEST1_TEST3_LVGL_BMP,                 // S:lvgl_pic/test1/test3/LVGL.bmp
	APP_LV_PIC_TEST2_LVGL_ICO,                       // S:lvgl_pic/test2/LVGL.ico
	APP_LV_PIC_TEST2_TEST1_LVGL_JPG,                 // S:lvgl_pic/test2/test1/LVGL.jpg
	APP_LV_PIC_TEST2_TEST2_LVGL_PNG,                 // S:lvgl_pic/test2/test2/LVGL.png
	APP_LV_PIC_TEST2_TEST3_LVGL_BMP,                 // S:lvgl_pic/test2/test3/LVGL.bmp
	APP_LV_PIC_TEST3_LVGL_ICO,                       // S:lvgl_pic/test3/LVGL.ico
	APP_LV_PIC_TEST3_TEST1_LVGL_JPG,                 // S:lvgl_pic/test3/test1/LVGL.jpg
	APP_LV_PIC_TEST3_TEST2_LVGL_PNG,                 // S:lvgl_pic/test3/test2/LVGL.png
	APP_LV_PIC_TEST3_TEST3_LVGL_BMP,                 // S:lvgl_pic/test3/test3/LVGL.bmp
	APP_LV_PIC_NUM,
} app_lv_pic_t;

/*@brief     获得图片资源字符串
 *@param[in] index图片资源索引(0~n-1)
 */
const char * app_lv_pic_str_find(uint32_t index);

#endif
