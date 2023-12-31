#ifndef APP_SYS_PIPE_H
#define APP_SYS_PIPE_H

typedef struct {
    app_sys_list_dln_t dl_node;
    /* 基本字段:系统必备 */
    uint64_t thread:10;     /* 接收者线程 */
    uint64_t module:10;     /* 接收者线程模组 */
    uint64_t event:10;      /* 接收者线程模组事件 */
    uint64_t priority:8;    /* 接收者线程模组事件优先级(数字越大优先级越高) */
    uint64_t ticks:20;      /* 事件包生成时的Tick,与处理时获取的Tick核对确认迟延数 */
    /* 事件包吸收回调: */
    /* 如果手动交付该回调,则使用事件包吸收功能 */
    /* 新的事件包根据回调作用到旧有的一个上去,且丢弃本事件 */
    bool (*absorb)(void *pkg_old, void *pkg_new);
    /* 扩展字段:用户自定义追加: */
    /* 事件可以携带通用结构信息 */
    union {
        /* 通用结构信息(数据量不定,支持任意类型和种类的数据): */
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
    app_sys_mem_slab_t mem_slab;
    app_critical_t critical;    /* 管道资源临界区保护 */
    uint32_t       number;      /* 管道项数 */
} app_sys_pipe_t;

/*@brief 初始化管道
 *@param pipe 管道实例
 *@param addr 内存地址(内存自备)
 *@param size 内存大小(字节)(num * sizeof(app_sys_pipe_pkg_t))
 */
void app_sys_pipe_ready(app_sys_pipe_t *pipe, uintptr_t addr, uintptr_t size);

/*@brief 获取管道资源包数量
 *@param pipe 管道实例
 *@retval 管道资源包数量
 */
uint32_t app_sys_pipe_num(app_sys_pipe_t *pipe);

/*@brief 交付一个包给管道
 *@param pipe     管道实例
 *@param package  事件资源包(栈资源,非堆资源或静态资源)
 *@retval  0:     正常接收
 *        +1:     事件被吸收
 *        -1:     内存块不足,失败
 */
int8_t app_sys_pipe_give(app_sys_pipe_t *pipe, app_sys_pipe_pkg_t *package);

/*@brief 从管道提取一个包
 *@param pipe     管道实例
 *@param package  事件资源包(栈资源,非堆资源或静态资源)
 *@param hit      对指定事件资源包进行命中
 */
void app_sys_pipe_take(app_sys_pipe_t *pipe, app_sys_pipe_pkg_t *package, bool hit);

/*@brief 迭代整个管道,访问所有事件包
 *@param pipe  管道实例
 *@param visit 事件包访问回调
 */
void app_sys_pipe_walk(app_sys_pipe_t *pipe, void (*visit)(app_sys_pipe_pkg_t *package));

#endif