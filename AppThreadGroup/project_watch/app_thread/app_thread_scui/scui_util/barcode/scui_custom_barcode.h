#ifndef SCUI_CUSTOM_BARCODE_H
#define SCUI_CUSTOM_BARCODE_H

/*@brief 自定义控件:插件:条形码生成器
 *@param event 自定义控件事件
 *@param clip  剪切域(绘制区域)
 *@param color 颜色(.color_lighten,.color_darken,)
 *@param data  url网址链接字符串
 *@param size  字符串长度
 *@param cover 背景覆盖
 */
void scui_custom_draw_barcode(scui_event_t *event, scui_area_t *clip,
                              scui_color_t  color, bool cover,
                              uint8_t *data, uint32_t size);

#endif

