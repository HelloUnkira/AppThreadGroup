#include "app_lv_view_sched.h"
extern app_lv_view_sched_t app_lv_view_sched;

/*@beirf:
 *这部分不属于调度系统, 作为全局插件存在
 */

#if 0

/* 弹窗事件界面丢弃表 */
/* 弹窗事件界面默认按优先级加入到显示中去, 但是这里可以填表选择丢弃的指定弹窗 */
static const bool ui_plug_global_popup_discard[ui_plug_global_event_popup_num][ui_plug_global_event_popup_num] = {
    // [当前显示的弹窗界面对应的全局事件][现在需要判定的弹窗界面对应的全局事件] = 是否丢弃

};

// 可选事件优先级, 按等级(准备了7个,不够就自行调整数字)
#define UI_PLUG_GLOBAL_POPUP_PRIO_LOWEST            (0x01)
#define UI_PLUG_GLOBAL_POPUP_PRIO_LOW                (0x10)
#define UI_PLUG_GLOBAL_POPUP_PRIO_NORMAL_BELOW        (0x70)
#define UI_PLUG_GLOBAL_POPUP_PRIO_NORMAL            (0x80)
#define UI_PLUG_GLOBAL_POPUP_PRIO_NORMAL_ABOVE        (0x90)
#define UI_PLUG_GLOBAL_POPUP_PRIO_HIGH                (0xE0)
#define UI_PLUG_GLOBAL_POPUP_PRIO_HIGHEST            (0xFF)

/* 全局弹窗事件映射表 */
static const ui_plug_global_popup_t ui_plug_global_popup_table[ui_plug_global_event_popup_num] = {
    // [事件] = {.page_id = 界面ID号, .prio = 优先级(数字越大优先级越高[0, 0xFF])},
    
};

// show_page_id > page_id --- return true;
static bool ui_plug_global_popup_page_prio_cb(uint32_t show_page_id, uint32_t page_id)
{
    LV_LOG_INFO("%s %d", __func__, __LINE__);
    /* 息屏状态下的AOD界面, 默认最高优先级 */
    /* 理论上没有任何弹窗优先级高于它, 因为弹窗显示前需要退出息屏状态 */
    if (show_page_id == app_lv_view_id_standby)
        return true;
    
    /* 查优先级信息 */
    uint32_t show_page_id_prio = 0;
    for (uint32_t idx = 0; idx < ARRAY_LEN(ui_plug_global_popup_table); idx++) 
    {
        if (ui_plug_global_popup_table[idx].page_id != show_page_id)
        {
            continue;
        }

        show_page_id_prio = ui_plug_global_popup_table[idx].prio;
        break;
    }
    LV_LOG_INFO("%s %d", __func__, __LINE__);
    /* 常规界面默认优先级为0, 任意弹窗都可覆盖它 */
    if (show_page_id_prio == 0)
        return false;
    
    /* 查优先级信息 */
    uint32_t page_id_prio = 0;
    for (uint32_t idx = 0; idx < ARRAY_LEN(ui_plug_global_popup_table); idx++) 
    {
        if (ui_plug_global_popup_table[idx].page_id != page_id)
        {
            continue;
        }

        page_id_prio = ui_plug_global_popup_table[idx].prio;
        break;
    }
    
    LV_LOG_INFO("%s %d", __func__, __LINE__);
    // 没有逆向映射, 查询效率有点低
    return show_page_id_prio > page_id_prio;
}

/* 界面弹窗 */
void ui_plug_global_popup_event_proc_cb(ui_plug_global_event_t global_event, bool sleep_tag)
{
    if (global_event >= ui_plug_global_event_popup_num)
        return;
    
    /* 检查需要弹窗的界面优先级情况 */
    uint32_t priority_cur = 0;
    uint32_t page_id_cur = app_lv_view_id_unknown;
    for (uint32_t idx = 0; idx < ARRAY_LEN(ui_plug_global_popup_table); idx++) 
    {
        if (idx != global_event)
        {
            continue;
        }
        
        page_id_cur  = ui_plug_global_popup_table[idx].page_id;
        priority_cur = ui_plug_global_popup_table[idx].prio;
        break;
    }
    // 断言:没有找到事件的对应界面ID号, 事件弹窗优先级大于0
    LV_ASSERT(page_id_cur != 0);
    LV_ASSERT(priority_cur > 0 && priority_cur <= 0xFF);
    
    // 倒着查, 如果遇到一个高优先级事件与当前低优先级事件匹配到, 确认是否丢弃该事件
    for (uint32_t top = app_lv_view_stack_nest(); top > 0; top--) {
        
        /* 查优先级信息 */
        uint32_t priority_top = 0;
        uint32_t stack_top = app_lv_view_stack_node(top);
        uint32_t event_top = ui_plug_global_event_none;
        // 息屏特殊界面, 绕过它
        if (stack_top == app_lv_view_id_standby)
            continue;
        
        // 先查这个事件是否在优先级表中, 如果没有退出此流程
        for (uint32_t idx = 0; idx < ARRAY_LEN(ui_plug_global_popup_table); idx++) 
        {
            if (ui_plug_global_popup_table[idx].page_id != stack_top)
            {
                continue;
            }
            
            event_top = idx;
            priority_top = ui_plug_global_popup_table[idx].prio;
            break;
        }
        /* 遇到了低优先级界面, 查询结束 */
        if (priority_top <= priority_cur)
            break;
        
        // 低优先级弹窗, 检查是否丢弃
        if (ui_plug_global_popup_discard[event_top][global_event])
            return;
    }
    
    if (sleep_tag) {
        // 待定中:提醒事件不要动画
        app_lv_view_stack_add_prio_by(page_id_cur, false,

            app_lv_view_transform_type_none, LV_DIR_NONE,
            ui_plug_global_popup_page_prio_cb);
    } else {
        // 按优先级压入栈中
        app_lv_view_stack_add_prio(page_id_cur, false,
            ui_plug_global_popup_page_prio_cb);
    }
}

/* 界面弹窗退出 */
void ui_plug_global_popup_quit_event_proc_cb(ui_plug_global_event_t global_event, bool sleep_tag)
{
    uint32_t page_id = ui_plug_global_popup_table[global_event].page_id;
    if (page_id != 0) app_lv_view_stack_del(page_id);
}

#endif

/* 这个模组填充所有外源事件 */
/* 包括且不限于各种界面弹窗等等 */
void ui_plug_global_event_proc_cb(ui_plug_global_event_t global_event, bool sleep_tag)
{
    // 默认的外源事件都更新一次时间, 保持活跃态
    if (!sleep_tag) lv_disp_trig_activity(NULL);
    
    #if 0
    // 特殊系统事件
    switch (global_event) 
    {
        case ui_plug_global_event_sleep_enter:
            ui_plug_check_time_enter_sleep();
            return;
        case ui_plug_global_event_sleep_exit:
            ui_plug_check_time_exit_sleep();
            return;
        case ui_plug_global_event_deep_sleep_enter:
            ui_plug_check_time_enter_deep_sleep();
            return;
        case ui_plug_global_event_deep_sleep_exit:
            ui_plug_check_time_exit_deep_sleep();
            return;
    }
    
    // 通用外源事件
    switch (global_event)
    {
        case 0: break;
    }
    
    // 最后再响应事件弹窗
    ui_plug_global_popup_event_proc_cb(global_event, sleep_tag);
    #endif
}

#if 1
// 息屏倒计时以及息屏返回主界面倒计时
static ui_plug_check_time_t ui_plug_check_time = {0};

void ui_plug_check_time_enter_deep_sleep(void)
{
    #if APP_LV_VIEW_CHECK_TIME_USE == 0
    return;
    #endif
    
    if (ui_plug_check_time.dlps_tag)
        return;
    ui_plug_check_time.dlps_tag = true;
    LV_LOG_INFO("%s", __func__);
    // 关掉监控定时器自己
    // ui_plug_check_time_pause();
    
    #if 1
    // 实际动作在这个if宏包裹
    /* CPU都给关掉, 除去RTC,GSensor和KEY其他全关了 */
    #endif
}

void ui_plug_check_time_exit_deep_sleep(void)
{
    #if APP_LV_VIEW_CHECK_TIME_USE == 0
    return;
    #endif
    
    if (!ui_plug_check_time.dlps_tag)
        return;
    ui_plug_check_time.dlps_tag = false;
    LV_LOG_INFO("%s", __func__);
    // 启用监控定时器自己
    // ui_plug_check_time_resume();
    
    #if 1
    // 实际动作在这个if宏包裹
    #endif
}

void ui_plug_check_time_enter_sleep(void)
{
    #if APP_LV_VIEW_CHECK_TIME_USE == 0
    return;
    #endif
    
    if (ui_plug_check_time.sleep_tag)
        return;
    ui_plug_check_time.sleep_tag = true;
    LV_LOG_INFO("%s", __func__);
    
    app_lv_view_stack_add_by(app_lv_view_id_standby, false,
        app_lv_view_transform_type_none, LV_DIR_NONE);
    
    #if 1
    // 实际动作在这个if宏包裹
    void ui_display_enter_sleep(void);
    ui_display_enter_sleep();
    #endif
}

void ui_plug_check_time_exit_sleep(void)
{
    #if APP_LV_VIEW_CHECK_TIME_USE == 0
    return;
    #endif
    
    /* 退出dlps和退出sleep是同步进行的 */
    ui_plug_check_time_exit_deep_sleep();
    
    if (!ui_plug_check_time.sleep_tag)
        return;
    ui_plug_check_time.sleep_tag = false;
    LV_LOG_INFO("%s", __func__);
    
    // 关掉息屏界面
    app_lv_view_stack_del_by(app_lv_view_id_standby,
        app_lv_view_transform_type_none, LV_DIR_NONE);
    
    // 复位倒吊状态
    ui_plug_check_time.dlps = true;
    ui_plug_check_time.back = true;
    
    #if 1
    // 实际动作在这个if宏包裹
    void ui_display_exit_sleep(void);
    ui_display_exit_sleep();
    #endif
}

/* 填0使用默认的状态值, 默认填0即可 */
void ui_plug_check_time_reset(uint8_t over_time, uint8_t idle_time)
{
    ui_plug_check_time.over_time_bak = over_time != 0 ? over_time : ui_plug_check_time.over_time_def;
    ui_plug_check_time.idle_time_bak = idle_time != 0 ? idle_time : ui_plug_check_time.idle_time_def;
    
    ui_plug_check_time.dlps = true;
    ui_plug_check_time.back = true;
    ui_plug_check_time.over_time = ui_plug_check_time.over_time_bak;
    ui_plug_check_time.idle_time = ui_plug_check_time.idle_time_bak;
}

// 谨慎使用:修改系统默认值(那俩个宏枚举量)
void ui_plug_check_time_def_reset(uint8_t over_time, uint8_t idle_time)
{
    ui_plug_check_time.over_time_def = over_time != 0 ? over_time : APP_LV_VIEW_CHECK_TIME_OVER;
    ui_plug_check_time.idle_time_def = idle_time != 0 ? idle_time : APP_LV_VIEW_CHECK_TIME_IDLE;

    ui_plug_check_time_reset(0, 0);
}

/* 填0使用默认的状态值, 默认填0即可 */
void ui_plug_check_time_once_reset(uint8_t over_time, uint8_t idle_time)
{
    ui_plug_check_time.once = true;
    ui_plug_check_time.over_time_once = over_time != 0 ? over_time : ui_plug_check_time.over_time_def;
    ui_plug_check_time.idle_time_once = idle_time != 0 ? idle_time : ui_plug_check_time.idle_time_def;
    
    ui_plug_check_time.dlps = true;
    ui_plug_check_time.back = true;
    ui_plug_check_time.over_time = ui_plug_check_time.over_time_once;
    ui_plug_check_time.idle_time = ui_plug_check_time.idle_time_once;
}

// 一秒执行一次
void ui_plug_check_time_update(void)
{
    LV_LOG_WARN("%d - %d", ui_plug_check_time.over_time, ui_plug_check_time.idle_time);
    
    /* 合并到lvgl流程(未活跃时间小于1秒, 这里复位到开始) */
    if (lv_disp_get_inactive_time(NULL) < 1000) {
        if (ui_plug_check_time.once) {
            ui_plug_check_time.over_time = ui_plug_check_time.over_time_once;
            ui_plug_check_time.idle_time = ui_plug_check_time.idle_time_once;
        } else {
            ui_plug_check_time.over_time = ui_plug_check_time.over_time_bak;
            ui_plug_check_time.idle_time = ui_plug_check_time.idle_time_bak;
         }
    }
    
    
    
    /* 约减超时等待 */
    if (ui_plug_check_time.over_time != 0 &&
        ui_plug_check_time.over_time != APP_LV_VIEW_CHECK_TIME_OVER_MAX)
        ui_plug_check_time.over_time--;
    /* 主界面超时等待结束发送休眠请求 */
    if (ui_plug_check_time.over_time == 0) {
        if (ui_plug_check_time.dlps) {
            ui_plug_check_time.dlps = false;
            LV_LOG_WARN("ui idle dlps enter");
            ui_plug_check_time.once = false;
            
            ui_plug_check_time_enter_sleep();
        } else {
            bool dlps_tag = false;
            /* 约减空闲等待 */
            if (ui_plug_check_time.idle_time != 0 &&
                ui_plug_check_time.idle_time != APP_LV_VIEW_CHECK_TIME_IDLE_MAX)
                ui_plug_check_time.idle_time--;
            /* 空闲等待结束回到主界面 */
            if (ui_plug_check_time.idle_time == APP_LV_VIEW_CHECK_TIME_IDLE_MAX)
                dlps_tag = true;
            if (ui_plug_check_time.idle_time == 0) {
                if (ui_plug_check_time.back) {
                    ui_plug_check_time.back = false;
                    LV_LOG_WARN("ui idle back home");
                    
                    app_lv_view_stack_reset_by(app_lv_view_id_home_watch, true,
                        app_lv_view_transform_type_none, LV_DIR_NONE);
                    
                    dlps_tag = true;
                }
            }
            
            if (dlps_tag) {
                // 这里甚至已经完全结束了, 可以彻底进入低功耗模式
                // 但这有要求, 唤醒时要保证重新启用定时器
                // 先待定写到此处
                ui_plug_check_time_enter_deep_sleep();
            }
        }
    }
}

static void ui_plug_check_time_update_timer_cb(lv_timer_t *t)
{
    ui_plug_check_time_update();
}

void ui_plug_check_time_ready(void)
{
    #if APP_LV_VIEW_CHECK_TIME_USE == 0
    return;
    #endif
    
    ui_plug_check_time.dlps = true;
    ui_plug_check_time.back = true;
    ui_plug_check_time.over_time = APP_LV_VIEW_CHECK_TIME_OVER;
    ui_plug_check_time.idle_time = APP_LV_VIEW_CHECK_TIME_IDLE;
    ui_plug_check_time.over_time_bak = APP_LV_VIEW_CHECK_TIME_OVER;
    ui_plug_check_time.idle_time_bak = APP_LV_VIEW_CHECK_TIME_IDLE;
    ui_plug_check_time.over_time_def = APP_LV_VIEW_CHECK_TIME_OVER;
    ui_plug_check_time.idle_time_def = APP_LV_VIEW_CHECK_TIME_IDLE;
    ui_plug_check_time.sleep_tag = false;
    
    ui_plug_check_time.timer = lv_timer_create(ui_plug_check_time_update_timer_cb, 1000, NULL);
    lv_timer_resume(ui_plug_check_time.timer);
    // 这种定时器, 一定是常启动的
    
    ui_plug_check_time_reset(0, 0);
}

void ui_plug_check_time_pause(void)
{
    #if APP_LV_VIEW_CHECK_TIME_USE == 0
    return;
    #endif
    
    LV_LOG_INFO("%s", __func__);
    lv_timer_pause(ui_plug_check_time.timer);
}

void ui_plug_check_time_resume(void)
{
    #if APP_LV_VIEW_CHECK_TIME_USE == 0
    return;
    #endif
    
    LV_LOG_INFO("%s", __func__);
    lv_timer_resume(ui_plug_check_time.timer);
}

#endif

#if 1

lv_coord_t s_global_swipe_touch_x0 = 0;
lv_coord_t s_global_swipe_touch_y0 = 0;
bool s_global_swipe_obj_bubble = false;

void ui_plug_swipe_pressed_cb(lv_event_t * e)
{
    (void)e;
    lv_indev_t * indev = lv_indev_get_act();
    if(!indev) return;

    lv_point_t p;
    lv_indev_get_point(indev, &p);
    s_global_swipe_touch_x0 = p.x;
    s_global_swipe_touch_y0 = p.y;
    s_global_swipe_obj_bubble = false;
}

void ui_plug_swipe_release_cb(lv_event_t * e)
{
    (void)e;
    lv_indev_t * indev = lv_indev_get_act();
    if(!indev) return;

    lv_point_t p;
    lv_indev_get_point(indev, &p);
    
    // 备注:其实用不着算的这么复杂
    // 直接使用LV_EVENT_GESTURE事件即可

    /* X 轴右滑 > 60px，Y 轴偏移 < 50px 时判定为右滑返回 */
    if ((p.x - s_global_swipe_touch_x0 > 60) &&
        (LV_ABS(p.y - s_global_swipe_touch_y0) < 50)) {
        s_global_swipe_touch_x0 = 0;
        s_global_swipe_touch_y0 = 0;
        
        // 界面跳转或者跟手运动的时候, 不响应右滑
        if (app_lv_view_sched.anim_lock ||
            app_lv_view_sched.move_lock ||
            app_lv_view_sched.jump_lock) {
            LV_LOG_WARN("view switch");
            return;
        }
        
        // 存在控件可滚动中, 不要响应
        if (s_global_swipe_obj_bubble)
            return;
        
        app_lv_view_id_t page_id = app_lv_view_sched.page_c;
        
        
        app_lv_view_event_custom_param.type = app_lv_view_event_custom_swipe_r;
        app_lv_view_event_custom_param.stop = false;
        lv_obj_t *page_c = lv_obj_get_child(app_lv_view_sched.view_c, 0);
        lv_event_send(page_c, APP_LV_VIEW_EVENT_CUSTOM, APP_LV_VIEW_EVENT_CUSTOM_PARAM);
        if (app_lv_view_event_custom_param.stop)
            return;
        

        
        // 留白(不响应跟手滑动):
        // if (page_id == xxxxx)
        //     return;
        // 留白(其他特殊处理):
        // if (page_id == xxxxx)
        //     return;

        if (page_id && app_lv_view_stack_nest() > 1)
        {
            // 补丁:不再发送LV_EVENT_SHORT_CLICK
            lv_indev_t *indev = lv_indev_get_act();
            // indev->proc.short_click_lock = true;

            app_lv_view_stack_del(page_id);
        }
    }
}

void ui_plug_page_jump_click_event_cb(lv_event_t * e)
{
    // 获取当前输入设备
    lv_indev_t * indev = lv_indev_get_act();
    if(!indev) return;

    // 获取释放时的坐标
    lv_point_t release_point;
    lv_indev_get_point(indev, &release_point);

    // 计算从按下到释放的移动距离（使用全局变量 s_global_swipe_touch_x0, s_global_swipe_touch_y0）
    int dx = LV_ABS(release_point.x - s_global_swipe_touch_x0);
    int dy = LV_ABS(release_point.y - s_global_swipe_touch_y0);

    LV_LOG_INFO("click cb: s_touch_x0=%d, s_touch_y0=%d\n", s_global_swipe_touch_x0, s_global_swipe_touch_y0);
    LV_LOG_INFO("click cb: dx=%d, dy=%d", dx, dy);

    // 设定点击容忍的移动阈值（像素），可根据实际体验调整
    const int CLICK_THRESHOLD = 20;
    if (dx > CLICK_THRESHOLD || dy > CLICK_THRESHOLD) 
    {
        LV_LOG_INFO("___移动过大,不执行跳转___\n");// 移动过大，视为滑动，不执行跳转
        return;
    }

    // 否则执行页面跳转
    // const char *page_name = (const char *)lv_event_get_user_data(e);
    uint32_t page_id = (uint32_t)lv_event_get_user_data(e);
    if (page_id) 
    {
        LV_LOG_INFO("-----跳转-----\n");
        app_lv_view_stack_add(page_id, false);
    }
}

void yc_swipe_pressed_cb(lv_event_t * e)
{
    (void)e;
    lv_indev_t * indev = lv_indev_get_act();
    if(!indev) 
    {
        return;
    }
    lv_point_t p;
    lv_indev_get_point(indev, &p);
    s_global_swipe_touch_x0 = p.x;
    s_global_swipe_touch_y0 = p.y;
}

bool yc_is_valid_click(lv_event_t *e) 
{
    // 获取当前输入设备
    lv_indev_t *indev = lv_indev_get_act();
    if (!indev) 
    {
        return false;
    }
    // 获取释放时的坐标
    lv_point_t release_point;
    lv_indev_get_point(indev, &release_point);

    // 计算从按下到释放的移动距离（全局变量 s_touch_x0, s_touch_y0 需在按压时保存）
    int dx = LV_ABS(release_point.x - s_global_swipe_touch_x0);
    int dy = LV_ABS(release_point.y - s_global_swipe_touch_y0);

    // 调试打印（可选）
    LV_LOG_INFO("click cb: s_touch_x0=%d, s_touch_y0=%d\n", s_global_swipe_touch_x0, s_global_swipe_touch_y0);
    LV_LOG_INFO("click cb: dx=%d, dy=%d\n", dx, dy);

    // 设定点击容忍的移动阈值（像素）
    const int CLICK_THRESHOLD = 20;
    if (dx > CLICK_THRESHOLD || dy > CLICK_THRESHOLD) 
    {
        LV_LOG_INFO("___移动过大,不执行选中___\n");
        return false;
    }
    return true;
}

static bool ui_plug_view_event_stop = false;
void ui_plug_watch_global_view_event_stop(bool stop)
{
    ui_plug_view_event_stop = stop;
}

static lv_point_t s_global_touch_press = {0};
static lv_point_t s_global_touch_dist_max = {0};
static int32_t s_global_touch_dist_pow2 = 0;

bool ui_plug_watch_global_move_check(void)
{
    // 抖动值, 不为0, 在这个值为半径的圆内, 都视为不移动
    const int32_t dither_val  = UI_PLUG_WATCH_GLOBAL_SHORT_CLICK_DITHER;
    const int32_t dither_pow2 = dither_val * dither_val;
    return s_global_touch_dist_pow2 > dither_pow2;
}

void ui_plug_watch_global_view_event_cb(lv_event_t * e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    
    switch (lv_event_get_code(e)) {
    case LV_EVENT_PRESSED: {
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &s_global_touch_press);
        s_global_touch_dist_max = s_global_touch_press;
        s_global_touch_dist_pow2 = 0;
        break;
    }
    case LV_EVENT_PRESSING:
    case LV_EVENT_RELEASED: {
        lv_point_t touch_current = {0};
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &touch_current);
        
        int32_t touch_dist_x = app_lv_view_dist(s_global_touch_press.x, touch_current.x);
        int32_t touch_dist_y = app_lv_view_dist(s_global_touch_press.y, touch_current.y);
        int32_t touch_dist_pow2 = touch_dist_x * touch_dist_x + touch_dist_y * touch_dist_y;
        
        if (s_global_touch_dist_pow2 < touch_dist_pow2) {
            s_global_touch_dist_pow2 = touch_dist_pow2;
            s_global_touch_dist_max     = touch_current;
        }
        
        if (ui_plug_watch_global_move_check()) {
            // 补丁:不再发送LV_EVENT_SHORT_CLICK
            lv_indev_t *indev = lv_indev_get_act();
            // indev->proc.short_click_lock = true;
        }
        break;
    }
    }


    
    if (ui_plug_view_event_stop)
        return;
    
    switch (lv_event_get_code(e)) {
    case LV_EVENT_PRESSING:
        // 存在控件可滚动中, 不要响应
        if (app_lv_view_sched_obj_scrollable_bubble(obj, LV_DIR_RIGHT, NULL, NULL))
            s_global_swipe_obj_bubble = true;
        break;
        
    case LV_EVENT_PRESSED:
        ui_plug_swipe_pressed_cb(e);
        break;
    case LV_EVENT_RELEASED:
        ui_plug_swipe_release_cb(e);
        break;
    }
}

#endif

static lv_coord_t ui_global_enc_way  = 0;
static lv_coord_t ui_global_enc_step = 0;
static void (*ui_plug_watch_global_enc_cb)(uint8_t way, uint8_t step) = NULL;

void ui_plug_watch_global_enc_cb_register(void (*enc_cb)(uint8_t way, uint8_t step))
{
    ui_plug_watch_global_enc_cb = enc_cb;
}

void ui_plug_watch_global_enc_async_call(void *user_data)
{
    static uint32_t enc_event_tick_t = 0;
    static uint32_t enc_event_val_t  = 0;
    static lv_dir_t enc_event_dir_t  = 0;
    
    if (!ui_plug_check_time.sleep_tag)
        lv_disp_trig_activity(NULL);
    
    if (ui_global_enc_step == 0)
        return;
    
    /* 界面跳转时屏蔽编码器工作 */
    if (app_lv_view_sched.anim_lock ||
        app_lv_view_sched.move_lock ||
        app_lv_view_sched.jump_lock) {
        LV_LOG_WARN("view switch");
        ui_global_enc_step = 0;
        return;
    }
    
    lv_coord_t enc_way  = 0;
    lv_coord_t enc_step = ui_global_enc_step;
    ui_global_enc_step  = 0;
    /* 暂定如此, 方向反了换一下即可 */
    if (ui_global_enc_way == 0) enc_way = -1;
    else enc_way = +1;
    
    // 如果外源注册了编码器全局响应, 这里不再做处理
    if (ui_plug_watch_global_enc_cb != NULL) {
        ui_plug_watch_global_enc_cb(enc_way, enc_step);
        return;
    }
    
    app_lv_view_event_custom_param.type = app_lv_view_event_custom_enc;
    app_lv_view_event_custom_param.stop = false;
    app_lv_view_event_custom_param.enc.way = enc_way;
    app_lv_view_event_custom_param.enc.step = enc_step;
    lv_obj_t *page_c = lv_obj_get_child(app_lv_view_sched.view_c, 0);
    lv_event_send(page_c, APP_LV_VIEW_EVENT_CUSTOM, APP_LV_VIEW_EVENT_CUSTOM_PARAM);
    if (app_lv_view_event_custom_param.stop)
        return;
    
    lv_dir_t enc_dir = LV_DIR_NONE;
    if (enc_way == -1) enc_dir = LV_DIR_TOP | LV_DIR_LEFT;
    if (enc_way == +1) enc_dir = LV_DIR_BOTTOM | LV_DIR_RIGHT;
    LV_ASSERT(enc_dir != LV_DIR_NONE);
    LV_LOG_INFO("enc_dir:%x", enc_dir);
    
    /* 判断当前界面是否可以滚动 */
    app_lv_view_config_t *config = app_lv_view_id_inst(app_lv_view_sched.page_c);
    if (config->enc_scroll_way != LV_DIR_NONE)
        enc_dir &= config->enc_scroll_way;
    
    #if 1
    // 滚动条目
    lv_group_t *g = NULL;
    lv_obj_t *obj_scrollable = NULL;
    lv_coord_t obj_dist = 0;
    
    // 光电编码器有个全局的滚动调速, 一个单位移动的距离:
    lv_coord_t obj_diff_x = 3 * enc_step;
    lv_coord_t obj_diff_y = 3 * enc_step;
    
    if ((enc_dir & (LV_DIR_TOP | LV_DIR_LEFT)) != 0) {
        
        if ((enc_dir & LV_DIR_TOP) != 0)
        if (app_lv_view_sched_obj_scrollable(page_c, LV_DIR_TOP, &obj_scrollable, &obj_dist)) {
            LV_LOG_INFO("obj_scrollable:%p", obj_scrollable);
            lv_obj_scroll_by(obj_scrollable, 0, -LV_MIN(obj_diff_y, obj_dist), true);
            return;
        }
        
        if ((enc_dir & LV_DIR_LEFT) != 0)
        if (app_lv_view_sched_obj_scrollable(page_c, LV_DIR_LEFT, &obj_scrollable, &obj_dist)) {
            LV_LOG_INFO("obj_scrollable:%p", obj_scrollable);
            lv_obj_scroll_by(obj_scrollable, -LV_MIN(obj_diff_x, obj_dist), 0, true);
            return;
        }
    }
    
    if ((enc_dir & (LV_DIR_BOTTOM | LV_DIR_RIGHT)) != 0) {
        
        if ((enc_dir & LV_DIR_BOTTOM) != 0)
        if (app_lv_view_sched_obj_scrollable(page_c, LV_DIR_BOTTOM, &obj_scrollable, &obj_dist)) {
            LV_LOG_INFO("obj_scrollable:%p", obj_scrollable);
            lv_obj_scroll_by(obj_scrollable, 0, +LV_MIN(obj_diff_y, obj_dist), true);
            return;
        }
        
        if ((enc_dir & LV_DIR_RIGHT) != 0)
        if (app_lv_view_sched_obj_scrollable(page_c, LV_DIR_RIGHT, &obj_scrollable, &obj_dist)) {
            LV_LOG_INFO("obj_scrollable:%p", obj_scrollable);
            lv_obj_scroll_by(obj_scrollable, +LV_MIN(obj_diff_x, obj_dist), 0, true);
            return;
        }
    }
    #endif
    
    // 滚动窗口
    uint32_t enc_elapse = lv_tick_elaps(enc_event_tick_t);
    enc_event_tick_t = lv_tick_get();
    
    LV_LOG_INFO("enc_way:%d", enc_way);
    LV_LOG_INFO("enc_elapse:%d", enc_elapse);
    if (enc_elapse > 500 || enc_event_dir_t != enc_way) {
        enc_event_dir_t  = enc_dir;
        enc_event_val_t  = enc_step;
    } else {
        enc_event_val_t += enc_step;
    }
    
    // 连续累积多次滚动, 窗口切换
    if (enc_event_val_t > 20) {
        enc_event_val_t = 0;
        // 编码器的窗口切换, 通常都是cover模式的
        // 因为编码器是层级交互, 所以在当前层级相关的界面, 都是平行界面
        
        if ((enc_dir & LV_DIR_TOP) != 0 && config->enc_b && config->page_b != 0) {
            LV_LOG_WARN("enc switch");
            app_lv_view_stack_cover(config->page_b);
            return;
        }
        if ((enc_dir & LV_DIR_BOTTOM) != 0 && config->enc_t && config->page_t != 0) {
            LV_LOG_WARN("enc switch");
            app_lv_view_stack_cover(config->page_t);
            return;
        }
        if ((enc_dir & LV_DIR_LEFT) != 0 && config->enc_r && config->page_r != 0) {
            LV_LOG_WARN("enc switch");
            app_lv_view_stack_cover(config->page_r);
            return;
        }
        if ((enc_dir & LV_DIR_RIGHT) != 0 && config->enc_l && config->page_l != 0) {
            LV_LOG_WARN("enc switch");
            app_lv_view_stack_cover(config->page_l);
            return;
        }
    }
}

static uint32_t s_key1_click_page_id = 0;
static void (*ui_plug_watch_global_key0_cb)(uint8_t key_event) = NULL;
static void (*ui_plug_watch_global_key1_cb)(uint8_t key_event) = NULL;

uint32_t ui_plug_watch_global_key1_click_jump_page_id(uint32_t page_id)
{
    s_key1_click_page_id = page_id;
}

void ui_plug_watch_global_key0_cb_register(void (*key_cb)(uint8_t key_event))
{
    ui_plug_watch_global_key0_cb = key_cb;
}

void ui_plug_watch_global_key1_cb_register(void (*key_cb)(uint8_t key_event))
{
    ui_plug_watch_global_key1_cb = key_cb;
}

void ui_plug_watch_global_key0_async_call(void *user_data)
{
    uintptr_t key_event = (uintptr_t)user_data;
    // 其他的双击三击等等都参考此写法即可

    // 超时息屏退出:::
    lv_disp_trig_activity(NULL);
    if (app_lv_view_stack_top() == app_lv_view_id_standby) {
        app_lv_view_stack_del(app_lv_view_id_standby);
    }
    
    // 如果外源注册了按键全局响应, 这里不再做处理
    if (ui_plug_watch_global_key0_cb != NULL) {
        ui_plug_watch_global_key0_cb(key_event);
        return;
    }
    
    app_lv_view_event_custom_param.type = app_lv_view_event_custom_key0;
    app_lv_view_event_custom_param.stop = false;
    app_lv_view_event_custom_param.key.event = key_event;
    lv_obj_t *page_c = lv_obj_get_child(app_lv_view_sched.view_c, 0);
    lv_event_send(page_c, APP_LV_VIEW_EVENT_CUSTOM, APP_LV_VIEW_EVENT_CUSTOM_PARAM);
    if (app_lv_view_event_custom_param.stop)
        return;
    
    if (false) // if (key_event == KEY_ACTION_CLICK)
    {
        uint32_t stack_nest = app_lv_view_stack_nest();
        uint32_t stack_top  = app_lv_view_stack_top();
        
        // 浮窗跳回它们的主界面, 这里写浮窗的句柄
        // 并不一定是主表盘, 任意节目自己的浮窗都是返回它自己
        if (stack_top == app_lv_view_id_home_t ||
            stack_top == app_lv_view_id_home_b) {
            
            app_lv_view_stack_reset(app_lv_view_id_home_watch, false);
            return;
        }
        
        // 这里可以补充特殊需求, 如主表盘单击休眠(可选), 或者跳转到列表界面
        if (stack_top == app_lv_view_id_home_watch) {
            // 跳转到列表界面
            // app_lv_view_stack_add(app_lv_view_id_menu, false);
            return;
        }
        
        // 这里可以补充任何界面的自定义跳转逻辑, 或者说不跳转
        // 只要 if (stack_top == 指定界面句柄) { return;}
        
        
        // 一个默认流程, 任意界面返回上一层
        if (stack_nest > 1) {
            app_lv_view_stack_del(stack_top);
            return;
        } else {
            // 一级界面回到主表盘
            app_lv_view_stack_reset(app_lv_view_id_home_watch, false);
        }
    }
    if (false) // if (key_event == KEY_ACTION_LONG)
    {
        uint32_t stack_nest = app_lv_view_stack_nest();
        uint32_t stack_top  = app_lv_view_stack_top();
        
        // 参上
        
        // 如任意界面长按跳转到开关机界面(弹窗)
        // 并且退出开关机界面返回到任意节目
        // app_lv_view_stack_add(/* 开关机界面句柄 */, false);
    }
}

void ui_plug_watch_global_key1_async_call(void *user_data)
{
    uintptr_t key_event = (uintptr_t)user_data;
    // 其他的双击三击等等都参考此写法即可
    
    // 超时息屏退出:::
    lv_disp_trig_activity(NULL);
    if (app_lv_view_stack_top() == app_lv_view_id_standby) {
        app_lv_view_stack_del(app_lv_view_id_standby);
    }
    
    // 如果外源注册了按键全局响应, 这里不再做处理
    if (ui_plug_watch_global_key1_cb != NULL) {
        ui_plug_watch_global_key1_cb(key_event);
        return;
    }
    
    app_lv_view_event_custom_param.type = app_lv_view_event_custom_key1;
    app_lv_view_event_custom_param.stop = false;
    app_lv_view_event_custom_param.key.event = key_event;
    lv_obj_t *page_c = lv_obj_get_child(app_lv_view_sched.view_c, 0);
    lv_event_send(page_c, APP_LV_VIEW_EVENT_CUSTOM, APP_LV_VIEW_EVENT_CUSTOM_PARAM);
    if (app_lv_view_event_custom_param.stop)
        return;
    
    if (false) // if (key_event == KEY_ACTION_CLICK)
    {
        if (s_key1_click_page_id != 0) {


            app_lv_view_stack_add(s_key1_click_page_id, false);
        } else {
            // 默认跳到运动列表界面
        }
    }
    if (false) // if (key_event == KEY_ACTION_LONG)
    {
    }
}

/* 外部注册回调使用该接口通知事件 */
void ui_plug_global_event_notify(ui_plug_global_event_t global_event)
{
    if (ui_plug_check_time.sleep_tag) {
        // 退出低功耗(这里是真实亮屏)
        switch (global_event) {
        case ui_plug_global_event_deep_sleep_exit:
        case ui_plug_global_event_sleep_exit:
            break;
        }
    } else {
        // 进入低功耗(这里是真实熄屏)
        switch (global_event) {
        case ui_plug_global_event_deep_sleep_enter:
        case ui_plug_global_event_sleep_enter:
            break;
        }
    }
    
    
    
    // 事件产生时是否为息屏状态???
    bool sleep_tag = ui_plug_check_time.sleep_tag;
    // os_taskq_post_msg("ui", 7, 'u', 'i', 'e', 'v', 't', global_event, sleep_tag);
    
    
    
    if (ui_plug_check_time.sleep_tag) {
        // 退出低功耗(这里是真实亮屏)
        switch (global_event) {
        case ui_plug_global_event_deep_sleep_exit:
        case ui_plug_global_event_sleep_exit:
            break;
        }
        // 唤醒事件分俩类，如果只是唤醒，这里return丢弃掉
        // 如果唤醒后事件继续传递，这里不要return
        
        // 其他不唤醒的事件，是选择直接丢弃，还是继续保留？
    } else {
        
        // 进入低功耗(这里是真实熄屏)
        switch (global_event) {
        case ui_plug_global_event_deep_sleep_enter:
        case ui_plug_global_event_sleep_enter:
            break;
        }
        // 结构保留...
    }
    
}

/* way(0:正转;1:反转),val(步进值1~n) */
void app_lv_view_enc_notify(uintptr_t way, uintptr_t val)
{
    if (ui_plug_check_time.sleep_tag)
        return;
    
    val = LV_ABS(val);
    /* 暂时默认步进为1, 后续再做扩充合并 */
    if (way != ui_global_enc_way) {
        ui_global_enc_step -= val;
        
        if (ui_global_enc_step < 0) {
            ui_global_enc_step = -ui_global_enc_step;
            ui_global_enc_way  = way;
        }
    } else {
        ui_global_enc_step += val;
    }

    #if 1
    // os_taskq_post_msg("ui", 6, 'u', 'i', 'e', 'n', 'c', way);
    #else
    lv_async_call_cancel(ui_plug_watch_global_enc_async_call, NULL);
    lv_async_call(ui_plug_watch_global_enc_async_call, NULL);
    #endif
}

uint32_t app_lv_view_event_custom = 0;
app_lv_view_event_custom_t app_lv_view_event_custom_param = {0};

void ui_plug_event_ready(void)
{
    app_lv_view_event_custom = lv_event_register_id();
}

void key_event_cb_deal(void *arg)
{
    // struct key_event *key = arg;
    if (arg == NULL) return;
    
    // LV_LOG_INFO("key_value: 0x%x, event: %d\n", key->value, key->event);
    
    
    switch (-1) {// switch (key->event) {
    case 0: // case KEY_ACTION_CLICK:
        LV_LOG_WARN("global key:%d event click", key->value);
        if (ui_plug_check_time.sleep_tag) {
            ui_plug_global_event_notify(ui_plug_global_event_sleep_exit);
            return;    // 息屏唤醒:丢弃这个按键事件
        }
        break;
    case 1: // case KEY_ACTION_LONG:
        LV_LOG_WARN("global key:%d event long",  key->value);
        if (ui_plug_check_time.sleep_tag) {
            ui_plug_global_event_notify(ui_plug_global_event_sleep_exit);
            return;    // 息屏唤醒:丢弃这个按键事件
        }
        break;
    }
    
    #if 1
    // os_taskq_post_msg("ui", 7, 'u', 'i', 'k', 'e', 'y', key->value, key->event);
    #else
    #if 0
    if (key->value == KEY_IO_NUM0) {
        app_lv_view_enc_notify(0, 1);
    }
    if (key->value == KEY_IO_NUM1) {
        app_lv_view_enc_notify(1, 1);
    }
    #else
    if (key->value == KEY_IO_NUM0) {
        lv_async_call(ui_plug_watch_global_key0_async_call, (void *)(uintptr_t)(key->event));
    }
    if (key->value == KEY_IO_NUM1) {
        lv_async_call(ui_plug_watch_global_key1_async_call, (void *)(uintptr_t)(key->event));
    }
    #endif
    #endif
}
