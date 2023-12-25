/*实现目标:
 *    同步原语
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 信号量操作流程集合
 *@param sem    实例
 *@param option 实例动作
 */
void scui_sem_process(scui_sem_t *sem, scui_sem_option_t option)
{
    switch (option) {
    case scui_sem_static:
        app_sem_process(&sem->sem, app_sem_static);
        break;
    case scui_sem_create:
        app_sem_process(&sem->sem, app_sem_create);
        break;
    case scui_sem_destroy:
        app_sem_process(&sem->sem, app_sem_destroy);
        break;
    case scui_sem_take:
        app_sem_process(&sem->sem, app_sem_take);
        break;
    case scui_sem_give:
        app_sem_process(&sem->sem, app_sem_give);
        break;
    default:
        break;
    }
}

/*@brief 互斥锁操作流程集合
 *@param mutex  实例
 *@param option 实例动作
 */
void scui_mutex_process(scui_mutex_t *mutex, scui_mutex_option_t option)
{
    switch (option) {
    case scui_mutex_static:
        app_mutex_process(&mutex->mutex, app_mutex_static);
        break;
    case scui_mutex_create:
        app_mutex_process(&mutex->mutex, app_mutex_create);
        break;
    case scui_mutex_destroy:
        app_mutex_process(&mutex->mutex, app_mutex_destroy);
        break;
    case scui_mutex_take:
        app_mutex_process(&mutex->mutex, app_mutex_take);
        break;
    case scui_mutex_give:
        app_mutex_process(&mutex->mutex, app_mutex_give);
        break;
    default:
        break;
    }
}
