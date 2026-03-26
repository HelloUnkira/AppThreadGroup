#ifndef SCUI_DRAW_TASK_H
#define SCUI_DRAW_TASK_H

typedef struct {
    scui_list_dln_t  dl_node;       /* 任务序列节点 */
    scui_draw_dsc_t *draw_dsc;      /* 绘制描述符 */
    scui_coord_t     draw_idx;      /* 绘制序列索引 */
    uint8_t          draw_clip[0];  /* 绘制区域哈希(柔性数组) */
} scui_draw_task_node_t;

typedef struct {
    scui_sem_t sem;
    scui_list_dll_t dl_list;        /* 任务序列列表 */
    uintptr_t     hash_size;        /* 绘制区域哈希数组大小 */
    uintptr_t     node_total;       /* 节点数量(总计) */
    uintptr_t     node_frame;       /* 节点数量(当前) */
} scui_draw_task_list_t;

/*@brief 就绪绘制任务序列
 */
void scui_draw_task_ready(void);

/*@brief 通知绘制任务序列
 *@param draw_dsc 绘制描述符
 */
void scui_draw_task_notify(scui_draw_dsc_t *draw_dsc);

/*@brief 派发绘制任务序列
 */
void scui_draw_task_dispatch(void);

#endif
