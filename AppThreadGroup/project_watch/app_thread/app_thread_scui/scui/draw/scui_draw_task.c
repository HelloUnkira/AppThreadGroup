/*实现目标:
 *    绘制任务流水线
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 绘制任务序列实例 */
static scui_draw_task_list_t scui_draw_task_list = {0};

/*@brief 任务节点哈希散列
 *@param task_node 任务节点
 */
static void scui_draw_task_node_hash(scui_draw_task_node_t *task_node)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    #if SCUI_DRAW_TASK_SEQ
    /* 将dst_surface作为目标分割成网格 */
    /* 计算绘制区域在网格计算其散列位图 */
    /* 虽不精确但足以快速筛选节点的散列 */
    scui_draw_dsc_t *draw_dsc = task_node->draw_dsc;
    scui_surface_t *dst_surface = draw_dsc->task_src.surface;
    
    scui_area_t draw_clip = draw_dsc->task_src.clip;
    scui_area_t draw_area = scui_surface_area(dst_surface);
    scui_area_m_to_s(&draw_clip, &draw_clip);
    
    scui_area_t draw_grid = {0};
    draw_grid.x1 = ((draw_clip.x1 + 0) * SCUI_DRAW_TASK_HASH_HBIT) / draw_area.w;
    draw_grid.x2 = ((draw_clip.x2 + 1) * SCUI_DRAW_TASK_HASH_HBIT) / draw_area.w;
    draw_grid.y1 = ((draw_clip.y1 + 0) * SCUI_DRAW_TASK_HASH_VBIT) / draw_area.h;
    draw_grid.y2 = ((draw_clip.y2 + 1) * SCUI_DRAW_TASK_HASH_VBIT) / draw_area.h;
    draw_grid.x1 = scui_clamp(draw_grid.x1, 0, SCUI_DRAW_TASK_HASH_HBIT - 1);
    draw_grid.x2 = scui_clamp(draw_grid.x2, 0, SCUI_DRAW_TASK_HASH_HBIT - 1);
    draw_grid.y1 = scui_clamp(draw_grid.y1, 0, SCUI_DRAW_TASK_HASH_VBIT - 1);
    draw_grid.y2 = scui_clamp(draw_grid.y2, 0, SCUI_DRAW_TASK_HASH_VBIT - 1);
    
    task_node->draw_surface = dst_surface;
    uint8_t *baddr = task_node->draw_clip;
    for (uint32_t idx_l = draw_grid.y1; idx_l <= draw_grid.y2; idx_l++) {
         uint32_t pos_s = idx_l * SCUI_DRAW_TASK_HASH_HBIT + draw_grid.x1;
         uint32_t pos_e = idx_l * SCUI_DRAW_TASK_HASH_HBIT + draw_grid.x2;
         scui_bits_ext_set(baddr, pos_s, pos_e, 8);
         SCUI_LOG_DEBUG("pos_s:%d, pos_e:%d", pos_s, pos_e);
    }
    
    #if 0
    /* 这部分调试日志用宏单独圈出 */
    /* 本段代码仅验证网格是否正确 */
    SCUI_LOG_WARN("draw_clip:<%d,%d,%d,%d>",
        draw_clip.x1, draw_clip.y1, draw_clip.x2, draw_clip.y2);
    SCUI_LOG_WARN("draw_grid:<%d,%d,%d,%d>",
        draw_grid.x1, draw_grid.y1, draw_grid.x2, draw_grid.y2);
    for (uint32_t idx_y = 0; idx_y < SCUI_DRAW_TASK_HASH_VBIT; idx_y++) {
    for (uint32_t idx_x = 0; idx_x < SCUI_DRAW_TASK_HASH_HBIT / 8; idx_x++) {
         uint8_t *taddr = baddr + idx_y * SCUI_DRAW_TASK_HASH_HBIT / 8 + idx_x;
        
        for (scui_coord_t idx_b = 7; idx_b >= 0; idx_b--)
            SCUI_LOG_WARN_RAW("%c", (*taddr & (1 << idx_b)) ? '1' : '0');
        
    }
        SCUI_LOG_WARN_RAW(SCUI_LOG_LINE);
    }
    #endif
    #endif
}

/*@brief 任务节点相交匹配
 *@param task_node_t 目标任务节点
 *@param task_node_c 当前任务节点
 *@retval 相交:true;不相交:false;
 */
static bool scui_draw_task_node_inter(scui_draw_task_node_t *task_node_t, scui_draw_task_node_t *task_node_c)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    #if SCUI_DRAW_TASK_SEQ
    for (scui_coord_t idx = 0; idx < task_list->hash_size; idx++) {
        uint8_t grid_unit_t = task_node_t->draw_clip[idx];
        uint8_t grid_unit_c = task_node_c->draw_clip[idx];
        
        if ((grid_unit_t & grid_unit_c) != 0)
            return true;
    }
    #endif
    
    return false;
}

/*@brief 寻找一个任务节点
 *@param task_node 任务节点
 *@param task_idx  任务铭牌
 *@retval 成功失败
 */
static bool scui_draw_task_node_find(scui_draw_task_node_t **task_node, scui_coord_t task_idx)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    #if SCUI_DRAW_TASK_SEQ
    bool task_node_inter = false;
    scui_draw_task_node_t *task_node_c = NULL;
    scui_draw_task_node_t *task_node_t = NULL;
    scui_list_dll_btra(&task_list->dl_list, node) {
        task_node_c = scui_own_ofs(scui_draw_task_node_t, dl_node, node);
        /* 寻找一个适合当前子任务处理的任务节点 */
        bool swacc = scui_draw_ctx_acc_check(task_node_c->draw_dsc);
        if ((task_idx == 0 && !swacc) || (task_idx != 0 && swacc)) {
            /* 对节点逆向索引查重, 找到第一个不重复的节点 */
            task_node_inter = false;
            
            /* 与当前工作节点查重 */
            for (scui_coord_t idx = 0; idx < SCUI_DRAW_TASK_ASYNC_NUM; idx++) {
                
                task_node_t = task_list->task_node[idx];
                if (task_node_t == NULL)
                    continue;
                
                /* 多目标任务混合时, 任务序列仅异步一个目标 */
                if (task_node_t->draw_surface != task_node_c->draw_surface) {
                    task_node_inter = true;
                    break;
                }
                
                task_node_t = task_list->task_node[idx];
                if (scui_draw_task_node_inter(task_node_c, task_node_t)) {
                    task_node_inter = true;
                    break;
                }
            }
            /* 查重失败, 寻找下一个节点 */
            if (task_node_inter)
                continue;
            
            /* 与任务队列前的节点查重 */
            for (scui_list_dln_t *node_t = scui_list_dln_prev(node);
                node_t != NULL; node_t = scui_list_dln_prev(node_t)) {
                
                task_node_t = scui_own_ofs(scui_draw_task_node_t, dl_node, node_t);
                
                /* 多目标任务混合时, 任务序列仅异步一个目标 */
                if (task_node_t->draw_surface != task_node_c->draw_surface)
                    continue;
                
                if (scui_draw_task_node_inter(task_node_c, task_node_t)) {
                    task_node_inter = true;
                    break;
                }
            }
            /* 查重失败, 寻找下一个节点 */
            if (task_node_inter)
                continue;
            
            /* 查重成功, 匹配到一个节点 */
            *task_node = task_node_c;
            return true;
        }
    }
    #endif
    
    return false;
}

/*@brief 子任务获取任务铭牌
 *@param task_idx 任务铭牌编号
 */
static void scui_draw_task_find_idx(scui_coord_t *task_idx)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    #if SCUI_DRAW_TASK_SEQ
    SCUI_ASSERT(task_idx != NULL);
    scui_mutex_process(&task_list->sched_mutex, scui_mutex_take);
    SCUI_ASSERT(task_list->task_idx < SCUI_DRAW_TASK_ASYNC_NUM);
    *task_idx = task_list->task_idx; task_list->task_idx++;
    scui_mutex_process(&task_list->sched_mutex, scui_mutex_give);
    #endif
}

/*@brief 绘制子任务线程(软件:0;硬件:~0)
 */
static void scui_draw_task_sched(scui_coord_t task_idx)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    #if SCUI_DRAW_TASK_SEQ
    while (true) {
        /* 等待调度派发子任务 */
        scui_sem_process(&task_list->async_sem[task_idx], scui_sem_take);
        
        scui_draw_dsc_t *draw_dsc = NULL;
        /* 在全局下解出独立资源draw_dsc, 然后处理它 */
        scui_mutex_process(&task_list->sched_mutex, scui_mutex_take);
        SCUI_ASSERT(scui_bit_ext_get(task_list->async_busy, task_idx, 8));
        scui_draw_task_node_t *task_node = task_list->task_node[task_idx];
        if (task_node != NULL && task_node->draw_dsc != NULL) {
            SCUI_LOG_WARN("task %d node:%d sched", task_idx, task_node->draw_idx);
            draw_dsc = task_node->draw_dsc; task_node->draw_dsc = NULL;
        }
        scui_mutex_process(&task_list->sched_mutex, scui_mutex_give);
        
        
        
        if (draw_dsc) {
            draw_dsc->sync = true;
            /* scui_draw_dsc_task目前无法完全可重入, 此处的锁需要优化 */
            scui_mutex_process(&task_list->sched_mutex, scui_mutex_take);
            scui_draw_dsc_task(draw_dsc);
            scui_mutex_process(&task_list->sched_mutex, scui_mutex_give);
        }
        
        
        
        /* 标记当前任务空闲 */
        scui_mutex_process(&task_list->sched_mutex, scui_mutex_take);
        SCUI_LOG_INFO("async busy %d rst", task_idx);
        scui_bit_ext_rst(task_list->async_busy, task_idx, 8);
        scui_mutex_process(&task_list->sched_mutex, scui_mutex_give);
        
        /* 当前子任务空闲通知 */
        scui_sem_process(&task_list->sched_sem, scui_sem_give);
    }
    #else
    SCUI_ASSERT(false);
    #endif
}

/*@brief 就绪绘制任务序列
 */
void scui_draw_task_ready(void)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    /* 绘制描述符资源锁(单任务无效化) */
    scui_mutex_process(&task_list->dsc_mutex, scui_mutex_static);
    
    /* 绘制描述符资源就绪 */
    uintptr_t size_mem = sizeof(scui_draw_dsc_t) * SCUI_DRAW_TASK_DSC_NUM;
    size_mem += sizeof(app_sys_mem_slab_t) + sizeof(uintptr_t) * 4;
    task_list->slab_mem = SCUI_MEM_ALLOC(scui_mem_type_graph, size_mem);
    
    uintptr_t addr_mem = (uintptr_t)task_list->slab_mem;
    addr_mem += sizeof(app_sys_mem_slab_t) + sizeof(uintptr_t);
    app_sys_mem_slab_ready(task_list->slab_mem, addr_mem, size_mem, sizeof(scui_draw_dsc_t));
    
    #if SCUI_DRAW_TASK_SEQ
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
    hash_size *= SCUI_DRAW_TASK_HASH_VBIT;
    hash_size += 1; /* 多余加1 */
    
    /* 绘制任务序列就绪 */
    scui_list_dll_reset(&task_list->dl_list);
    task_list->hash_size = hash_size;
    SCUI_LOG_WARN("hash:%d", hash_size);
    
    /* 绘制任务信号量互斥锁就绪 */
    scui_sem_process(&task_list->sched_sem, scui_sem_static);
    scui_mutex_process(&task_list->sched_mutex, scui_mutex_static);
    for (scui_coord_t idx = 0; idx < SCUI_DRAW_TASK_ASYNC_NUM; idx++)
        scui_sem_process(&task_list->async_sem[idx], scui_sem_static);
    
    /* 初始给定调度信号量的激活状态(冗余) */
    /* scui_sem_process(&task_list->sched_sem, scui_sem_give); */
    
    /* 软件子任务铭牌固定为0 */
    task_list->task_idx = 1;
    #endif
}

/*@brief 软件绘制子任务调度
 *       优先级<硬件
 */
void scui_draw_task_sched_sw(void)
{
    scui_coord_t task_idx = 0;
    scui_draw_task_sched(task_idx);
}

/*@brief 硬件绘制子任务调度
 *       优先级<系统
 */
void scui_draw_task_sched_hw(void)
{
    scui_coord_t task_idx = 0;
    scui_draw_task_find_idx(&task_idx);
    scui_draw_task_sched(task_idx);
}

/*@brief 派发绘制任务序列
 */
void scui_draw_task_dispatch(void)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    #if SCUI_DRAW_TASK_SEQ
    uintptr_t list_node_cnt = 0;
    SCUI_LOG_WARN("draw task ready");
    SCUI_LOG_WARN("draw task total node:%d", task_list->node_total);
    SCUI_LOG_WARN("draw task frame node:%d", task_list->node_frame);
    
    /* unsupport yet */
    SCUI_ASSERT(false);
    
    #if 0
    /* 测试:就地响应目标,仅同步测试任务序列完备性 */
    for (scui_list_dln_t *list_node = NULL; true; true) {
        list_node = scui_list_dll_head(&task_list->dl_list);
        if (list_node == NULL) break;
        
        /* 取出这个实例, 处理节点 */
        scui_list_dll_remove(&task_list->dl_list, list_node);
        list_node_cnt += 1;
        
        /* 就地响应目标(就地执行任务调度) */
        scui_draw_task_node_t *task_node = NULL;
        task_node = scui_own_ofs(scui_draw_task_node_t, dl_node, list_node);
        task_node->draw_dsc->sync = true;
        scui_draw_dsc_task(task_node->draw_dsc);
        SCUI_MEM_FREE(task_node);
    }
    
    /* 没有异步任务, 直接退出, 亦无需信号量阻塞 */
    /* scui_sem_process(&task_list->sched_sem, scui_sem_give); */
    /* scui_sem_process(&task_list->sched_sem, scui_sem_take); */
    #else
    while (true) {
        
        bool task_idle = true;
        /* 获取当前任务忙碌状态, 存在子任务忙碌, 等待空闲 */
        scui_mutex_process(&task_list->sched_mutex, scui_mutex_take);
        for (scui_coord_t idx = 0; idx < SCUI_DRAW_TASK_ASYNC_NUM; idx++) {
            bool busy = scui_bit_ext_get(task_list->async_busy, idx, 8);
            if (busy) {task_idle = false; continue;}
            
            scui_draw_task_node_t *task_node = NULL;
            /* 回收之前旧的资源(冗余回收, 不一定存在) */
            task_node = task_list->task_node[idx];
            if (task_list->task_node[idx] != NULL) {
                task_list->task_node[idx]  = NULL;
                SCUI_ASSERT(task_node->draw_dsc == NULL);
                SCUI_MEM_FREE(task_node);
                task_node = NULL;
            }
            
            /* 如果任务派发队列不为空 */
            if (scui_list_dll_head(&task_list->dl_list) != NULL) {
                task_idle = false;
                
                /* 对该子任务尝试寻找一个绘制节点(直接加大锁, 熬大夜) */
                if (scui_draw_task_node_find(&task_node, idx)) {
                    SCUI_ASSERT(task_node != NULL);
                    SCUI_LOG_WARN("task %d node:%d submit", idx, task_node->draw_idx);
                    scui_list_dll_remove(&task_list->dl_list, &task_node->dl_node);
                    list_node_cnt += 1;
                    
                    /* 替子任务提前加忙等待(防止被自己意外处理) */
                    task_list->task_node[idx] = task_node;
                    SCUI_LOG_INFO("async busy %d set", idx);
                    scui_bit_ext_set(task_list->async_busy, idx, 8);
                    
                    /* 为子任务提交一个工作信号 */
                    scui_sem_process(&task_list->async_sem[idx], scui_sem_give);
                }
            }
        }
        scui_mutex_process(&task_list->sched_mutex, scui_mutex_give);
        /* 如果子任务都空闲, 调度流程结束 */
        if (task_idle) break;
        
        /* 等待任意一个子任务结束 */
        scui_sem_process(&task_list->sched_sem, scui_sem_take);
    }
    #endif
    
    SCUI_ASSERT(task_list->node_frame == list_node_cnt);
    SCUI_LOG_WARN("draw task finish");
    task_list->node_frame = 0;
    #endif
}

/*@brief 绘制描述符实例申请
 *@param draw_dsc 绘制描述符实例地址
 */
void scui_draw_dsc_ready(scui_draw_dsc_t **draw_dsc)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    /* 全局唯一绘制描述符申请 */
    SCUI_ASSERT(draw_dsc != NULL);
    scui_mutex_process(&task_list->dsc_mutex, scui_mutex_take);
    *draw_dsc = app_sys_mem_slab_alloc(task_list->slab_mem);
    scui_mutex_process(&task_list->dsc_mutex, scui_mutex_give);
    SCUI_ASSERT(*draw_dsc != NULL);
    
    /* 此处不使用memset, 使用者需要给定完整参数集, 未给定参数为未知值 */
    /* memset(*draw_dsc, 0, sizeof(scui_draw_dsc_t)); */
}

/*@brief 绘制任务序列准入
 *@param draw_dsc 绘制描述符
 */
void scui_draw_dsc_task(scui_draw_dsc_t *draw_dsc)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    #if SCUI_DRAW_TASK_SEQ == 0
    draw_dsc->sync = true;
    #endif
    
    if (draw_dsc->sync) {
        /* 同步绘制节点执行就地调度 */
        /* 优先使用硬件加速适配, 其次使用软件执行 */
        bool unsupport = !scui_draw_ctx_acc_sched(draw_dsc);
        if (unsupport) scui_draw_ctx_sched(draw_dsc);
        
        /* 全局唯一绘制描述符释放 */
        scui_mutex_process(&task_list->dsc_mutex, scui_mutex_take);
        app_sys_mem_slab_free(task_list->slab_mem, draw_dsc);
        scui_mutex_process(&task_list->dsc_mutex, scui_mutex_give);
        
    } else {
        #if SCUI_DRAW_TASK_SEQ
        scui_coord_t task_node_size = sizeof(scui_draw_task_node_t);
        task_node_size += task_list->hash_size;
        
        scui_draw_task_node_t *task_node = NULL;
        task_node = SCUI_MEM_ZALLOC(scui_mem_type_mix, task_node_size);
        task_node->draw_idx = task_list->node_frame;
        task_node->draw_dsc = draw_dsc;
        
        scui_list_dln_reset(&task_node->dl_node);
        scui_list_dll_ainsert(&task_list->dl_list, NULL, &task_node->dl_node);
        scui_draw_task_node_hash(task_node);
        task_list->node_total++;
        task_list->node_frame++;
        
        SCUI_LOG_INFO("task node:%d ready", task_node->draw_idx);
        #endif
    }
}
