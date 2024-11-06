/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 */
void scui_draw_graph(scui_draw_graph_dsc_t *draw_graph)
{
    #if SCUI_DRAW_GRAPH_USE_EGUI
    scui_draw_graph_EGUI(draw_graph);
    #endif
}
