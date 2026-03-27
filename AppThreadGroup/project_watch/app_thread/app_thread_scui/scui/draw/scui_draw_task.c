/*实现目标:
 *    绘制任务流水线
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 绘制任务序列实例 */
static scui_draw_task_list_t scui_draw_task_list = {0};

/*@brief 任务节点哈希散列
 */
static void scui_draw_task_node_hash(scui_draw_task_node_t *task_node)
{
    #if SCUI_DRAW_TASK_SEQ
    /* 将dst_surface作为目标分割成网格 */
    /* 计算绘制区域在网格计算其散列位图 */
    /* 虽不精确但足以快速筛选节点的散列 */
    
    
    #endif
}

/*@brief 就绪绘制任务序列
 */
void scui_draw_task_ready(void)
{
    #if SCUI_DRAW_TASK_SEQ
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    /*备注:
     *为快速计算是否产生区域重合
     *我们不使用原来的求交集操作
     *而是用粗粒度的区域哈希运算
     *它并不正确而仅用于快速筛选
     */
    
    /* 哈希尺寸的要求: 1.是8的倍数 */
    /* 2.屏幕宽高比例与哈希宽高比保持一致 */
    SCUI_ASSERT(SCUI_DRAW_TASK_HASH_HBIT % 8 == 0);
    SCUI_ASSERT(SCUI_DRAW_TASK_HASH_VBIT % 8 == 0);
    SCUI_ASSERT(SCUI_DRAW_TASK_HASH_HBIT * SCUI_VER_RES ==
                SCUI_DRAW_TASK_HASH_VBIT * SCUI_HOR_RES);
    
    /* 哈希尺寸计算 */
    scui_coord_t hash_size = 1;
    hash_size *= SCUI_DRAW_TASK_HASH_HBIT / 8;
    hash_size *= SCUI_DRAW_TASK_HASH_VBIT / 8;
    hash_size += 1; /* 多余加1 */
    
    /* 绘制任务序列就绪 */
    scui_sem_process(&task_list->sem, scui_sem_static);
    scui_list_dll_reset(&task_list->dl_list);
    task_list->hash_size = hash_size;
    
    /* 初始给定一个信号量, 标记为绘制任务为空 */
    scui_sem_process(&task_list->sem, scui_sem_give);
    #endif
}

/*@brief 通知绘制任务序列
 *@param draw_dsc 绘制描述符
 */
void scui_draw_task_notify(scui_draw_dsc_t *draw_dsc)
{
    #if SCUI_DRAW_TASK_SEQ
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    scui_draw_task_node_t *task_node = NULL;
    
    scui_coord_t task_node_size = sizeof(scui_draw_task_node_t);
    task_node_size += task_list->hash_size;
    
    task_node = SCUI_MEM_ZALLOC(scui_mem_type_mix, task_node_size);
    task_node->draw_dsc = draw_dsc;
    
    scui_list_dln_reset(&task_node->dl_node);
    scui_list_dll_ainsert(&task_list->dl_list, NULL, &task_node->dl_node);
    
    scui_draw_task_node_hash(task_node);
    task_list->node_total++;
    task_list->node_frame++;
    #endif
}

/*@brief 派发绘制任务序列
 */
void scui_draw_task_dispatch(void)
{
    #if SCUI_DRAW_TASK_SEQ
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    scui_draw_task_node_t *task_node = NULL;
    uintptr_t list_node_cnt = 0;
    
    SCUI_LOG_WARN("draw task work:");
    SCUI_LOG_WARN("draw task total node:%d", task_list->node_total);
    SCUI_LOG_WARN("draw task frame node:%d", task_list->node_frame);
    
    #if 1
    /* 测试:就地响应目标 */
    for (scui_list_dln_t *list_node = NULL; true; true) {
        list_node = scui_list_dll_head(&task_list->dl_list);
        if (list_node == NULL) break;
        
        /* 取出这个实例,处理节点 */
        scui_list_dll_remove(&task_list->dl_list, list_node);
        
        list_node_cnt += 1;
        /* 就地响应目标(就地执行任务调度) */
        task_node = scui_own_ofs(scui_draw_task_node_t, dl_node, list_node);
        task_node->draw_dsc->sync = true;
        
        scui_draw_ctx_sched(task_node->draw_dsc);
        SCUI_MEM_FREE(task_node);
    }
    
    scui_sem_process(&task_list->sem, scui_sem_give);
    #else
    /* 激活子任务线程,让其自取任务处理 */
    /* 直到所有子任务都取不到任务为止(所有任务都完毕) */
    
    
    #endif
    
    SCUI_ASSERT(task_list->node_frame == list_node_cnt);
    /* 绘制任务序列尚未清空, 程序阻塞在此, 直到可用 */
    scui_sem_process(&task_list->sem, scui_sem_take);
    SCUI_LOG_WARN("draw task over:");
    task_list->node_frame = 0;
    #endif
}
