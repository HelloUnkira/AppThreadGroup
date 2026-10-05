/*实现目标:
 *    图像蒙版
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 图像蒙版构建
 *@param handle 图像句柄
 *@param area   图像尺寸
 */
void scui_image_mask_make(scui_handle_t *handle, scui_area_t *area)
{
    SCUI_ASSERT(handle != NULL && area != NULL);
    SCUI_ASSERT(area->w != 0 && area->h != 0);
    
    /* 图像实例为内部资源, 只对外暴露句柄 */
    scui_image_t *image = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_image_t));
    image->format = scui_pixel_cf_alpha8;
    
    scui_image_make(image, area);
    /* 蒙版图初值为全通(255), 叠加蒙版为覆盖率相乘 */
    scui_draw_byte_new(true, image->pixel.data_bin, 0xff, image->pixel.size_bin);
    
    /* 申请句柄并绑定 */
    *handle = scui_handle_find();
    scui_handle_linker(*handle, image);
}

/*@brief 图像蒙版销毁
 *@param handle 图像句柄
 */
void scui_image_mask_burn(scui_handle_t handle)
{
    scui_image_t *image = scui_handle_source_check(handle);
    
    /* 先解绑句柄, 再回收图像实例 */
    scui_handle_clear(handle);
    scui_image_burn(image);
    SCUI_MEM_FREE(image);
}

/*@brief 图像蒙版应用
 *@param handle 图像句柄
 *@param mask   蒙版句柄
 */
void scui_image_mask_apply(scui_handle_t handle, scui_handle_t mask)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    SCUI_ASSERT(mask   != SCUI_HANDLE_INVALID);
    
    scui_image_t *image_dst  = scui_handle_source_check(handle);
    scui_image_t *image_mask = scui_handle_source_check(mask);
    SCUI_ASSERT(image_mask->format == scui_pixel_cf_alpha8);
    
    scui_surface_t surface_dst = {0};
    scui_image_to_surface(image_dst, &surface_dst);
    
    /* 蒙版与图像原点对齐整幅作用 */
    scui_area_t dst_clip = {.w = image_dst->pixel.width, .h = image_dst->pixel.height,};
    scui_draw_image_mask(true, &surface_dst, dst_clip, image_mask, dst_clip);
}

/*@brief 图像蒙版绘制(泛用, 内部: 六个类型共用一套协议字段)
 *       直线:     取pos_1/pos_2两端点, invert保留侧
 *       扇形:     取pos_1顶点, angle_s/angle_e起止角度
 *       圆:       取pos_1圆心, radius半径, invert保留侧
 *       圆角矩形: 取pos_1中心, pos_2半宽高, radius圆角半径, invert保留侧
 *       渐变:     取pos_1.y起始沿, pos_2.y结束沿, alpha_s/alpha_e两端透明度
 *       多边形:   取points顶点数组/point_cnt数量, invert保留侧
 *@param handle  图像句柄
 *@param area    绘制区域(NULL:整图)
 *@param type    蒙版类型(0:直线;1:扇形;2:圆;3:圆角矩形;4:渐变;5:多边形)
 *@param pos_1   直线端点1 / 扇形顶点 / 圆心 / 矩形中心 / 渐变起始沿
 *@param pos_2   直线端点2 / 矩形半宽高 / 渐变结束沿
 *@param angle_s 扇形起始角度(顺时针,0=右)
 *@param angle_e 扇形结束角度(顺时针,0=右)
 *@param radius  圆半径 / 矩形圆角半径
 *@param points  多边形顶点数组(首尾隐式相连)
 *@param point_cnt 多边形顶点数量(不小于3)
 *@param alpha_s 渐变起始沿透明度
 *@param alpha_e 渐变结束沿透明度
 *@param invert  保留侧(直线:0=左;1=右;圆/矩形:0=内;1=外)
 *@param alpha   全局透明度
 */
static void scui_image_mask_draw(scui_handle_t handle, scui_area_t *area, scui_coord_t type,
    scui_point_t pos_1, scui_point_t pos_2, scui_coord_t angle_s, scui_coord_t angle_e,
    scui_coord_t radius, const scui_point_t *points, scui_coord_t point_cnt,
    scui_alpha_t alpha_s, scui_alpha_t alpha_e,
    scui_coord_t invert, scui_alpha_t alpha)
{
    scui_image_t *image = scui_handle_source_check(handle);
    SCUI_ASSERT(image->format == scui_pixel_cf_alpha8);
    SCUI_ASSERT(scui_betw_lr(type, 0, 5));
    
    scui_surface_t surface = {0};
    scui_image_to_surface(image, &surface);
    
    /* 绘制区域缺省为整图 */
    scui_area_t dst_clip = {
        .w = image->pixel.width,
        .h = image->pixel.height,
    };
    
    if (area != NULL) dst_clip = *area;
    scui_draw_mask(true, &surface, dst_clip, alpha, type,
        pos_1, pos_2, angle_s, angle_e, radius, points, point_cnt,
        alpha_s, alpha_e, invert);
}

/*@brief 图像蒙版绘制(直线:半平面)
 *@param handle 图像句柄
 *@param area   绘制区域(NULL:整图)
 *@param pos_1  直线端点1
 *@param pos_2  直线端点2
 *@param invert 保留侧(0:左;1:右)
 *@param alpha  全局透明度
 */
void scui_image_mask_line(scui_handle_t handle, scui_area_t *area,
    scui_point_t pos_1, scui_point_t pos_2, scui_coord_t invert, scui_alpha_t alpha)
{
    scui_image_mask_draw(handle, area, 0,
        pos_1, pos_2, 0, 0, 0, NULL, 0, scui_alpha_cover, scui_alpha_cover, invert, alpha);
}

/*@brief 图像蒙版绘制(扇形)
 *@param handle  图像句柄
 *@param area    绘制区域(NULL:整图)
 *@param vertex  扇形顶点
 *@param angle_s 起始角度(顺时针,0=右)
 *@param angle_e 结束角度(顺时针,0=右)
 *@param alpha   全局透明度
 */
void scui_image_mask_angle(scui_handle_t handle, scui_area_t *area,
    scui_point_t vertex, scui_coord_t angle_s, scui_coord_t angle_e, scui_alpha_t alpha)
{
    /* 扇形不使用pos_2, 仅为协议字段填充 */
    scui_image_mask_draw(handle, area, 1,
        vertex, vertex, angle_s, angle_e, 0, NULL, 0, scui_alpha_cover, scui_alpha_cover, 0, alpha);
}

/*@brief 图像蒙版绘制(圆)
 *@param handle 图像句柄
 *@param area   绘制区域(NULL:整图)
 *@param center 圆心
 *@param radius 半径
 *@param invert 保留侧(0:内;1:外)
 *@param alpha  全局透明度
 */
void scui_image_mask_radius(scui_handle_t handle, scui_area_t *area,
    scui_point_t center, scui_coord_t radius, scui_coord_t invert, scui_alpha_t alpha)
{
    scui_point_t pos_zero = {0};
    scui_image_mask_draw(handle, area, 2,
        center, pos_zero, 0, 0, radius, NULL, 0, scui_alpha_cover, scui_alpha_cover, invert, alpha);
}

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
    scui_coord_t invert, scui_alpha_t alpha)
{
    scui_image_mask_draw(handle, area, 3,
        center, extents, 0, 0, radius, NULL, 0, scui_alpha_cover, scui_alpha_cover, invert, alpha);
}

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
    scui_alpha_t alpha_s, scui_alpha_t alpha_e, scui_alpha_t alpha)
{
    scui_point_t pos_s = {.y = y_s,};
    scui_point_t pos_e = {.y = y_e,};
    scui_image_mask_draw(handle, area, 4,
        pos_s, pos_e, 0, 0, 0, NULL, 0, alpha_s, alpha_e, 0, alpha);
}

/*@brief 图像蒙版绘制(多边形:凸多边形)
 *@param handle    图像句柄
 *@param area      绘制区域(NULL:整图)
 *@param points    顶点数组(首尾隐式相连)
 *@param point_cnt 顶点数量(不小于3)
 *@param invert    保留侧(0:内;1:外)
 *@param alpha     全局透明度
 */
void scui_image_mask_polygon(scui_handle_t handle, scui_area_t *area,
    const scui_point_t *points, scui_coord_t point_cnt, scui_coord_t invert, scui_alpha_t alpha)
{
    scui_point_t pos_zero = {0};
    scui_image_mask_draw(handle, area, 5,
        pos_zero, pos_zero, 0, 0, 0, points, point_cnt,
        scui_alpha_cover, scui_alpha_cover, invert, alpha);
}
