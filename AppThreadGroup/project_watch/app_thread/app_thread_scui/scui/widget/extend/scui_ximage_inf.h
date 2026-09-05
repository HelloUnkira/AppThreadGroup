#ifndef SCUI_XIMAGE_INF_H
#define SCUI_XIMAGE_INF_H

/*@brief 图像控件图像帧播放
 *@param handle 图像控件句柄
 *@param image  图像帧句柄(gif/lottie)
 *@param speed  播放速度(SCUI_SCALE_COF:正常速度)
 *@param loop   播放次数(-1:无限)
 */
void scui_ximage_vedio_play(scui_handle_t handle, scui_handle_t image,
    scui_multi_t speed, scui_multi_t loop);

/*@brief 图像控件图像帧播放控制
 *@param handle 图像控件句柄
 *@param work   播放:1;停止:0;
 */
void scui_ximage_vedio_work(scui_handle_t handle, bool work);

/*@brief 图像控件二维码
 *@param handle 图像控件句柄
 *@param data   url字符串
 *@param size   url字符串长度
 *@param color  图像颜色
 *@param scale  缩放系数(SCUI_SCALE_COF为原尺寸)
 */
void scui_ximage_qrcode(scui_handle_t handle, uint8_t *data, uint32_t size,
    scui_color_t color, scui_multi_t scale);

/*@brief 图像控件条形码
 *@param handle 图像控件句柄
 *@param data   url字符串
 *@param size   url字符串长度
 *@param color  图像颜色
 *@param scale  缩放系数(SCUI_SCALE_COF为原尺寸)
 */
void scui_ximage_barcode(scui_handle_t handle, uint8_t *data, uint32_t size,
    scui_color_t color, scui_multi_t scale);

/*@brief 图像控件图像序列陈列
 *@param handle 图像控件句柄
 *@param list   图像句柄列表
 *@param num    图像数量
 *@param span   图像间隙
 *@param way    方向(0:水平方向;1:垂直方向)
 */
void scui_ximage_sequence(scui_handle_t handle, scui_handle_t *list,
    scui_coord_t num, scui_coord_t span, bool way);

/*@brief 图像控件图像帧替换播放
 *@param handle 图像控件句柄
 *@param list   图像句柄列表
 *@param num    图像数量
 *@param speed  播放速度(SCUI_SCALE_COF:正常速度)
 *@param loop   播放次数(-1:无限)
 */
void scui_ximage_replace_play(scui_handle_t handle, scui_handle_t *list,
    scui_coord_t num, scui_multi_t speed, scui_multi_t loop);

/*@brief 图像控件图像帧替换播放控制
 *@param handle 图像控件句柄
 *@param work   播放:1;停止:0;
 */
void scui_ximage_replace_work(scui_handle_t handle, bool work);

#endif