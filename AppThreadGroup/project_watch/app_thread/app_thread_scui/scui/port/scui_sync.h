#ifndef SCUI_SYNC_H
#define SCUI_SYNC_H

typedef struct {
    app_sem_t sem;
} scui_sem_t;

typedef enum {
    scui_sem_default = 0,   /* 占位符 */
    scui_sem_static,        /* 信号量准备(内存常驻) */
    scui_sem_create,        /* 信号量创建 */
    scui_sem_destroy,       /* 信号量销毁 */
    scui_sem_take,          /* 信号量获取 */
    scui_sem_give,          /* 信号量释放 */
} scui_sem_option_t;

/*@brief 信号量操作流程集合
 *@param sem    实例
 *@param option 实例动作
 */
void scui_sem_process(scui_sem_t *sem, scui_sem_option_t option);

typedef struct {
    app_mutex_t mutex;
} scui_mutex_t;

typedef enum {
    scui_mutex_default = 0,     /* 占位符 */
    scui_mutex_static,          /* 互斥锁准备(内存常驻) */
    scui_mutex_create,          /* 互斥锁创建 */
    scui_mutex_destroy,         /* 互斥锁销毁 */
    scui_mutex_take,            /* 互斥锁获取 */
    scui_mutex_give,            /* 互斥锁释放 */
} scui_mutex_option_t;

/*@brief 互斥锁操作流程集合
 *@param mutex  实例
 *@param option 实例动作
 */
void scui_mutex_process(scui_mutex_t *mutex, scui_mutex_option_t option);

#endif
