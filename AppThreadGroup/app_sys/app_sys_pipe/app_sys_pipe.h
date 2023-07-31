#ifndef APP_SYS_PIPE_H
#define APP_SYS_PIPE_H

typedef struct {
    #define  APP_SYS_PIPE_SLAB_UNIT         50  /* 分配颗粒(偏好性(性能:大,内存:小)) */
    #define  APP_SYS_PIPE_SLAB_DEBOUNCE     10  /* 去抖动 */
    void    *prev;      /* 上一管理区 */
    void    *next;      /* 下一管理区 */
    uint8_t *mem_s;     /* 管理区起始地址 */
    uint8_t *mem_e;     /* 管理区结束地址 */
    uint8_t *blk_list;  /* 空闲块链表集 */
    uint32_t blk_used;  /* 使用字节明细 */
    uint32_t blk_size;  /* 块大小 */
    uint32_t blk_num;   /* 块数量 */
} app_sys_pipe_slab_t;

typedef struct {
    void    *buddy;         /* 管道是优先队列(它的兄弟) */
    uint64_t thread:10;     /* 接收者线程 */
    uint64_t module:10;     /* 接收者线程模组 */
    uint64_t event:10;      /* 接收者线程模组事件 */
    uint64_t priority:8;    /* 接收者线程模组事件优先级(数字越大优先级越高) */
    uint64_t dynamic:1;     /* 本次传输包裹状态 */
    uint32_t size;          /* 协议数据流大小 */
    void    *data;          /* 协议数据流(浅拷贝) */
} app_sys_pipe_pkg_t;

typedef struct {
    app_sys_pipe_pkg_t *head;       /* 管道头 */
    app_sys_pipe_pkg_t *tail;       /* 管道尾 */
    uint32_t            number;     /* 管道项数 */
    app_critical_t      critical;   /* 管道资源临界区保护 */
} app_sys_pipe_t;

/*@brief 初始化管道分配器
 */
void app_sys_pipe_slab_ready(void);

/*@brief     初始化管道
 *@param[in] pipe 管道实例
 */
void app_sys_pipe_ready(app_sys_pipe_t *pipe);

/*@brief     获取管道资源包数量
 *@param[in] pipe 管道实例
 *@retval    管道资源包数量
 */
uint32_t app_sys_pipe_pkg_num(app_sys_pipe_t *pipe);

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