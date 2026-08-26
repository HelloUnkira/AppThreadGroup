/*实现目标:
 *    设计一个协议数据传输接口
 *    它向下适配平台框架并向上适配线程组的协议传输流程
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

static app_module_protocol_t *app_module_protocol_notify_node = NULL;
static app_module_protocol_notify_list_t app_module_protocol_notify_list = {0};

/*@brief 优先队列比较回调
 */
static bool app_module_protocol_notify_prio(app_sys_list_dln_t *node_1, app_sys_list_dln_t *node_2)
{
    app_module_protocol_t *protocol_node_a = app_sys_own_ofs(app_module_protocol_t, dl_node, node_1);
    app_module_protocol_t *protocol_node_b = app_sys_own_ofs(app_module_protocol_t, dl_node, node_2);
    return protocol_node_a->priority > protocol_node_b->priority;
}

/*@brief ack超时回调:file超时则失败,否则关闭当前节点
 */
static void app_module_protocol_notify_timer_handler(void *timer)
{
    #if APP_MODULE_PROTOCOL_USE_NANOPB
    app_nanopb_xfer_file_timeout();
    #endif
    
    /* 输出超时信息:消息类型/信道 */
    if (app_module_protocol_notify_node != NULL) {
        app_module_protocol_notify_node->close = 1;
        APP_SYS_LOG_WARN("protocol ack timeout: type:%u chan:%u",
            app_module_protocol_notify_node->type,
            app_module_protocol_notify_node->chan);
    }
    
    app_module_protocol_t trigger = {0};
    app_module_protocol_notify(&trigger, 0);
}

/*@brief linker桥接:投递高优先linker事件(接收侧ack完成驱动发送侧)
 *@param protocol 链路节点(栈资源)
 */
void app_module_protocol_linker(app_module_protocol_t *protocol)
{
    app_module_protocol_t *copy = app_mem_alloc(sizeof(app_module_protocol_t));
    memcpy(copy, protocol, sizeof(app_module_protocol_t));
    app_thread_package_t package = {
        .thread   = app_thread_id_manage,
        .module   = app_thread_manage_protocol,
        .event    = app_thread_manage_protocol_linker,
        .priority = app_thread_package_priority_real_time,
        .dynamic  = true,
        .data     = copy,
        .size     = sizeof(app_module_protocol_t),
    };
    app_thread_package_notify(&package);
}

/*@brief 传输协议
 *@param protocol 传输协议包(栈资源,非堆资源或静态资源)
 *@param priority 子优先级(0为默认,大者先发)
 */
void app_module_protocol_notify(app_module_protocol_t *protocol, uint32_t priority)
{
    /* 空触发:直接投事件驱动调度 */
    if (protocol->type == app_module_protocol_default) {
        app_module_protocol_t *copy = app_mem_alloc(sizeof(app_module_protocol_t));
        memcpy(copy, protocol, sizeof(app_module_protocol_t));
        app_thread_package_t package = {
            .thread  = app_thread_id_manage,
            .module  = app_thread_manage_protocol,
            .event   = app_thread_manage_protocol_notify,
            .dynamic = true,
            .data    = copy,
            .size    = sizeof(app_module_protocol_t),
        };
        app_thread_package_notify(&package);
        return;
    }
    /* 业务消息:拷贝入等待队列,投空事件触发调度 */
    app_module_protocol_t *node = app_mem_alloc(sizeof(app_module_protocol_t));
    memcpy(node, protocol, sizeof(app_module_protocol_t));
    node->priority = priority;
    app_sys_list_dln_reset(&node->dl_node);
    app_mutex_process(&app_module_protocol_notify_list.mutex, app_mutex_take);
    app_sys_queue_dlpq_enqueue(&app_module_protocol_notify_list.list, &node->dl_node, app_module_protocol_notify_prio);
    app_mutex_process(&app_module_protocol_notify_list.mutex, app_mutex_give);
    app_module_protocol_t trigger = {0};
    app_module_protocol_notify(&trigger, 0);
}

/*@brief 传输协议(接收)
 *@param protocol 传输协议包(栈资源,非堆资源或静态资源)
 */
void app_module_protocol_respond(app_module_protocol_t *protocol)
{
    app_module_protocol_t *copy = app_mem_alloc(sizeof(app_module_protocol_t));
    memcpy(copy, protocol, sizeof(app_module_protocol_t));
    app_thread_package_t package = {
        .thread   = app_thread_id_manage,
        .module   = app_thread_manage_protocol,
        .event    = app_thread_manage_protocol_respond,
        .priority = app_thread_package_priority_normal_above,
        .dynamic  = true,
        .data     = copy,
        .size     = sizeof(app_module_protocol_t),
    };
    app_thread_package_notify(&package);
}

/*@brief linker回调:ack到达,推进file状态机并入队下一个file,关闭当前节点
 *@param data 链路事件负载
 *@param size 负载大小
 */
void app_module_protocol_linker_handler(uint8_t *data, uint32_t size)
{
    app_module_protocol_t *protocol = (void *)data;
    if (protocol == NULL) return;
    switch (protocol->type) {
    case app_module_protocol_ack: {
        #if APP_MODULE_PROTOCOL_USE_NANOPB
        app_nanopb_xfer_ack_arg_t *arg = (void *)protocol->data;
        app_nanopb_xfer_file_ack(arg->code == AppPB_ACK_Code_SUCCEED);
        #endif
        
        if (app_module_protocol_notify_node != NULL)
            app_module_protocol_notify_node->close = 1;
        
        app_module_protocol_t trigger = {0};
        app_module_protocol_notify(&trigger, 0);
        break;
    }
    default:
        break;
    }
    if (protocol->dynamic)
        app_mem_free(protocol->data);
}

/*@brief notify调度:取队首→分派发送→瞬时清空或一应一答等ack
 *@param data 传输数据
 *@param size 传输数据大小
 */
void app_module_protocol_notify_handler(uint8_t *data, uint32_t size)
{
    app_module_protocol_t *protocol = (void *)data;
    APP_SYS_ASSERT(protocol->type == app_module_protocol_default);
    
    /* 检查队列是否有ack出去(特殊):ack不阻塞,若无条件发出去并清掉 */
    while (true) {
        app_module_protocol_t *protocol_node = NULL;
        app_mutex_process(&app_module_protocol_notify_list.mutex, app_mutex_take);
        app_sys_list_dln_t *node = app_sys_list_dll_head(&app_module_protocol_notify_list.list);
        if (node != NULL) protocol_node = app_sys_own_ofs(app_module_protocol_t, dl_node, node);
        if (protocol_node != NULL && protocol_node->type == app_module_protocol_ack)
            app_sys_list_dll_remove(&app_module_protocol_notify_list.list, node);
        app_mutex_process(&app_module_protocol_notify_list.mutex, app_mutex_give);
        if (protocol_node == NULL || protocol_node->type != app_module_protocol_ack) break;
        
        #if APP_MODULE_PROTOCOL_USE_NANOPB
        bool wait_ack = app_nanopb_xfer_notify(protocol_node);
        APP_SYS_ASSERT(!wait_ack);
        #endif
        
        if (protocol_node->dynamic) app_mem_free(protocol_node->data);
        app_mem_free(protocol_node);
    }
    
    /* 命中ack或超时:关闭当前节点 */
    if (app_module_protocol_notify_node != NULL &&
        app_module_protocol_notify_node->close) {
        app_sys_timer_stop(&app_module_protocol_notify_list.timer);
        app_module_protocol_t *protocol_node = app_module_protocol_notify_node;
        app_module_protocol_notify_node = NULL;
        
        if (protocol_node->dynamic) app_mem_free(protocol_node->data);
        app_mem_free(protocol_node);
    }
    
    /* 当前有节点在等ack:不取新的 */
    if (app_module_protocol_notify_node != NULL)
        goto done;
    
    /* 取队首挂为当前节点 */
    app_mutex_process(&app_module_protocol_notify_list.mutex, app_mutex_take);
    app_sys_list_dln_t *node = app_sys_list_dll_head(&app_module_protocol_notify_list.list);
    if (node != NULL) app_sys_list_dll_remove(&app_module_protocol_notify_list.list, node);
    app_mutex_process(&app_module_protocol_notify_list.mutex, app_mutex_give);
    if (node == NULL) goto done;
    
    app_sys_list_dln_reset(node);
    app_module_protocol_notify_node = app_sys_own_ofs(app_module_protocol_t, dl_node, node);
    
    #if APP_MODULE_PROTOCOL_USE_NANOPB
    app_module_protocol_t *protocol_node = app_module_protocol_notify_node;
    bool wait_ack = app_nanopb_xfer_notify(protocol_node);
    #endif
    
    if (wait_ack) {
        /* 一应一答:置pending并起ack超时 */
        app_module_protocol_notify_node->pending = 1;
        app_module_protocol_notify_list.timer.expired = app_module_protocol_notify_timer_handler;
        app_module_protocol_notify_list.timer.peroid  = app_module_protocol_ack_timeout;
        app_sys_timer_start(&app_module_protocol_notify_list.timer);
        
    } else {
        app_module_protocol_t *protocol_node = app_module_protocol_notify_node;
        app_module_protocol_notify_node = NULL;
        
        if (protocol_node->dynamic) app_mem_free(protocol_node->data);
        app_mem_free(protocol_node);
        
        app_module_protocol_t trigger = {0};
        app_module_protocol_notify(&trigger, 0);
    }
    
    done:
    if (protocol->dynamic)
        app_mem_free(protocol->data);
}

/*@brief 传输协议(接收)
 *@param data 传输数据
 *@param size 传输数据大小
 */
void app_module_protocol_respond_handler(uint8_t *data, uint32_t size)
{
    app_module_protocol_t *protocol = (void *)data;
    /* 检查数据流 */
    APP_SYS_LOG_INFO_RAW("size:%d%s", protocol->size, app_sys_log_line());
    APP_SYS_LOG_INFO_RAW("data:");
    for (uint32_t idx = 0; idx < protocol->size; idx++)
        APP_SYS_LOG_INFO_RAW("%02x ", protocol->data[idx]);
        APP_SYS_LOG_INFO_RAW(app_sys_log_line());
    
    #if APP_MODULE_PROTOCOL_USE_NANOPB
    app_nanopb_xfer_respond(protocol->data, protocol->size);
    #endif
    
    if (protocol->dynamic)
        app_mem_free(protocol->data);
}

#if APP_MODULE_PROTOCOL_TEST_ENABLE

/*@brief 协议自检(回环发包):一次性入队整条序列,由协议层一应一答逐个推进
 */
static void app_module_protocol_notify_node_test_run(void)
{
#if APP_MODULE_PROTOCOL_USE_NANOPB
    static const app_module_protocol_type_t msg[] = {
        app_module_protocol_device_info, app_module_protocol_device_param,
        app_module_protocol_elec_card,   app_module_protocol_system_clock,
        app_module_protocol_world_clock,  app_module_protocol_alarm,
        app_module_protocol_weather,     app_module_protocol_heart_rate,
        app_module_protocol_music,       app_module_protocol_msg_info,
        app_module_protocol_contact,     app_module_protocol_sport_tgt,
        app_module_protocol_user_phys,   app_module_protocol_motion_sum,
        app_module_protocol_sport_state, app_module_protocol_not_disturb,
        app_module_protocol_position,    app_module_protocol_fem_cycle,
        app_module_protocol_account,     app_module_protocol_sport_mng,
        app_module_protocol_sport_rcd,
        app_module_protocol_file,        /* 文件内部多子步,经timer/linker推进 */
    };
    for (uint32_t idx = 0; idx < app_sys_arr_len(msg); idx++) {
        app_module_protocol_t node = {
            .type = msg[idx],
            .chan = app_module_transfer_chan_low,
        };
        app_module_protocol_notify(&node, 0);
    }
#endif
}
#endif

/*@brief 协议模组初始化
 *       内部使用: 被manage线程使用
 */
void app_module_protocol_ready(void)
{
    app_sys_list_dll_reset(&app_module_protocol_notify_list.list);
    app_mutex_process(&app_module_protocol_notify_list.mutex, app_mutex_static);
    #if APP_MODULE_PROTOCOL_TEST_ENABLE
    app_module_protocol_notify_node_test_run();
    #endif
}
