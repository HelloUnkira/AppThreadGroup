#ifndef SCUI_MATRIX_H
#define SCUI_MATRIX_H

/*@brief 矩阵输出
 *@param matrix 矩阵实例
 */
void scui_matrix_check(scui_matrix_t *matrix);

/*@brief 矩阵生成
 *@param matrix    矩阵实例
 *@param face2_src 平面
 *@param face2_dst 平面
 *@retval 成功或失败
 */
bool scui_matrix_by_face2(scui_matrix_t *matrix, scui_face2_t *face2_src, scui_face2_t *face2_dst);

/*@brief 矩阵生成
 *@param matrix    矩阵实例
 *@param face2_src 平面
 *@param face3_dst 平面(空间)
 *@retval 成功或失败
 */
bool scui_matrix_by_face3(scui_matrix_t *matrix, scui_face2_t *face2_src, scui_face3_t *face3_dst);

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
 *@param offset 偏移值
 */
void scui_matrix_translate(scui_matrix_t *matrix, scui_point2_t *offset);

/*@brief 矩阵透视
 *@param matrix 矩阵实例
 *@param view   透视点
 */
void scui_matrix_perspective(scui_matrix_t *matrix, scui_point2_t *view);

/*@brief 矩阵缩放
 *@param matrix 矩阵实例
 *@param offset 缩放比(1.0为基准线)
 */
void scui_matrix_scale(scui_matrix_t *matrix, scui_point2_t *scale);

/*@brief 矩阵旋转
 *@param matrix 矩阵实例
 *@param angle  旋转角度
 *@param axis   旋转轴向(0x00:Z轴向;0x01:X轴向;0x02:Y轴向;)
 */
void scui_matrix_rotate(scui_matrix_t *matrix, scui_coord3_t angle, uint8_t axis);

/*@brief 矩阵旋转
 *@param matrix 矩阵实例
 *@param angle  旋转角度
 *@param axis   旋转轴向(0x00:ZYX;0x01:ZXY;0x02:YZX;0x03:YXZ;0x04:XYZ;0x05:XZY;)
 */
void scui_matrix_rotate3(scui_matrix_t *matrix, scui_point3_t *angle, uint8_t axis);

/*@brief 矩阵仿射(blit)
 *@param matrix 矩阵实例
 *@param size   尺寸(src)
 *@param face   面(dst)
 */
void scui_matrix_affine_blit(scui_matrix_t *matrix, scui_size2_t *size, scui_face3_t *face);

/*@brief 矩阵投影(blit)
 *@param matrix 矩阵实例
 *@param size   尺寸(src)
 *@param face   面(dst)
 *@retval 成功或失败
 */
bool scui_matrix_perspective_blit(scui_matrix_t *matrix, scui_size2_t *size, scui_face3_t *face);

/*@brief 矩阵投影(blit)
 *@param matrix 矩阵实例
 *@param size   尺寸(src)
 *@param face   面(dst)
 *@param view   视点
 *@retval 成功或失败
 */
bool scui_matrix_perspective_view_blit(scui_matrix_t *matrix, scui_size2_t *size, scui_face3_t *face, scui_view3_t *view);

#endif
