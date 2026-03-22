/*实现目标:
 *    绘制任务流水线
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

// 绘制任务序列实例
static scui_draw_task_list_t scui_draw_task_list_m = {0};
static scui_draw_task_list_t scui_draw_task_list_s[SCUI_DRAW_TASK_NUM] = {0};

/*@brief 就绪绘制任务序列
 */
void scui_draw_task_ready(void)
{
    #if SCUI_DRAW_TASK_SEQ
    scui_draw_task_list_t *task_list_m = &scui_draw_task_list_m;
    scui_draw_task_list_t *task_list_s = NULL;
    
    // 绘制任务序列就绪
    scui_sem_process(&task_list_m->sem, scui_sem_static);
    scui_list_dll_reset(&task_list_m->dl_list);
    
    for (scui_coord_t list_i = 0; list_i < SCUI_DRAW_TASK_NUM; list_i++) {
        task_list_s = &scui_draw_task_list_s[list_i];
        
        // 绘制任务序列就绪
        scui_sem_process(&task_list_s->sem, scui_sem_static);
        scui_list_dll_reset(&task_list_s->dl_list);
    }
    
    // 初始给定一个信号量, 标记为绘制任务为空
    scui_sem_process(&task_list_m->sem, scui_sem_give);
    #endif
}

/*@brief 通知绘制任务序列
 *@param draw_dsc 绘制描述符
 */
void scui_draw_task_notify(scui_draw_dsc_t *draw_dsc)
{
    #if SCUI_DRAW_TASK_SEQ
    scui_draw_task_list_t *task_list_m = &scui_draw_task_list_m;
    scui_draw_task_node_t *task_node_t = NULL;
    
    task_node_t = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_draw_task_node_t));
    task_node_t->draw_dsc = draw_dsc;
    
    scui_list_dln_reset(&task_node_t->dl_node);
    scui_list_dll_ainsert(&task_list_m->dl_list, NULL, &task_node_t->dl_node);
    #endif
}

/*@brief 派发绘制任务序列
 */
void scui_draw_task_dispatch(void)
{
    #if SCUI_DRAW_TASK_SEQ
    scui_draw_task_list_t *task_list_m = &scui_draw_task_list_m;
    scui_draw_task_list_t *task_list_s = NULL;
    scui_draw_task_node_t *task_node_t = NULL;
    
    #if 1
    // 测试:就地响应目标
    scui_list_dln_t *list_node = NULL;
    while ((list_node = scui_list_dll_head(&task_list_m->dl_list)) != NULL) {
        scui_list_dll_remove(&task_list_m->dl_list, list_node);
        
        // 就地响应目标(就地执行任务调度)
        task_node_t = scui_own_ofs(scui_draw_task_node_t, dl_node, list_node);
        task_node_t->draw_dsc->sync = true;
        
        
        static uint32_t cnt = 0; cnt++;
        if (cnt % 10 == 0) SCUI_LOG_WARN("draw_ctx: %d", cnt);
        
        scui_draw_ctx_sched(task_node_t->draw_dsc);
        SCUI_MEM_FREE(task_node_t);
    }
    
    scui_sem_process(&task_list_m->sem, scui_sem_give);
    #else
    // 步调1:对绘制目标进行乱序重排
    // 步调2:将任务整理到各个子序列
    // 步调3:让子序列自取任务直到任务队列清空
    
    #endif
    
    /* 绘制任务序列尚未清空, 程序阻塞在此, 直到可用 */
    scui_sem_process(&task_list_m->sem, scui_sem_take);
    #endif
}
