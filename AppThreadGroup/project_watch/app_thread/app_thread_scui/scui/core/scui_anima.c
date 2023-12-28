/*实现目标:
 *    简易动画
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/* 动画实例句柄列表 */
static scui_anima_list_t scui_anima_list = {0};

/*@brief 更新动画迭代数
 *@param elapse 过渡tick
 */
void scui_anima_elapse_update(uint32_t elapse)
{
    scui_anima_list.elapse += elapse;
    
    if (scui_anima_list.elapse >= SCUI_ANIMA_TICK) {
        scui_event_t event = {
            .type = scui_event_anima_elapse,
        };
        scui_event_notify(&event);
    }
}

/*@brief 更新动画
 */
void scui_anima_update(void)
{
    for (uint32_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        
        scui_anima_t *anima = scui_handle_get(scui_anima_list.list[idx]);
        
        /* 动画未运行 */
        if (!anima->running)
             continue;
        
        /* 起始调 */
        if (anima->start && anima->first) {
            anima->first = false;
            anima->start(anima);
        }
        /* 回调中销毁了动画实例 */
        if (scui_anima_list.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        /* 回调中销毁了动画实例,并且生成了新的动画 */
        if (anima != scui_handle_get(scui_anima_list.list[idx])) {
            idx--;
            continue;
        }
        
        /* 动画tick迭代 */
        anima->reduce += scui_anima_list.elapse;
        if (anima->reduce > anima->peroid)
            anima->reduce = anima->peroid;
        
        uint32_t value_c = anima->path(anima);
        /* 更新value */
        if (anima->value_c != value_c) {
            anima->value_c  = value_c;
            /* 过渡调 */
            if (anima->expired)
                anima->expired(anima);
            /* 回调中销毁了动画实例 */
            if (scui_anima_list.list[idx] == SCUI_HANDLE_INVALID)
                continue;
            /* 回调中销毁了动画实例,并且生成了新的动画 */
            if (anima != scui_handle_get(scui_anima_list.list[idx])) {
                idx--;
                continue;
            }
        }
        
        bool reload = false;
        /* 当次轮转未结束 */
        if (anima->reduce < anima->peroid)
            reload = true;
        /* 常加载轮转 */
        if (anima->reload == SCUI_ANIMA_INFINITE)
            reload = true;
        /* 递减周期 */
        if (anima->reload != 0)
            anima->reload--;
        if (anima->reload != 0)
            reload = true;
        
        /* 重加载 */
        if (reload) {
            scui_anima_list.list[idx] = scui_handle_new();
            scui_handle_set(scui_anima_list.list[idx], anima);
            continue;
        }
        
        /* 结束调 */
        if (anima->ready)
            anima->ready(anima);
        /* 回调中销毁了动画实例 */
        if (scui_anima_list.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        /* 回调中销毁了动画实例,并且生成了新的动画 */
        if (anima != scui_handle_get(scui_anima_list.list[idx])) {
            idx--;
            continue;
        }
        /* 回收节点 */
        scui_handle_del(scui_anima_list.list[idx]);
        scui_anima_list.list[idx] = SCUI_HANDLE_INVALID;
        SCUI_MEM_FREE(anima);
    }
}

/*@brief 创建动画
 *@param anima  动画实例
 *@param handle 动画句柄
 */
void scui_anima_create(scui_anima_t *anima, scui_handle_t *handle)
{
    if (anima == NULL || handle == NULL) {
        APP_SYS_LOG_ERROR("invalid args");
        return;
    }
    
    scui_anima_t *item = anima;
    for (uint32_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++) {
        if (scui_anima_list.list[idx] != SCUI_HANDLE_INVALID)
            continue;
        scui_anima_list.list[idx]  = scui_handle_new();
        anima = SCUI_MEM_ALLOC(scui_mem_is_part, sizeof(scui_anima_t));
        scui_handle_set(scui_anima_list.list[idx], anima);
       *handle = scui_anima_list.list[idx];
       *anima = *item;
        anima->running = false;
        anima->reduce  = 0;
        anima->value_c = 0;
        anima->first   = 0;
        /* 默认使用线性回调 */
        if (anima->path == NULL)
            anima->path  = scui_anima_path_linear;
        /* 动画限制最小周期 */
        if (anima->peroid < SCUI_ANIMA_TICK)
            anima->peroid = SCUI_ANIMA_TICK;
        /* 默认值变动为周期 */
        if (anima->value_s == anima->value_e) {
            anima->value_e  = 100;
            anima->value_s  = 0;
        }
        return;
    }
    
    /* 动画实例过多 */
    APP_SYS_LOG_ERROR("anima too much:");
    for (uint32_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++)
        if (scui_anima_list.list[idx] != SCUI_HANDLE_INVALID) {
            anima = scui_handle_get(scui_anima_list.list[idx]);
            APP_SYS_LOG_ERROR("expired:%p, peroid:%u, reload:%u",
                              anima->expired, anima->peroid, anima->reload);
        }
}

/*@brief 创建动画
 *@param handle 动画句柄
 */
void scui_anima_destroy(scui_handle_t handle)
{
    if (handle == SCUI_HANDLE_INVALID) {
        APP_SYS_LOG_ERROR("invalid args");
        return;
    }
    
    for (uint32_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++)
        if (scui_anima_list.list[idx] == handle) {
            scui_anima_t *anima = scui_handle_get(scui_anima_list.list[idx]);
            scui_handle_del(scui_anima_list.list[idx]);
            scui_anima_list.list[idx] = SCUI_HANDLE_INVALID;
            SCUI_MEM_FREE(anima);
            return;
        }
    
    /* 句柄实例错误 */
    APP_SYS_ASSERT(false);
}

/*@brief 开始动画
 *@param handle 动画句柄
 */
void scui_anima_start(scui_handle_t handle)
{
    if (handle == SCUI_HANDLE_INVALID) {
        APP_SYS_LOG_ERROR("invalid args");
        return;
    }
    
    for (uint32_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++)
        if (scui_anima_list.list[idx] == handle) {
            scui_anima_t *anima = scui_handle_get(scui_anima_list.list[idx]);
            anima->running = false;
            return;
        }
    
    /* 句柄实例错误 */
    APP_SYS_ASSERT(false);
}

/*@brief 结束动画
 *@param handle 动画句柄
 */
void scui_anima_stop(scui_handle_t handle)
{
    if (handle == SCUI_HANDLE_INVALID) {
        APP_SYS_LOG_ERROR("invalid args");
        return;
    }
    
    for (uint32_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++)
        if (scui_anima_list.list[idx] == handle) {
            scui_anima_t *anima = scui_handle_get(scui_anima_list.list[idx]);
            anima->running = true;
            return;
        }
    
    /* 句柄实例错误 */
    APP_SYS_ASSERT(false);
}

/*@brief 动画是否运行
 *@param handle 动画句柄
 *@retval 动画运行状态
 */
bool scui_anima_running(scui_handle_t handle)
{
    if (handle == SCUI_HANDLE_INVALID) {
        APP_SYS_LOG_ERROR("invalid args");
        return false;
    }
    
    for (uint32_t idx = 0; idx < SCUI_ANIMA_LIMIT; idx++)
        if (scui_anima_list.list[idx] == handle) {
            scui_anima_t *anima = scui_handle_get(scui_anima_list.list[idx]);
            return anima->running;
        }
    
    /* 句柄实例错误 */
    APP_SYS_ASSERT(false);
}

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
uint32_t scui_anima_path_step(void *instance)
{
    scui_anima_t *anima = instance;
    
    if (anima->reduce <  anima->peroid)
        return anima->value_s;
    if (anima->reduce >= anima->peroid)
        return anima->value_e;
}

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
uint32_t scui_anima_path_linear(void *instance)
{
    scui_anima_t *anima = instance;
    
    /* 将当前时间进度映射到[0, SCUI_ANIMA_RES_COF] */
    uint32_t step = app_sys_map(anima->reduce, 0, anima->peroid, 0, SCUI_ANIMA_RES_COF);
    
    uint32_t value_c = 0;
    value_c   = step * (anima->value_e - anima->value_s);
    value_c >>= SCUI_ANIMA_RES_SHIFT;
    value_c  += anima->value_s;
    return value_c;
}

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
uint32_t scui_anima_path_ease_in(void *instance)
{
    scui_anima_t *anima = instance;
    
    /* 将当前时间进度映射到[0, SCUI_BEZIER_VAL_MAX] */
    uint32_t time = app_sys_map(anima->reduce, 0, anima->peroid, 0, SCUI_BEZIER_VAL_MAX);
    uint32_t step = scui_bezier3(time, 0, 50, 100, SCUI_BEZIER_VAL_MAX);
    
    uint32_t value_c = 0;
    value_c   = step * (anima->value_e - anima->value_s);
    value_c >>= SCUI_BEZIER_VAL_SHIFT;
    value_c  += anima->value_s;
    return value_c;
}

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
uint32_t scui_anima_path_ease_out(void *instance)
{
    scui_anima_t *anima = instance;
    
    /* 将当前时间进度映射到[0, SCUI_BEZIER_VAL_MAX] */
    uint32_t time = app_sys_map(anima->reduce, 0, anima->peroid, 0, SCUI_BEZIER_VAL_MAX);
    uint32_t step = scui_bezier3(time, 0, 900, 950, SCUI_BEZIER_VAL_MAX);
    
    uint32_t value_c = 0;
    value_c   = step * (anima->value_e - anima->value_s);
    value_c >>= SCUI_BEZIER_VAL_SHIFT;
    value_c  += anima->value_s;
    return value_c;
}

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
uint32_t scui_anima_path_ease_in_out(void *instance)
{
    scui_anima_t *anima = instance;
    
    /* 将当前时间进度映射到[0, SCUI_BEZIER_VAL_MAX] */
    uint32_t time = app_sys_map(anima->reduce, 0, anima->peroid, 0, SCUI_BEZIER_VAL_MAX);
    uint32_t step = scui_bezier3(time, 0, 50, 952, SCUI_BEZIER_VAL_MAX);
    
    uint32_t value_c = 0;
    value_c   = step * (anima->value_e - anima->value_s);
    value_c >>= SCUI_BEZIER_VAL_SHIFT;
    value_c  += anima->value_s;
    return value_c;
}

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
uint32_t scui_anima_path_overshoot(void *instance)
{
    scui_anima_t *anima = instance;
    
    /* 将当前时间进度映射到[0, SCUI_BEZIER_VAL_MAX] */
    uint32_t time = app_sys_map(anima->reduce, 0, anima->peroid, 0, SCUI_BEZIER_VAL_MAX);
    uint32_t step = scui_bezier3(time, 0, 1000, 1300, SCUI_BEZIER_VAL_MAX);
    
    uint32_t value_c = 0;
    value_c   = step * (anima->value_e - anima->value_s);
    value_c >>= SCUI_BEZIER_VAL_SHIFT;
    value_c  += anima->value_s;
    return value_c;
}

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
uint32_t scui_anima_path_bounce(void *instance)
{
    scui_anima_t *anima = instance;
    
    /* 将当前时间进度映射到[0, SCUI_BEZIER_VAL_MAX] */
    uint32_t time = app_sys_map(anima->reduce, 0, anima->peroid, 0, SCUI_BEZIER_VAL_MAX);
    uint32_t diff = anima->value_e - anima->value_s;
    
    /* 3反弹有5个部分:3个向下和2个向上,一部分是t/5长 */
    
    if (time < 408) {   /* 下降 */
        time *= 2500;
        time  = time >> SCUI_BEZIER_VAL_SHIFT;
    } else if (time < 614) {    /* 第一次回弹 */
        time -= 408;
        time *= 5;
        time  = SCUI_BEZIER_VAL_MAX - time;
        diff /= 20;
    } else if (time < 819) {    /* 后退 */
        time -= 614;
        time *= 5;
        diff /= 20;
    } else if (time < 921) {    /* 第二次回弹 */
        time -= 819;
        time *= 10;
        time  = SCUI_BEZIER_VAL_MAX - time;
        diff /= 40;
    } else if (time < SCUI_BEZIER_VAL_MAX) {    /* 后退 */
        time -= 921;
        time *= 10;
        diff /= 40;
    }
    
    if (time > SCUI_BEZIER_VAL_MAX)
        time = SCUI_BEZIER_VAL_MAX;
    
    uint32_t step = scui_bezier3(time, SCUI_BEZIER_VAL_MAX, 800, 500, 0);
    
    uint32_t value_c = 0;
    value_c   = step * diff;
    value_c >>= SCUI_BEZIER_VAL_SHIFT;
    value_c  += anima->value_s;
    return value_c;
}
