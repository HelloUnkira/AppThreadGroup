#ifndef APP_MODULE_PROTOCOL_H
#define APP_MODULE_PROTOCOL_H

typedef enum {
    app_module_protocol_default = 0,
    app_module_protocol_ack,
    app_module_protocol_sync,
    
    app_module_protocol_device_info,
    app_module_protocol_device_param,
    app_module_protocol_elec_card,
    app_module_protocol_system_clock,
    app_module_protocol_world_clock,
    app_module_protocol_alarm,
    app_module_protocol_weather,
    app_module_protocol_heart_rate,
    app_module_protocol_music,
    app_module_protocol_msg_info,
    app_module_protocol_contact,
    app_module_protocol_sport_tgt,
    app_module_protocol_user_phys,
    app_module_protocol_motion_sum,
    app_module_protocol_sport_state,
    app_module_protocol_not_disturb,
    app_module_protocol_position,
    app_module_protocol_fem_cycle,
    app_module_protocol_account,
    app_module_protocol_sport_mng,
    app_module_protocol_sport_rcd,
    app_module_protocol_file,
    app_module_protocol_file_step,   /* 文件步进(内部使用) */
    app_module_protocol_ctrl_step,   /* ack轮询步进(内部使用) */
} app_module_protocol_notify_type_t;

typedef struct {
    uint32_t type;      //传输类型(notify)
    uint32_t status;    //传输流程状态,内部约定(notify)
    uint8_t *data;      //传输数据
    uint32_t size;      //传输数据大小
    uint64_t dynamic:1; //传输数据是否为动态
} app_module_protocol_t;

/* 协议事件优先级:应答/同步走异步发送优先响应 */
#define app_module_protocol_ack_priority   app_thread_package_priority_normal_above
#define app_module_protocol_sync_priority  app_thread_package_priority_real_time

/*@brief 传输协议
 *@param protocol 传输协议包(栈资源,非堆资源或静态资源)
 *@param priority 事件优先级(app_thread_package_priority_*,0为默认)
 */
void app_module_protocol_notify(app_module_protocol_t *protocol, uint32_t priority);

/*@brief 传输协议
 *@param protocol 传输协议包(栈资源,非堆资源或静态资源)
 */
void app_module_protocol_respond(app_module_protocol_t *protocol);

/*@brief 传输协议
 *@param data 传输数据
 *@param size 传输数据大小
 */
void app_module_protocol_notify_handler(uint8_t *data, uint32_t size);

/*@brief 传输协议
 *@param data 传输数据
 *@param size 传输数据大小
 */
void app_module_protocol_respond_handler(uint8_t *data, uint32_t size);

/*@brief 系统时钟模组初始化
 *       内部使用: 被namage线程使用
 */
void app_module_protocol_ready(void);

#endif
