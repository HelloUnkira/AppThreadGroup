/*实现目标:
 *    通用矩阵运算
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/* 矩阵转数列 */
//#define app_sys_matrix_to_array(j, i, row) ((j) * (row) + (i))
  #define app_sys_matrix_to_array(i, j, col) ((i) * (col) + (j))

/* 矩阵精度0 */
#define app_sys_matrix_zero     (1E-6)

/*@brief 矩阵交换运算(matrix[m][n]):matrix0 <==> matrix1
 *@param matrix0 矩阵
 *@param matrix1 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 */
void app_sys_matrix_swap(double *matrix0, double *matrix1, uint32_t m, uint32_t n)
{
    #if 1
    for (uint32_t t = 0; t < m * n; t++) {
         double tmp = matrix0[t];
         matrix0[t] = matrix1[t];
         matrix1[t] = tmp;
    }
    #else
    for (uint32_t i = 0; i < m; i++)
    for (uint32_t j = 0; j < n; j++) {
         uint32_t idx = app_sys_matrix_to_array(i, j, n);
         double tmp   = matrix0[idx];
         matrix0[idx] = matrix1[idx];
         matrix1[idx] = tmp;
    }
    #endif
}

/*@brief 矩阵拷贝运算(matrix[m][n]):matrix0 = matrix1
 *@param matrix0 矩阵
 *@param matrix1 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 */
void app_sys_matrix_copy(double *matrix0, double *matrix1, uint32_t m, uint32_t n)
{
    #if 1
    for (uint32_t t = 0; t < m * n; matrix0[t] = matrix1[t], t++);
    #else
    for (uint32_t i = 0; i < m; i++)
    for (uint32_t j = 0; j < n; j++) {
         uint32_t idx = app_sys_matrix_to_array(i, j, n);
         matrix0[idx] = matrix1[idx];
    }
    #endif
}

/*@brief 矩阵和运算(matrix[m][n]):matrix += data
 *@param matrix 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 *@param data 标量
 */
void app_sys_matrix_sumdata(double *matrix, uint32_t m, uint32_t n, double data)
{
    #if 1
    for (uint32_t t = 0; t < m * n; matrix[t] += data, t++);
    #else
    for (uint32_t i = 0; i < m; i++)
    for (uint32_t j = 0; j < n; j++)
         matrix[app_sys_matrix_to_array(i, j, n)] += data;
    #endif
}

/*@brief 矩阵星运算(matrix[m][n]):matrix *= data
 *@param matrix 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 *@param data 标量
 */
void app_sys_matrix_stardata(double *matrix, uint32_t m, uint32_t n, double data)
{
    #if 1
    for (uint32_t t = 0; t < m * n; matrix[t] *= data, t++);
    #else
    for (uint32_t i = 0; i < m; i++)
    for (uint32_t j = 0; j < n; j++)
         matrix[app_sys_matrix_to_array(i, j, n)] *= data;
    #endif
}

/*@brief 矩阵和运算(matrix0[m][n],matrix1[m][n],matrix2[m][n]):matrix0 = matrix1 + matrix2
 *@param matrix1 矩阵
 *@param matrix2 矩阵
 *@param matrix0 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 */
void app_sys_matrix_sum(double *matrix0, double *matrix1, double *matrix2, uint32_t m, uint32_t n)
{
    #if 1
    for (uint32_t t = 0; t < m * n; matrix0[t] = matrix1[t] + matrix2[t], t++);
    #else
    /* 公式:matrix0[i][j] = matrix1[i][j] (+,-) matrix2[i][j] */
    for (uint32_t i = 0; i < m; i++)
    for (uint32_t j = 0; j < n; j++) {
         uint32_t idx = app_sys_matrix_to_array(i, j, n);
         matrix0[idx] = matrix1[idx] + matrix2[idx];
    }
    #endif
}

/*@brief 矩阵星运算(matrix0[m][n],matrix1[m][k],matrix2[k][n]):matrix0 = matrix1 * matrix2
 *@param matrix1 矩阵
 *@param matrix2 矩阵
 *@param matrix0 矩阵
 *@param m 矩阵1一阶值
 *@param k 矩阵1二阶值,矩阵2一阶值
 *@param n 矩阵2二阶值
 */
void app_sys_matrix_star(double *matrix0, double *matrix1, double *matrix2, uint32_t m, uint32_t k, uint32_t n)
{
    /* 公式:matrix0[i][j] = SUM{t=(0,k-1)} matrix1[i][t] * matrix2[t][j] */
    for (uint32_t i = 0; i < m; i++)
    for (uint32_t j = 0; j < n; j++) {
         uint32_t idx1 = app_sys_matrix_to_array(i, j, n);
         matrix0[idx1] = 0.0;
    for (uint32_t t = 0; t < k; t++) {
         uint32_t idx2 = app_sys_matrix_to_array(i, t, k);
         uint32_t idx3 = app_sys_matrix_to_array(t, j, n);
         matrix0[idx1] += matrix1[idx2] * matrix2[idx3];
    }
    }
}

/*@brief 矩阵幂运算(matrix0[n][n],matrix1[n][n],matrix2[n][n]):matrix0 = matrix1 ** exp
 *@param matrix1 矩阵
 *@param matrix2 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@param exp 标量
 */
void app_sys_matrix_exp(double *matrix0, double *matrix1, double *matrix2, uint32_t n, uint32_t exp)
{
    app_sys_matrix_copy(matrix0, matrix1, n, n);
    for (uint32_t i = 0; i < exp; i++) {
         app_sys_matrix_copy(matrix2, matrix0, n, n);
         app_sys_matrix_star(matrix0, matrix2, matrix1, n, n, n);
    }
}

/*@brief 矩阵转置运算(matrix0[m][n],matrix1[n][m]):matrix0 = matrix1**t
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param m 矩阵一阶值
 *@param n 矩阵二阶值
 */
void app_sys_matrix_trans(double *matrix0, double *matrix1, uint32_t m, uint32_t n)
{
    /* 公式:matrix0[i][j] = matrix1[j][i] */
    for (uint32_t i = 0; i < m; i++)
    for (uint32_t j = 0; j < n; j++) {
         uint32_t idx1 = app_sys_matrix_to_array(i, j, n);
         uint32_t idx2 = app_sys_matrix_to_array(j, i, n);
         matrix0[idx1] = matrix1[idx2];
    }
}

/*@brief 矩阵(方阵)逆运算(matrix0[n][n],matrix1[n][n],matrix2[n][2*n]):matrix0 = matrix1**(-1)
 *@param matrix1 矩阵
 *@param matrix2 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 成功失败
 */
bool app_sys_matrix_inv(double *matrix0, double *matrix1, double *matrix2, uint32_t n)
{
    if (n == 0)
        return false;
    /* 1.一阶矩阵:直接求逆 */
    if (n == 1) {
        matrix0[app_sys_matrix_to_array(0, 0, n)] = 1 /
        matrix1[app_sys_matrix_to_array(0, 0, n)];
        return true;
    }
    /* 2.二阶矩阵:伴随矩阵法 */
    if (n == 2) {
        double tmp =
        matrix1[app_sys_matrix_to_array(0, 0, n)] *
        matrix1[app_sys_matrix_to_array(1, 1, n)] -
        matrix1[app_sys_matrix_to_array(0, 1, n)] *
        matrix1[app_sys_matrix_to_array(1, 0, n)];
        matrix0[app_sys_matrix_to_array(0, 0, n)] = +
        matrix1[app_sys_matrix_to_array(1, 1, n)] / tmp;
        matrix0[app_sys_matrix_to_array(1, 1, n)] = +
        matrix1[app_sys_matrix_to_array(0, 0, n)] / tmp;
        matrix0[app_sys_matrix_to_array(0, 1, n)] = -
        matrix1[app_sys_matrix_to_array(0, 1, n)] / tmp;
        matrix0[app_sys_matrix_to_array(1, 0, n)] = -
        matrix1[app_sys_matrix_to_array(1, 0, n)] / tmp;
        return true;
    }
    /* 3.高阶矩阵:高斯消元法(LU分解法)(不使用) */
    /* 3.高阶矩阵:全选主元高斯-约当消去法(高效) */
    if (n >= 3) {
        /* 1.生成增广矩阵:matrix2 = {matrix1, E} */
        for (uint32_t i = 0; i < n; i++)
        for (uint32_t j = 0; j < n; j++)
            matrix2[app_sys_matrix_to_array(i, j, n * 2)] =
            matrix1[app_sys_matrix_to_array(i, j, n)];
        for (uint32_t i = 0; i < n; i++)
        for (uint32_t j = 0; j < n; j++)
            matrix2[app_sys_matrix_to_array(i, j + n, n * 2)] = (i == j) ? 1 : 0;
        /* 2.全选主元:(共计N次全选主元) */
        for (uint32_t i = 0; i < n; i++) {
            /* 1.列选主元,检查精度:  app_sys_matrix_zero */
            double   master_max = 0.0;
            uint32_t master_idx = 0;
            /* 要选取绝对值最大的值,即使是负数简易变换也能归一化 */
            for (uint32_t j = i; j < n; j++) {
                double tmp = matrix2[app_sys_matrix_to_array(j, i, n * 2)];
                       tmp = tmp < 0 ? -tmp : tmp;
                if (master_max < tmp) {
                    master_max = tmp;
                    master_idx = j;
                }
            }
            /* 如果连主元都有为0的可能,那么矩阵是奇异矩阵 */
            if (master_max <= app_sys_matrix_zero * 2)
                return false;
            /* 2.经过行变换移动到主对角线上 */
            if (master_idx != i) {
                /* 交换主元所在行与目标行 */
                for (uint32_t j = 0; j < n * 2; j++) {
                    double tmp1 = matrix2[app_sys_matrix_to_array(master_idx, j, n * 2)];
                    double tmp2 = matrix2[app_sys_matrix_to_array(i, j, n * 2)];
                    matrix2[app_sys_matrix_to_array(master_idx, j, n * 2)] = tmp2;
                    matrix2[app_sys_matrix_to_array(i, j, n * 2)] = tmp1;
                }
            }
            master_max = matrix2[app_sys_matrix_to_array(i, i, n * 2)];
            /* 3.主元行归一化 */
            for (uint32_t j = 0; j < n * 2; j++)
                matrix2[app_sys_matrix_to_array(i, j, n * 2)] /= master_max;
            /* 4.主元列归零化 */
            for (uint32_t j = 0; j < n; j++)
                if (i != j) {
                    double tmp = matrix2[app_sys_matrix_to_array(j, i, n * 2)];
                    for (uint32_t k = 0; k < n * 2; k++)
                        matrix2[app_sys_matrix_to_array(j, k, n * 2)] -=
                        matrix2[app_sys_matrix_to_array(i, k, n * 2)] * tmp;
                }
        }
        /* 3.拷贝逆矩阵 */
        for (uint32_t i = 0; i < n; i++)
        for (uint32_t j = 0; j < n; j++)
            matrix0[app_sys_matrix_to_array(i, j, n)] = 
            matrix2[app_sys_matrix_to_array(i, j + n, n * 2)];
        return true;
    }
    return false;
}

/*@brief 行列式矩阵(方阵)值运算(matrix0[n][n], matrix1[n][n]): |matrix0|
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 行列式结果
 */
double app_sys_matrix_val(double *matrix0, double *matrix1, uint32_t n)
{
    /* 备注:逆序数法涉及到全排列问题较复杂,暂未研究,插眼存档 */
    /* 矩阵行列式:高斯消元法(生成上三角矩阵) */
    /* 1.将其拷贝到缓冲矩阵中 */
    for (uint32_t i = 0; i < n; i++)
    for (uint32_t j = 0; j < n; j++) {
         uint32_t idx = app_sys_matrix_to_array(i, j, n);
         matrix1[idx] = matrix0[idx];
    }
    /* 2.列清除,只清除当前行下面的列 */
    for (uint32_t i = 0; i < n; i++) {
        double tmp = matrix1[app_sys_matrix_to_array(i, i, n)];
        /* 只要遇到主元为0,终止并退出 */
        if (app_sys_matrix_zero > (tmp < 0 ? -tmp : tmp))
            return 0.0;
        for (uint32_t j = i + 1; j < n; j++) {
                double tmp1 = matrix1[app_sys_matrix_to_array(j, i, n)];
                for (uint32_t k = 0; k < n; k++)
                    matrix1[app_sys_matrix_to_array(j, k, n)] -=
                    matrix1[app_sys_matrix_to_array(i, k, n)] / tmp * tmp1;
        }
    }
    /* 3.计算结果 */
    double ret = 1.0;
    for (uint32_t i = 0; i < n; i++)
        ret *= matrix1[app_sys_matrix_to_array(i, i, n)];
    return ret;
}

/*@brief 矩阵cholesky主分解流程(matrix[n][n], vector[n])
 *@param vector 向量
 *@param matrix 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 成功失败
 */
bool app_sys_matrix_cholesky_dcm(double *matrix, double *vector, uint32_t n)
{
    /* cholesky分解矩阵反演代码,改编自
       http://jean-pierre.moreau.pagesperso-orange.fr/Cplus/choles_cpp.txt */
    
    for (uint32_t i = 0; i < n; i++)
    for (uint32_t j = i; j < n; j++) {
         double sum = matrix[app_sys_matrix_to_array(i, j, n)];
         for (uint32_t k = 0; k < i; k++) {
              uint32_t idx1 = app_sys_matrix_to_array(i, k, n);
              uint32_t idx2 = app_sys_matrix_to_array(j, k, n);
              sum -= matrix[idx1] * matrix[idx2];
         }
         if (i == j) {
             if (sum <= app_sys_matrix_zero)
                 return false;
             vector[i] = sqrt(sum);
         } else {
            matrix[app_sys_matrix_to_array(j, i, n)] = sum / vector[i];
         }
    }
    return true;
}

/*@brief 矩阵cholesky Decompose流程(matrix[n][n], matrix1[n][n])
 *@param vector  向量
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 成功失败
 */
bool app_sys_matrix_cholesky_dc(double *matrix0, double *matrix1, double *vector, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
    for (uint32_t j = 0; j < n; j++) {
         uint32_t idx = app_sys_matrix_to_array(i, j, n);
         matrix0[idx] = matrix1[idx];
    }
    if (!app_sys_matrix_cholesky_dcm(matrix0, vector, n))
         return false;
    for (uint32_t i = 0; i < n; i++) {
         uint32_t idx = app_sys_matrix_to_array(i, i, n);
         matrix0[idx] = vector[i];
         for (uint32_t j = i + 1; j < n; j++) {
              uint32_t idx = app_sys_matrix_to_array(i, j, n);
              matrix0[idx] = 0;
        }
    }
    return true;
}

/*@brief 矩阵cholesky分解反演流程(matrix0[n][n], matrix1[n][n]): matrix0 = matrix1**(-1)
 *@param vector  向量
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 成功失败
 */
bool app_sys_matrix_cholesky_dci(double *matrix0, double *matrix1, double *vector, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
    for (uint32_t j = 0; j < n; j++) {
         uint32_t idx = app_sys_matrix_to_array(i, j, n);
         matrix0[idx] = matrix1[idx];
    }
    if (!app_sys_matrix_cholesky_dcm(matrix0, vector, n))
         return false;
    for (uint32_t i = 0; i < n; i++) {
         uint32_t idx = app_sys_matrix_to_array(i, i, n);
         matrix0[idx] = 1.0 / vector[i];
         for (uint32_t j = i + 1; j < n; j++) {
              double sum = 0.0;
              for (uint32_t k = i; k < j; k++) {
                   uint32_t idx1 = app_sys_matrix_to_array(j, k, n);
                   uint32_t idx2 = app_sys_matrix_to_array(k, i, n);
                   sum -= matrix0[idx1] * matrix0[idx2];
              }
              matrix0[app_sys_matrix_to_array(j, i, n)] = sum / vector[j];
        }
    }
    return true;
}

/*@brief 矩阵cholesky分解反演流程(matrix0[n][n], matrix1[n][n]): matrix0 = matrix1**(-1)
 *@param vector  向量
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@retval 成功失败
 */
bool app_sys_matrix_cholesky_inv(double *matrix0, double *matrix1, double *vector, uint32_t n)
{
    if (!app_sys_matrix_cholesky_dci(matrix0, matrix1, vector, n))
         return false;
    for (uint32_t i = 0; i < n; i++)
    for (uint32_t j = i + 1; j < n; j++)
         matrix0[app_sys_matrix_to_array(i, j, n)] = 0.0;
    for (uint32_t i = 0; i < n; i++) {
         uint32_t idx0  = app_sys_matrix_to_array(i, i, n);
         matrix0[idx0] *= matrix0[idx0];
         for (uint32_t k = i + 1; k < n; k++) {
              uint32_t idx1  = app_sys_matrix_to_array(k, i, n);
              matrix0[idx0] += matrix0[idx1] * matrix0[idx1];
         }
         for (uint32_t j = i + 1; j < n; j++)
         for (uint32_t k = j; k < n; k++) {
              uint32_t idx0 = app_sys_matrix_to_array(i, j, n);
              uint32_t idx1 = app_sys_matrix_to_array(k, i, n);
              uint32_t idx2 = app_sys_matrix_to_array(k, j, n);
              matrix0[idx0] += matrix0[idx1] * matrix0[idx2];
         }
    }
    for (uint32_t i = 0; i < n; i++)
    for (uint32_t j = 0; j < i; j++) {
         uint32_t idx0 = app_sys_matrix_to_array(i, j, n);
         uint32_t idx1 = app_sys_matrix_to_array(j, i, n);
         matrix0[idx0] = matrix0[idx1];
    }
    return true;
}

/*@brief 矩阵cholesky分解计算矩阵的行列式(matrix0[n][n], matrix1[n][n])
 *@param vector  向量
 *@param matrix1 矩阵
 *@param matrix0 矩阵
 *@param n 矩阵一阶二阶值
 *@param ret 行列式结果
 *@retval 成功失败
 */
bool app_sys_matrix_cholesky_det(double *matrix0, double *matrix1, double *vector, uint32_t n, double *ret)
{
    if (!app_sys_matrix_cholesky_dc(matrix0, matrix1, vector, n))
         return false;
    
    double num = 1.0;
    
    for (uint32_t i = 0; i < n; i++)
         num *= matrix0[app_sys_matrix_to_array(i, i, n)];
    
     num = num * num;
    *ret = num;
    return true;
}
