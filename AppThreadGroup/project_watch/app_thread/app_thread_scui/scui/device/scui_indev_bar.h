#ifndef SCUI_INDEV_BAR_H
#define SCUI_INDEV_BAR_H

typedef struct {
    scui_indev_state_t state;
    scui_coord_t bar_near;
    scui_coord_t bar_last;
    scui_coord_t bar_cnt;
    uint64_t cnt_tick;
    uint32_t move_cnt:20;
    uint32_t move_tag:1;
} scui_indev_bar_t;

/*@brief 输入设备事件合并
 */
void scui_indev_bar_event_merge(void);

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_bar_notify(scui_indev_data_t *data);

/*@brief 输入设备初始化
 */
void scui_indev_bar_ready(void);

#endif
