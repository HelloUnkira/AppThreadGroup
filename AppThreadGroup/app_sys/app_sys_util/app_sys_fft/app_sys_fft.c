/*实现目标:
 *    快速傅里叶变换
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief     快速傅里叶变换
 *@param[in] fft 傅里叶变换数据实例
 */
void app_sys_fft_run(app_sys_fft_t *fft)
{
    uint32_t *rev  = fft->rev;
    double   *real = fft->real;
    double   *imag = fft->imag;
    
    for (uint32_t idx = 0; idx < fft->len; rev[idx++] = 0);
    //1.回溯前的准备,先进入到递归的最后一层,也即回溯的第一层
    {
        uint32_t ofs = -1;
        while ((1 << (++ofs)) < fft->len);
        for (uint32_t idx = 0; idx < fft->len; idx++)
            rev[idx]=(rev[idx >> 1] >> 1) | ((idx & 1) << (ofs - 1));
    }
    //2.接下来交换元素到递归的最后一层(为回溯做好准备)
    for (uint32_t idx = 0; idx < fft->len; idx++) {
        if (idx >= rev[idx])
            continue;
        uint32_t idx_t = rev[idx];
        double   tmp1  = real[idx];
        double   tmp2  = imag[idx];
        real[idx] = real[idx_t];
        imag[idx] = imag[idx_t];
        real[idx_t] = tmp1;
        imag[idx_t] = tmp2;
    }
    //3.从递归的最后一层开始进行回溯(h即表示当前层级单元元素量)
    //对应于分治树的最后一层,此时该层每一个单元只有一个元素,递归的底部
    for (uint32_t nest = 1; nest < fft->len; nest *= 2) {
        //计算单位根,注意:E**(i * x) = cos(x) + i * sin(x)
        double real_w1 = cos(fft->flag * acos(-1) / nest);
        double imag_w1 = sin(fft->flag * acos(-1) / nest);
        //计算每一个子单元,注意这里J的跨度是2*nest,跳过了后面的H个单元
        for (uint32_t idx_t = 0; idx_t < fft->len; idx_t += 2 * nest) {
            double real_wn = 1.0;//起始运动节点在wn(w0)时
            double imag_wn = 0.0;//每次进动后都偏移一个单位根
            //这里的k跨度是h,所以x为左半部分元素,y为右半部分元素
            for (uint32_t idx_k = idx_t; idx_k < idx_t + nest; idx_k++) {
                double real_x = real[idx_k];       //左半部分元素
                double imag_x = imag[idx_k];
                double real_y = real[idx_k + nest];   //右半部分元素
                double imag_y = imag[idx_k + nest];
                double tmp1 = 0.0;
                double tmp2 = 0.0;
                //右半部分比左半部分多出w的偏移,为右半部分旋转wn(计算y*=wn)
                //复数*运算:z1*z2 = (x1x2 - y1y2) + idx(x1y2 + x2y1)
                tmp1 = real_wn * real_y - imag_wn * imag_y;
                tmp2 = real_wn * imag_y + imag_wn * real_y;
                real_y = tmp1;
                imag_y = tmp2;
                //复数±运算:z1±z2 = (x1 ± x2) + idx(y1 ± y2)
                real[idx_k]     = real_x + real_y;
                imag[idx_k]     = imag_x + imag_y;
                real[idx_k + nest] = real_x - real_y;
                imag[idx_k + nest] = imag_x - imag_y;
                //计算下一个单位根(几何意义是单位根旋转,运算是wn*=w1)
                tmp1 = real_wn * real_w1 - imag_wn * imag_w1;
                tmp2 = real_wn * imag_w1 + imag_wn * real_w1;
                real_wn = tmp1;
                imag_wn = tmp2;
            }
        }
    }
    //4.判断是否是DFT还是IDFT
    for(uint32_t idx = 0; fft->flag == -1 && idx < fft->len; idx++) {
        //复数/运算:z1/z2 = ((x1x2 + y1y2) / (x2**2 + y2**2)) + idx
        //                  ((x2y1 - x1y2) / (x2**2 + y2**2))
        //当y2 == 0时,退化为:z1/x2 = x1 / x2 + idx (y1 / x2)
        real[idx] /= fft->len;
        imag[idx] /= fft->len;
    }
}
