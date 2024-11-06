#ifndef SCUI_DRAW_GRAPH_EGUI_H
#define SCUI_DRAW_GRAPH_EGUI_H

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_graph 绘制描述符实例
 */
void scui_draw_graph_EGUI(scui_draw_graph_dsc_t *draw_graph);

/* EmbeddedGUI: */
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
#pragma pack(push, 1)
typedef struct {
    int16_t offset;
    int16_t count;
    int16_t pixel;
} scui_draw_circle_item_t;

typedef struct {
    int16_t radius;
    int16_t count;
    const uint8_t *data;
    const scui_draw_circle_item_t *item;
} scui_draw_circle_info_t;
#pragma pack(pop)

#define SCUI_DRAW_CIRCLE_RES_RANGE      500
/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

#endif
