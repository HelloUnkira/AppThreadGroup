#ifndef APP_SYS_MATRIX_H
#define APP_SYS_MATRIX_H

/*@brief 矩阵交换运算(matrix[m][n]):matrix0 <==> matrix1
 *@param matrix0 矩阵
 *@param matrix1 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 */
void app_sys_matrix_swap(double *matrix0, double *matrix1, uint32_t m, uint32_t n);

/*@brief 矩阵拷贝运算(matrix[m][n]):matrix0 = matrix1
 *@param matrix0 矩阵
 *@param matrix1 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 */
void app_sys_matrix_copy(double *matrix0, double *matrix1, uint32_t m, uint32_t n);

/*@brief 矩阵和运算(matrix[m][n]):matrix += data
 *@param matrix 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 *@param data 标量
 */
void app_sys_matrix_sumdata(double *matrix, uint32_t m, uint32_t n, double data);

/*@brief 矩阵星运算(matrix[m][n]):matrix *= data
 *@param matrix 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 *@param data 标量
 */
void app_sys_matrix_stardata(double *matrix, uint32_t m, uint32_t n, double data);

/*@brief 矩阵和运算(matrix0[m][n],matrix1[m][n],matrix2[m][n]):matrix0 = matrix1 + matrix2
 *@param matrix1 矩阵
 *@param matrix2 矩阵
 *@param matrix0 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 */
void app_sys_matrix_sum(double *matrix0, double *matrix1, double *matrix2, uint32_t m, uint32_t n);

/*@brief 矩阵星运算(matrix0[m][n],matrix1[m][k],matrix2[k][n]):matrix0 = matrix1 * matrix2
 *@param matrix1 矩阵
 *@param matrix2 矩阵
 *@param matrix0 矩阵
 *@param m 矩阵1一阶值
 *@param k 矩阵1二阶值,矩阵2一阶值
 *@param n 矩阵2二阶值
 */
void app_sys_matrix_star(double *matrix0, double *matrix1, double *matrix2, uint32_t m, uint32_t k, uint32_t n);

/*@brief 矩阵幂运算(matrix0[n][n],matrix1[n][n],matrix2[n][n]):matrix0 = matrix1 ** exp
 *@param matrix1 矩阵
 *@param matrix2 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@param exp 标量
 */
void app_sys_matrix_exp(double *matrix0, double *matrix1, double *matrix2, uint32_t n, uint32_t exp);

/*@brief 矩阵转置运算(matrix0[m][n],matrix1[n][m]):matrix0 = matrix1**t
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 */
void app_sys_matrix_trans(double *matrix0, double *matrix1, uint32_t m, uint32_t n);

/*@brief 矩阵(方阵)逆运算(matrix0[n][n],matrix1[n][n],matrix2[n][2*n]):matrix0 = matrix1**(-1)
 *@param matrix1 矩阵
 *@param matrix2 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 成功失败
 */
bool app_sys_matrix_inv(double *matrix0, double *matrix1, double *matrix2, uint32_t n);

/*@brief 行列式矩阵(方阵)值运算(matrix0[n][n], matrix1[n][n]): |matrix0|
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 行列式结果
 */
double app_sys_matrix_val(double *matrix0, double *matrix1, uint32_t n);

/*@brief 矩阵cholesky主分解流程(matrix[n][n], vector[n])
 *@param vector 向量
 *@param matrix 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 成功失败
 */
bool app_sys_matrix_cholesky_dcm(double *matrix, double *vector, uint32_t n);

/*@brief 矩阵cholesky Decompose流程(matrix[n][n], matrix1[n][n])
 *@param vector  向量
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 成功失败
 */
bool app_sys_matrix_cholesky_dc(double *matrix0, double *matrix1, double *vector, uint32_t n);

/*@brief 矩阵cholesky分解反演流程(matrix0[n][n], matrix1[n][n]): matrix0 = matrix1**(-1)
 *@param vector  向量
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 成功失败
 */
bool app_sys_matrix_cholesky_dci(double *matrix0, double *matrix1, double *vector, uint32_t n);

/*@brief 矩阵cholesky分解反演流程(matrix0[n][n], matrix1[n][n]): matrix0 = matrix1**(-1)
 *@param vector  向量
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 成功失败
 */
bool app_sys_matrix_cholesky_inv(double *matrix0, double *matrix1, double *vector, uint32_t n);

/*@brief 矩阵cholesky分解计算矩阵的行列式(matrix0[n][n], matrix1[n][n])
 *@param vector  向量
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@param ret 行列式结果
 *@retval 成功失败
 */
bool app_sys_matrix_cholesky_det(double *matrix0, double *matrix1, double *vector, uint32_t n, double *ret);

#endif
