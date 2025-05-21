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
    
    #if SCUI_MEM_FEAT_MINI
    scui_area_t clip_seg;
    scui_area_t clip_seg_refr[2];
    #endif
} scui_frame_buffer_t;

#if SCUI_MEM_FEAT_MINI

/*@brief 获得帧缓冲区段区域
 *@param clip_seg 段区域
 */
void scui_frame_buffer_seg(scui_area_t *clip_seg);

/*@brief 就绪帧缓冲区段区域
 */
void scui_frame_buffer_seg_ready(void);

/*@brief 偏移帧缓冲区段区域
 *@param 段区域有效性
 */
bool scui_frame_buffer_seg_offset(void);

#endif

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
 *@param clip 屏幕剪切域
 */
void scui_frame_buffer_refr_routine(void (*refr)(scui_surface_t *surface, scui_area_t *clip));

#endif
