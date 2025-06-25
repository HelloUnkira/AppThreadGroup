/*实现目标:
 *    路径动画
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 动画实例句柄列表 */
static scui_anima_list_t scui_anima_list = {0};

/*@brief 动画事件吸收回调
 */
static bool scui_event_anima_elapse_absorb(void *evt_old, void *evt_new)
{
    scui_event_t *event_old = evt_old;
    scui_event_t *event_new = evt_new;
    
    /* 将ptr值转移到它上面: */
    event_old->tick += event_new->tick;
    return true;
}

/*@brief 距离上次动画嘀嗒数
 *       响应事件回调使用:scui_event_anima_elapse
 *@param tick 嘀嗒数
 */
uint32_t scui_anima_elapse_last(void)
{
    return scui_anima_list.elapse;
}

/*@brief 更新动画迭代数
 *@param elapse 过渡tick
 */
void scui_anima_elapse_new(uint32_t elapse)
{
    scui_anima_list.elapse += elapse;
    
    if (scui_anima_list.elapse >= SCUI_ANIMA_TICK) {
        scui_event_t event = {
            .object = SCUI_HANDLE_SYSTEM,
            .type   = scui_event_anima_elapse,
            .absorb = scui_event_anima_elapse_absorb,
            .tick   = 1,
        };
        scui_event_notify(&event);
    }
}

/*@brief 更新动画
 */
void scui_anima_update(void)
{
    scui_anima_t *anima = NULL;
    for (scui_handle_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        
        anima = scui_handle_source(scui_anima_list.list[idx]);
        scui_anima_list.refr_sched = false;
        
        /* 动画未运行 */
        if (!anima->running)
             continue;
        
        /* 起始调 */
        if (anima->first && anima->ready) {
            anima->first = false;
            anima->ready(anima);
        }
        /* 回调中销毁了动画实例 */
        if (scui_anima_list.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        /* 回调中销毁了动画实例,并且生成了新的动画 */
        if (scui_anima_list.refr_sched) {
            scui_anima_list.refr_sched = false;
            idx--;
            continue;
        }
        
        /* 动画tick迭代 */
        anima->reduce += scui_anima_list.elapse;
        /* 动画delay */
        if (anima->reduce < 0)
            continue;
        
        if (anima->reduce > anima->period)
            anima->reduce = anima->period;
        
        int32_t value_c = anima->path(anima->reduce,
            0, anima->period, anima->value_s, anima->value_e);
        
        /* 更新value */
        if (anima->value_c != value_c) {
            anima->value_c  = value_c;
            /* 过渡调 */
            if (anima->expire)
                anima->expire(anima);
            /* 回调中销毁了动画实例 */
            if (scui_anima_list.list[idx] == SCUI_HANDLE_INVALID)
                continue;
            /* 回调中销毁了动画实例,并且生成了新的动画 */
            if (scui_anima_list.refr_sched) {
                scui_anima_list.refr_sched = false;
                idx--;
                continue;
            }
        }
        
        /* 当次轮转未结束 */
        if (anima->reduce < anima->period)
            continue;
        
        bool reload = false;
        /* 当次轮转未结束 */
        if (anima->reduce >= anima->period)
            anima->value_c = 0;
        /* 常加载轮转 */
        if (anima->reload == SCUI_ANIMA_INFINITE)
            reload = true;
        /* 递减周期 */
        if (anima->reload != SCUI_ANIMA_INFINITE &&
            anima->reload != 0)
            anima->reload--;
        if (anima->reload != 0)
            reload = true;
        
        /* 重加载 */
        if (reload) {
            anima->reduce = -anima->delay;
            /* 支持双向回拨 */
            if (anima->replay) {
                anima->playback = anima->playback ? false : true;
                /* 这是互锁语义,除非回调干扰 */
                /* 此时交换value_s和value_e */
                int32_t value_ts = anima->value_s;
                int32_t value_te = anima->value_e;
                anima->value_s = value_te;
                anima->value_e = value_ts;
            }
            /* 更新value_c */
            if (anima->value_s < anima->value_e)
                anima->value_c = anima->value_s - 1;
            if (anima->value_s > anima->value_e)
                anima->value_c = anima->value_s + 1;
            continue;
        }
        
        /* 关闭动画 */
        anima->running = false;
        /* 结束调 */
        if (anima->finish)
            anima->finish(anima);
        /* 回调中销毁了动画实例 */
        if (scui_anima_list.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        /* 回调中销毁了动画实例,并且生成了新的动画 */
        if (scui_anima_list.refr_sched) {
            scui_anima_list.refr_sched = false;
            idx--;
            continue;
        }
    }
    
    /* 当次流逝已处理完毕,归零 */
    scui_anima_list.elapse = 0;
}

/*@brief 销毁动画
 *@param handle 动画句柄
 */
void scui_anima_destroy(scui_handle_t handle)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_anima_t *anima = NULL;
    
    for (scui_handle_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] != handle)
            continue;
        
        anima = scui_handle_source(scui_anima_list.list[idx]);
        scui_handle_clear(scui_anima_list.list[idx]);
        scui_anima_list.list[idx] = SCUI_HANDLE_INVALID;
        scui_anima_list.refr_sched = true;
        SCUI_MEM_FREE(anima);
        return;
    }
    
    /* 句柄实例错误 */
    SCUI_ASSERT(false);
}

/*@brief 创建动画
 *@param maker  动画构造器
 *@param handle 动画句柄
 */
void scui_anima_create(scui_anima_t *maker, scui_handle_t *handle)
{
    SCUI_ASSERT(maker != NULL && handle != NULL);
    scui_anima_t *anima = NULL;
    
    /* 重复性检查(销毁重复的旧动画) */
    for (scui_handle_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        
        anima = scui_handle_source(scui_anima_list.list[idx]);
        if (anima->object == maker->object &&
            anima->expire == maker->expire) {
            scui_anima_destroy(scui_anima_list.list[idx]);
        }
    }
    
    for (scui_handle_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] != SCUI_HANDLE_INVALID)
            continue;
        
        scui_anima_list.list[idx] = scui_handle_find();
        anima = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_anima_t));
        scui_handle_linker(scui_anima_list.list[idx], anima);
        *handle = scui_anima_list.list[idx];
        *anima = *maker;
        
        anima->handle   = scui_anima_list.list[idx];
        anima->reduce   = -anima->delay;
        anima->running  = false;
        anima->playback = false;
        anima->first    = true;
        /* 默认使用线性回调 */
        if (anima->path == NULL)
            anima->path  = scui_map_linear;
        /* 动画限制最小周期 */
        if (anima->period < SCUI_ANIMA_TICK)
            anima->period = SCUI_ANIMA_TICK;
        /* 默认值变动为周期 */
        if (anima->value_s == anima->value_e) {
            anima->value_e  = 100;
            anima->value_s  = 0;
        }
        /* 更新value_c */
        if (anima->value_s < anima->value_e)
            anima->value_c = anima->value_s - 1;
        if (anima->value_s > anima->value_e)
            anima->value_c = anima->value_s + 1;
        return;
    }
    
    /* 动画实例过多 */
    SCUI_LOG_ERROR("anima too much:");
    for (scui_handle_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        
        anima = scui_handle_source(scui_anima_list.list[idx]);
        SCUI_LOG_ERROR("expire:%p, object:%u, period:%u, reload:%u",
            anima->expire, anima->object, anima->period, anima->reload);
    }
}

/*@brief 开始动画
 *@param handle 动画句柄
 */
void scui_anima_start(scui_handle_t handle)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_anima_t *anima = NULL;
    
    for (scui_handle_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] != handle)
            continue;
            
        anima = scui_handle_source(scui_anima_list.list[idx]);
        anima->running = true;
        anima->first   = true;
        return;
    }
    /* 句柄实例错误 */
    SCUI_ASSERT(false);
}

/*@brief 结束动画
 *@param handle 动画句柄
 */
void scui_anima_stop(scui_handle_t handle)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_anima_t *anima = NULL;
    
    for (scui_handle_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] != handle)
            continue;
        
        anima = scui_handle_source(scui_anima_list.list[idx]);
        anima->running = false;
        return;
    }
    /* 句柄实例错误 */
    SCUI_ASSERT(false);
}

/*@brief 回收对象动画
 *@param handle 对象句柄
 */
void scui_anima_object_recycle(scui_handle_t handle)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_anima_t *anima = NULL;
    
    for (scui_handle_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        
        /* 跳过非目标对象动画 */
        anima = scui_handle_source(scui_anima_list.list[idx]);
        if (anima->object != handle)
            continue;
        
        /* 销毁该动画 */
        scui_anima_destroy(scui_anima_list.list[idx]);
    }
}

/*@brief 动画是否运行
 *@param handle 动画句柄
 *@retval 动画运行状态
 */
bool scui_anima_running(scui_handle_t handle)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_anima_t *anima = NULL;
    
    for (scui_handle_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] != handle)
            continue;
        
        anima = scui_handle_source(scui_anima_list.list[idx]);
        return anima->running;
    }
    /* 句柄实例错误 */
    SCUI_ASSERT(false);
}

/*@brief 动画实例
 *@param handle 动画句柄
 *@param anima  动画实例
 *@retval 动画实例有效性
 */
bool scui_anima_inst(scui_handle_t handle, scui_anima_t **anima)
{
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID && anima != NULL);
    *anima = NULL;
    
    for (scui_handle_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] != handle)
            continue;
        
        *anima = scui_handle_source(scui_anima_list.list[idx]);
        return true;
    }
    
    return false;
}

/*@brief 动画周期计算
 *@param speed_ms 速度(dist/ms)
 *@param dist_s   起始距离
 *@param dist_e   结束距离
 *@retval 周期
 */
int32_t scui_anima_peroid_calc(int32_t speed_ms, int32_t dist_s, int32_t dist_e)
{
    int32_t dist = scui_dist(dist_s, dist_e);
    int32_t period = dist * 1000 / speed_ms;
    
    return period;
}
