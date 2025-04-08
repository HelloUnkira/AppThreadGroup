#ifndef SCUI_COORD3_H
#define SCUI_COORD3_H

/*@brief 坐标点
 */
typedef float scui_coord3_t;

/*@brief 坐标
 */
typedef union {
    struct {
        scui_coord3_t x;
        scui_coord3_t y;
    };
    struct {
        scui_coord3_t w;
        scui_coord3_t h;
    };
    scui_coord3_t meta[2];
} scui_point2_t;

/*@brief 坐标
 */
typedef union {
    struct {
        scui_coord3_t x;
        scui_coord3_t y;
        scui_coord3_t z;
    };
    scui_coord3_t meta[3];
} scui_point3_t;

/*@brief 区域
 */
typedef struct {
    /* 逆时针:0:lu;1:ru;2:dr;3:du; */
    scui_point2_t point2[4];
} scui_area2_t;

/*@brief 区域
 */
typedef struct {
    /* 逆时针:0:lu;1:ru;2:dr;3:du; */
    scui_point3_t point3[4];
} scui_area3_t;

/*@brief 矩阵
 */
typedef struct {
    scui_coord3_t meta[3][3];
} scui_matrix_t;

typedef scui_point2_t scui_size2_t;     // 尺寸
typedef scui_point3_t scui_axis3_t;     // 轴
typedef scui_point3_t scui_view3_t;     // 视点
typedef scui_point3_t scui_normal3_t;   // 法线
typedef scui_point3_t scui_vertex3_t;   // 向量
typedef scui_area2_t  scui_face2_t;     // 面
typedef scui_area3_t  scui_face3_t;     // 面

/*@brief 矩阵计算旋转法线z轴
 *@param normal3  法线
 *@param normal_z 法线z轴
 *@param matrix   矩阵实例
 */
void scui_mormal3_z_by_matrix(scui_normal3_t *normal3, scui_coord3_t *normal_z, scui_matrix_t *matrix);

/*@brief 坐标变换(矩阵)
 *@param point3 坐标
 *@param matrix 矩阵实例
 */
void scui_point3_transform_by_matrix(scui_point3_t *point3, scui_matrix_t *matrix);

/*@brief 区域变换(矩阵)
 *@param area3  区域
 *@param matrix 矩阵实例
 */
void scui_area3_transform_by_matrix(scui_area3_t *area3, scui_matrix_t *matrix);

/*@brief 坐标透视
 *@param point3 坐标
 *@param view3  视点坐标(.z: 视点距离)
 */
void scui_point3_perspective(scui_point3_t *point3, scui_view3_t *view3);

/*@brief 区域透视
 *@param area3 区域
 *@param view3 视点坐标(.z: 视点距离)
 */
void scui_area3_perspective(scui_area3_t *area3, scui_view3_t *view3);

/*@brief 坐标转换(point3->point2)
 *@param point3 坐标
 *@param point2 坐标
 */
void scui_point3_to_point2(scui_point3_t *point3, scui_point2_t *point2);

/*@brief 坐标转换(point2->point3)
 *@param point3 坐标
 *@param point2  坐标
 */
void scui_point3_by_point2(scui_point3_t *point3, scui_point2_t *point2);

/*@brief 坐标偏移
 *@param point3 坐标
 *@param offset 偏移
 */
void scui_point3_offset(scui_point3_t *point3, scui_point3_t *offset);

/*@brief 坐标偏移
 *@param point3 坐标
 *@param offset 坐标
 */
void scui_point3_offset_xy(scui_point3_t *point3, scui_point2_t *offset);

/*@brief 区域转换(area3->area2)
 *@param area3 区域
 *@param area2 区域
 */
void scui_area3_to_area2(scui_area3_t *area3, scui_area2_t *area2);

/*@brief 区域转换(area2->area3)
 *@param area3 区域
 *@param area2 区域
 */
void scui_area3_by_area2(scui_area3_t *area3, scui_area2_t *area2);

/*@brief 区域转换(area->area2)
 *@param area2 区域
 *@param area  区域
 */
void scui_area2_by_area(scui_area2_t *area2, scui_area_t *area);

/*@brief 区域转换(area->area3)
 *@param area2 区域
 *@param area  区域
 */
void scui_area3_by_area(scui_area3_t *area3, scui_area_t *area);

/*@brief 区域偏移
 *@param area3  区域
 *@param offset 坐标
 */
void scui_area3_offset(scui_area3_t *area3, scui_point3_t *offset);

/*@brief 区域偏移
 *@param area3  区域
 *@param offset 坐标
 */
void scui_area3_offset_xy(scui_area3_t *area3, scui_point2_t *offset);

/*@brief 面中心点
 *@param area3  区域
 *@param point3 坐标
 */
void scui_area3_center(scui_area3_t *area3, scui_point3_t *point3);

/*@brief 面中心点z轴
 *@param area3    区域
 *@param point3_z 坐标z轴
 */
void scui_area3_center_z(scui_area3_t *area3, scui_coord3_t *point3_z);

#endif
