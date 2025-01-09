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
    *normal_z = matrix->meta[2][0] * normal3->x + matrix->meta[2][1] * normal3->y + matrix->meta[2][2] * normal3->z;
}

/*@brief 坐标变换(矩阵)
 *@param point3 坐标
 *@param matrix 矩阵实例
 */
void scui_point3_transform_by_matrix(scui_point3_t *point3, scui_matrix_t *matrix)
{
    scui_point3_t point3_t;
    
    for (uint8_t row = 0; row < 3; row++) {
        point3_t.meta[row] = matrix->meta[row][0] * point3->x +
                             matrix->meta[row][1] * point3->y +
                             matrix->meta[row][2] * point3->z;
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
    scui_point3_t point3 = {0};
    
    point3 = area3->point3[0];
    scui_point3_transform_by_matrix(&point3, matrix);
    area3->point3[0] = point3;
    
    point3 = area3->point3[1];
    scui_point3_transform_by_matrix(&point3, matrix);
    area3->point3[1] = point3;
    
    point3 = area3->point3[2];
    scui_point3_transform_by_matrix(&point3, matrix);
    area3->point3[2] = point3;
    
    point3 = area3->point3[3];
    scui_point3_transform_by_matrix(&point3, matrix);
    area3->point3[3] = point3;
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

/*@brief 区域转换(area3->area2)
 *@param area3 区域
 *@param area2 区域
 */
void scui_area3_to_area2(scui_area3_t *area3, scui_area2_t *area2)
{
    scui_point3_to_point2(&area3->point3[0], &area2->point2[0]);
    scui_point3_to_point2(&area3->point3[1], &area2->point2[1]);
    scui_point3_to_point2(&area3->point3[2], &area2->point2[2]);
    scui_point3_to_point2(&area3->point3[3], &area2->point2[3]);
}

/*@brief 区域转换(area2->area3)
 *@param area3 区域
 *@param area2 区域
 */
void scui_area3_by_area2(scui_area3_t *area3, scui_area2_t *area2)
{
    scui_point3_by_point2(&area3->point3[0], &area2->point2[0]);
    scui_point3_by_point2(&area3->point3[1], &area2->point2[1]);
    scui_point3_by_point2(&area3->point3[2], &area2->point2[2]);
    scui_point3_by_point2(&area3->point3[3], &area2->point2[3]);
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

/*@brief 区域偏移
 *@param area3  区域
 *@param offset 坐标
 */
void scui_area3_offset(scui_area3_t *area3, scui_point3_t *offset)
{
    scui_point3_t point3 = {0};
    
    point3 = area3->point3[0];
    scui_point3_offset(&point3, offset);
    area3->point3[0] = point3;
    
    point3 = area3->point3[1];
    scui_point3_offset(&point3, offset);
    area3->point3[1] = point3;
    
    point3 = area3->point3[2];
    scui_point3_offset(&point3, offset);
    area3->point3[2] = point3;
    
    point3 = area3->point3[3];
    scui_point3_offset(&point3, offset);
    area3->point3[3] = point3;
}

/*@brief 区域偏移
 *@param area3  区域
 *@param offset 坐标
 */
void scui_area3_offset_xy(scui_area3_t *area3, scui_point2_t *offset)
{
    scui_point3_t point3 = {0};
    
    point3 = area3->point3[0];
    scui_point3_offset_xy(&point3, offset);
    area3->point3[0] = point3;
    
    point3 = area3->point3[1];
    scui_point3_offset_xy(&point3, offset);
    area3->point3[1] = point3;
    
    point3 = area3->point3[2];
    scui_point3_offset_xy(&point3, offset);
    area3->point3[2] = point3;
    
    point3 = area3->point3[3];
    scui_point3_offset_xy(&point3, offset);
    area3->point3[3] = point3;
}

/*@brief 坐标透视
 *@param point3 坐标
 *@param view3  视点坐标(.z: 视点距离)
 */
void scui_point3_perspective(scui_point3_t *point3, scui_view3_t *view3)
{
    #if 1
    point3->x = view3->x - (view3->x - point3->x) * view3->z / (view3->z - point3->z);
    point3->y = view3->y - (view3->y - point3->y) * view3->z / (view3->z - point3->z);
    #else
    point3->x = view3->x + (point3->x - view3->x) * view3->z / (view3->z + point3->z);
    point3->y = view3->y + (point3->y - view3->y) * view3->z / (view3->z + point3->z);
    #endif
}

/*@brief 区域透视
 *@param area3 区域
 *@param view3 视点坐标(.z: 视点距离)
 */
void scui_area3_perspective(scui_area3_t *area3, scui_view3_t *view3)
{
    scui_point3_t point3 = {0};
    
    point3 = area3->point3[0];
    scui_point3_perspective(&point3, view3);
    area3->point3[0] = point3;
    
    point3 = area3->point3[1];
    scui_point3_perspective(&point3, view3);
    area3->point3[1] = point3;
    
    point3 = area3->point3[2];
    scui_point3_perspective(&point3, view3);
    area3->point3[2] = point3;
    
    point3 = area3->point3[3];
    scui_point3_perspective(&point3, view3);
    area3->point3[3] = point3;
}
