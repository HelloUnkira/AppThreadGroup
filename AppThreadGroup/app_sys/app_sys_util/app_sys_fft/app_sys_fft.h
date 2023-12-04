#ifndef APP_SYS_FFT_H
#define APP_SYS_FFT_H

typedef struct {
    int32_t  *rev;      //递归的最后一层下标表(辅助数组)
    double   *real;     //要处理的复数集实数部
    double   *imag;     //要处理的复数集虚数部
    uint32_t  len;      //复数集的长度
    int8_t    flag;     //+1:fft,-1:ifft
} app_sys_fft_t;

/*@brief 快速傅里叶变换
 *@param fft 傅里叶变换数据实例
 */
void app_sys_fft_run(app_sys_fft_t *fft);

#endif
