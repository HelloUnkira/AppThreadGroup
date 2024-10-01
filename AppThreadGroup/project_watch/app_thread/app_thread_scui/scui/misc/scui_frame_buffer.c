/*实现目标:
 *    帧缓冲管理
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_frame_buffer_t scui_frame_buffer = {0};

/*@brief 画布帧缓冲区刷新画布实例
 *@retval 刷新画布实例
 */
scui_surface_t * scui_frame_buffer_refr(void)
{
    SCUI_LOG_DEBUG("");
    #if SCUI_FRAME_BUFFER_ASYNC
    return &scui_frame_buffer.surface[1 - scui_frame_buffer.draw_idx];
    #else
    return &scui_frame_buffer.surface[0];
    #endif
}

/*@brief 画布帧缓冲区绘制画布实例
 *@retval 绘制画布实例
 */
scui_surface_t * scui_frame_buffer_draw(void)
{
    SCUI_LOG_DEBUG("");
    #if SCUI_FRAME_BUFFER_ASYNC
    return &scui_frame_buffer.surface[scui_frame_buffer.draw_idx];
    #else
    return &scui_frame_buffer.surface[0];
    #endif
}

/*@brief 画布帧缓冲区初始化
 */
void scui_frame_buffer_ready(void)
{
    scui_sem_process(&scui_frame_buffer.sem_draw, scui_sem_static);
    scui_sem_process(&scui_frame_buffer.sem_refr, scui_sem_static);
    scui_mutex_process(&scui_frame_buffer.mutex,  scui_mutex_static);
    
    /* 让draw流程变得可触发,之后循环互锁 */
    scui_sem_process(&scui_frame_buffer.sem_draw, scui_sem_give);
    // scui_sem_process(&scui_frame_buffer.sem_refr, scui_sem_give);
}

/*@brief 绘制忙碌等待
 */
void scui_frame_buffer_draw_wait(void)
{
    SCUI_LOG_INFO("draw wait start");
    bool refr_hold = false;
    do {
        scui_mutex_process(&scui_frame_buffer.mutex, scui_mutex_take);
        #if SCUI_FRAME_BUFFER_ASYNC
        refr_hold = scui_frame_buffer.refr_hold & (1 << scui_frame_buffer.draw_idx);
        #else
        refr_hold = scui_frame_buffer.refr_hold & (1 << 0);
        #endif
        scui_mutex_process(&scui_frame_buffer.mutex, scui_mutex_give);
    } while (refr_hold);
    SCUI_LOG_INFO("draw wait over");
}

/*@brief 刷新画布触发
 */
void scui_frame_buffer_refr_toggle(void)
{
    SCUI_LOG_INFO("wait draw sem");
    scui_sem_process(&scui_frame_buffer.sem_draw, scui_sem_take);
    
    SCUI_LOG_INFO("lock draw, switch draw & refr");
    scui_mutex_process(&scui_frame_buffer.mutex, scui_mutex_take);
    /* 切换上一刷新块,它将成为新的绘制块 */
    #if SCUI_FRAME_BUFFER_ASYNC
    scui_frame_buffer.refr_hold |= (1 << scui_frame_buffer.draw_idx);
    scui_frame_buffer.draw_idx = 1 - scui_frame_buffer.draw_idx;
    #else
    scui_frame_buffer.refr_hold |= (1 << 0);
    scui_frame_buffer.draw_idx = 0;
    #endif
    scui_mutex_process(&scui_frame_buffer.mutex, scui_mutex_give);
    
    SCUI_LOG_INFO("send refr sem");
    scui_sem_process(&scui_frame_buffer.sem_refr, scui_sem_give);
}

/*@brief 刷新画布流程
 *@param refr 刷新流程
 */
void scui_frame_buffer_refr_routine(void (*refr)(scui_surface_t *surface))
{
    SCUI_LOG_INFO("wait refr sem");
    scui_sem_process(&scui_frame_buffer.sem_refr, scui_sem_take);
    
    scui_surface_t *surface = scui_frame_buffer_refr();
    SCUI_ASSERT(surface != NULL && surface->pixel != NULL);
    
    if (refr)
        refr(surface);
    
    SCUI_LOG_INFO("unlock refr");
    scui_mutex_process(&scui_frame_buffer.mutex, scui_mutex_take);
    #if SCUI_FRAME_BUFFER_ASYNC
    scui_frame_buffer.refr_hold &= ~(1 << (1 - scui_frame_buffer.draw_idx));
    #else
    scui_frame_buffer.refr_hold &= ~(1 << 0);
    #endif
    scui_mutex_process(&scui_frame_buffer.mutex, scui_mutex_give);
    
    SCUI_LOG_INFO("send draw sem");
    scui_sem_process(&scui_frame_buffer.sem_draw, scui_sem_give);
}
