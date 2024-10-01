#ifndef SCUI_FRAME_BUFFER_H
#define SCUI_FRAME_BUFFER_H

/*@brief 帧缓冲区
 *       帧缓冲区是1或2个画布的集合
 */
typedef struct {
    scui_sem_t sem_draw;
    scui_sem_t sem_refr;
    scui_mutex_t mutex;
    scui_surface_t surface[2];
    uint8_t refr_hold:2;
    uint8_t draw_idx:1;
} scui_frame_buffer_t;

/*@brief 画布帧缓冲区刷新画布实例
 *@retval 刷新画布实例
 */
scui_surface_t * scui_frame_buffer_refr(void);

/*@brief 画布帧缓冲区绘制画布实例
 *@retval 绘制画布实例
 */
scui_surface_t * scui_frame_buffer_draw(void);

/*@brief 画布帧缓冲区初始化
 */
void scui_frame_buffer_ready(void);

/*@brief 绘制忙碌等待
 */
void scui_frame_buffer_draw_wait(void);

/*@brief 刷新画布触发
 */
void scui_frame_buffer_refr_toggle(void);

/*@brief 刷新画布流程
 *@param refr 刷新流程
 */
void scui_frame_buffer_refr_routine(void (*refr)(scui_surface_t *surface));

#endif
