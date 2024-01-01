/*实现目标:
 *    帧缓冲管理
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
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
    /* 切换上一刷新块,它将成为新的绘制块 */
    #if SCUI_SURFACE_FB_LIMIT == 2
    app_mutex_process(&scui_surface_fb.mutex, app_mutex_take);
    scui_surface_fb.draw_idx = 1 - scui_surface_fb.draw_idx;
    app_mutex_process(&scui_surface_fb.mutex, app_mutex_give);
    #endif
}

/*@brief 锁定当前绘制块,并且切换绘制块
 *       刷新异步正式开始时执行它锁定绘制块
 *       绘制块实例需要在之前获取
 */
void scui_surface_fb_refr_lock(void)
{
    #if SCUI_SURFACE_FB_LIMIT == 2
    app_mutex_process(&scui_surface_fb.mutex, app_mutex_take);
    scui_surface_fb.refr_hold |= (1 < scui_surface_fb.draw_idx);
    app_mutex_process(&scui_surface_fb.mutex, app_mutex_give);
    #endif
}

/*@brief 解锁当前刷新块,并且产生信号
 */
void scui_surface_fb_refr_unlock(void)
{
    #if SCUI_SURFACE_FB_LIMIT == 2
    app_mutex_process(&scui_surface_fb.mutex, app_mutex_take);
    scui_surface_fb.refr_hold &= ~(1 < (1 - scui_surface_fb.draw_idx));
    app_mutex_process(&scui_surface_fb.mutex, app_mutex_give);
    #endif
}

/*@brief 等待刷新信号
 */
void scui_surface_fb_refr_wait(void)
{
    app_sem_process(&scui_surface_fb.sem_refr, app_sem_take);
}

/*@brief 产生刷新信号
 */
void scui_surface_fb_refr_notify(void)
{
    app_sem_process(&scui_surface_fb.sem_refr, app_sem_give);
}

/*@brief 等待绘制信号
 */
void scui_surface_fb_draw_wait(void)
{
    app_sem_process(&scui_surface_fb.sem_draw, app_sem_take);
}

/*@brief 产生绘制信号
 */
void scui_surface_fb_draw_notify(void)
{
    app_sem_process(&scui_surface_fb.sem_refr, app_sem_give);
}

/*@brief 画布帧缓冲区刷新画布实例
 *@retval 刷新画布实例
 */
scui_surface_t * scui_surface_fb_refr(void)
{
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
    app_sem_process(&scui_surface_fb.sem_draw, app_sem_static);
    app_sem_process(&scui_surface_fb.sem_refr, app_sem_static);
    #if SCUI_SURFACE_FB_LIMIT == 2
    app_mutex_process(&scui_surface_fb.mutex,  app_mutex_static);
    #endif
    
    /* 让draw流程变得可触发,之后循环互锁 */
    scui_surface_fb_draw_notify();
}
