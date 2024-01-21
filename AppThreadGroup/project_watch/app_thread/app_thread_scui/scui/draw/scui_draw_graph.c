/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS        1
#define SCUI_LOG_LOCAL_LEVEL         0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

typedef struct {
    scui_area_t clip;               // 绘制区域(相对画布坐标)
    scui_color_gradient_t color;    // 颜色(主色调和渐变色调)
    scui_point_t pos_s;             // 起始点(相对画布坐标)
    scui_point_t pos_e;             // 结束点(相对画布坐标)
    scui_coord_t width;             // 宽度
} scui_draw_dsc_line_t;

/*@brief 线条绘制
 *@param dst_surface 画布实例
 *@param dst_clip    画布绘制区域
 *@param dsc         线条绘制描述(参数集合)
 */
void scui_draw_line(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                    scui_draw_dsc_line_t *dsc)
{
    /* @等待适配,要用的时候再去实现 */

    /* 在画布指定区域绘制线条 */
    /* 线条有一个被允许的绘制区域 */
    /* 线条实际绘制需要在绘制区域内 */
    /* 注意:抗锯齿,带灰阶,透明到底图 */
}
