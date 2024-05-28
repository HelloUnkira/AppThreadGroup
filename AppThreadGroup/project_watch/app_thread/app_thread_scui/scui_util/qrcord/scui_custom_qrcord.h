#ifndef SCUI_CUSTOM_QRCORD_H
#define SCUI_CUSTOM_QRCORD_H

/*@brief 自定义控件:插件:二维码生成器
 *@param handle 自定义控件
 *@param clip   剪切域(绘制区域)
 *@param color  亮色与暗色
 *@param data   url网址链接字符串
 *@param size   字符串长度
 *@param cover  背景覆盖
 */
void scui_custom_draw_qrcord(scui_handle_t handle, scui_area_t *clip,
                             scui_color_mix_t color, bool cover,
                             uint8_t *data, uint32_t size);

#endif
