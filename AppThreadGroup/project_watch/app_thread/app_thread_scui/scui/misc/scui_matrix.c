/*实现目标:
 *    矩阵运算
 *备注:
 *    (A1 * A2 ... An)^(-1) = An^(-1) ... A2^(-1) * A1^(-1)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 矩阵输出
 *@param matrix 矩阵实例
 */
void scui_matrix_check(scui_matrix_t *matrix)
{
    SCUI_LOG_DEBUG("matrix:");
    SCUI_LOG_DEBUG("[%f,%f,%f]", matrix->meta[0][0], matrix->meta[0][1], matrix->meta[0][2]);
    SCUI_LOG_DEBUG("[%f,%f,%f]", matrix->meta[1][0], matrix->meta[1][1], matrix->meta[1][2]);
    SCUI_LOG_DEBUG("[%f,%f,%f]", matrix->meta[2][0], matrix->meta[2][1], matrix->meta[2][2]);
}

/*@brief 矩阵生成
 *@param matrix    矩阵实例
 *@param face2_src 平面
 *@param face2_dst 平面
 *@retval 成功或失败
 */
bool scui_matrix_by_face2(scui_matrix_t *matrix, scui_face2_t *face2_src, scui_face2_t *face2_dst)
{
    /* 计算将(xi,yi)映射到(ui,vi)的矩阵变换系数(i=1,2,3,4):
     *
     *      c00*xi + c01*yi + c02
     * ui = ---------------------
     *      c20*xi + c21*yi + c22
     *
     *      c10*xi + c11*yi + c12
     * vi = ---------------------
     *      c20*xi + c21*yi + c22
     *
     * 通过求解线性方程组计算系数:
     * / x0 y0  1  0  0  0 -x0*u0 -y0*u0 \ /c00\ /u0\
     * | x1 y1  1  0  0  0 -x1*u1 -y1*u1 | |c01| |u1|
     * | x2 y2  1  0  0  0 -x2*u2 -y2*u2 | |c02| |u2|
     * | x3 y3  1  0  0  0 -x3*u3 -y3*u3 |.|c10|=|u3|,
     * |  0  0  0 x0 y0  1 -x0*v0 -y0*v0 | |c11| |v0|
     * |  0  0  0 x1 y1  1 -x1*v1 -y1*v1 | |c12| |v1|
     * |  0  0  0 x2 y2  1 -x2*v2 -y2*v2 | |c20| |v2|
     * \  0  0  0 x3 y3  1 -x3*v3 -y3*v3 / \c21/ \v3/
     *
     * where:
     *   cij - 矩阵系数, c22 = 1
     */
    
    /* 四点变换 */
    scui_coord3_t src_x[4] = {face2_src->point2[0].x, face2_src->point2[1].x, face2_src->point2[2].x, face2_src->point2[3].x,};
    scui_coord3_t src_y[4] = {face2_src->point2[0].y, face2_src->point2[1].y, face2_src->point2[2].y, face2_src->point2[3].y,};
    scui_coord3_t dst_x[4] = {face2_dst->point2[0].x, face2_dst->point2[1].x, face2_dst->point2[2].x, face2_dst->point2[3].x,};
    scui_coord3_t dst_y[4] = {face2_dst->point2[0].y, face2_dst->point2[1].y, face2_dst->point2[2].y, face2_dst->point2[3].y,};
    
    const uint8_t M = 8;
    const uint8_t N = 1;
    
    scui_coord3_t matrix_a[8][8] = {
        src_x[0], src_y[0], 1.0f, 0.0f, 0.0f, 0.0f, -src_x[0] * dst_x[0], -src_y[0] * dst_x[0],
        src_x[1], src_y[1], 1.0f, 0.0f, 0.0f, 0.0f, -src_x[1] * dst_x[1], -src_y[1] * dst_x[1],
        src_x[2], src_y[2], 1.0f, 0.0f, 0.0f, 0.0f, -src_x[2] * dst_x[2], -src_y[2] * dst_x[2],
        src_x[3], src_y[3], 1.0f, 0.0f, 0.0f, 0.0f, -src_x[3] * dst_x[3], -src_y[3] * dst_x[3],
        
        0.0f, 0.0f, 0.0f, src_x[0], src_y[0], 1.0f, -src_x[0] * dst_y[0], -src_y[0] * dst_y[0],
        0.0f, 0.0f, 0.0f, src_x[1], src_y[1], 1.0f, -src_x[1] * dst_y[1], -src_y[1] * dst_y[1],
        0.0f, 0.0f, 0.0f, src_x[2], src_y[2], 1.0f, -src_x[2] * dst_y[2], -src_y[2] * dst_y[2],
        0.0f, 0.0f, 0.0f, src_x[3], src_y[3], 1.0f, -src_x[3] * dst_y[3], -src_y[3] * dst_y[3],
    };
    
    scui_coord3_t matrix_b[9] = {    // 齐次矩阵
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
            if (scui_abs(matrix_a[j][i]) >
                scui_abs(matrix_a[k][i]))
                k = j;
        /* 只要遇到主元为0, 终止并退出 */
        if (scui_abs(matrix_a[k][i]) < SCUI_ZERO_VAL_F) {
            // 将矩阵清空, 此时已无用
            for (uint8_t idx_i = 0; idx_i < 3; idx_i++)
            for (uint8_t idx_j = 0; idx_j < 3; idx_j++)
                matrix->meta[idx_i][idx_j] = 0.0f;
            // 这里不必打印, 存在计算不出矩阵的可能性
            // SCUI_LOG_ERROR("gauss transform failure");
            return false;
        }
        
        /* 2.将主元行交换到对角线当前列所在行 */
        /*   只从i列开始以后的列进行行交换, 前面的已经处理完了 */
        if (k != i) {
            for (int8_t j = i; j < M; j++) {
                scui_coord3_t var_t1 = matrix_a[i][j];
                scui_coord3_t var_t2 = matrix_a[k][j];
                matrix_a[i][j] = var_t2;
                matrix_a[k][j] = var_t1;
            }
            for (int8_t j = 0; j < N; j++) {
                scui_coord3_t var_t1 = matrix_b[i + j];
                scui_coord3_t var_t2 = matrix_b[k + j];
                matrix_b[i + j] = var_t2;
                matrix_b[k + j] = var_t1;
            }
        }
        
        scui_coord3_t cof = -1.0f / matrix_a[i][i];
        /* 3.通过反主元系数, 清0从主元开始列下面的所有列 */
        for (int8_t j = i + 1; j < M; j++) {
            scui_coord3_t delta = matrix_a[j][i] * cof;
            
            for (int8_t k = i + 1; k < M; k++)
                matrix_a[j][k] += matrix_a[i][k] * delta;
            
            for (int8_t k = 0; k < N; k++)
                matrix_b[j + k] += matrix_b[i + k] * delta;
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
    for (int8_t j = 0;     j <  N; j++) {
        
        scui_coord3_t delta = matrix_b[i + j];
        /* 从下往上, 不断地迭代减去非主对角线的贡献度 */
        for (int8_t k = i + 1; k < M; k++)
            delta -= matrix_a[i][k] * matrix_b[k + j];
        
        /* 最后只剩下主对角线的贡献度, 计算出M */
        matrix_b[i + j] = delta / matrix_a[i][i];
    }
    
    matrix_b[8] = 1.0f;
    
    /* 得到最终matrix */
    *matrix = *(scui_matrix_t *)matrix_b;
    
    return true;
}

/*@brief 矩阵生成
 *@param matrix    矩阵实例
 *@param face2_src 平面
 *@param face3_dst 平面(空间)
 *@retval 成功或失败
 */
bool scui_matrix_by_face3(scui_matrix_t *matrix, scui_face2_t *face2_src, scui_face3_t *face3_dst)
{
    scui_face2_t face2_dst = {0};
    scui_area3_to_area2(face3_dst, &face2_dst);
    return scui_matrix_by_face2(matrix, face2_src, &face2_dst);
}

/*@brief 矩阵无效检查(奇异矩阵)
 *@param matrix 矩阵实例
 *@retval 是否无效
 */
bool scui_matrix_invalid(scui_matrix_t *matrix)
{
    // 计算矩阵的行列式, 奇异矩阵的行列式值为0
    
    scui_coord3_t det00 = matrix->meta[1][1] * matrix->meta[2][2] -
                  matrix->meta[2][1] * matrix->meta[1][2];
    scui_coord3_t det01 = matrix->meta[2][0] * matrix->meta[1][2] -
                  matrix->meta[1][0] * matrix->meta[2][2];
    scui_coord3_t det02 = matrix->meta[1][0] * matrix->meta[2][1] -
                  matrix->meta[2][0] * matrix->meta[1][1];
    
    scui_coord3_t det = matrix->meta[0][0] * det00 +
                matrix->meta[0][1] * det01 +
                matrix->meta[0][2] * det02;
    
    if (det >= -1E-6 && det <= 1E-6)
        return true;
    else
        return false;
}

/*@brief 矩阵乘法逆(matrix = 1 / matrix)
 *@param matrix 矩阵实例
 */
void scui_matrix_inverse(scui_matrix_t *matrix)
{
    #if 0
    /* 摘抄自lv_matrix:::. */
    
    float det00 = matrix->meta[1][1] * matrix->meta[2][2] - matrix->meta[2][1] * matrix->meta[1][2];
    float det01 = matrix->meta[2][0] * matrix->meta[1][2] - matrix->meta[1][0] * matrix->meta[2][2];
    float det02 = matrix->meta[1][0] * matrix->meta[2][1] - matrix->meta[2][0] * matrix->meta[1][1];
    
    float delta = matrix->meta[0][0] * det00 + matrix->meta[0][1] * det01 + matrix->meta[0][2] * det02;
    // 这里有一个返回值的,这里改成断言
    SCUI_ASSERT(scui_abs(delta) > SCUI_ZERO_VAL_F);
    delta = 1.0f / delta;
    
    scui_matrix_t matrix_t;
    matrix_t.meta[0][0] = delta * det00;
    matrix_t.meta[0][1] = delta * (matrix->meta[2][1] * matrix->meta[0][2] - matrix->meta[0][1] * matrix->meta[2][2]);
    matrix_t.meta[0][2] = delta * (matrix->meta[0][1] * matrix->meta[1][2] - matrix->meta[1][1] * matrix->meta[0][2]);
    matrix_t.meta[1][0] = delta * det01;
    matrix_t.meta[1][1] = delta * (matrix->meta[0][0] * matrix->meta[2][2] - matrix->meta[2][0] * matrix->meta[0][2]);
    matrix_t.meta[1][2] = delta * (matrix->meta[1][0] * matrix->meta[0][2] - matrix->meta[0][0] * matrix->meta[1][2]);
    
    if (scui_dist(matrix->meta[2][0], 0.0f) <= SCUI_ZERO_VAL_F &&
        scui_dist(matrix->meta[2][1], 0.0f) <= SCUI_ZERO_VAL_F &&
        scui_dist(matrix->meta[2][2], 1.0f) <= SCUI_ZERO_VAL_F) {
    matrix_t.meta[2][0] = 0.0f;
    matrix_t.meta[2][1] = 0.0f;
    matrix_t.meta[2][2] = 1.0f;
    } else {
    matrix_t.meta[2][0] = delta * det02;
    matrix_t.meta[2][1] = delta * (matrix->meta[2][0] * matrix->meta[0][1] - matrix->meta[0][0] * matrix->meta[2][1]);
    matrix_t.meta[2][2] = delta * (matrix->meta[0][0] * matrix->meta[1][1] - matrix->meta[1][0] * matrix->meta[0][1]);
    }
   *matrix = matrix_t;
    
    #else
    scui_coord3_t detal = matrix->meta[0][0] * matrix->meta[1][1] * matrix->meta[2][2] +
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
    #endif
}

/*@brief 矩阵乘法(matrix *= matrix1)
 *@param matrix  矩阵实例
 *@param matrix1 矩阵实例
 */
void scui_matrix_multiply(scui_matrix_t *matrix, scui_matrix_t *matrix1)
{
    scui_matrix_t matrix_t;
    
    for (uint8_t row = 0; row < 3; row++)
    for (uint8_t col = 0; col < 3; col++) {
        scui_coord3_t m_v0 = matrix->meta[row][0] * matrix1->meta[0][col];
        scui_coord3_t m_v1 = matrix->meta[row][1] * matrix1->meta[1][col];
        scui_coord3_t m_v2 = matrix->meta[row][2] * matrix1->meta[2][col];
        
        matrix_t.meta[row][col] = m_v0 + m_v1 + m_v2;
    }
    
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
 *@param offset 偏移值
 */
void scui_matrix_translate(scui_matrix_t *matrix, scui_point2_t *offset)
{
    // 平移是非线性变换
    // 3D空间下平移:不支持
    // 2D平面上平移:支持
    
    scui_matrix_t matrix_t = {
        .meta = {
            {1.0f, 0.0f, offset->x},
            {0.0f, 1.0f, offset->y},
            {0.0f, 0.0f,      1.0f},
        },
    };
    scui_matrix_multiply(matrix, &matrix_t);
}

/*@brief 矩阵透视
 *@param matrix 矩阵实例
 *@param view   透视点
 */
void scui_matrix_perspective(scui_matrix_t *matrix, scui_point2_t *view)
{
    scui_matrix_t matrix_t = {
        .meta = {
            {   1.0f,    0.0f, 0.0f},
            {   0.0f,    1.0f, 0.0f},
            {view->x, view->y, 1.0f},
        },
    };
    scui_matrix_multiply(matrix, &matrix_t);
}

/*@brief 矩阵倾斜(错切)
 *@param matrix 矩阵实例
 *@param skew   倾斜系数
 */
void scui_matrix_skew(scui_matrix_t *matrix, scui_point2_t *skew)
{
    scui_coord3_t tan_x = scui_tan(SCUI_RAD_BY_A(skew->x));
    scui_coord3_t tan_y = scui_tan(SCUI_RAD_BY_A(skew->y));
    
    scui_matrix_t matrix_t = {
        .meta = {
            { 1.0f, tan_x, 0.0f},
            {tan_y,  1.0f, 0.0f},
            { 0.0f,  0.0f, 1.0f},
        },
    };
    scui_matrix_multiply(matrix, &matrix_t);
}

/*@brief 矩阵缩放
 *@param matrix 矩阵实例
 *@param offset 缩放比(1.0为基准线)
 */
void scui_matrix_scale(scui_matrix_t *matrix, scui_point2_t *scale)
{
    scui_matrix_t matrix_t = {
        .meta = {
            {scale->x,     0.0f, 0.0f},
            {    0.0f, scale->y, 0.0f},
            {    0.0f,     0.0f, 1.0f},
        },
    };
    scui_matrix_multiply(matrix, &matrix_t);
}

/*@brief 矩阵旋转
 *@param matrix 矩阵实例
 *@param chord  旋转角度弦(y:sin; x:cos)
 *@param axis   旋转轴向(0x00:Z轴向;0x01:X轴向;0x02:Y轴向;)
 */
void scui_matrix_rotate_c(scui_matrix_t *matrix, scui_point2_t *chord, uint8_t axis)
{
    scui_coord3_t sin_a = chord->y;     // y: sin(angle)
    scui_coord3_t cos_a = chord->x;     // x: cos(angle)
    scui_matrix_t matrix_t = {0};
    
    /*X轴向:
     *     [  1    0        0     ]
     *     [  0  cos(rx) -sin(rx) ]
     *     [  0  sin(rx)  cos(rx) ]
     *Y轴向:
     *     [  cos(ry)  0  sin(ry) ]
     *     [    0      1    0     ]
     *     [ -sin(ry)  0  cos(ry) ]
     *Z轴向:
     *     [  cos(rz) -sin(rz)  0 ]
     *     [  sin(rz)  cos(rz)  0 ]
     *     [    0        0      1 ]
     */
    
    switch (axis) {
    case 0x00: {      /* Z轴向 */
        matrix_t = (scui_matrix_t){
            .meta = {
                {cos_a, -sin_a, 0.0f},
                {sin_a,  cos_a, 0.0f},
                { 0.0f,   0.0f, 1.0f},
            },
        };
        break;
    }
    case 0x01: {    /* X轴向 */
        matrix_t = (scui_matrix_t){
            .meta = {
                { 1.0f,  0.0f,   0.0f},
                { 0.0f, cos_a, -sin_a},
                { 0.0f, sin_a,  cos_a},
            },
        };
        break;
    }
    case 0x02: {    /* Y轴向 */
        matrix_t = (scui_matrix_t){
            .meta = {
                { cos_a, 0.0f, sin_a},
                {  0.0f, 1.0f,  0.0f},
                {-sin_a, 0.0f, cos_a},
            },
        };
        break;
    }
    default:
        SCUI_LOG_ERROR("unknown axis:%x", axis);
        break;
    }
    
    scui_matrix_multiply(matrix, &matrix_t);
}

/*@brief 矩阵旋转
 *@param matrix 矩阵实例
 *@param angle  旋转角度
 *@param axis   旋转轴向(0x00:Z轴向;0x01:X轴向;0x02:Y轴向;)
 */
void scui_matrix_rotate_a(scui_matrix_t *matrix, scui_coord3_t angle, uint8_t axis)
{
    scui_coord3_t sin_a = scui_sin(SCUI_RAD_BY_A(angle));
    scui_coord3_t cos_a = scui_cos(SCUI_RAD_BY_A(angle));
    scui_point2_t chord = {.y = sin_a, .x = cos_a,};
    scui_matrix_rotate_c(matrix, &chord, axis);
}

/*@brief 矩阵旋转
 *@param matrix 矩阵实例
 *@param angle  旋转角度弦[x, y, z](y:sin; x:cos)
 *@param axis   旋转轴向(0x00:ZYX;0x01:ZXY;0x02:YZX;0x03:YXZ;0x04:XYZ;0x05:XZY;)
 */
void scui_matrix_rotate_c3(scui_matrix_t *matrix, scui_point2_t chord[3], uint8_t axis)
{
    scui_coord3_t sin_rx = chord[0].y;
    scui_coord3_t cos_rx = chord[0].x;
    scui_coord3_t sin_ry = chord[1].y;
    scui_coord3_t cos_ry = chord[1].x;
    scui_coord3_t sin_rz = chord[2].y;
    scui_coord3_t cos_rz = chord[2].x;
    
    switch(axis) {
    case 0x00:
        /* rz * ry * rx */
        matrix->meta[0][0] = cos_rz * cos_ry;
        matrix->meta[0][1] = cos_rz * sin_ry * sin_rx - sin_rz * cos_rx;
        matrix->meta[0][2] = cos_rz * sin_ry * cos_rx + sin_rz * sin_rx;
        matrix->meta[1][0] = sin_rz * cos_ry;
        matrix->meta[1][1] = sin_rz * sin_ry * sin_rx + cos_rz * cos_rx;
        matrix->meta[1][2] = sin_rz * sin_ry * cos_rx - cos_rz * sin_rx;
        matrix->meta[2][0] = -sin_ry;
        matrix->meta[2][1] = cos_ry * sin_rx;
        matrix->meta[2][2] = cos_ry * cos_rx;
        break;
    case 0x01:
        /* rz * rx * ry */
        matrix->meta[0][0] = cos_rz * cos_ry - sin_rz * sin_rx * sin_ry;
        matrix->meta[0][1] = -sin_rz * cos_rx;
        matrix->meta[0][2] = cos_rz * sin_ry + sin_rz * sin_rx * cos_ry;
        matrix->meta[1][0] = sin_rz * cos_ry + cos_rz * sin_rx * sin_ry;
        matrix->meta[1][1] = cos_rz * cos_rx;
        matrix->meta[1][2] = sin_rz * sin_ry - cos_rz * sin_rx * cos_ry;
        matrix->meta[2][0] = -cos_rx * sin_ry;
        matrix->meta[2][1] = sin_rx;
        matrix->meta[2][2] = cos_rx * cos_ry;
        break;
    case 0x02:
        /* ry * rz * rx */
        matrix->meta[0][0] = cos_ry * cos_rz;
        matrix->meta[0][1] = -cos_ry * sin_rz * cos_rx + sin_ry * sin_rx;
        matrix->meta[0][2] = cos_ry * sin_rz * sin_rx + sin_ry * cos_rx;
        matrix->meta[1][0] = sin_rz;
        matrix->meta[1][1] = cos_rz * cos_rx;
        matrix->meta[1][2] = -cos_rz * sin_rx;
        matrix->meta[2][0] = -sin_ry * cos_rz;
        matrix->meta[2][1] = sin_ry * sin_rz * cos_rx + cos_ry * sin_rx;
        matrix->meta[2][2] = -sin_ry * sin_rz * sin_rx + cos_ry * cos_rx;
        break;
    case 0x03:
        /* ry * rx * rz */
        matrix->meta[0][0] = cos_ry * cos_rz + sin_ry * sin_rx * sin_rz;
        matrix->meta[0][1] = -cos_ry * sin_rz + sin_ry * sin_rx * cos_rz;
        matrix->meta[0][2] = sin_ry * cos_rx;
        matrix->meta[1][0] = cos_rx * sin_rz;
        matrix->meta[1][1] = cos_rx * cos_rz;
        matrix->meta[1][2] = -sin_rx;
        matrix->meta[2][0] = -sin_ry * cos_rz + cos_ry * sin_rx * sin_rz;
        matrix->meta[2][1] = sin_ry * sin_rz + cos_ry * sin_rx * cos_rz;
        matrix->meta[2][2] = cos_ry * cos_rx;
        break;
    case 0x04:
        /* rx * ry * rz */
        matrix->meta[0][0] = cos_ry * cos_rz;
        matrix->meta[0][1] = -cos_ry * sin_rz;
        matrix->meta[0][2] = sin_ry;
        matrix->meta[1][0] = cos_rx * sin_rz + sin_rx * sin_ry * cos_rz;
        matrix->meta[1][1] = cos_rx * cos_rz - sin_rx * sin_ry * sin_rz;
        matrix->meta[1][2] = -sin_rx * cos_ry;
        matrix->meta[2][0] = sin_rx * sin_rz - cos_rx * sin_ry * cos_rz;
        matrix->meta[2][1] = sin_rx * cos_rz + cos_rx * sin_ry * sin_rz;
        matrix->meta[2][2] = cos_rx * cos_ry;
        break;
    case 0x05:
        /* rx * rz * ry */
        matrix->meta[0][0] = cos_rz * cos_ry;
        matrix->meta[0][1] = -sin_rz;
        matrix->meta[0][2] = cos_rz * sin_ry;
        matrix->meta[1][0] = cos_rx * sin_rz * cos_ry + sin_rx * sin_ry;
        matrix->meta[1][1] = cos_rx * cos_rz;
        matrix->meta[1][2] = cos_rx * sin_rz * sin_ry - sin_rx * cos_ry;
        matrix->meta[2][0] = sin_rx * sin_rz * cos_ry - cos_rx * sin_ry;
        matrix->meta[2][1] = sin_rx * cos_rz;
        matrix->meta[2][2] = sin_rx * sin_rz * sin_ry + cos_rx * cos_ry;
        break;
    default:
        SCUI_LOG_ERROR("unknown axis:%x", axis);
    }
}

/*@brief 矩阵旋转
 *@param matrix 矩阵实例
 *@param angle  旋转角度
 *@param axis   旋转轴向(0x00:ZYX;0x01:ZXY;0x02:YZX;0x03:YXZ;0x04:XYZ;0x05:XZY;)
 */
void scui_matrix_rotate_a3(scui_matrix_t *matrix, scui_point3_t *angle, uint8_t axis)
{
    scui_point2_t chord[3] = {0};
    chord[0].y = scui_sin(SCUI_RAD_BY_A(angle->x));
    chord[0].x = scui_cos(SCUI_RAD_BY_A(angle->x));
    chord[1].y = scui_sin(SCUI_RAD_BY_A(angle->y));
    chord[1].x = scui_cos(SCUI_RAD_BY_A(angle->y));
    chord[2].y = scui_sin(SCUI_RAD_BY_A(angle->z));
    chord[2].x = scui_cos(SCUI_RAD_BY_A(angle->z));
    scui_matrix_rotate_c3(matrix, chord, axis);
}

/*@brief 矩阵仿射(blit)
 *@param matrix 矩阵实例
 *@param size   尺寸(src)
 *@param face   面(dst)
 */
void scui_matrix_affine_blit(scui_matrix_t *matrix, scui_size2_t *size, scui_face3_t *face)
{
    /* 仿射参考来源: From 6.5.2 of OpenVG1.1 Spec */
    
    scui_coord3_t sx  = (face->point3[1].x - face->point3[0].x) / size->w;
    scui_coord3_t sy  = (face->point3[3].y - face->point3[0].y) / size->h;
    scui_coord3_t shx = (face->point3[3].x - face->point3[0].x) / size->h;
    scui_coord3_t shy = (face->point3[1].y - face->point3[0].y) / size->w;
    scui_coord3_t tx  = (face->point3[0].x);
    scui_coord3_t ty  = (face->point3[0].y);
    
    scui_matrix_t matrix_t = {
        .meta = {
            {  sx,  shx,   tx},
            { shy,   sy,   ty},
            {0.0f, 0.0f, 1.0f},
        },
    };
    *matrix = matrix_t;
}

/*@brief 矩阵投影(blit)
 *@param matrix 矩阵实例
 *@param size   尺寸(src)
 *@param face   面(dst)
 *@retval 成功或失败
 */
bool scui_matrix_perspective_blit(scui_matrix_t *matrix, scui_size2_t *size, scui_face3_t *face)
{
    scui_face2_t face2_src = {.point2 = {{0,0},{size->w,0},{size->w,size->h},{0,size->h},}};
    return scui_matrix_by_face3(matrix, &face2_src, face);
}

/*@brief 矩阵投影(blit)
 *@param matrix 矩阵实例
 *@param size   尺寸(src)
 *@param face   面(dst)
 *@param view   视点
 *@retval 成功或失败
 */
bool scui_matrix_perspective_view_blit(scui_matrix_t *matrix, scui_size2_t *size, scui_face3_t *face, scui_view3_t *view)
{
    scui_face3_t face3_view = *face;
    scui_area3_perspective(&face3_view, view);
    return scui_matrix_perspective_blit(matrix, size, &face3_view);
}
