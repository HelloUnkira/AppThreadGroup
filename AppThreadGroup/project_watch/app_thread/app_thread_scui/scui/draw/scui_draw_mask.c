/*实现目标:
 *    蒙版绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 蒙版绘制:半平面(直线两侧保留其一)
 *@param dst_surface 画布实例
 *@param dst_clip    实际绘制区域(画布坐标系)
 *@param pos_1       直线端点1
 *@param pos_2       直线端点2
 *@param invert      保留侧:0=左;1=右
 *@param alpha       全局透明度
 */
static void scui_draw_ctx_mask_line(scui_surface_t *dst_surface, scui_area_t *dst_clip,
    scui_point_t pos_1, scui_point_t pos_2, scui_coord_t invert, scui_alpha_t alpha)
{
    scui_multi_t dx = (scui_multi_t)pos_2.x - pos_1.x;
    scui_multi_t dy = (scui_multi_t)pos_2.y - pos_1.y;
    scui_multi_t len_sq = dx * dx + dy * dy;
    if (len_sq == 0) return;
    
    scui_multi_t len_i = 0;
    scui_multi_t len_f = 0;
    scui_sqrt(len_sq, &len_i, &len_f, 0x8000);
    scui_multi_t len = scui_max(len_i, 1);
    /* 直线长度: 只有整数部, 仅影响抗锯齿坡度的相对误差 */
    
    /* 叉积(cross大于0为直线左侧, 屏幕坐标系y轴向下)
     * 递推求解: 列+1则cross += dy; 行+1则cross -= dx
     */
    scui_multi_t limit = len / 2 + 2;   /* 覆盖率饱和边界, 同时约束乘法不溢出 */
    scui_multi_t cross = dy * ((scui_multi_t)dst_clip->x - pos_1.x) -
                         dx * ((scui_multi_t)dst_clip->y - pos_1.y);
    
    for (scui_multi_t row = 0; row < dst_clip->h; row++) {
        scui_multi_t cross_v = cross;
        uint8_t *dst_ofs = scui_surface_pixel_ofs(dst_surface, dst_clip->y + row, dst_clip->x);
        
        for (scui_multi_t col = 0; col < dst_clip->w; col++) {
            scui_multi_t dist = (scui_multi_t)(256 * scui_clamp(cross_v, -limit, limit) / len);
            if (invert == 0) dist = -dist;
            
            scui_alpha_t cov = (scui_alpha_t)scui_clamp(128 - dist, 0, 255);
            if (alpha != scui_alpha_cover) cov = scui_alpha_mix(cov, alpha);
            dst_ofs[col] = (uint8_t)scui_alpha_mix(dst_ofs[col], cov);
            cross_v += dy;
        }
        cross -= dx;
    }
}

/*@brief 蒙版绘制:扇形(两射线之间的楔形)
 *@param dst_surface 画布实例
 *@param dst_clip    实际绘制区域(画布坐标系)
 *@param vertex      射线顶点
 *@param angle_s     起始角度(顺时针, 0=右)
 *@param angle_e     结束角度(顺时针, 0=右)
 *@param alpha       全局透明度
 */
static void scui_draw_ctx_mask_angle(scui_surface_t *dst_surface, scui_area_t *dst_clip,
    scui_point_t vertex, scui_coord_t angle_s, scui_coord_t angle_e, scui_alpha_t alpha)
{
    /* 射线方向向量(三角函数放大4096倍, 模长恒为4096) */
    scui_multi_t sd_x = scui_cos4096(angle_s);
    scui_multi_t sd_y = scui_sin4096(angle_s);
    scui_multi_t ed_x = scui_cos4096(angle_e);
    scui_multi_t ed_y = scui_sin4096(angle_e);
    
    /* 顺时针跨角大于180度为优角(取并), 否则为凸楔形(取交) */
    scui_coord_t span = scui_mabs(angle_e - angle_s, 360);
    
    scui_multi_t limit = 4096 / 2 + 2;  /* 覆盖率饱和边界, 同时约束乘法不溢出 */
    for (scui_multi_t row = 0; row < dst_clip->h; row++) {
        scui_multi_t oy = (scui_multi_t)dst_clip->y + row - vertex.y;
        uint8_t *dst_ofs = scui_surface_pixel_ofs(dst_surface, dst_clip->y + row, dst_clip->x);
        
        for (scui_multi_t col = 0; col < dst_clip->w; col++) {
            scui_multi_t ox = (scui_multi_t)dst_clip->x + col - vertex.x;
            
            /* 起始边保留cross小于0侧, 终止边保留cross大于0侧 */
            scui_multi_t sc = sd_y * ox - sd_x * oy;
            scui_multi_t ec = ed_y * ox - ed_x * oy;
            scui_multi_t sd = (scui_multi_t)(256 * scui_clamp(sc, -limit, limit) / 4096);
            scui_multi_t ed = (scui_multi_t)(256 * scui_clamp(ec, -limit, limit) / 4096);
            scui_alpha_t cov_s = (scui_alpha_t)scui_clamp(128 - sd, 0, 255);
            scui_alpha_t cov_e = (scui_alpha_t)scui_clamp(128 + ed, 0, 255);
            scui_alpha_t cov = span > 180 ? scui_max(cov_s, cov_e) : scui_min(cov_s, cov_e);
            
            if (alpha != scui_alpha_cover) cov = scui_alpha_mix(cov, alpha);
            dst_ofs[col] = (uint8_t)scui_alpha_mix(dst_ofs[col], cov);
        }
    }
}

/*@brief 蒙版绘制:圆(保留圆内或圆外)
 *@param dst_surface 画布实例
 *@param dst_clip    实际绘制区域(画布坐标系)
 *@param center      圆心
 *@param radius      半径
 *@param invert      保留侧:0=内;1=外
 *@param alpha       全局透明度
 */
static void scui_draw_ctx_mask_radius(scui_surface_t *dst_surface, scui_area_t *dst_clip,
    scui_point_t center, scui_coord_t radius, scui_coord_t invert, scui_alpha_t alpha)
{
    radius = scui_max(radius, 1);
    
    scui_multi_t r_sq  = (scui_multi_t)radius * radius;
    scui_multi_t limit = radius + 1;    /* 覆盖率饱和边界, 同时约束乘法不溢出 */
    scui_multi_t pos_l = radius + 2;    /* 超出此距离必定饱和, 用于约束平方 */
    
    for (scui_multi_t row = 0; row < dst_clip->h; row++) {
        uint8_t *dst_ofs = scui_surface_pixel_ofs(dst_surface, dst_clip->y + row, dst_clip->x);
        scui_multi_t oy = scui_clamp((scui_multi_t)dst_clip->y + row - center.y, -pos_l, pos_l);
        scui_multi_t oy_sq = oy * oy;
        
        for (scui_multi_t col = 0; col < dst_clip->w; col++) {
            scui_multi_t ox = scui_clamp((scui_multi_t)dst_clip->x + col - center.x, -pos_l, pos_l);
            
            /* 距离平方差(大于0为圆外), 按半径折算成256倍距离(一阶精度) */
            scui_multi_t delta = ox * ox + oy_sq - r_sq;
            delta = scui_clamp(delta, -limit, limit);
            
            scui_multi_t dist = (scui_multi_t)(128 * delta / radius);
            if (invert != 0) dist = -dist;
            
            scui_alpha_t cov = (scui_alpha_t)scui_clamp(128 - dist, 0, 255);
            if (alpha != scui_alpha_cover) cov = scui_alpha_mix(cov, alpha);
            dst_ofs[col] = (uint8_t)scui_alpha_mix(dst_ofs[col], cov);
        }
    }
}

/*@brief 蒙版绘制:圆角矩形(保留矩形内或矩形外)
 *@param dst_surface 画布实例
 *@param dst_clip    实际绘制区域(画布坐标系)
 *@param center      矩形中心
 *@param extents     矩形半宽高
 *@param radius      圆角半径(0为直角矩形)
 *@param invert      保留侧:0=内;1=外
 *@param alpha       全局透明度
 */
static void scui_draw_ctx_mask_rect(scui_surface_t *dst_surface, scui_area_t *dst_clip,
    scui_point_t center, scui_point_t extents, scui_coord_t radius,
    scui_coord_t invert, scui_alpha_t alpha)
{
    scui_multi_t half_w = extents.x;
    scui_multi_t half_h = extents.y;
    if (half_w <= 0 || half_h <= 0) return;
    
    radius = scui_clamp(radius, 0, scui_min(half_w, half_h));
    
    /* 核心矩形(圆角圆心集合)的半宽高 */
    scui_multi_t core_w = half_w - radius;
    scui_multi_t core_h = half_h - radius;
    scui_multi_t r_sq   = (scui_multi_t)radius * radius;
    scui_multi_t limit  = radius + 1;   /* 覆盖率饱和边界, 同时约束乘法不溢出 */
    scui_multi_t pos_l  = radius + 2;   /* 超出此距离必定饱和, 用于约束平方 */
    scui_multi_t range  = scui_max(half_w, half_h);
    
    for (scui_multi_t row = 0; row < dst_clip->h; row++) {
        uint8_t *dst_ofs = scui_surface_pixel_ofs(dst_surface, dst_clip->y + row, dst_clip->x);
        scui_multi_t oy = (scui_multi_t)dst_clip->y + row - center.y;
        oy = scui_clamp(scui_abs(oy), 0, pos_l + range);
        scui_multi_t dy = oy - core_h;
        
        for (scui_multi_t col = 0; col < dst_clip->w; col++) {
            scui_multi_t ox = (scui_multi_t)dst_clip->x + col - center.x;
            ox = scui_clamp(scui_abs(ox), 0, pos_l + range);
            scui_multi_t dx = ox - core_w;
            
            scui_multi_t ex = scui_max(dx, 0);
            scui_multi_t ey = scui_max(dy, 0);
            scui_multi_t dist = 0;
            
            if (radius > 0 && ex > 0 && ey > 0) {
                /* 圆角区: 圆心在角心, 与圆同样一阶折算 */
                scui_multi_t delta = ex * ex + ey * ey - r_sq;
                delta = scui_clamp(delta, -limit, limit);
                dist = (scui_multi_t)(128 * delta / radius);
            } else if (ex > 0 || ey > 0) {
                /* 直边区: d = max(ex, ey) - radius */
                dist = 256 * (scui_max(ex, ey) - radius);
            } else {
                /* 核心区: d = max(dx, dy) - radius */
                dist = 256 * (scui_max(dx, dy) - radius);
            }
            
            if (invert != 0) dist = -dist;
            scui_alpha_t cov = (scui_alpha_t)scui_clamp(128 - dist, 0, 255);
            if (alpha != scui_alpha_cover) cov = scui_alpha_mix(cov, alpha);
            dst_ofs[col] = (uint8_t)scui_alpha_mix(dst_ofs[col], cov);
        }
    }
}

/*@brief 蒙版绘制:渐变(自起始沿向结束沿的垂直透明度渐变)
 *@param dst_surface 画布实例
 *@param dst_clip    实际绘制区域(画布坐标系)
 *@param y_s         起始沿(以上为起始透明度)
 *@param y_e         结束沿(以下为结束透明度)
 *@param alpha_s     起始沿透明度
 *@param alpha_e     结束沿透明度
 *@param alpha       全局透明度
 */
static void scui_draw_ctx_mask_fade(scui_surface_t *dst_surface, scui_area_t *dst_clip,
    scui_coord_t y_s, scui_coord_t y_e, scui_alpha_t alpha_s, scui_alpha_t alpha_e,
    scui_alpha_t alpha)
{
    scui_multi_t diff = scui_max((scui_multi_t)y_e - y_s + 1, 1);
    scui_multi_t alpha_d = (scui_multi_t)alpha_e - alpha_s;
    
    for (scui_multi_t row = 0; row < dst_clip->h; row++) {
        scui_multi_t y = (scui_multi_t)dst_clip->y + row;
        scui_alpha_t cov = alpha_e;
        
        if (y <= y_s) cov = alpha_s;
        else if (scui_betw_xx(y, y_s, y_e)) cov = (scui_alpha_t)((y - y_s) * alpha_d / diff + alpha_s);
        
        if (alpha != scui_alpha_cover) cov = scui_alpha_mix(cov, alpha);
        uint8_t *dst_ofs = scui_surface_pixel_ofs(dst_surface, (scui_coord_t)y, dst_clip->x);
        for (scui_multi_t col = 0; col < dst_clip->w; col++)
            dst_ofs[col] = (uint8_t)scui_alpha_mix(dst_ofs[col], cov);
    }
}

/*@brief 蒙版绘制:多边形(凸多边形, 逐行扫描取两条边界边求交)
 *@param dst_surface 画布实例
 *@param dst_clip    实际绘制区域(画布坐标系)
 *@param points      顶点数组(首尾隐式相连)
 *@param point_cnt   顶点数量(不小于3)
 *@param invert      保留侧:0=内;1=外
 *@param alpha       全局透明度
 */
static void scui_draw_ctx_mask_polygon(scui_surface_t *dst_surface, scui_area_t *dst_clip,
    const scui_point_t *points, scui_coord_t point_cnt, scui_coord_t invert, scui_alpha_t alpha)
{
    if (points == NULL || point_cnt < 3)
        return;
    
    /* 内点: 顶点均值(凸多边形必在内部), 用于统一各边保留侧 */
    scui_multi_t inside_x = 0;
    scui_multi_t inside_y = 0;
    
    for (scui_coord_t idx = 0; idx < point_cnt; idx++) {
        inside_x += points[idx].x;
        inside_y += points[idx].y;
    }
    inside_x /= point_cnt;
    inside_y /= point_cnt;
    
    for (scui_multi_t row = 0; row < dst_clip->h; row++) {
        scui_multi_t y = (scui_multi_t)dst_clip->y + row;
        scui_point_t edge_pos[2][2] = {0};
        scui_multi_t edge_cnt = 0;
        
        /* 扫描线取交: 收集跨越当前行的边界边(凸多边形恒为两条) */
        for (scui_coord_t idx = 0; idx < point_cnt && edge_cnt < 2; idx++) {
            scui_point_t pos_1 = points[idx];
            scui_point_t pos_2 = points[idx + 1 < point_cnt ? idx + 1 : 0];
            scui_coord_t y_min = scui_min(pos_1.y, pos_2.y);
            scui_coord_t y_max = scui_max(pos_1.y, pos_2.y);
            if (!scui_betw_lx(y, y_min, y_max))
                continue;
            
            edge_pos[edge_cnt][0] = pos_1;
            edge_pos[edge_cnt][1] = pos_2;
            edge_cnt++;
        }
        if (edge_cnt != 2)
            continue;
        
        /* 各边几何量: 方向向量, 长度, 叉积初值(内部在叉积大于0侧) */
        scui_multi_t edge_dy[2]    = {0};
        scui_multi_t edge_len[2]   = {0};
        scui_multi_t edge_lim[2]   = {0};
        scui_multi_t edge_cross[2] = {0};
        
        for (scui_coord_t idx = 0; idx < 2; idx++) {
            scui_multi_t dx = (scui_multi_t)edge_pos[idx][1].x - edge_pos[idx][0].x;
            scui_multi_t dy = (scui_multi_t)edge_pos[idx][1].y - edge_pos[idx][0].y;
            scui_multi_t cross_in = dy * (inside_x - edge_pos[idx][0].x) -
                                    dx * (inside_y - edge_pos[idx][0].y);
            
            /* 内点在叉积小于0侧则反向, 统一为内部在叉积大于0侧 */
            if (cross_in < 0) {
                scui_point_t pos_tmp = edge_pos[idx][0];
                edge_pos[idx][0] = edge_pos[idx][1];
                edge_pos[idx][1] = pos_tmp;
                dx = -dx;
                dy = -dy;
            }
            
            /* 直线长度: 只有整数部, 仅影响抗锯齿坡度的相对误差 */
            scui_multi_t len_sq = dx * dx + dy * dy;
            scui_multi_t len_i = 0;
            scui_multi_t len_f = 0;
            scui_sqrt(len_sq, &len_i, &len_f, 0x8000);
            
            edge_dy[idx]    = dy;
            edge_len[idx]   = scui_max(len_i, 1);
            edge_lim[idx]   = edge_len[idx] / 2 + 2;   /* 覆盖率饱和边界, 同时约束乘法不溢出 */
            edge_cross[idx] = dy * ((scui_multi_t)dst_clip->x - edge_pos[idx][0].x) -
                              dx * ((scui_multi_t)y - edge_pos[idx][0].y);
        }
        
        uint8_t *dst_ofs = scui_surface_pixel_ofs(dst_surface, (scui_coord_t)y, dst_clip->x);
        
        for (scui_multi_t col = 0; col < dst_clip->w; col++) {
            /* 两条边界边覆盖率取交(内部), 再按保留侧取补 */
            scui_multi_t cross_a = scui_clamp(edge_cross[0], -edge_lim[0], edge_lim[0]);
            scui_multi_t cross_b = scui_clamp(edge_cross[1], -edge_lim[1], edge_lim[1]);
            scui_multi_t cov_a = 128 + 256 * cross_a / edge_len[0];
            scui_multi_t cov_b = 128 + 256 * cross_b / edge_len[1];
            scui_alpha_t cov = (scui_alpha_t)scui_clamp(scui_min(cov_a, cov_b), 0, 255);
            if (invert != 0)
                cov = 255 - cov;
            if (alpha != scui_alpha_cover)
                cov = scui_alpha_mix(cov, alpha);
            dst_ofs[col] = (uint8_t)scui_alpha_mix(dst_ofs[col], cov);
            
            edge_cross[0] += edge_dy[0];
            edge_cross[1] += edge_dy[1];
        }
    }
}

/*@brief 蒙版绘制
 *@param draw_dsc 绘制描述符实例
 */
void scui_draw_ctx_mask(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->mask.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->mask.dst_clip;
    scui_alpha_t    src_alpha   =  draw_dsc->mask.src_alpha;
    scui_coord_t    src_type    =  draw_dsc->mask.src_type;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    SCUI_ASSERT(dst_surface->format == scui_pixel_cf_alpha8);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    switch (src_type) {
    case 0:
        scui_draw_ctx_mask_line(dst_surface, &dst_clip_v,
            draw_dsc->mask.src_pos_1, draw_dsc->mask.src_pos_2,
            draw_dsc->mask.src_invert, src_alpha);
        break;
    case 1:
        scui_draw_ctx_mask_angle(dst_surface, &dst_clip_v,
            draw_dsc->mask.src_pos_1,
            draw_dsc->mask.src_angle_s, draw_dsc->mask.src_angle_e, src_alpha);
        break;
    case 2:
        scui_draw_ctx_mask_radius(dst_surface, &dst_clip_v,
            draw_dsc->mask.src_pos_1, draw_dsc->mask.src_radius,
            draw_dsc->mask.src_invert, src_alpha);
        break;
    case 3:
        scui_draw_ctx_mask_rect(dst_surface, &dst_clip_v,
            draw_dsc->mask.src_pos_1, draw_dsc->mask.src_pos_2,
            draw_dsc->mask.src_radius, draw_dsc->mask.src_invert, src_alpha);
        break;
    case 4:
        scui_draw_ctx_mask_fade(dst_surface, &dst_clip_v,
            draw_dsc->mask.src_pos_1.y, draw_dsc->mask.src_pos_2.y,
            draw_dsc->mask.src_alpha_s, draw_dsc->mask.src_alpha_e, src_alpha);
        break;
    case 5:
        scui_draw_ctx_mask_polygon(dst_surface, &dst_clip_v,
            draw_dsc->mask.src_points, draw_dsc->mask.src_point_cnt,
            draw_dsc->mask.src_invert, src_alpha);
        break;
    default:
        SCUI_LOG_WARN("mask:%d invalid.", (int)src_type);
        break;
    }
}
