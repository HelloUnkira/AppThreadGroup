/*实现目标:
 *    帧缓冲管理
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_surface_fb_t scui_surface_fb = {0};
/* frame buffer 只能是1块或俩块 */
#if SCUI_SURFACE_FB_LIMIT != 1 && SCUI_SURFACE_FB_LIMIT != 2
#error "scui frame buffer just one block or two block"
#endif

/*@brief 显示块与刷新块交换
 */
void scui_surface_fb_switch(void)
{
    SCUI_LOG_INFO("");
    /* 切换上一刷新块,它将成为新的绘制块 */
    #if SCUI_SURFACE_FB_LIMIT == 2
    scui_mutex_process(&scui_surface_fb.mutex, scui_mutex_take);
    scui_surface_fb.draw_idx = 1 - scui_surface_fb.draw_idx;
    scui_mutex_process(&scui_surface_fb.mutex, scui_mutex_give);
    #endif
}

/*@brief 锁定当前绘制块,并且切换绘制块
 *       刷新异步正式开始时执行它锁定绘制块
 *       绘制块实例需要在之前获取
 */
void scui_surface_fb_refr_lock(void)
{
    SCUI_LOG_INFO("");
    #if SCUI_SURFACE_FB_LIMIT == 2
    scui_mutex_process(&scui_surface_fb.mutex, scui_mutex_take);
    scui_surface_fb.refr_hold |= (1 < scui_surface_fb.draw_idx);
    scui_mutex_process(&scui_surface_fb.mutex, scui_mutex_give);
    #endif
}

/*@brief 解锁当前刷新块,并且产生信号
 */
void scui_surface_fb_refr_unlock(void)
{
    SCUI_LOG_INFO("");
    #if SCUI_SURFACE_FB_LIMIT == 2
    scui_mutex_process(&scui_surface_fb.mutex, scui_mutex_take);
    scui_surface_fb.refr_hold &= ~(1 < (1 - scui_surface_fb.draw_idx));
    scui_mutex_process(&scui_surface_fb.mutex, scui_mutex_give);
    #endif
}

/*@brief 等待刷新信号
 */
void scui_surface_fb_refr_wait(void)
{
    SCUI_LOG_INFO("");
    scui_sem_process(&scui_surface_fb.sem_refr, scui_sem_take);
}

/*@brief 产生刷新信号
 */
void scui_surface_fb_refr_notify(void)
{
    SCUI_LOG_INFO("");
    scui_sem_process(&scui_surface_fb.sem_refr, scui_sem_give);
}

/*@brief 等待绘制信号
 */
void scui_surface_fb_draw_wait(void)
{
    SCUI_LOG_INFO("");
    scui_sem_process(&scui_surface_fb.sem_draw, scui_sem_take);
}

/*@brief 产生绘制信号
 */
void scui_surface_fb_draw_notify(void)
{
    SCUI_LOG_INFO("");
    scui_sem_process(&scui_surface_fb.sem_draw, scui_sem_give);
}

/*@brief 画布帧缓冲区刷新画布实例
 *@retval 刷新画布实例
 */
scui_surface_t * scui_surface_fb_refr(void)
{
    SCUI_LOG_INFO("");
    #if SCUI_SURFACE_FB_LIMIT == 2
    return &scui_surface_fb.surface[1 - scui_surface_fb.draw_idx];
    #else
    return &scui_surface_fb.surface[0];
    #endif
}

/*@brief 画布帧缓冲区绘制画布实例
 *@retval 绘制画布实例
 */
scui_surface_t * scui_surface_fb_draw(void)
{
    SCUI_LOG_INFO("");
    #if SCUI_SURFACE_FB_LIMIT == 2
    return &scui_surface_fb.surface[scui_surface_fb.draw_idx];
    #else
    return &scui_surface_fb.surface[0];
    #endif
}

/*@brief 画布帧缓冲区初始化
 */
void scui_surface_fb_ready(void)
{
    scui_sem_process(&scui_surface_fb.sem_draw, scui_sem_static);
    scui_sem_process(&scui_surface_fb.sem_refr, scui_sem_static);
    #if SCUI_SURFACE_FB_LIMIT == 2
    scui_mutex_process(&scui_surface_fb.mutex,  scui_mutex_static);
    #endif
    
    /* 让draw流程变得可触发,之后循环互锁 */
    scui_surface_fb_draw_notify();
    scui_surface_fb_refr_notify();
}

/*@brief 绘制画布例程(例)
 *@param draw 绘制流程
 */
void scui_surface_draw_routine(void (*draw)(scui_surface_t *surface))
{
    /*例:
     *      draw线程:
     *      #if SCUI_SURFACE_FB_LIMIT == 1
     *      scui_surface_fb_draw_wait();
     *      #endif
     *      
     *      scui_surface_t *suface = scui_surface_fb_draw();
     *      // draw surface...
     *      
     *      #if SCUI_SURFACE_FB_LIMIT == 2
     *      scui_surface_fb_draw_wait();
     *      #endif
     *      
     *      scui_surface_fb_refr_lock();
     *      scui_surface_fb_refr_notify();
     *      scui_surface_fb_switch();
     */
    
    #if SCUI_SURFACE_FB_LIMIT == 1
    SCUI_LOG_DEBUG("");
    scui_surface_fb_draw_wait();
    #endif
    
    SCUI_LOG_DEBUG("");
    scui_surface_t *surface = scui_surface_fb_draw();
    SCUI_ASSERT(surface != NULL && surface->pixel != NULL);
    
    SCUI_LOG_DEBUG("");
    if (draw)
        draw(surface);
    
    #if SCUI_SURFACE_FB_LIMIT == 2
    SCUI_LOG_DEBUG("");
    scui_surface_fb_draw_wait();
    #endif
    
    SCUI_LOG_DEBUG("");
    scui_surface_fb_refr_lock();
    SCUI_LOG_DEBUG("");
    scui_surface_fb_switch();
    SCUI_LOG_DEBUG("");
    scui_surface_fb_refr_notify();
}

/*@brief 刷新画布例程(例)
 *@param refr 刷新流程
 */
void scui_surface_refr_routine(void (*refr)(scui_surface_t *surface))
{
    /*例:
     *      refr异步工作:
     *      scui_surface_fb_refr_wait();
     *      scui_surface_t *suface = scui_surface_fb_refr();
     *      // refr surface...
     *      // 如果是DMA加速,此处额外等待一个信号量
     *      // 注意:这部分不能死等待,需要有一个超时机制
     *      scui_surface_fb_refr_unlock();
     *      scui_surface_fb_draw_notify();
     */
    
    SCUI_LOG_DEBUG("");
    scui_surface_fb_refr_wait();
    
    SCUI_LOG_DEBUG("");
    scui_surface_t *surface = scui_surface_fb_refr();
    SCUI_ASSERT(surface != NULL && surface->pixel != NULL);
    
    if (refr)
        refr(surface);
    
    SCUI_LOG_DEBUG("");
    scui_surface_fb_refr_unlock();
    SCUI_LOG_DEBUG("");
    scui_surface_fb_draw_notify();
}
