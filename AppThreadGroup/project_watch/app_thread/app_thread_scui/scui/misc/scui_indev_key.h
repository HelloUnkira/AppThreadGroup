#ifndef SCUI_INDEV_KEY_H
#define SCUI_INDEV_KEY_H

typedef struct {
    struct {
        scui_indev_state_t state;
        scui_coord_t key_id;
        scui_coord_t key_val;
        scui_coord_t key_cnt;
        uint64_t cnt_tick;
        bool wait_release;
    } item[SCUI_INDEV_KEY_LIMIT];
} scui_indev_key_t;

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_key_notify(scui_indev_data_t *data);

/*@brief 输入设备等待抬起
 *@param key_id 关键字
 */
void scui_indev_key_wait_release(scui_coord_t key_id);

/*@brief 输入设备初始化
 */
void scui_indev_key_ready(void);

#endif
