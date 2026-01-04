#ifndef SCUI_DRAW_GRAPH_EGUI_H
#define SCUI_DRAW_GRAPH_EGUI_H

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
