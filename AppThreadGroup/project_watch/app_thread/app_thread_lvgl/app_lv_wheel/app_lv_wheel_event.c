/*实现目标:
 *    lvgl轮盘场景事件抓取
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/*@brief 轮盘跟手特效触发门限后不断更新当前位置
 *       确定子窗口并更新它所在位置以及需要到达的位置
 */
static int8_t app_lv_wheel_threshold(app_lv_wheel_src_t *wheel_src, lv_point_t point)
{
    lv_obj_t *obj = NULL;
    app_lv_wheel_t *wheel = wheel_src->wheel;
    /* 获得最大位移距离,为参考检查方向 */
    lv_coord_t wheel_w = lv_obj_get_width(app_lv_wheel_obj_inst());
    lv_coord_t wheel_h = lv_obj_get_height(app_lv_wheel_obj_inst());
    lv_coord_t delta_x = app_sys_dist(wheel_src->click_pos.x, point.x);
    lv_coord_t delta_y = app_sys_dist(wheel_src->click_pos.y, point.y);
    /* 已经检查过门限则不需要再检查 */
    if (wheel_src->scroll_way != LV_DIR_NONE) {
        /* 中间捕获滑动,跟手动画以实际点为起点,不断地向point靠近 */
        obj = wheel->sibling[wheel_src->obj_idx]->root;
        /* 确定子窗口同时更新子窗口位置 */
        switch (wheel_src->obj_idx) {
        case 0: /* Scroll From Left */
            wheel_src->scroll_pos.x = point.x - wheel_w;
            break;
        case 1: /* Scroll From Right */
            wheel_src->scroll_pos.x = point.x;
            break;
        case 2: /* Scroll From Top */
            wheel_src->scroll_pos.y = point.y - wheel_h;
            break;
        case 3: /* Scroll From Bottom */
            wheel_src->scroll_pos.y = point.y;
            break;
        default:
            APP_SYS_ASSERT(wheel_src->obj_idx <= 4);
            break;
        }
        return -1;
    }
    /* 门限判别 */
    if (wheel_src->threshold > delta_x && wheel_src->threshold > delta_y)
        return 0;
    /* 浮动窗口方向滤除 */
    if (delta_x > delta_y) {
        /* 非指定类型的方向不响应 */
        if (wheel->style[0] != app_lv_wheel_style_float ||
            wheel->style[1] != app_lv_wheel_style_float)
            /* 存在未回滚的子窗口 */
            if (wheel_src->cover)
                return 0;
    } else {
        /* 非指定类型的方向不响应 */
        if (wheel->style[2] != app_lv_wheel_style_float ||
            wheel->style[3] != app_lv_wheel_style_float)
            /* 存在未回滚的子窗口 */
            if (wheel_src->cover)
                return 0;
    }
    /* 第一次捕获滑动,跟手动画以resume_pos为起点,不断地向point靠近 */
    wheel_src->resume_pos = (lv_point_t){.x = 0,.y = 0};
    wheel_src->arrive_pos = (lv_point_t){.x = 0,.y = 0};
    wheel_src->scroll_pos = (lv_point_t){.x = 0,.y = 0};
    /* 方向判别(相等情况下优先垂直方向):y */
    if (delta_x > delta_y) {
        /* Scroll From Left */
        if ((wheel_src->click_pos.x < point.x && !wheel_src->cover) ||
            (wheel_src->click_pos.x > point.x &&  wheel_src->cover)) {
            if (wheel->sibling[0] == NULL)
                return 0;
            if (wheel->sibling[0] == wheel->self)
                return 0;
            if (wheel->style[0] != app_lv_wheel_style_float &&
                wheel->style[0] != app_lv_wheel_style_rotate)
                return 0;
            if (wheel_src->cover && wheel_src->obj_idx == 1)
                return 0;
            wheel_src->obj_idx = 0;
            obj = wheel->sibling[wheel_src->obj_idx]->root;
            APP_SYS_ASSERT(obj != NULL);
            /* 确定子窗口同时更新子窗口位置 */
            wheel_src->resume_pos.x = wheel_src->cover ? 0 : -wheel_w;
            wheel_src->arrive_pos.x = wheel_src->cover ? -wheel_w : 0;
            wheel_src->scroll_pos.x = point.x - wheel_w;
        }
        /* Scroll From Right */
        if ((wheel_src->click_pos.x > point.x && !wheel_src->cover) ||
            (wheel_src->click_pos.x < point.x &&  wheel_src->cover)) {
            if (wheel->sibling[1] == NULL)
                return 0;
            if (wheel->sibling[1] == wheel->self)
                return 0;
            if (wheel->style[1] != app_lv_wheel_style_float &&
                wheel->style[1] != app_lv_wheel_style_rotate)
                return 0;
            if (wheel_src->cover && wheel_src->obj_idx == 0)
                return 0;
            wheel_src->obj_idx = 1;
            obj = wheel->sibling[wheel_src->obj_idx]->root;
            APP_SYS_ASSERT(obj != NULL);
            /* 确定子窗口同时更新子窗口位置 */
            wheel_src->resume_pos.x = wheel_src->cover ? 0 : +wheel_w;
            wheel_src->arrive_pos.x = wheel_src->cover ? +wheel_w : 0;
            wheel_src->scroll_pos.x = point.x;
        }
        wheel_src->scroll_way = LV_DIR_HOR;
        /* 不存在相等的情况 */
    } else {
        /* Scroll From Top */
        if ((wheel_src->click_pos.y < point.y && !wheel_src->cover) ||
            (wheel_src->click_pos.y > point.y &&  wheel_src->cover)) {
            if (wheel->sibling[2] == NULL)
                return 0;
            if (wheel->sibling[2] == wheel->self)
                return 0;
            if (wheel->style[2] != app_lv_wheel_style_float &&
                wheel->style[2] != app_lv_wheel_style_rotate)
                return 0;
            if (wheel_src->cover && wheel_src->obj_idx == 3)
                return 0;
            wheel_src->obj_idx = 2;
            obj = wheel->sibling[wheel_src->obj_idx]->root;
            APP_SYS_ASSERT(obj != NULL);
            /* 确定子窗口同时更新子窗口位置 */
            wheel_src->resume_pos.y = wheel_src->cover ? 0 : -wheel_h;
            wheel_src->arrive_pos.y = wheel_src->cover ? -wheel_h : 0;
            wheel_src->scroll_pos.y = point.y - wheel_h;
        }
        /* Scroll From Bottom */
        if ((wheel_src->click_pos.y > point.y && !wheel_src->cover) ||
            (wheel_src->click_pos.y < point.y &&  wheel_src->cover)) {
            if (wheel->sibling[3] == NULL)
                return 0;
            if (wheel->sibling[3] == wheel->self)
                return 0;
            if (wheel->style[3] != app_lv_wheel_style_float &&
                wheel->style[3] != app_lv_wheel_style_rotate)
                return 0;
            if (wheel_src->cover && wheel_src->obj_idx == 2)
                return 0;
            wheel_src->obj_idx = 3;
            obj = wheel->sibling[wheel_src->obj_idx]->root;
            APP_SYS_ASSERT(obj != NULL);
            /* 确定子窗口同时更新子窗口位置 */
            wheel_src->resume_pos.y = wheel_src->cover ? 0 : +wheel_h;
            wheel_src->arrive_pos.y = wheel_src->cover ? +wheel_h : 0;
            wheel_src->scroll_pos.y = point.y;
        }
        wheel_src->scroll_way = LV_DIR_VER;
        /* 不存在相等的情况 */
    }
    /* 起始位置在resume处 */
    lv_obj_set_x(obj, wheel_src->resume_pos.x);
    lv_obj_set_y(obj, wheel_src->resume_pos.y);
    /*  */
    return 1;
}

/*@brief 跟手特效按下事件响应
 */
static void app_lv_wheel_event_press(app_lv_wheel_src_t *wheel_src, lv_point_t point)
{
    app_lv_wheel_t *wheel = wheel_src->wheel;
    /* 获得当前点作为滚动点 */
    wheel_src->scroll_pos = point;
    /* 捕获到滑动以后,跟手动画以current_pos为起点,不断地向point点靠近 */
    int8_t state = app_lv_wheel_threshold(wheel_src, point);
    /* 未抓获滑动 */
    if (wheel_src->scroll_way == LV_DIR_NONE || state == 0)
        return;
    APP_SYS_LOG_INFO("LV_EVENT_PRESSING");
    lv_obj_t *obj = wheel->sibling[wheel_src->obj_idx]->root;
    /* 设置动画迭代参数 */
    if (wheel_src->scroll_way == LV_DIR_HOR)
        lv_anim_set_values(&wheel_src->anim_follow, lv_obj_get_x(obj),
                            wheel_src->scroll_pos.x);
    if (wheel_src->scroll_way == LV_DIR_VER)
        lv_anim_set_values(&wheel_src->anim_follow, lv_obj_get_y(obj),
                            wheel_src->scroll_pos.y);
    /* 跟手动画 */
    if (state == 1)
        lv_anim_start(&wheel_src->anim_follow);
    else
        /* 只有第一次跟手才需要启动动画播放,后面都使用本事件的定时器更新即可 */
        /* 动画没跑完,上面已经更新周期,让它继续跑下去即可,动画跑完了,手动更新 */
        if (lv_anim_get_playtime(&wheel_src->anim_follow) >= wheel_src->anim_period - 1) {
            if (wheel_src->scroll_way == LV_DIR_HOR)
                app_lv_wheel_anim_exec_cb(wheel_src, wheel_src->scroll_pos.x);
            if (wheel_src->scroll_way == LV_DIR_VER)
                app_lv_wheel_anim_exec_cb(wheel_src, wheel_src->scroll_pos.y);
        }
}

/*@brief 跟手特效抬起事件响应
 */
static void app_lv_wheel_event_release(app_lv_wheel_src_t *wheel_src, lv_point_t point)
{
    app_lv_wheel_t *wheel = wheel_src->wheel;
    /* 未抓获滚动方向 */
    if (wheel_src->scroll_way == LV_DIR_NONE)
        return;
    lv_obj_t *obj = wheel->sibling[wheel_src->obj_idx]->root;
    /* 动画将剩下未走完的行程执行完毕 */
    if (wheel_src->scroll_way == LV_DIR_HOR) {
        /*  回弹,前进 */
        if (LV_ABS(point.x - wheel_src->click_pos.x) < wheel_src->rollback_x)
            lv_anim_set_values(&wheel_src->anim_follow, lv_obj_get_x(obj), wheel_src->resume_pos.x);
        else {
            lv_anim_set_values(&wheel_src->anim_follow, lv_obj_get_x(obj), wheel_src->arrive_pos.x);
            wheel_src->cover = wheel_src->cover ? false : true;
        }
    }
    if (wheel_src->scroll_way == LV_DIR_VER) {
        /*  回弹,前进 */
        if (LV_ABS(point.y - wheel_src->click_pos.y) < wheel_src->rollback_y)
            lv_anim_set_values(&wheel_src->anim_follow, lv_obj_get_y(obj), wheel_src->resume_pos.y);
        else {
            lv_anim_set_values(&wheel_src->anim_follow, lv_obj_get_y(obj), wheel_src->arrive_pos.y);
            wheel_src->cover = wheel_src->cover ? false : true;
        }
    }
    lv_anim_start(&wheel_src->anim_follow);
    wheel_src->touch_over = true;
}

/*@brief 轮盘跟手特效回调
 */
void app_lv_wheel_event_cb(lv_event_t * e)
{
    app_lv_wheel_src_t *wheel_src = lv_event_get_user_data(e);
    app_lv_wheel_t *wheel = wheel_src->wheel;
    /* 获得落点 */
    lv_point_t  point = {0};
    lv_indev_t *indev = lv_indev_get_act();
    lv_indev_get_point(indev, &point);
    /* 我们只需要响应pointer类型的事件 */
    if (lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER)
        return;
    /* 重做LV_INDEV_TYPE_POINTER类型事件 */
    switch(lv_event_get_code(e)) {
    case LV_EVENT_GESTURE: {
        lv_dir_t dir = lv_indev_get_gesture_dir(indev);
        APP_SYS_LOG_INFO("LV_EVENT_GESTURE:%x", dir);
        /* 对于手势我们需要做一些额外的处理,但做在默认事件响应中 */
        break;
    }
    /* 窗口内部产生的滚动,此周期内的滑动完全放弃(不响应) */
    case LV_EVENT_SCROLL_BEGIN: {
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL_BEGIN");
        wheel_src->scroll = true;
        break;
    }
    case LV_EVENT_SCROLL_END: {
        APP_SYS_LOG_INFO("LV_EVENT_SCROLL_END");
        wheel_src->scroll = true;
        break;
    }
    case LV_EVENT_SCROLL: {
        APP_SYS_LOG_DEBUG("LV_EVENT_SCROLL");
        wheel_src->scroll = true;
        if (lv_indev_get_scroll_obj(indev) != NULL) {
            lv_obj_t *obj = lv_indev_get_scroll_obj(indev);
            APP_SYS_LOG_DEBUG("scroll_x:%d",        lv_obj_get_scroll_x(obj));
            APP_SYS_LOG_DEBUG("scroll_y:%d",        lv_obj_get_scroll_y(obj));
            APP_SYS_LOG_DEBUG("scroll_top:%d",      lv_obj_get_scroll_top(obj));
            APP_SYS_LOG_DEBUG("scroll_bottom:%d",   lv_obj_get_scroll_bottom(obj));
            APP_SYS_LOG_DEBUG("scroll_left:%d",     lv_obj_get_scroll_left(obj));
            APP_SYS_LOG_DEBUG("scroll_right:%d",    lv_obj_get_scroll_right(obj));
        }
        break;
    }
    /* 按下时,记录按下点 */
    case LV_EVENT_PRESSED: {
        /* 轮盘是否工作 */
        if (!wheel_src->onoff)
             break;
        /* 重复事件锁检查 */
        if (wheel_src->event_lock)
            break;
        APP_SYS_LOG_INFO("LV_EVENT_PRESSED");
        wheel_src->event_lock = true;
        wheel_src->click_pos  = point;
        wheel_src->touch_over = false;
        break;
    }
    /* lvgl会定期产生按压中事件,持续调整动画让它跟手走 */
    case LV_EVENT_PRESSING: {
        /* 轮盘是否工作 */
        if (!wheel_src->onoff)
             break;
        /* 重复事件锁检查 */
        if (!wheel_src->event_lock)
             break;
        if (wheel_src->touch_over)
            break;
        /* 子窗口滚动锁定 */
        if (wheel_src->scroll) {
            wheel_src->click_pos = point;
            break;
        }
        /* 按下事件 */
        app_lv_wheel_event_press(wheel_src, point);
        break;
    }
    /* 抬起时,进行回弹动画 */
    case LV_EVENT_PRESS_LOST:
    case LV_EVENT_RELEASED: {
        /* 轮盘是否工作 */
        if (!wheel_src->onoff)
             break;
        /* 重复事件锁检查 */
        if (!wheel_src->event_lock)
             break;
        if (wheel_src->touch_over)
            break;
        wheel_src->event_lock = false;
        APP_SYS_LOG_INFO("LV_EVENT_RELEASED");
        /* 子窗口滚动锁定 */
        if (wheel_src->scroll) {
            wheel_src->click_pos = point;
            break;
        }
        /* 抬起事件 */
        app_lv_wheel_event_release(wheel_src, point);
        break;
    }
    /* ... */
    default:
        break;
    }
}

/*@brief 轮盘跟手特效回调启停
 *@param onoff 开启或关闭
 */
void app_lv_wheel_event_onoff(bool onoff)
{
    app_lv_wheel_src_t *wheel_src = app_lv_wheel_src_inst();
    wheel_src->onoff = onoff;
}

/*@brief 轮盘跟手特效回滚(如果需要回滚)
 *@param state 0:浮动模式检查;1:手势模式检查
 *@param dir   滚动模式方向
 *@retval 事件响应或忽略
 */
bool app_lv_wheel_rollback(uint8_t state, lv_dir_t dir)
{
    app_lv_wheel_src_t *wheel_src = app_lv_wheel_src_inst();
    app_lv_wheel_t *wheel = wheel_src->wheel;
    
    /* 浮动模式回滚 */
    if (state == 0) {
        /* 非指定类型的回滚不响应 */
        switch (wheel->style[wheel_src->obj_idx]) {
        case app_lv_wheel_style_float:
            /* 存在未回滚的子窗口 */
            if (wheel_src->cover)
                break;
            return false;
        default:
            return false;
        }
        /* 动画将剩下未走完的行程执行完毕 */
        lv_obj_t *obj = wheel->sibling[wheel_src->obj_idx]->root;
        /* 提前解锁以放弃抬起事件 */
        wheel_src->event_lock = false;
        wheel_src->touch_over = true;
        wheel_src->cover      = false;
        /* 手动添加方向 */
        if (wheel_src->obj_idx == 0 || wheel_src->obj_idx == 1)
            wheel_src->scroll_way = LV_DIR_HOR;
        if (wheel_src->obj_idx == 2 || wheel_src->obj_idx == 3)
            wheel_src->scroll_way = LV_DIR_VER;
        /* 这里是回弹回去 */
        if (wheel_src->scroll_way == LV_DIR_HOR)
            lv_anim_set_values(&wheel_src->anim_follow, lv_obj_get_x(obj), wheel_src->resume_pos.x);
        if (wheel_src->scroll_way == LV_DIR_VER)
            lv_anim_set_values(&wheel_src->anim_follow, lv_obj_get_y(obj), wheel_src->resume_pos.y);
        lv_anim_start(&wheel_src->anim_follow);
        return true;
    }
    /* 手势模式回滚 */
    if (state == 1) {
        lv_coord_t wheel_w = lv_obj_get_width(app_lv_wheel_obj_inst());
        lv_coord_t wheel_h = lv_obj_get_height(app_lv_wheel_obj_inst());
        /* 非指定类型的回滚不响应 */
        uint8_t obj_idx = 4;
        switch (dir) {
        case LV_DIR_LEFT:   obj_idx = 1; break;
        case LV_DIR_RIGHT:  obj_idx = 0; break;
        case LV_DIR_TOP:    obj_idx = 3; break;
        case LV_DIR_BOTTOM: obj_idx = 2; break;
        default: return false;
        }
        if (wheel->sibling[obj_idx] == NULL)
            return false;
        switch (wheel->style[obj_idx]) {
        case app_lv_wheel_style_gesture:
        case app_lv_wheel_style_rotate:
            break;
        default:
            return false;
        }
        wheel_src->obj_idx = obj_idx;
        /* 动画将剩下未走完的行程执行完毕 */
        lv_obj_t *obj = wheel->sibling[wheel_src->obj_idx]->root;
        /* 更新回滚与前进方向 */
        switch (wheel_src->obj_idx) {
        case 0: /* Scroll From Left */
            wheel_src->resume_pos.x = wheel_src->cover ? 0 : -wheel_w;
            wheel_src->arrive_pos.x = wheel_src->cover ? -wheel_w : 0;
            break;
        case 1: /* Scroll From Right */
            wheel_src->resume_pos.x = wheel_src->cover ? 0 : +wheel_w;
            wheel_src->arrive_pos.x = wheel_src->cover ? +wheel_w : 0;
            break;
        case 2: /* Scroll From Top */
            wheel_src->resume_pos.y = wheel_src->cover ? 0 : -wheel_h;
            wheel_src->arrive_pos.y = wheel_src->cover ? -wheel_h : 0;
            break;
        case 3: /* Scroll From Bottom */
            wheel_src->resume_pos.y = wheel_src->cover ? 0 : +wheel_h;
            wheel_src->arrive_pos.y = wheel_src->cover ? +wheel_h : 0;
            break;
        default:
            return false;
        }
        /* 起始位置在resume处 */
        lv_obj_set_x(obj, wheel_src->resume_pos.x);
        lv_obj_set_y(obj, wheel_src->resume_pos.y);
        /* 提前解锁以放弃抬起事件 */
        wheel_src->event_lock = false;
        wheel_src->touch_over = true;
        wheel_src->cover      = true;
        /* 手动添加方向 */
        if (wheel_src->obj_idx == 0 || wheel_src->obj_idx == 1)
            wheel_src->scroll_way = LV_DIR_HOR;
        if (wheel_src->obj_idx == 2 || wheel_src->obj_idx == 3)
            wheel_src->scroll_way = LV_DIR_VER;
        /* 这里是前往兄弟 */
        if (wheel_src->scroll_way == LV_DIR_HOR)
            lv_anim_set_values(&wheel_src->anim_follow, wheel_src->resume_pos.x, wheel_src->arrive_pos.x);
        if (wheel_src->scroll_way == LV_DIR_VER)
            lv_anim_set_values(&wheel_src->anim_follow, wheel_src->resume_pos.y, wheel_src->arrive_pos.y);
        lv_anim_start(&wheel_src->anim_follow);
        return true;
    }
    /* 未识别模式 */
    return false;
}

/*@brief 轮盘跟手特效工作状态
 */
bool app_lv_wheel_status(lv_dir_t dir)
{
    app_lv_wheel_src_t *wheel_src = app_lv_wheel_src_inst();
    
    if (wheel_src->scroll_way & dir)
        return true;
    
    // if (lv_anim_get_playtime(&wheel_src->anim_follow) <= wheel_src->anim_period - 1)
    //     return true;
    
    return false;
}
