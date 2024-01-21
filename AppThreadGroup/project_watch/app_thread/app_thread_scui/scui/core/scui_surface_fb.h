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

/*@brief 绘制画布例程(例)
 *@param draw 绘制流程
 */
void scui_surface_draw_routine(void (*draw)(scui_surface_t *surface));

/*@brief 刷新画布例程(例)
 *@param refr 刷新流程
 */
void scui_surface_refr_routine(void (*refr)(scui_surface_t *surface));

#endif
