#ifndef APP_MODULE_PROTOCOL_H
#define APP_MODULE_PROTOCOL_H

/*@brief 协议通知类型 */
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
    app_module_protocol_display_info,
    app_module_protocol_app_bind,
    app_module_protocol_power,
    app_module_protocol_sleep_set,
    app_module_protocol_watch_cfg,
    app_module_protocol_file,        /* 文件传输(启动引擎,后续子步由timer驱动) */
} app_module_protocol_type_t;

/* 协议发送:事件子优先级 */
#define APP_MODULE_PROTOCOL_PRIO_NORMAL    app_thread_package_priority_normal + 0
#define APP_MODULE_PROTOCOL_PRIO_FILE      app_thread_package_priority_normal + 1
#define APP_MODULE_PROTOCOL_PRIO_SYNC      app_thread_package_priority_normal + 2
#define APP_MODULE_PROTOCOL_PRIO_ACK       app_thread_package_priority_normal + 3

/* 协议发送:ACK超时(ms) */
#define app_module_protocol_ack_timeout    3000

/*@brief 协议发送节点
 */
typedef struct {
    app_sys_list_dln_t dl_node;
    /* 基础成员信息 */
    uint32_t type;      //传输类型(notify)
    uint8_t *data;      //传输数据
    uint32_t size;      //传输数据大小
    uint64_t dynamic:1; //动态数据标记
    /* 节点成员状态 */
    uint8_t  chan;      //传输信道
    uint8_t  close;     //节点关闭标记
    uint8_t  pending;   //已发等ack
    uint8_t  priority;  //子优先级(队列排序)
} app_module_protocol_t;

/*@brief 协议发送队列
 */
typedef struct {
    app_mutex_t mutex;
    app_sys_timer_t timer;
    app_sys_list_dll_t list;
    /* 当前发送节点单独定义为实例,不在队列内 */
} app_module_protocol_notify_list_t;

/*@brief 传输协议
 *@param protocol 链路节点(栈资源,非堆资源或静态资源)
 */
void app_module_protocol_linker(app_module_protocol_t *protocol);

/*@brief 传输协议
 *@param protocol 传输协议包(栈资源,非堆资源或静态资源)
 *@param priority 子优先级(0为默认,大者先发)
 */
void app_module_protocol_notify(app_module_protocol_t *protocol, uint32_t priority);

/*@brief 传输协议(接收)
 *@param protocol 传输协议包(栈资源,非堆资源或静态资源)
 */
void app_module_protocol_respond(app_module_protocol_t *protocol);

/*@brief 传输协议(链接)
 *@param data 链路事件负载
 *@param size 负载大小
 */
void app_module_protocol_linker_handler(uint8_t *data, uint32_t size);

/*@brief 传输协议(发送)
 *@param data 传输数据
 *@param size 传输数据大小
 */
void app_module_protocol_notify_handler(uint8_t *data, uint32_t size);

/*@brief 传输协议(接收)
 *@param data 传输数据
 *@param size 传输数据大小
 */
void app_module_protocol_respond_handler(uint8_t *data, uint32_t size);

/*@brief 协议模组初始化
 *       内部使用: 被namage线程使用
 */
void app_module_protocol_ready(void);

#endif
