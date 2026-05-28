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
    
    #if SCUI_FRAME_BUFFER_SEG
    scui_area_t clip_seg;
    scui_area_t clip_seg_refr[2];
    #endif
} scui_frame_buffer_t;

static inline scui_area_t  scui_frame_buffer_area(void)
{return (scui_area_t){.w = SCUI_HOR_RES,.h = SCUI_VER_RES,};}

#if SCUI_FRAME_BUFFER_SEG

/*@brief 绘制画布段偏移重定向
 *@param dst_surface 绘制画布
 *@param dst_clip    绘制画布剪切域
 *@param dst_ofs     绘制画布偏移
 *@param src_clip    源画布剪切域
 *@param seg_ofs     画布段偏移
 *@retval 有效无效
 */
bool scui_frame_buffer_clip_seg(scui_surface_t *dst_surface,
    scui_area_t *dst_clip, scui_point_t *dst_ofs,
    scui_area_t *src_clip, scui_point_t *seg_ofs);

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
