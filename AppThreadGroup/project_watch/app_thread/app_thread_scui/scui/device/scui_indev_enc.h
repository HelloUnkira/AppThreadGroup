#ifndef SCUI_INDEV_ENC_H
#define SCUI_INDEV_ENC_H

typedef struct {
    /* merge diff */
    uint64_t event_diff_tick;
    scui_event_t event_diff;
} scui_indev_enc_t;

/*@brief 输入设备事件合并
 */
void scui_indev_enc_event_merge(void);

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_enc_notify(scui_indev_data_t *data);

/*@brief 输入设备初始化
 */
void scui_indev_enc_ready(void);

#endif
