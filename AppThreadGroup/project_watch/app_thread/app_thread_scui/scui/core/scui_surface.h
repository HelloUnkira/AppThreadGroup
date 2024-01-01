#ifndef SCUI_SURFACE_H
#define SCUI_SURFACE_H

// 绘制实体抽象:画布
// 画布是一整块区域或不存在(共用画布)
typedef struct {
    uint8_t     *pixel;     // 画布像素流地址
    scui_area_t  clip;      // 画布空间剪切域
    scui_alpha_t alpha;     // 画布区域透明度
} scui_surface_t;

/*@brief 画布为空检查
 *@param surface 画布实例
 *@retval 是否为空
 */
inline bool scui_surface_empty(scui_surface_t *surface);

/*@brief draw画布设置
 *@param surface 画布实例
 */
void scui_surface_draw_set(scui_surface_t *surface);

/*@brief draw画布获取
 *@param surface 画布实例
 */
void scui_surface_draw_get(scui_surface_t *surface);

#endif
