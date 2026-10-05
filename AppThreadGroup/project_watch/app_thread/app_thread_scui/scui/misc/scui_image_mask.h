#ifndef SCUI_IMAGE_MASK_H
#define SCUI_IMAGE_MASK_H

/*@brief 图像蒙版构建
 *@param handle 图像句柄
 *@param area   图像尺寸
 */
void scui_image_mask_make(scui_handle_t *handle, scui_area_t *area);

/*@brief 图像蒙版销毁
 *@param handle 图像句柄
 */
void scui_image_mask_burn(scui_handle_t handle);

/*@brief 图像蒙版应用
 *@param handle 图像句柄
 *@param mask   蒙版句柄
 */
void scui_image_mask_apply(scui_handle_t handle, scui_handle_t mask);

/*@brief 图像蒙版绘制(直线:半平面)
 *@param handle 图像句柄
 *@param area   绘制区域(NULL:整图)
 *@param pos_1  直线端点1
 *@param pos_2  直线端点2
 *@param invert 保留侧(0:左;1:右)
 *@param alpha  全局透明度
 */
void scui_image_mask_line(scui_handle_t handle, scui_area_t *area,
    scui_point_t pos_1, scui_point_t pos_2, scui_coord_t invert, scui_alpha_t alpha);

/*@brief 图像蒙版绘制(扇形)
 *@param handle  图像句柄
 *@param area    绘制区域(NULL:整图)
 *@param vertex  扇形顶点
 *@param angle_s 起始角度(顺时针,0=右)
 *@param angle_e 结束角度(顺时针,0=右)
 *@param alpha   全局透明度
 */
void scui_image_mask_angle(scui_handle_t handle, scui_area_t *area,
    scui_point_t vertex, scui_coord_t angle_s, scui_coord_t angle_e, scui_alpha_t alpha);

/*@brief 图像蒙版绘制(圆)
 *@param handle 图像句柄
 *@param area   绘制区域(NULL:整图)
 *@param center 圆心
 *@param radius 半径
 *@param invert 保留侧(0:内;1:外)
 *@param alpha  全局透明度
 */
void scui_image_mask_radius(scui_handle_t handle, scui_area_t *area,
    scui_point_t center, scui_coord_t radius, scui_coord_t invert, scui_alpha_t alpha);

/*@brief 图像蒙版绘制(圆角矩形)
 *@param handle  图像句柄
 *@param area    绘制区域(NULL:整图)
 *@param center  矩形中心
 *@param extents 矩形半宽高
 *@param radius  圆角半径(0为直角矩形)
 *@param invert  保留侧(0:内;1:外)
 *@param alpha   全局透明度
 */
void scui_image_mask_rect(scui_handle_t handle, scui_area_t *area,
    scui_point_t center, scui_point_t extents, scui_coord_t radius,
    scui_coord_t invert, scui_alpha_t alpha);

/*@brief 图像蒙版绘制(渐变)
 *@param handle  图像句柄
 *@param area    绘制区域(NULL:整图)
 *@param y_s     起始沿(以上为起始透明度)
 *@param y_e     结束沿(以下为结束透明度)
 *@param alpha_s 起始沿透明度
 *@param alpha_e 结束沿透明度
 *@param alpha   全局透明度
 */
void scui_image_mask_fade(scui_handle_t handle, scui_area_t *area, scui_coord_t y_s, scui_coord_t y_e,
    scui_alpha_t alpha_s, scui_alpha_t alpha_e, scui_alpha_t alpha);

/*@brief 图像蒙版绘制(多边形:凸多边形)
 *@param handle    图像句柄
 *@param area      绘制区域(NULL:整图)
 *@param points    顶点数组(首尾隐式相连)
 *@param point_cnt 顶点数量(不小于3)
 *@param invert    保留侧(0:内;1:外)
 *@param alpha     全局透明度
 */
void scui_image_mask_polygon(scui_handle_t handle, scui_area_t *area,
    const scui_point_t *points, scui_coord_t point_cnt, scui_coord_t invert, scui_alpha_t alpha);

#endif
