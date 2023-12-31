#ifndef SCUI_INDEV_PTR_H
#define SCUI_INDEV_PTR_H

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_ptr_notify(scui_indev_data_t *data);

/*@brief 输入设备初始化
 */
void scui_indev_ptr_ready(void);

#endif
