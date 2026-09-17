#ifndef SCUI_OVERTIME_H
#define SCUI_OVERTIME_H

typedef struct {
    scui_handle_t anima;
    scui_sbitfd_t use:1;            /* 使用标记 */
    scui_sbitfd_t tick:18;          /* 超时等待 */
    scui_sbitfd_t tick_bak:18;      /* 超时时长 */
} scui_overtime_work_t;

/*@brief scui 超时模组重置
 *@param tick 超时时长(SCUI_OVERTIME_UNUSE)
 */
void scui_overtime_reset(uint32_t tick);

/*@brief scui 超时模组
 */
void scui_overtime_ready(void);

/*@brief scui 超时模组启停
 *@param work 启停
 */
void scui_overtime_work(bool work);

#endif
