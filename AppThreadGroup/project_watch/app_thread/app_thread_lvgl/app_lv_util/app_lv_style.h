#ifndef APP_LV_STYLE_H
#define APP_LV_STYLE_H

/*@brief     水平像素转换(未使用)
 *           将x/100像素点放大到指定显示器像素
 *@param[in] 水平像素
 *@retval    转换后的像素
 */
lv_coord_t app_lv_style_zoom_x(lv_coord_t x);

/*@brief     垂直像素转换(未使用)
 *           将y/100像素点放大到指定显示器像素
 *@param[in] 垂直像素
 *@retval    转换后的像素
 */
lv_coord_t app_lv_style_zoom_y(lv_coord_t y);

/*@brief     水平宽度百分比
 *@param[in] 水平像素
 *@retval    转换后的像素
 */
lv_coord_t app_lv_style_hor_pct(uint8_t pct);

/*@brief     垂直宽度百分比
 *@param[in] 垂直像素
 *@retval    转换后的像素
 */
lv_coord_t app_lv_style_ver_pct(uint8_t pct);

/*@brief    默认全局刷新动画
 *          立即初始化并启动,用于数据资源常态刷新
 *param[in] scene  界面
 *param[in] anim   动画
 *param[in] exec   动画刷新回调
 *param[in] repeat 更新次数
 *param[in] start  起始值
 *param[in] end    结束值
 *param[in] period 周期
 */
void app_lv_style_object_anim(lv_obj_t *obj, lv_anim_t *anim, lv_anim_exec_xcb_t exec, uint16_t repeat, int32_t start, int32_t end, uint32_t period);

/*@brief     对齐中心内容域
 *param[in]  parent 父控件
 *param[in]  align  部分对齐
 */
void app_lv_style_object_content_align(lv_obj_t *obj, lv_align_t align);

#endif
