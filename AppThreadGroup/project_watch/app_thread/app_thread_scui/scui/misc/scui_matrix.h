#ifndef SCUI_MATRIX_H
#define SCUI_MATRIX_H

typedef union {
    struct {
        float x;
        float y;
        float z;
    };
    float meta[3];
} scui_matrix_pox_t;

typedef struct {
    /* 逆时针:0:lu;1:ru;2:dr;3:du; */
    scui_matrix_pox_t pox[4];
} scui_matrix_pox4_t;

typedef struct {
    float meta[3][3];
} scui_matrix_t;

/*备注:矩阵特性
 *    (A1 * A2 ... An)^(-1) = An^(-1) ... A2^(-1) * A1^(-1)
 */

/*@brief 矩阵坐标转换(pox->point)
 *@param pox    矩阵坐标
 *@param point  坐标
 */
void scui_matrix_pox_to_point(scui_matrix_pox_t *pox, scui_point_t *point);

/*@brief 矩阵坐标转换(point->pox)
 *@param pox    矩阵坐标
 *@param point  坐标
 */
void scui_matrix_pox_by_point(scui_matrix_pox_t *pox, scui_point_t *point);

/*@brief 矩阵坐标变换
 *@param matrix 矩阵实例
 *@param pox    矩阵坐标
 */
void scui_matrix_pox_transfer(scui_matrix_t *matrix, scui_matrix_pox_t *pox);

/*@brief 矩阵区域变换
 *@param matrix 矩阵实例
 *@param src    矩阵区域
 *@param dst    矩阵区域
 */
void scui_matrix_pox4_transfer(scui_matrix_t *matrix, scui_matrix_pox4_t *src, scui_matrix_pox4_t *dst);

/*@brief 矩阵生成
 *@param matrix 矩阵实例
 *@param src    平面区域
 *@param dst    平面区域
 *@retval 成功或失败
 */
bool scui_matrix_by_area(scui_matrix_t *matrix, scui_area_t *src, scui_area_t *dst);

/*@brief 矩阵乘法可逆检查
 *@param matrix 矩阵实例
 */
bool scui_matrix_inverse_check(scui_matrix_t *matrix);

/*@brief 矩阵乘法逆(matrix = 1 / matrix)
 *@param matrix 矩阵实例
 */
void scui_matrix_inverse(scui_matrix_t *matrix);

/*@brief 矩阵乘法(matrix *= matrix1)
 *@param matrix  矩阵实例
 *@param matrix1 矩阵实例
 */
void scui_matrix_multiply(scui_matrix_t *matrix, scui_matrix_t *matrix1);

/*@brief 矩阵单位化(单位矩阵)
 *@param matrix 矩阵实例
 */
void scui_matrix_identity(scui_matrix_t *matrix);

/*@brief 矩阵平移
 *@param matrix 矩阵实例
 *@param ofs_x  水平偏移值
 *@param ofs_y  垂直偏移值
 */
void scui_matrix_translate(scui_matrix_t *matrix, float ofs_x, float ofs_y);

/*@brief 矩阵缩放
 *@param matrix 矩阵实例
 *@param ofs_x  水平缩放比(1.0为基准线)
 *@param ofs_y  垂直缩放比(1.0为基准线)
 */
void scui_matrix_scale(scui_matrix_t *matrix, float ofs_x, float ofs_y);

/*@brief 矩阵旋转
 *@param matrix 矩阵实例
 *@param angle  旋转角度
 */
void scui_matrix_rotate(scui_matrix_t *matrix, int32_t angle);

/*@brief 矩阵透视
 *@param matrix 矩阵实例
 *@param pos_x  水平透视点
 *@param pos_y  垂直透视点
 */
void scui_matrix_perspective(scui_matrix_t *matrix, float pos_x, float pos_y);

#endif
