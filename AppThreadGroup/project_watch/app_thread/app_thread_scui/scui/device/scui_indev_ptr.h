#ifndef SCUI_INDEV_PTR_H
#define SCUI_INDEV_PTR_H

typedef struct {
    scui_indev_state_t state;
    scui_point_t ptr_near;
    scui_point_t ptr_last;
    scui_coord_t ptr_cnt;
    uint64_t cnt_tick;
    uint32_t move_cnt:20;
    uint32_t move_tag:1;
} scui_indev_ptr_t;

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_ptr_notify(scui_indev_data_t *data);

/*@brief 输入设备初始化
 */
void scui_indev_ptr_ready(void);

/*@brief 输入设备手势方向计算
 *@param event 输入设备事件
 *@retval 手势方向
 */
scui_opt_dir_t scui_indev_ptr_dir(scui_event_t *event);

#endif
