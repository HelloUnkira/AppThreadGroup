/*实现目标:
 *    通用矩阵功能测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 通用矩阵功能测试
 */
void app_sys_app_sys_matrix_test(void)
{
    {
        uint32_t n = 5;
        double matrix0[5][5] = {0};
        double matrix1[5][5] = {0};
        double matrix2[5][10] = {0};
        double matrix3[5][5] = {0};
        
        for (uint32_t i = 0; i < n; i++)
        for (uint32_t j = 0; j < n; j++)
            matrix1[i][j] = rand() % 100;
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        bool ret1 = app_sys_matrix_inv((double *)matrix0, (double *)matrix1, (double *)matrix2, n);
        APP_SYS_LOG_INFO("app_sys_matrix_inv:%d", ret1);
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        APP_SYS_LOG_INFO("matrix1:");
        for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = 0; j < n; j++)
            APP_SYS_LOG_INFO_RAW("%lf\t", matrix1[i][j]);
            APP_SYS_LOG_INFO_RAW(app_sys_msg_line());
        }
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        APP_SYS_LOG_INFO("matrix2:");
        for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = 0; j < n; j++)
            APP_SYS_LOG_INFO_RAW("%lf\t", matrix2[i][j]);
            APP_SYS_LOG_INFO_RAW(app_sys_msg_line());
        }
        APP_SYS_LOG_INFO_RAW(app_sys_msg_line());
        for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = n; j < n * 2; j++)
            APP_SYS_LOG_INFO_RAW("%lf\t", matrix2[i][j]);
            APP_SYS_LOG_INFO_RAW(app_sys_msg_line());
        }
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        APP_SYS_LOG_INFO("matrix0:");
        for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = 0; j < n; j++)
            APP_SYS_LOG_INFO_RAW("%lf\t", matrix0[i][j]);
            APP_SYS_LOG_INFO_RAW(app_sys_msg_line());
        }
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        app_sys_matrix_star((double *)matrix3, (double *)matrix1, (double *)matrix0, n, n, n);
        APP_SYS_LOG_INFO("app_sys_matrix_star:");
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        APP_SYS_LOG_INFO("matrix3:");
        for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = 0; j < n; j++)
            APP_SYS_LOG_INFO_RAW("%lf\t", matrix3[i][j]);
            APP_SYS_LOG_INFO_RAW(app_sys_msg_line());
        }
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        double ret0 = app_sys_matrix_val((double *)matrix1, (double *)matrix3, n);
        APP_SYS_LOG_INFO("app_sys_matrix_val:%lf", ret0);
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        APP_SYS_LOG_INFO("matrix3:");
        for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = 0; j < n; j++)
            APP_SYS_LOG_INFO_RAW("%lf\t", matrix3[i][j]);
            APP_SYS_LOG_INFO_RAW(app_sys_msg_line());
        }
        APP_SYS_LOG_INFO("------------------------------------------------------------");
    }
    
    {
        uint32_t n = 4;
        double matrix0[4][4] = {0};
        double matrix1[4][4] = {0};
        double matrix3[4][4] = {0};
        double vector[4] = {0};
        
        matrix1[0][0]= 5;
        matrix1[1][0]=-1; matrix1[1][1]= 5;
        matrix1[2][0]=-1; matrix1[2][1]=-1; matrix1[2][2]= 5;
        matrix1[3][0]=-1; matrix1[3][1]=-1; matrix1[3][2]=-1; matrix1[3][3]= 5;
        
        for (uint32_t i = 0; i < n; i++)
        for (uint32_t j = 0; j < n; j++)
             matrix1[i][j]=matrix1[j][i];
        
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        bool ret2 = app_sys_matrix_cholesky_inv((double *)matrix0, (double *)matrix1, vector, n);
        APP_SYS_LOG_INFO("app_sys_matrix_cholesky_inv:%d", ret2);
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        APP_SYS_LOG_INFO("matrix1:");
        for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = 0; j < n; j++)
            APP_SYS_LOG_INFO_RAW("%lf\t", matrix1[i][j]);
            APP_SYS_LOG_INFO_RAW(app_sys_msg_line());
        }
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        APP_SYS_LOG_INFO("matrix0:");
        for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = 0; j < n; j++)
            APP_SYS_LOG_INFO_RAW("%lf\t", matrix0[i][j]);
            APP_SYS_LOG_INFO_RAW(app_sys_msg_line());
        }
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        app_sys_matrix_star((double *)matrix3, (double *)matrix1, (double *)matrix0, n, n, n);
        APP_SYS_LOG_INFO("app_sys_matrix_Star:");
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        APP_SYS_LOG_INFO("matrix3:");
        for (uint32_t i = 0; i < n; i++) {
        for (uint32_t j = 0; j < n; j++)
            APP_SYS_LOG_INFO_RAW("%lf\t", matrix3[i][j]);
            APP_SYS_LOG_INFO_RAW(app_sys_msg_line());
        }
        APP_SYS_LOG_INFO("------------------------------------------------------------");
        double sum = 0.0;
        bool ret1 = app_sys_matrix_cholesky_det((double *)matrix0, (double *)matrix1, vector, n, &sum);
        APP_SYS_LOG_INFO("app_sys_matrix_cholesky_det:%d, %lf", ret1, sum);
        APP_SYS_LOG_INFO("------------------------------------------------------------");
    }
    
}
