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
 *@retval 相交:false;不相交:true;
 */
static bool scui_draw_task_node_check(scui_draw_task_node_t *task_node_t, scui_draw_task_node_t *task_node_c)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    #if SCUI_DRAW_TASK_SEQ
    for (scui_coord_t idx = 0; idx < task_list->hash_size; idx++) {
        uint8_t grid_unit_t = task_node_t->draw_clip[idx];
        uint8_t grid_unit_c = task_node_t->draw_clip[idx];
        
        if ((grid_unit_t & grid_unit_c) != 0)
            false;
    }
    #endif
    
    return true;
}

/*@brief 就绪绘制任务序列
 */
void scui_draw_task_ready(void)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    /* 绘制描述符资源就绪 */
    uintptr_t size_mem = sizeof(scui_draw_dsc_t) * SCUI_CACHE_DRAW_DSC_NUM;
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
    
    /* 绘制信号量就绪并初始标记 */
    scui_sem_process(&task_list->sem, scui_sem_static);
    scui_sem_process(&task_list->sem, scui_sem_give);
    #endif
}

/*@brief 派发绘制任务序列
 */
void scui_draw_task_dispatch(void)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    #if SCUI_DRAW_TASK_SEQ
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
        scui_draw_task_node_t *task_node = NULL;
        task_node = scui_own_ofs(scui_draw_task_node_t, dl_node, list_node);
        task_node->draw_dsc->sync = true;
        
        /* 优先使用硬件加速适配, 其次使用软件执行 */
        bool unsupport = !scui_draw_ctx_acc_sched(task_node->draw_dsc);
        if (unsupport) scui_draw_ctx_sched(task_node->draw_dsc);
        /* 全局唯一绘制描述符释放 */
        app_sys_mem_slab_free(task_list->slab_mem, task_node->draw_dsc);
        
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

/*@brief 绘制描述符实例申请
 *@param draw_dsc 绘制描述符实例地址
 */
void scui_draw_dsc_ready(scui_draw_dsc_t **draw_dsc)
{
    scui_draw_task_list_t *task_list = &scui_draw_task_list;
    
    /* 全局唯一绘制描述符申请 */
    SCUI_ASSERT(draw_dsc != NULL);
    *draw_dsc = app_sys_mem_slab_alloc(task_list->slab_mem);
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
    
    #if SCUI_DRAW_TASK_SEQ
    if (!draw_dsc->sync) {
        
        scui_coord_t task_node_size = sizeof(scui_draw_task_node_t);
        task_node_size += task_list->hash_size;
        
        scui_draw_task_node_t *task_node = NULL;
        task_node = SCUI_MEM_ZALLOC(scui_mem_type_mix, task_node_size);
        task_node->draw_dsc = draw_dsc;
        
        scui_list_dln_reset(&task_node->dl_node);
        scui_list_dll_ainsert(&task_list->dl_list, NULL, &task_node->dl_node);
        scui_draw_task_node_hash(task_node);
        task_list->node_total++;
        task_list->node_frame++;
        return;
    }
    #endif
    
    /* 单任务及同步绘制节点执行就地调度 */
    /* 优先使用硬件加速适配, 其次使用软件执行 */
    bool unsupport = !scui_draw_ctx_acc_sched(draw_dsc);
    if (unsupport) scui_draw_ctx_sched(draw_dsc);
    /* 全局唯一绘制描述符释放 */
    app_sys_mem_slab_free(task_list->slab_mem, draw_dsc);
}
