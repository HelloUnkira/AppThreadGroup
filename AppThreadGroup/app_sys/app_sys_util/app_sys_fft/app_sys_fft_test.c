/*实现目标:
 *    快速傅里叶变换测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 快速傅里叶变换测试
 */
void app_sys_fft_test(void)
{
    int len = 16;
    int array1[16] = {0};
    int array2[16] = {0};
    int k = 0,i = 0;
    double real_list[16] = {
         2.2,  4.5,  6.7,  8.5,
        10.2, 12.3, 14.5, 16.2,
        19.3, 21.2, 25.2, 29.4,
        36.4, 39.2, 45.2, 50.1};
    double imag_list[16] = {0};
    
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    APP_SYS_LOG_INFO("REAL:");
    for (k = 0; k < len; k += 4) {
        for (i = k; i < k + 4; i++)
            APP_SYS_LOG_INFO_RAW("%lf\t",real_list[i]);
            APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    }
    APP_SYS_LOG_INFO("IMAG:");
    for (k = 0; k < len; k += 4) {
        for (i = k; i < k + 4; i++)
            APP_SYS_LOG_INFO_RAW("%lf\t",imag_list[i]);
            APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    
    //result:
    //real_list = {
    //     341.1,   9.7, -23.3, -24.7,
    //     -23.5, -26.8, -26.9, -26.7,
    //     -21.7, -26.7, -26.9, -26.8,
    //     -23.5, -24.7, -23.3,   9.7}; 
    //imag_list = {
    //      0.0, -124.5, -66.1, -37.0,
    //    -27.0,  -19.9, -10.5, -2.5,
    //      0.0,    2.5,  10.5, 19.9,
    //     27.0,   37.0,  66.1, 124.5};
    
    //1.启用快速傅里叶变换
    app_sys_fft_t fft_inst1 = {
        .real = real_list,
        .imag = imag_list,
        .len  = len,
        .rev  = array1,
        .flag = +1,
    };
    app_sys_fft_run(&fft_inst1);
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    APP_SYS_LOG_INFO("FFT:");
    for (k = 0; k < len; k++)
        APP_SYS_LOG_INFO_RAW("%d ",array1[k]);
        APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    APP_SYS_LOG_INFO("REAL:");
    for (k = 0; k < len; k += 4) {
        for (i = k; i < k + 4; i++)
            APP_SYS_LOG_INFO_RAW("%lf\t",real_list[i]);
            APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    }
    APP_SYS_LOG_INFO("IMAG:");
    for (k = 0; k < len; k += 4) {
        for (i = k; i < k + 4; i++)
            APP_SYS_LOG_INFO_RAW("%lf\t",imag_list[i]);
            APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    
    //2.启用快速傅里叶逆变换
    app_sys_fft_t fft_inst2 = {
        .real = real_list,
        .imag = imag_list,
        .len  = len,
        .rev  = array2,
        .flag = -1,
    };
    app_sys_fft_run(&fft_inst2);
    APP_SYS_LOG_INFO("------------------------------------------------------------");
    for (k = 0; k < len; k++)
        APP_SYS_LOG_INFO_RAW("%d ",array2[k]);
        APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    APP_SYS_LOG_INFO("REAL:");
    for (k = 0; k < len; k += 4) {
        for (i = k; i < k + 4; i++)
            APP_SYS_LOG_INFO_RAW("%lf\t",real_list[i]);
            APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    }
    APP_SYS_LOG_INFO("IMAG:");
    for (k = 0; k < len; k += 4) {
        for (i = k; i < k + 4; i++)
            APP_SYS_LOG_INFO_RAW("%lf\t",imag_list[i]);
            APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    }
    APP_SYS_LOG_INFO("------------------------------------------------------------");
}
