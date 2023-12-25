#ifndef SCUI_SURFACE_FB_H
#define SCUI_SURFACE_FB_H

// 帧缓冲区:
// 帧缓冲区是1或2个画布的集合
typedef struct {
    scui_surface_t surface[SCUI_SURFACE_FB_LIMIT];
    scui_sem_t sem_draw;
    scui_sem_t sem_refr;
    #if SCUI_SURFACE_FB_LIMIT == 2
    scui_mutex_t mutex;
    uint8_t refr_hold:2;
    uint8_t draw_idx:1;
    #endif
} scui_surface_fb_t;

/*@brief 显示块与刷新块交换
 */
void scui_surface_fb_switch(void);

/*@brief 锁定当前绘制块,并且切换绘制块
 *       刷新异步正式开始时执行它锁定绘制块
 *       绘制块实例需要在之前获取
 */
void scui_surface_fb_refr_lock(void);

/*@brief 解锁当前刷新块,并且产生信号
 */
void scui_surface_fb_refr_unlock(void);

/*@brief 等待刷新信号
 */
void scui_surface_fb_refr_wait(void);

/*@brief 产生刷新信号
 */
void scui_surface_fb_refr_notify(void);

/*@brief 等待绘制信号
 */
void scui_surface_fb_draw_wait(void);

/*@brief 产生绘制信号
 */
void scui_surface_fb_draw_notify(void);

/*@brief 画布帧缓冲区刷新画布实例
 *@retval 刷新画布实例
 */
scui_surface_t * scui_surface_fb_refr(void);

/*@brief 画布帧缓冲区绘制画布实例
 *@retval 绘制画布实例
 */
scui_surface_t * scui_surface_fb_draw(void);

/*@brief 画布帧缓冲区初始化
 */
void scui_surface_fb_ready(void);

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
 *      
 *      
 *      
 *      refr异步工作:
 *      scui_surface_fb_refr_wait();
 *      scui_surface_t *suface = scui_surface_fb_refr();
 *      // refr surface...
 *      // 如果是DMA加速,此处额外等待一个信号量
 *      // 注意:这部分不能死等待,需要有一个超时机制
 *      scui_surface_fb_draw_notify();
 *      scui_surface_fb_refr_unlock();
 */

#endif
