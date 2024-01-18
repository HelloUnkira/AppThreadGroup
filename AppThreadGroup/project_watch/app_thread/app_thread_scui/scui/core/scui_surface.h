#ifndef SCUI_SURFACE_H
#define SCUI_SURFACE_H

/*@brief 绘制实体抽象:画布
 *       画布是块独立缓冲区或共享绘制画布的缓冲区
 *       画布是根控件绘制域的完全映射
 *       每一个控件都有自己的画布
 *       仅根控件具备其独立缓冲区
 *       非根控件共享根控件缓冲区
 *       在一个控件树中的像素流地址都是统一的一个
 *       画布空间剪切域描述了控件允许在画布进行绘制的区域
 *       画布空间透明度描述了控件绘制区域的透明度属性
 */
typedef struct {
    uint8_t     *pixel;     // 画布像素流地址
    scui_area_t  clip;      // 画布空间剪切域
    scui_alpha_t alpha;     // 画布空间透明度
} scui_surface_t;

#endif
