/*实现目标:
 *    坐标点运算
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 矩阵计算旋转法线z轴
 *@param normal3  法线
 *@param normal_z 法线z轴
 *@param matrix   矩阵实例
 */
void scui_mormal3_z_by_matrix(scui_normal3_t *normal3, scui_coord3_t *normal_z, scui_matrix_t *matrix)
{
    scui_coord3_t m_dx = matrix->meta[2][0] * normal3->x;
    scui_coord3_t m_dy = matrix->meta[2][1] * normal3->y;
    scui_coord3_t m_dz = matrix->meta[2][2] * normal3->z;
    
    *normal_z = m_dx + m_dy + m_dz;
}

/*@brief 坐标变换(矩阵)
 *@param point3 坐标
 *@param matrix 矩阵实例
 */
void scui_point3_transform_by_matrix(scui_point3_t *point3, scui_matrix_t *matrix)
{
    scui_point3_t point3_t;
    
    for (scui_coord_t row = 0; row < 3; row++) {
        scui_coord3_t m_dx = matrix->meta[row][0] * point3->x;
        scui_coord3_t m_dy = matrix->meta[row][1] * point3->y;
        scui_coord3_t m_dz = matrix->meta[row][2] * point3->z;
        
        point3_t.meta[row] = m_dx + m_dy + m_dz;
        // point3_t.meta[row] = roundf(point3_t.meta[row]);
    }
    
    *point3 = point3_t;
}

/*@brief 区域变换(矩阵)
 *@param area3  区域
 *@param matrix 矩阵实例
 */
void scui_area3_transform_by_matrix(scui_area3_t *area3, scui_matrix_t *matrix)
{
    for (scui_coord_t idx = 0; idx < 4; idx++) {
        
        scui_point3_t point3 = area3->point3[idx];
        scui_point3_transform_by_matrix(&point3, matrix);
        area3->point3[idx] = point3;
    }
}

/*@brief 坐标透视
 *@param point3 坐标
 *@param view3  视点坐标(.z: 视点距离)
 */
void scui_point3_perspective(scui_point3_t *point3, scui_view3_t *view3)
{
    #if 0
    point3->x = view3->x - (view3->x - point3->x) * view3->z / (view3->z - point3->z);
    point3->y = view3->y - (view3->y - point3->y) * view3->z / (view3->z - point3->z);
    point3->z = 1.0;
    #else
    scui_coord3_t perspective = view3->z / (view3->z - point3->z);
    
    point3->x = view3->x - (view3->x - point3->x) * perspective;
    point3->y = view3->y - (view3->y - point3->y) * perspective;
    point3->z = 1.0; // point3->z *= perspective;
    #endif
}

/*@brief 区域透视
 *@param area3 区域
 *@param view3 视点坐标(.z: 视点距离)
 */
void scui_area3_perspective(scui_area3_t *area3, scui_view3_t *view3)
{
    for (scui_coord_t idx = 0; idx < 4; idx++) {
        
        scui_point3_t point3 = area3->point3[idx];
        scui_point3_perspective(&point3, view3);
        area3->point3[idx] = point3;
    }
}

/*@brief 坐标转换(point3->point2)
 *@param point3 坐标
 *@param point2 坐标
 */
void scui_point3_to_point2(scui_point3_t *point3, scui_point2_t *point2)
{
    if (fabs(point3->z) <= 1e-6 || isnan(point3->z) || isinf(point3->z)) {
        point2->x = point3->x;
        point2->y = point3->y;
    } else {
        point2->x = point3->x / point3->z;
        point2->y = point3->y / point3->z;
        // point2->x = roundf(point2->x);
        // point2->y = roundf(point2->y);
    }
}

/*@brief 坐标转换(point2->point3)
 *@param point3 坐标
 *@param point2  坐标
 */
void scui_point3_by_point2(scui_point3_t *point3, scui_point2_t *point2)
{
    point3->x = point2->x;
    point3->y = point2->y;
    point3->z = 1.0f;
}

/*@brief 坐标偏移
 *@param point3 坐标
 *@param offset 偏移
 */
void scui_point3_offset(scui_point3_t *point3, scui_point3_t *offset)
{
    point3->x += offset->x;
    point3->y += offset->y;
    point3->z += offset->z;
}

/*@brief 坐标偏移
 *@param point3 坐标
 *@param point2 坐标
 */
void scui_point3_offset_xy(scui_point3_t *point3, scui_point2_t *offset)
{
    point3->x += offset->x;
    point3->y += offset->y;
}

/*@brief 区域转换(area3->area2)
 *@param area3 区域
 *@param area2 区域
 */
void scui_area3_to_area2(scui_area3_t *area3, scui_area2_t *area2)
{
    for (scui_coord_t idx = 0; idx < 4; idx++)
        scui_point3_to_point2(&area3->point3[idx], &area2->point2[idx]);
}

/*@brief 区域转换(area2->area3)
 *@param area3 区域
 *@param area2 区域
 */
void scui_area3_by_area2(scui_area3_t *area3, scui_area2_t *area2)
{
    for (scui_coord_t idx = 0; idx < 4; idx++) {
        scui_point3_by_point2(&area3->point3[idx], &area2->point2[idx]);
        // xy平面构建出的空间面, 应该落在xy平面内
        area3->point3[idx].z = 0.0f;
    }
}

/*@brief 区域转换(area2->area)
 *@param area2 区域
 *@param area  区域
 */
void scui_area2_to_area(scui_area2_t *area2, scui_area_t *area)
{
    scui_area_t area_s = {0};
    area_s.x1 = area2->point2[0].x;
    area_s.y1 = area2->point2[0].y;
    area_s.x2 = area2->point2[1].x;
    area_s.y1 = area2->point2[1].y;
    area_s.x2 = area2->point2[2].x;
    area_s.y2 = area2->point2[2].y;
    area_s.x1 = area2->point2[3].x;
    area_s.y2 = area2->point2[3].y;
    scui_area_m_by_s(area, &area_s);
}

/*@brief 区域转换(area->area2)
 *@param area2 区域
 *@param area  区域
 */
void scui_area2_by_area(scui_area2_t *area2, scui_area_t *area)
{
    scui_area_t area_s = {0};
    scui_area_m_to_s(area, &area_s);
    area2->point2[0].x = area_s.x1;
    area2->point2[0].y = area_s.y1;
    area2->point2[1].x = area_s.x2;
    area2->point2[1].y = area_s.y1;
    area2->point2[2].x = area_s.x2;
    area2->point2[2].y = area_s.y2;
    area2->point2[3].x = area_s.x1;
    area2->point2[3].y = area_s.y2;
}

/*@brief 区域转换(area3->area)
 *@param area2 区域
 *@param area  区域
 */
void scui_area3_to_area(scui_area3_t *area3, scui_area_t *area)
{
    scui_area2_t area2 = {0};
    scui_area3_to_area2(area3, &area2);
    scui_area2_to_area(&area2, area);
}

/*@brief 区域转换(area->area3)
 *@param area2 区域
 *@param area  区域
 */
void scui_area3_by_area(scui_area3_t *area3, scui_area_t *area)
{
    scui_area2_t area2 = {0};
    scui_area2_by_area(&area2, area);
    scui_area3_by_area2(area3, &area2);
}

/*@brief 区域偏移
 *@param area3  区域
 *@param offset 坐标
 */
void scui_area3_offset(scui_area3_t *area3, scui_point3_t *offset)
{
    for (scui_coord_t idx = 0; idx < 4; idx++) {
        
        scui_point3_t point3 = area3->point3[idx];
        scui_point3_offset(&point3, offset);
        area3->point3[idx] = point3;
    }
}

/*@brief 区域偏移
 *@param area3  区域
 *@param offset 坐标
 */
void scui_area3_offset_xy(scui_area3_t *area3, scui_point2_t *offset)
{
    for (scui_coord_t idx = 0; idx < 4; idx++) {
        
        scui_point3_t point3 = area3->point3[idx];
        scui_point3_offset_xy(&point3, offset);
        area3->point3[idx] = point3;
    }
}

/*@brief 面中心点
 *@param area3  区域
 *@param point3 坐标
 */
void scui_area3_center(scui_area3_t *area3, scui_point3_t *point3)
{
    point3->x = area3->point3[0].x + area3->point3[1].x + area3->point3[2].x + area3->point3[3].x;
    point3->y = area3->point3[0].y + area3->point3[1].y + area3->point3[2].y + area3->point3[3].y;
    point3->z = area3->point3[0].z + area3->point3[1].z + area3->point3[2].z + area3->point3[3].z;
    
    point3->x /= 4;
    point3->y /= 4;
    point3->z /= 4;
}

/*@brief 面中心点z轴
 *@param area3    区域
 *@param point3_z 坐标z轴
 */
void scui_area3_center_z(scui_area3_t *area3, scui_coord3_t *point3_z)
{
    *point3_z  = area3->point3[0].z + area3->point3[1].z + area3->point3[2].z + area3->point3[3].z;
    *point3_z /= 4;
}

/*@brief 面垂直检查
 *@param area3  区域
*@param  axis   法线轴向(0x00:Z轴向;0x01:X轴向;0x02:Y轴向;)
 */
bool scui_area3_parallel(scui_area3_t *area3, uint8_t axis)
{
    const scui_coord3_t eps = 1.1920929e-7f;
    
    switch (axis) {
    case 0x00:  // 平行XY平面
        if (scui_dist(area3->point3[0].z, area3->point3[0].z) < eps &&
            scui_dist(area3->point3[0].z, area3->point3[1].z) < eps &&
            scui_dist(area3->point3[0].z, area3->point3[2].z) < eps)
            return true;
        
        return false;
    case 0x01:  // 平行YZ平面
        
        if (scui_dist(area3->point3[0].x, area3->point3[0].x) < eps &&
            scui_dist(area3->point3[0].x, area3->point3[1].x) < eps &&
            scui_dist(area3->point3[0].x, area3->point3[2].x) < eps)
            return true;
        
        return false;
    case 0x02:  // 平行XZ平面
        if (scui_dist(area3->point3[0].y, area3->point3[0].y) < eps &&
            scui_dist(area3->point3[0].y, area3->point3[1].y) < eps &&
            scui_dist(area3->point3[0].y, area3->point3[2].y) < eps)
            return true;
        
        return false;
    default:
        SCUI_LOG_ERROR("unknown axis:%x", axis);
        break;
    }
}
