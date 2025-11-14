/*实现目标:
 *    输入设备事件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"
#include "app_thread_group.h"

static scui_mutex_t scui_indev_mutex;

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_notify(scui_indev_data_t *data)
{
    scui_mutex_process(&scui_indev_mutex, scui_mutex_take);
    if (data->type == scui_indev_type_ptr)
        scui_indev_ptr_notify(data);
    if (data->type == scui_indev_type_enc)
        scui_indev_enc_notify(data);
    if (data->type == scui_indev_type_key)
        scui_indev_key_notify(data);
    scui_mutex_process(&scui_indev_mutex, scui_mutex_give);
}

static void scui_indev_anima_expire(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    
    // 合并一些事件, 如多次点击合并成一个
    scui_mutex_process(&scui_indev_mutex, scui_mutex_take);
    scui_indev_ptr_event_merge();
    scui_indev_enc_event_merge();
    scui_indev_key_event_merge();
    scui_mutex_process(&scui_indev_mutex, scui_mutex_give);
}

/*@brief 输入设备初始化
 */
void scui_indev_ready(void)
{
    scui_mutex_process(&scui_indev_mutex, scui_mutex_static);
    
    scui_indev_ptr_ready();
    scui_indev_enc_ready();
    scui_indev_key_ready();
    
    scui_anima_t anima = {0};
    anima.expire = scui_indev_anima_expire;
    anima.reload = SCUI_ANIMA_INFINITE;
    anima.period = SCUI_ANIMA_TICK * 2;
    
    #if SCUI_INDEV_EVENT_MERGE
    scui_handle_t scui_indev_anima = SCUI_HANDLE_INVALID;
    scui_anima_create(&anima, &scui_indev_anima);
    scui_anima_start(scui_indev_anima);
    #endif
}
