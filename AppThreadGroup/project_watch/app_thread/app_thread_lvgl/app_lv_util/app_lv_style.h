#ifndef APP_LV_STYLE_H
#define APP_LV_STYLE_H

/*@brief 水平像素转换(未使用)
 *           将x/100像素点放大到指定显示器像素
 *@param x 水平像素
 *@retval 转换后的像素
 */
lv_coord_t app_lv_style_zoom_x(lv_coord_t x);

/*@brief 垂直像素转换(未使用)
 *           将y/100像素点放大到指定显示器像素
 *@param y 垂直像素
 *@retval 转换后的像素
 */
lv_coord_t app_lv_style_zoom_y(lv_coord_t y);

/*@brief 水平宽度百分比
 *@param pct 水平像素
 *@retval 转换后的像素
 */
lv_coord_t app_lv_style_hor_pct(uint8_t pct);

/*@brief 垂直宽度百分比
 *@param pct 垂直像素
 *@retval 转换后的像素
 */
lv_coord_t app_lv_style_ver_pct(uint8_t pct);

/*@brief 透明度更新
 *@param obj 对象
 *@param opa 透明度
 */
void app_lv_style_opa_update(lv_obj_t *obj, uint8_t opa);

/*@brief 默认控件全局刷新动画
 *@param obj    对象
 *@param anim   动画
 *@param exec   动画刷新回调
 *@param repeat 更新次数
 *@param start  起始值
 *@param end    结束值
 *@param period 周期
 */
void app_lv_style_object_anim(lv_obj_t *obj, lv_anim_t *anim, lv_anim_exec_xcb_t exec, uint16_t repeat, int32_t start, int32_t end, uint32_t period);

/*@brief 对齐中心内容域
 *@param obj   对象
 *@param align 部分对齐
 */
void app_lv_style_object_content_align(lv_obj_t *obj, lv_align_t align);

#endif
