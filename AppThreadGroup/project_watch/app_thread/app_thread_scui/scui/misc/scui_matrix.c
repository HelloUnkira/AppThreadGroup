/*实现目标:
 *    矩阵运算
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 矩阵坐标转换(pox->point)
 *@param pox    矩阵坐标
 *@param point  坐标
 */
void scui_matrix_pox_to_point(scui_matrix_pox_t *pox, scui_point_t *point)
{
    point->x = (scui_coord_t)(pox->x / pox->z);
    point->y = (scui_coord_t)(pox->y / pox->z);
}

/*@brief 矩阵坐标转换(point->pox)
 *@param pox    矩阵坐标
 *@param point  坐标
 */
void scui_matrix_pox_by_point(scui_matrix_pox_t *pox, scui_point_t *point)
{
    pox->x = (float)point->x;
    pox->y = (float)point->y;
    pox->z = 1.0f;
}

/*@brief 矩阵坐标变换
 *@param matrix 矩阵实例
 *@param pox    矩阵坐标
 */
void scui_matrix_pox_transfer(scui_matrix_t *matrix, scui_matrix_pox_t *pox)
{
    scui_matrix_pox_t pox_t;
    
    for (uint8_t row = 0; row < 3; row++)
        pox_t.meta[row] = matrix->meta[row][0] * pox->x +
                          matrix->meta[row][1] * pox->y +
                          matrix->meta[row][2] * pox->z;
    
    *pox = pox_t;
}

/*@brief 矩阵区域变换
 *@param matrix 矩阵实例
 *@param src    矩阵区域
 *@param dst    矩阵区域
 */
void scui_matrix_pox4_transfer(scui_matrix_t *matrix, scui_matrix_pox4_t *src, scui_matrix_pox4_t *dst)
{
    scui_matrix_pox_t pox = {0};
    
    pox = src->pox[0];
    scui_matrix_pox_transfer(matrix, &pox);
    dst->pox[0] = pox;
    
    pox = src->pox[1];
    scui_matrix_pox_transfer(matrix, &pox);
    dst->pox[1] = pox;
    
    pox = src->pox[2];
    scui_matrix_pox_transfer(matrix, &pox);
    dst->pox[2] = pox;
    
    pox = src->pox[3];
    scui_matrix_pox_transfer(matrix, &pox);
    dst->pox[3] = pox;
}

/*@brief 矩阵生成
 *@param matrix 矩阵实例
 *@param src    平面区域
 *@param dst    平面区域
 *@retval 成功或失败
 */
bool scui_matrix_by_area(scui_matrix_t *matrix, scui_area_t *src, scui_area_t *dst)
{
    scui_area_m_to_s(src);
    scui_area_m_to_s(dst);
    
    /* 时钟旋转四点 */
    float src_x[4] = {src->x1, src->x2, src->x2, src->x1,};
    float src_y[4] = {src->y1, src->y1, src->y2, src->y2,};
    float dst_x[4] = {dst->x1, dst->x2, dst->x2, dst->x1,};
    float dst_y[4] = {dst->y1, dst->y1, dst->y2, dst->y2,};
    
    scui_area_s_to_m(src);
    scui_area_s_to_m(dst);
    
    const float eps = 1.1920929e-7f;
    
    const uint8_t M = 8, step_a = 8;
    const uint8_t N = 1, step_b = 1;
    
    float matrix_a[8 * 8] = {
        src_x[0], src_x[0], 1.0f, 0.0f, 0.0f, 0.0f, -src_x[0] * dst_x[0], -src_y[0] * dst_x[0],
        src_x[1], src_x[1], 1.0f, 0.0f, 0.0f, 0.0f, -src_x[1] * dst_x[1], -src_y[1] * dst_x[1],
        src_x[2], src_x[2], 1.0f, 0.0f, 0.0f, 0.0f, -src_x[2] * dst_x[2], -src_y[2] * dst_x[2],
        src_x[3], src_x[3], 1.0f, 0.0f, 0.0f, 0.0f, -src_x[3] * dst_x[3], -src_y[3] * dst_x[3],
        
        0.0f, 0.0f, 0.0f, src_x[0], src_x[0], 1.0f, -src_x[0] * dst_y[0], -src_y[0] * dst_y[0],
        0.0f, 0.0f, 0.0f, src_x[1], src_x[1], 1.0f, -src_x[1] * dst_y[1], -src_y[1] * dst_y[1],
        0.0f, 0.0f, 0.0f, src_x[2], src_x[2], 1.0f, -src_x[2] * dst_y[2], -src_y[2] * dst_y[2],
        0.0f, 0.0f, 0.0f, src_x[3], src_x[3], 1.0f, -src_x[3] * dst_y[3], -src_y[3] * dst_y[3],
    };
    
    float matrix_b[9] = {    // 齐次矩阵
        dst_x[0], dst_x[1], dst_x[2], dst_x[3],
        dst_y[0], dst_y[1], dst_y[2], dst_y[3],
    };
    
    /* 进行高斯消元:生成上三角矩阵 */
    /* 注意:因为我们并不需要真正意义上将A转为上三角矩阵 */
    /*      所以我们只更新主对角线右边的值,在迭代中默认左边值已经归零(实际不为零) */
    
    for (int8_t i = 0; i < M; i++) {
        /* 1.全选主元, 选择该列的每一个元素, 选最大的值所在行 */
        int8_t k = i;
        
        for (int8_t j = i + 1; j < M; j++)
            if (scui_abs(matrix_a[j * step_a + i]) >
                scui_abs(matrix_a[k * step_a + i]))
                k = j;
        /* 只要遇到主元为0, 终止并退出 */
        if (scui_abs(matrix_a[k * step_a + i] < eps))
            return false;
        
        /* 2.将主元行交换到对角线当前列所在行 */
        /*   只从i列开始以后的列进行行交换, 前面的已经处理完了 */
        if (k != i) {
            for (int8_t j = i; j < M; j++) {
                float var_t1 = matrix_a[i * step_a + j];
                float var_t2 = matrix_a[k * step_a + j];
                matrix_a[i * step_a + j] = var_t2;
                matrix_a[k * step_a + j] = var_t1;
            }
            for (int8_t j = 0; j < N; j++) {
                float var_t1 = matrix_b[i * step_b + j];
                float var_t2 = matrix_b[k * step_b + j];
                matrix_b[i * step_b + j] = var_t2;
                matrix_b[k * step_b + j] = var_t1;
            }
        }
        
        float cof = -1 / matrix_a[i * step_a + i];
        /* 3.通过反主元系数, 清0从主元开始列下面的所有列 */
        for (int8_t j = i + 1; j < M; j++) {
            float delta = matrix_a[j * step_a + i] * cof;
            
            for (int8_t k = i + 1; k < M; k++)
                matrix_a[j * step_a + k] += matrix_a[i * step_a + k];
            
            for (int8_t k = 0; k < N; k++)
                matrix_b[j * step_b + k] += matrix_b[i * step_b + k];
        }
    }
    
    /* 进行回代:将上三角矩阵消为单位矩阵 */
    /* 注意:因为我们并不需要使用A,所以这里跳过实际更新A的值 */
    /*      只需要使用主对角线上的值做最后一次调整 */
    
    /* 注意:回代计算的过程中,不断地对B扣除A的之前元素的贡献度 */
    /*      最后计算出的元素贡献度就是主对角线产生的贡献度 */
    /*      这个过程像是解方程一样,也是消为单位矩阵的过程 */
    
    /* [a b c d e f g h] [v_a] ==> v_a = hM(h) + gM(g) + fM(f) + eM(e) + dM(d) + cM(c) + bM(b) + aM(a)
     * [x b c d e f g h] [v_b] ==> v_b = hM(h) + gM(g) + fM(f) + eM(e) + dM(d) + cM(c) + bM(b)
     * [x x c d e f g h] [v_c] ==> v_c = hM(h) + gM(g) + fM(f) + eM(e) + dM(d) + cM(c)
     * [x x x d e f g h] [v_d] ==> v_d = hM(h) + gM(g) + fM(f) + eM(e) + dM(d)
     * [x x x x e f g h] [v_e] ==> v_e = hM(h) + gM(g) + fM(f) + eM(e)
     * [x x x x x f g h] [v_f] ==> v_f = hM(h) + gM(g) + fM(f)
     * [x x x x x x g h] [v_g] ==> v_g = hM(h) + gM(g)
     * [x x x x x x x h] [v_h] ==> v_h = hM(h)
     */
    
    /* 通过不断的逆向回代v_x到方程组里面依次解出M(h)~M(a) */
    /* 每一个方程的参数abcdefgh都是不一样的数值, 所以每一步都要迭代计算 */
    
    for (int8_t i = M - 1; i >= 0; i--)
    for (int8_t j = 0;     j < N;  j++) {
        
        float delta = matrix_b[i * step_b + j];
        /* 从下往上, 不断地迭代减去非主对角线的贡献度 */
        for (int8_t k = i + 1; k < M; k++)
            delta -= matrix_a[i * step_a + k] * matrix_b[k * step_b + j];
        
        /* 最后只剩下主对角线的贡献度, 计算出M */
        matrix_b[i * step_b + j] = delta / matrix_a[i * step_a + i];
    }
    
    matrix_b[8] = 1.0f;
    
    /* 得到最终matrix */
    *matrix = *(scui_matrix_t *)matrix_b;
    
    return true;
}

/*@brief 矩阵乘法可逆检查
 *@param matrix 矩阵实例
 */
bool scui_matrix_inverse_check(scui_matrix_t *matrix)
{
    float det00 = matrix->meta[1][1] * matrix->meta[2][2] -
                  matrix->meta[2][1] * matrix->meta[1][2];
    float det01 = matrix->meta[2][0] * matrix->meta[1][2] -
                  matrix->meta[1][0] * matrix->meta[2][2];
    float det02 = matrix->meta[1][0] * matrix->meta[2][1] -
                  matrix->meta[2][0] * matrix->meta[1][1];
    
    float det = matrix->meta[0][0] * det00 +
                matrix->meta[0][1] * det01 +
                matrix->meta[0][2] * det02;
    
    if (det >= -1E-6 && det <= 1E-6)
        return false;
    else
        return false;
}

/*@brief 矩阵乘法逆(matrix = 1 / matrix)
 *@param matrix 矩阵实例
 */
void scui_matrix_inverse(scui_matrix_t *matrix)
{
    float detal = matrix->meta[0][0] * matrix->meta[1][1] * matrix->meta[2][2] +
                  matrix->meta[0][1] * matrix->meta[1][2] * matrix->meta[2][0] +
                  matrix->meta[0][2] * matrix->meta[1][0] * matrix->meta[2][1] -
                  matrix->meta[0][0] * matrix->meta[1][2] * matrix->meta[2][1] -
                  matrix->meta[0][1] * matrix->meta[1][0] * matrix->meta[2][2] -
                  matrix->meta[0][2] * matrix->meta[1][1] * matrix->meta[2][0];

    scui_matrix_t matrix_t;
    
    matrix_t.meta[0][0] = +(matrix->meta[1][1] * matrix->meta[2][2] -
                            matrix->meta[1][2] * matrix->meta[2][1]) / detal;
    matrix_t.meta[1][0] = -(matrix->meta[2][2] * matrix->meta[1][0] -
                            matrix->meta[1][2] * matrix->meta[2][0]) / detal;
    matrix_t.meta[2][0] = +(matrix->meta[1][0] * matrix->meta[2][1] -
                            matrix->meta[2][0] * matrix->meta[1][1]) / detal;
    
    matrix_t.meta[0][1] = -(matrix->meta[0][1] * matrix->meta[2][2] -
                            matrix->meta[0][2] * matrix->meta[2][1]) / detal;
    matrix_t.meta[1][1] = +(matrix->meta[2][2] * matrix->meta[0][0] -
                            matrix->meta[2][0] * matrix->meta[0][2]) / detal;
    matrix_t.meta[2][1] = -(matrix->meta[0][0] * matrix->meta[2][1] -
                            matrix->meta[2][0] * matrix->meta[0][1]) / detal;
    
    matrix_t.meta[0][2] = +(matrix->meta[0][1] * matrix->meta[1][2] -
                            matrix->meta[1][1] * matrix->meta[0][2]) / detal;
    matrix_t.meta[1][2] = -(matrix->meta[0][0] * matrix->meta[1][2] -
                            matrix->meta[0][2] * matrix->meta[1][0]) / detal;
    matrix_t.meta[2][2] = +(matrix->meta[0][0] * matrix->meta[1][1] -
                            matrix->meta[0][1] * matrix->meta[1][0]) / detal;
    
   *matrix = matrix_t;
}

/*@brief 矩阵乘法(matrix *= matrix1)
 *@param matrix  矩阵实例
 *@param matrix1 矩阵实例
 */
void scui_matrix_multiply(scui_matrix_t *matrix, scui_matrix_t *matrix1)
{
    scui_matrix_t matrix_t;
    
    for (uint8_t row = 0; row < 3; row++)
    for (uint8_t col = 0; col < 3; col++)
        matrix_t.meta[row][col] = 
                 matrix->meta[row][0] * matrix1->meta[0][col] +
                 matrix->meta[row][1] * matrix1->meta[1][col] +
                 matrix->meta[row][2] * matrix1->meta[2][col];
    
   *matrix = matrix_t;
}

/*@brief 矩阵单位化(单位矩阵)
 *@param matrix 矩阵实例
 */
void scui_matrix_identity(scui_matrix_t *matrix)
{
    scui_matrix_t matrix_t = {
        .meta = {
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
        },
    };
    *matrix = matrix_t;
}

/*@brief 矩阵平移
 *@param matrix 矩阵实例
 *@param ofs_x  水平偏移值
 *@param ofs_y  垂直偏移值
 */
void scui_matrix_translate(scui_matrix_t *matrix, float ofs_x, float ofs_y)
{
    scui_matrix_t matrix_t = {
        .meta = {
            {1.0f, 0.0f, ofs_x},
            {0.0f, 1.0f, ofs_y},
            {0.0f, 0.0f,  1.0f},
        },
    };
    scui_matrix_multiply(matrix, &matrix_t);
}

/*@brief 矩阵缩放
 *@param matrix 矩阵实例
 *@param ofs_x  水平缩放比(1.0为基准线)
 *@param ofs_y  垂直缩放比(1.0为基准线)
 */
void scui_matrix_scale(scui_matrix_t *matrix, float ofs_x, float ofs_y)
{
    scui_matrix_t matrix_t = {
        .meta = {
            {ofs_x,  0.0f, 0.0f},
            { 0.0f, ofs_y, 0.0f},
            { 0.0f,  0.0f, 1.0f},
        },
    };
    scui_matrix_multiply(matrix, &matrix_t);
}

/*@brief 矩阵旋转
 *@param matrix 矩阵实例
 *@param angle  旋转角度
 */
void scui_matrix_rotate(scui_matrix_t *matrix, int32_t angle)
{
    float sin_a = scui_sin4096(angle) / 4096.0f;
    float cos_a = scui_cos4096(angle) / 4096.0f;
    
    scui_matrix_t matrix_t = {
        .meta = {
            {cos_a, -sin_a, 0.0f},
            {sin_a,  cos_a, 0.0f},
            { 0.0f,   0.0f, 1.0f},
        },
    };
    scui_matrix_multiply(matrix, &matrix_t);
}

/*@brief 矩阵透视
 *@param matrix 矩阵实例
 *@param pos_x  水平透视点
 *@param pos_y  垂直透视点
 */
void scui_matrix_perspective(scui_matrix_t *matrix, float pos_x, float pos_y)
{
    scui_matrix_t matrix_t = {
        .meta = {
            { 1.0f,  0.0f, 0.0f},
            { 0.0f,  1.0f, 0.0f},
            {pos_x, pos_y, 1.0f},
        },
    };
    scui_matrix_multiply(matrix, &matrix_t);
}
