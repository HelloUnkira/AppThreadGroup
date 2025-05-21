/*实现目标:
 *    帧缓冲管理
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_frame_buffer_t scui_frame_buffer = {0};

#if SCUI_MEM_FEAT_MINI

/*@brief 获得帧缓冲区段区域
 *@param clip_seg 段区域
 */
void scui_frame_buffer_seg(scui_area_t *clip_seg)
{
    *clip_seg = scui_frame_buffer.clip_seg;
}

/*@brief 就绪帧缓冲区段区域
 */
void scui_frame_buffer_seg_ready(void)
{
    scui_surface_t *surface_fb = scui_frame_buffer_draw();
    scui_frame_buffer.clip_seg = scui_surface_area(surface_fb);
}

/*@brief 偏移帧缓冲区段区域
 *@param 段区域有效性
 */
bool scui_frame_buffer_seg_offset(void)
{
    scui_area_t clip_seg = scui_frame_buffer.clip_seg;
    scui_coord_t hor_res = scui_disp_get_hor_res();
    scui_coord_t ver_res = scui_disp_get_ver_res();
    
    clip_seg.y += SCUI_FRAME_BUFFER_SEG;
    if (clip_seg.y + clip_seg.w > ver_res)
        clip_seg.w = ver_res - clip_seg.y;
    
    scui_frame_buffer.clip_seg = clip_seg;
    scui_area_t screen = {.w = hor_res,.h = ver_res,};
    return scui_area_inside(&screen, &scui_frame_buffer.clip_seg);
}

#endif

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
        #if SCUI_FRAME_BUFFER_ASYNC
        // refr_hold = scui_frame_buffer.refr_hold & (1 << scui_frame_buffer.draw_idx);
        #else
        scui_mutex_process(&scui_frame_buffer.mutex, scui_mutex_take);
        refr_hold = scui_frame_buffer.refr_hold & (1 << 0);
        scui_mutex_process(&scui_frame_buffer.mutex, scui_mutex_give);
        #endif
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
 *@param clip 屏幕剪切域
 */
void scui_frame_buffer_refr_routine(void (*refr)(scui_surface_t *surface, scui_area_t *clip))
{
    SCUI_LOG_INFO("wait refr sem");
    scui_sem_process(&scui_frame_buffer.sem_refr, scui_sem_take);
    
    scui_surface_t *surface = scui_frame_buffer_refr();
    SCUI_ASSERT(surface != NULL && surface->pixel != NULL);
    
    if (refr) {
        #if SCUI_MEM_FEAT_MINI
        scui_area_t clip_seg = {0};
        scui_frame_buffer_seg(&clip_seg);
        refr(surface, &clip_seg);
        #else
        refr(surface, NULL);
        #endif
    }
    
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
