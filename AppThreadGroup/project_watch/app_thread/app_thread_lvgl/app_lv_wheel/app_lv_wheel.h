#ifndef APP_LV_WHEEL_H
#define APP_LV_WHEEL_H

typedef struct {
    app_lv_scene_t *self;               // 中心界面
    app_lv_scene_t *sibling[4];         // 左兄弟,右兄弟,上兄弟,下兄弟
    enum {                              // 与兄弟的交互风格
        app_lv_wheel_style_none,
        app_lv_wheel_style_float,       // 浮动交互(淡入淡出)
        app_lv_wheel_style_rotate,      // 滚动交互
        app_lv_wheel_style_num,
    } style[4];
} app_lv_wheel_t;

typedef struct {
    /*  */
    uint8_t obj_idx;            // 操作对象索引
    app_lv_wheel_t *wheel;      // 轮盘实例
    lv_point_t click_pos;       // 位置记录:按下点
    lv_point_t resume_pos;      // 位置记录:回弹点
    lv_point_t arrive_pos;      // 位置记录:到达点
    lv_point_t scroll_pos;      // 位置记录:当前点
    lv_dir_t   scroll_way;      // 交互方向
    lv_coord_t threshold;       // 检测门限,位移值(自设定,参考值:30)
    lv_coord_t rollback_x;      // 水平交互,回退值(自设定,参考值:width  / 3)
    lv_coord_t rollback_y;      // 垂直交互,回退值(自设定,参考值:height / 3)
    lv_anim_t  anim_follow;     // 动画特效(跟手)
    uint16_t   anim_period;     // 动画特效:周期(ms)(自设定)
    uint8_t    scroll:1;        // 窗口内部产生滚动,轮盘不响应
    uint8_t    cover:1;         // 覆盖标记
    uint8_t    touch_over:1;    // 已经抬起,点击结束
    uint8_t    event_lock:1;    // 按下抬起事件(按下和抬起事件互锁)
                                // 因为轮盘事件是从所有事件中生成的汇总
                                // 所以准备重复事件拦截锁,因为我们只会有一次互锁响应
    /*  */
} app_lv_wheel_src_t;

/*@brief     lvgl wheel更新事件回调
 *           内部使用: 被lvgl线程使用
 *@param[in] wheel 轮盘实例
 */
void app_lv_wheel_update_handle(app_lv_wheel_t *wheel);

/*@brief  轮盘场景对象
 *@retval 轮盘场景对象
 */
lv_obj_t * app_lv_wheel_obj_inst(void);

/*@brief  轮盘场景
 *@retval 轮盘场景
 */
app_lv_wheel_t * app_lv_wheel_inst(void);

/*@brief  轮盘场景资源
 *@retval 轮盘场景资源
 */
app_lv_wheel_src_t * app_lv_wheel_src_inst(void);

#endif
