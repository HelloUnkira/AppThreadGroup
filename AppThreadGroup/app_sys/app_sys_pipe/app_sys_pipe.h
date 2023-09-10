#ifndef APP_SYS_PIPE_H
#define APP_SYS_PIPE_H

typedef struct {
    app_sys_list_dln_t dl_node;
    uint64_t thread:10;     /* 接收者线程 */
    uint64_t module:10;     /* 接收者线程模组 */
    uint64_t event:10;      /* 接收者线程模组事件 */
    uint64_t priority:8;    /* 接收者线程模组事件优先级(数字越大优先级越高) */
    uint64_t ticks:20;      /* 事件包生成时的Tick,与处理时获取的Tick核对确认迟延数 */
    /* 扩展字段:用户自定义追加: */
    /* 事件可以携带通用结构信息 */
    union {
        /* 通用结构信息(数据量极少,支持任意类型和种类的数据): */
        struct {
            uint64_t dynamic:1;     /* 协议数据流是动态生成,使用完毕要回收 */
            uint64_t size:24;       /* 协议数据流大小(16M) */
            void    *data;          /* 协议数据流(浅拷贝) */
        };
        /* 扩展迷你信息(数据量极少,通常表示状态类数据): */
        uintptr_t byte_align;
        uint64_t  byte_fixed;
        /* ... */
    };
} app_sys_pipe_pkg_t;

typedef struct {
    app_sys_list_dll_t dl_list;
    app_critical_t critical;    /* 管道资源临界区保护 */
    uint32_t       number;      /* 管道项数 */
} app_sys_pipe_t;

/*@brief 初始化管道资源
 */
void app_sys_pipe_src_ready(void);

/*@brief     初始化管道
 *@param[in] pipe 管道实例
 */
void app_sys_pipe_ready(app_sys_pipe_t *pipe);

/*@brief     获取管道资源包数量
 *@param[in] pipe 管道实例
 *@retval    管道资源包数量
 */
uint32_t app_sys_pipe_num(app_sys_pipe_t *pipe);

/*@brief     交付一个包给管道
 *@param[in] pipe     管道实例
 *@param[in] package  事件资源包(栈资源,非堆资源或静态资源)
 *@param[in] normal   不使用优先级
 */
void app_sys_pipe_give(app_sys_pipe_t *pipe, app_sys_pipe_pkg_t *package, bool normal);

/*@brief      从管道提取一个包
 *@param[in]  pipe     管道实例
 *@param[out] package  事件资源包(栈资源,非堆资源或静态资源)
 *@param[in]  hit      对指定事件资源包进行命中
 */
void app_sys_pipe_take(app_sys_pipe_t *pipe, app_sys_pipe_pkg_t *package, bool hit);

#endif