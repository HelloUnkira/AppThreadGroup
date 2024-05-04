#ifndef SCUI_ANIMA_H
#define SCUI_ANIMA_H

/*@brief 动画更新频率,动画最大数量
 */
#define SCUI_ANIMA_TICK         (1000 / 60)
#define SCUI_ANIMA_LIMIT        (10)

/*@brief 缩放系数(Coefficient),为2的指数倍
 */
#define SCUI_ANIMA_RES_COF      (1024)
#define SCUI_ANIMA_RES_SHIFT    (10)

/*@brief 动画轮调:常循环
 */
#define SCUI_ANIMA_INFINITE     (0xFFFF)

/*@brief 动画的运动状态
 */
typedef enum {
    scui_anima_status_idle = 0,
    scui_anima_status_run,
    scui_anima_status_break,
} scui_anima_status_t;

/*@brief 动画行程回调
 */
typedef int32_t (*scui_anima_path_t)(void *anima);

/*@brief 动画开始回调(第一次执行开始之前的回调)
 */
typedef void (*scui_anima_start_t)(void *anima);

/*@brief 动画就绪回调(最后一次执行完之后的回调)
 */
typedef void (*scui_anima_ready_t)(void *anima);

/*@brief 动画过期回调
 */
typedef void (*scui_anima_expired_t)(void *anima);

/*@brief 动画
 */
typedef struct {
    scui_anima_path_t    path;      /* 动画行程回调 */
    scui_anima_start_t   start;     /* 动画开始回调 */
    scui_anima_ready_t   ready;     /* 动画就绪回调 */
    scui_anima_expired_t expired;   /* 动画过期回调 */
    uint32_t reload;                /* 重加载次数(0==1) */
    uint32_t peroid;                /* 回调周期 */
    int32_t  value_s;               /* 起始值(默认0) */
    int32_t  value_e;               /* 结束值(默认100) */
    int32_t  value_c;               /* 当前值(回调使用) */
    uint32_t reduce;                /* 内部使用:约减数 */
    uint32_t running:1;             /* 内部使用:动画状态 */
    uint32_t first:1;               /* 内部使用:第一次结束 */
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

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
int32_t scui_anima_path_step(void *instance);

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
int32_t scui_anima_path_linear(void *instance);

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
int32_t scui_anima_path_ease_in(void *instance);

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
int32_t scui_anima_path_ease_out(void *instance);

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
int32_t scui_anima_path_ease_in_out(void *instance);

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
int32_t scui_anima_path_overshoot(void *instance);

/*@brief 动画行程回调
 *@param instance 动画实例
 *@retval 动画进度值
 */
int32_t scui_anima_path_bounce(void *instance);

#endif
