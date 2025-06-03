#ifndef SCUI_ANIMA_H
#define SCUI_ANIMA_H

/*@brief 动画轮调:常循环
 */
#define SCUI_ANIMA_INFINITE     (0xFFFF)

/*@brief 动画回调:准备,过期,完成
 */
typedef void (*scui_anima_prepare_t)(void *anima);
typedef void (*scui_anima_expired_t)(void *anima);
typedef void (*scui_anima_finish_t)(void *anima);

/*@brief 动画
 */
typedef struct {
    scui_map_cb_t        path;      /* 动画行程回调 */
    scui_anima_prepare_t prepare;   /* 动画准备回调(第一次执行之前) */
    scui_anima_expired_t expired;   /* 动画过期回调 */
    scui_anima_finish_t  finish;    /* 动画完成回调(最后一次执行完之后) */
    int32_t delay;                  /* 起始迟延(周期) */
    int32_t period;                 /* 回调周期 */
    int32_t reload;                 /* 重载次数(0==1,常循环:SCUI_ANIMA_INFINITE) */
    int32_t replay:1;               /* 双向回播标记 */
    int32_t playback:1;             /* 双向回播方向(true:value_s和value_e被交换) */
    int32_t value_s;                /* 起始值(默认0) */
    int32_t value_e;                /* 结束值(默认100) */
    int32_t value_c;                /* 当前值(回调使用) */
    int32_t reduce;                 /* 内部使用:约减数 */
    int32_t running:1;              /* 内部使用:动画状态 */
    int32_t first:1;                /* 内部使用:prepare回调标记 */
    /* scui定制化动画数据: */
    scui_handle_t handle;
} scui_anima_t;

/*@brief 动画列表
 */
typedef struct {
    bool refr_sched;
    bool run_status;
    uint32_t elapse;
    scui_handle_t list[SCUI_ANIMA_LIMIT];
} scui_anima_list_t;

/*@brief 更新动画迭代数
 *@param elapse 过渡tick
 */
void scui_anima_elapse(uint32_t elapse);

/*@brief 更新动画
 *@param handle 动画句柄
 */
void scui_anima_update(scui_handle_t handle);

/*@brief 创建动画
 *@param anima  动画实例
 *@param handle 动画句柄
 */
void scui_anima_create(scui_anima_t *anima, scui_handle_t *handle);

/*@brief 创建动画
 *@param handle 动画句柄
 */
void scui_anima_destroy(scui_handle_t handle);

/*@brief 开始动画
 *@param handle 动画句柄
 */
void scui_anima_start(scui_handle_t handle);

/*@brief 结束动画
 *@param handle 动画句柄
 */
void scui_anima_stop(scui_handle_t handle);

/*@brief 动画实例
 *@param handle 动画句柄
 *@param anima  动画实例
 *@retval 动画实例有效性
 */
bool scui_anima_inst(scui_handle_t handle, scui_anima_t **anima);

/*@brief 动画是否运行
 *@param handle 动画句柄
 *@retval 动画运行状态
 */
bool scui_anima_running(scui_handle_t handle);

/*@brief 动画周期计算
 *@param speed_ms 速度(dist/ms)
 *@param dist_s   起始距离
 *@param dist_e   结束距离
 *@retval 周期
 */
uint32_t scui_anima_peroid_calc(uint32_t speed_ms, int32_t dist_s, int32_t dist_e);

/*@brief 距离上次动画嘀嗒数
 *       响应事件回调使用:scui_event_anima_elapse
 *@param tick 嘀嗒数
 */
uint32_t scui_anima_passby_tick(void);

#endif
