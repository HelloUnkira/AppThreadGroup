/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 拆分部件,这部分直接引用 */
extern scui_window_mgr_t scui_window_mgr;

/*@brief 窗口切换风格自动更新
 *@param type 窗口切换风格
 *@param dir  窗口切换方向
 */
static void scui_window_switch_type_update(scui_window_switch_type_t type, scui_opt_dir_t dir)
{
    scui_window_mgr.switch_args.dir  = dir;
    scui_window_mgr.switch_args.type = type;
    /* 自适应需要更新窗口切换状态 */
    if (scui_window_mgr.switch_args.type == scui_window_switch_auto)
        scui_window_mgr.switch_args.type  = scui_window_mgr.switch_args.cfg_type;
    /* 自适应需要更新窗口切换状态 */
    if (scui_window_mgr.switch_args.dir == scui_opt_dir_none)
        scui_window_mgr.switch_args.dir  = scui_window_mgr.switch_args.cfg_dir;
}

/*@brief 窗口隐藏
 *@param handle 窗口句柄
 *@param any    所有或者仅含有资源的窗口
 */
static void scui_window_switch_hide_without(scui_handle_t handle, bool any)
{
    SCUI_LOG_INFO("");
    
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++)
        if (scui_window_mgr.list[idx] != SCUI_HANDLE_INVALID &&
            scui_window_mgr.list[idx] != handle) {
            scui_widget_t *widget = scui_handle_source_check(scui_window_mgr.list[idx]);
            scui_window_t *window = (void *)widget;
            SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
            if (any || (scui_widget_surface_only(widget) && !window->resident))
                scui_widget_hide(scui_window_mgr.list[idx], false);
        }
}

/*@brief 窗口切换动画配置
 *@param anima 动画实例
 */
static void scui_window_switch_anima_cfg(scui_anima_t *anima)
{
    switch (scui_window_mgr.switch_args.type) {
    case scui_window_switch_cube:
    case scui_window_switch_flip2:
    case scui_window_switch_grid:
    case scui_window_switch_circle:
    case scui_window_switch_center_inout:
    case scui_window_switch_center_out:
    case scui_window_switch_center_in:
    case scui_window_switch_cover_out:
    case scui_window_switch_cover_in:
        anima->path = scui_map_ease_in_out;
        anima->period = anima->period;
        break;
    case scui_window_switch_flip1:
    case scui_window_switch_zoom2:
    case scui_window_switch_zoom1:
    case scui_window_switch_move:
        // 非叠加类型的移动可适合超调效果
        anima->path = scui_map_overshoot;
        anima->period = anima->period * 3 / 2;
        // 非叠加类型的移动可适合重力回弹效果
        anima->path = scui_map_bounce;
        anima->period = anima->period * 3 / 2;
        break;
    default:
        anima->path = scui_map_linear;
        anima->period = anima->period;
        break;
    }
}

/*@brief 窗口跳转动画回调
 */
static void scui_window_jump_anima_prepare(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief 窗口跳转动画回调
 */
static void scui_window_jump_anima_expired(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t  *anima  = instance;
    scui_point_t   point  = {0};
    scui_widget_t *widget = NULL;
    
    scui_multi_t ofs = 0;
    if (scui_opt_bits_check(scui_window_mgr.switch_args.dir, scui_opt_dir_ver))
        ofs = SCUI_VER_RES;
    if (scui_opt_bits_check(scui_window_mgr.switch_args.dir, scui_opt_dir_hor))
        ofs = SCUI_HOR_RES;
    
    ofs *= anima->value_c;
    ofs /= 100;
    
    scui_window_mgr.switch_args.pct = anima->value_c;
    scui_window_mgr.switch_args.ofs = ofs;
    
    /* 更新point: */
    switch (scui_window_mgr.switch_args.dir) {
    case scui_opt_dir_to_u:
        point.y = -scui_window_mgr.switch_args.ofs;
        scui_widget_move_pos(scui_window_mgr.switch_args.list[0], &point);
        point.y += +scui_widget_clip(scui_window_mgr.switch_args.list[0]).h;
        scui_widget_move_pos(scui_window_mgr.switch_args.list[1], &point);
        break;
    case scui_opt_dir_to_d:
        point.y = +scui_window_mgr.switch_args.ofs;
        scui_widget_move_pos(scui_window_mgr.switch_args.list[0], &point);
        point.y += -scui_widget_clip(scui_window_mgr.switch_args.list[1]).h;
        scui_widget_move_pos(scui_window_mgr.switch_args.list[1], &point);
        break;
    case scui_opt_dir_to_l:
        point.x = -scui_window_mgr.switch_args.ofs;
        scui_widget_move_pos(scui_window_mgr.switch_args.list[0], &point);
        point.x += +scui_widget_clip(scui_window_mgr.switch_args.list[0]).w;
        scui_widget_move_pos(scui_window_mgr.switch_args.list[1], &point);
        break;
    case scui_opt_dir_to_r:
        point.x = +scui_window_mgr.switch_args.ofs;
        scui_widget_move_pos(scui_window_mgr.switch_args.list[0], &point);
        point.x += -scui_widget_clip(scui_window_mgr.switch_args.list[1]).w;
        scui_widget_move_pos(scui_window_mgr.switch_args.list[1], &point);
        break;
    default:
        SCUI_LOG_ERROR("error switch dir 0x%08x", scui_window_mgr.switch_args.dir);
        break;
    }
    
    /* 有自己的独立buffer,直接refr */
    widget = scui_handle_source_check(scui_window_mgr.switch_args.list[0]);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    
    if (scui_widget_surface_only(widget))
        scui_widget_refr(widget->myself, false);
    else
        scui_widget_draw(widget->myself, NULL, false);
}

/*@brief 窗口跳转动画回调
 */
static void scui_window_jump_anima_finish(void *instance)
{
    SCUI_LOG_INFO("");
    scui_point_t   point  = {0};
    scui_widget_t *widget = NULL;
    
    if (scui_window_mgr.switch_args.pct == 0) {
        scui_window_active(scui_window_mgr.switch_args.list[0]);
        scui_window_stack_update(scui_window_mgr.switch_args.list[0], 0);
        scui_widget_hide(scui_window_mgr.switch_args.list[1], true);
    }
    if (scui_window_mgr.switch_args.pct == 100) {
        scui_window_active(scui_window_mgr.switch_args.list[1]);
        scui_window_stack_update(scui_window_mgr.switch_args.list[1], 0);
        scui_widget_hide(scui_window_mgr.switch_args.list[0], true);
    }
    
    if (scui_window_mgr.switch_args.anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scui_window_mgr.switch_args.anima);
        scui_anima_destroy(scui_window_mgr.switch_args.anima);
        scui_window_mgr.switch_args.anima = SCUI_HANDLE_INVALID;
    }
    
    scui_window_mgr.switch_args.lock_jump = false;
    scui_widget_global_scroll_flag(0x01, &scui_window_mgr.switch_args.key);
}

/*@brief 移动动画事件
 *@param type (0x00:移动开始; 0x01:移动结束; 0x02:移动中)
 */
static void scui_window_move_anima_state(uint8_t type)
{
    static bool over_switch = true;
    
    switch (type) {
    case 0x00: {
        /* 上一次滚动已经结束才可产生下一次滚动开始事件 */
        /* 否则为滚动被打断,此时应该吸收中间的事件 */
        if (!over_switch) break;
        over_switch = false;
        
        // 背景窗口虚化
        if (scui_window_mgr.switch_args.type == scui_window_switch_cover_in ||
            scui_window_mgr.switch_args.type == scui_window_switch_cover_out) {
            scui_handle_t handle_t = SCUI_HANDLE_INVALID;
            
            if (scui_window_mgr.switch_args.type == scui_window_switch_cover_in)
                handle_t = scui_window_mgr.switch_args.list[0];
            if (scui_window_mgr.switch_args.type == scui_window_switch_cover_out)
                handle_t = scui_window_mgr.switch_args.list[1];
            
            // 问题:存在之后的窗口重绘失效当前虚化
            // 暂定:锁定窗口绘制, 禁止绘制该控件树
            scui_widget_t *widget = scui_handle_source_assert(handle_t);
            scui_window_t *window = (void *)widget;
            window->draw_lock = true;
            
            /* 背景窗口虚化 */
            scui_widget_draw(handle_t, NULL, false);
            scui_widget_draw_blur(handle_t, NULL);
            SCUI_LOG_INFO("");
        }
        break;
    }
    case 0x01: {
        if (over_switch) break;
        over_switch = true;
        
        if (scui_window_mgr.switch_args.type == scui_window_switch_cover_in ||
            scui_window_mgr.switch_args.type == scui_window_switch_cover_out) {
            scui_handle_t handle_t = SCUI_HANDLE_INVALID;
            
            if (scui_window_mgr.switch_args.type == scui_window_switch_cover_in)
                handle_t = scui_window_mgr.switch_args.list[0];
            if (scui_window_mgr.switch_args.type == scui_window_switch_cover_out)
                handle_t = scui_window_mgr.switch_args.list[1];
            
            // 窗口绘制解锁(解锁所有窗口即可)
            for (scui_handle_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++) {
                if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID)
                    continue;
                scui_handle_t  handle = scui_window_mgr.list[idx];
                scui_widget_t *widget = scui_handle_source_assert(handle);
                scui_window_t *window = (void *)widget;
                window->draw_lock = false;
            }
            
            scui_widget_draw(handle_t, NULL, false);
        }
        break;
    }
    case 0x02: {
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口切换初始化
 */
static void scui_window_move_anima_cfg(void)
{
    /* 动画轨迹, 动画速度 */
    scui_map_cb_t anima_path[4] = {0};
    scui_coord_t anima_speed[4] = SCUI_WINDOW_SWITCH_MOVE_SPD;
    
    anima_path[0] = scui_map_linear;
    anima_path[1] = scui_map_linear;
    anima_path[2] = scui_map_linear;
    anima_path[3] = scui_map_ease_in_out;
    
    for (scui_handle_t idx = 0; idx < 4; idx++) {
        if (scui_window_mgr.switch_args.anima_path[idx]  == NULL)
            scui_window_mgr.switch_args.anima_path[idx]   = anima_path[idx];
        if (scui_window_mgr.switch_args.anima_speed[idx] == 0)
            scui_window_mgr.switch_args.anima_speed[idx]  = anima_speed[idx];
    }
    
    scui_anima_t anima_t = {.period = -1,};
    scui_window_switch_anima_cfg(&anima_t);
    scui_window_mgr.switch_args.anima_path[1] = anima_t.path;
    scui_window_mgr.switch_args.anima_path[2] = anima_t.path;
    scui_window_mgr.switch_args.anima_path[3] = anima_t.path;
}

/*@brief 窗口切换动画配置
 *@param tag 切换类型
 */
static void scui_window_move_anima_tag(uint8_t tag)
{
    for (scui_handle_t idx = 0; idx < 4; idx++)
        scui_window_mgr.switch_args.anima_tag[idx] = false;
    
    if (tag < 4)
        scui_window_mgr.switch_args.anima_tag[tag] = true;
        scui_window_mgr.switch_args.anima_tag[3] = true;
}

/*@brief 窗口移动动画回调
 */
static void scui_window_move_anima_prepare(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief 窗口移动动画回调
 */
static void scui_window_move_anima_expired(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    
    scui_window_move_anima_state(0x02);
    
    scui_widget_t *widget = NULL;
    scui_multi_t pct = scui_abs(anima->value_c) * 100;
    if (scui_opt_bits_check(scui_window_mgr.switch_args.dir, scui_opt_dir_hor))
         pct /= SCUI_HOR_RES;
    if (scui_opt_bits_check(scui_window_mgr.switch_args.dir, scui_opt_dir_ver))
         pct /= SCUI_VER_RES;
    scui_window_mgr.switch_args.pct = pct;
    SCUI_LOG_DEBUG("pct:%d", scui_window_mgr.switch_args.pct);
    
    scui_point_t point = {0};
    if (scui_opt_bits_check(scui_window_mgr.switch_args.dir, scui_opt_dir_hor))
        point.x = anima->value_c;
    if (scui_opt_bits_check(scui_window_mgr.switch_args.dir, scui_opt_dir_ver))
        point.y = anima->value_c;
    scui_window_mgr.switch_args.point = point;
    SCUI_LOG_DEBUG("point:<%d,%d>", point.x, point.y);
    
    /* 更新point */
    scui_widget_move_pos(scui_window_mgr.switch_args.list[0], &point);
    if (scui_window_mgr.switch_args.dir == scui_opt_dir_to_u)
        point.y += scui_widget_clip(scui_window_mgr.switch_args.list[0]).h;
    if (scui_window_mgr.switch_args.dir == scui_opt_dir_to_d)
        point.y -= scui_widget_clip(scui_window_mgr.switch_args.list[1]).h;
    if (scui_window_mgr.switch_args.dir == scui_opt_dir_to_l)
        point.x += scui_widget_clip(scui_window_mgr.switch_args.list[0]).w;
    if (scui_window_mgr.switch_args.dir == scui_opt_dir_to_r)
        point.x -= scui_widget_clip(scui_window_mgr.switch_args.list[1]).w;
    scui_widget_move_pos(scui_window_mgr.switch_args.list[1], &point);
    
    /* 有自己的独立buffer,直接refr */
    widget = scui_handle_source_check(scui_window_mgr.switch_args.list[0]);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    
    if (scui_widget_surface_only(widget))
        scui_widget_refr(widget->myself, false);
    else
        scui_widget_draw(widget->myself, NULL, false);
}

/*@brief 窗口移动动画回调
 */
static void scui_window_move_anima_finish(void *instance)
{
    SCUI_LOG_INFO("");
    
    if (scui_window_mgr.switch_args.anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scui_window_mgr.switch_args.anima);
        scui_anima_destroy(scui_window_mgr.switch_args.anima);
        scui_window_mgr.switch_args.anima = SCUI_HANDLE_INVALID;
    }
    
    // 仅最后的动画帧才进行资源回收
    if (scui_window_mgr.switch_args.hold_move)
        return;
    
    scui_window_move_anima_state(0x01);
    
    if (scui_window_mgr.switch_args.pct == 0) {
        scui_window_active(scui_window_mgr.switch_args.list[0]);
        scui_window_stack_update(scui_window_mgr.switch_args.list[0], 0);
        scui_widget_hide(scui_window_mgr.switch_args.list[1], true);
        scui_window_switch_type_update(scui_window_switch_move, scui_opt_dir_none);
    }
    if (scui_window_mgr.switch_args.pct == 100) {
        scui_window_active(scui_window_mgr.switch_args.list[1]);
        scui_window_stack_update(scui_window_mgr.switch_args.list[1], 0);
        scui_widget_hide(scui_window_mgr.switch_args.list[0], true);
        scui_window_switch_type_update(scui_window_switch_move, scui_opt_dir_none);
    }
    
    scui_window_mgr.switch_args.lock_move = false;
    scui_widget_global_scroll_flag(0x01, &scui_window_mgr.switch_args.key);
}

/*@brief 窗口移动动画自动化
 *@param value_s   起始值
 *@param value_e   结束值
 *@param value_all 计数值
 *@param period    周期值
 */
static void scui_window_move_anima_auto(int32_t value_s, int32_t value_e, int32_t value_all, uint32_t period)
{
    scui_anima_t anima = {0};
    anima.prepare = scui_window_move_anima_prepare;
    anima.expired = scui_window_move_anima_expired;
    anima.finish  = scui_window_move_anima_finish;
    anima.value_s = value_s;
    anima.value_e = value_e;
    anima.period  = period != 0 ? period : scui_abs(anima.value_e - anima.value_s);
    
    // 确定当前动画的路径
    // 计算当前动画的周期
    scui_window_move_anima_cfg();
    for (scui_handle_t idx = 0; idx < 4; idx++)
        if (scui_window_mgr.switch_args.anima_tag[idx]) {
            anima.path = scui_window_mgr.switch_args.anima_path[idx];
            anima.period = anima.period * 1000 / scui_window_mgr.switch_args.anima_speed[idx];
            break;
        }
    
    SCUI_LOG_INFO("<%d, %d>", value_s, value_e);
    
    if (scui_window_mgr.switch_args.anima == SCUI_HANDLE_INVALID)
        scui_window_move_anima_state(0x00);
    
    if (scui_window_mgr.switch_args.anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scui_window_mgr.switch_args.anima);
        scui_anima_destroy(scui_window_mgr.switch_args.anima);
        scui_window_mgr.switch_args.anima = SCUI_HANDLE_INVALID;
    }
    if (value_s == value_e) {
        anima.value_c = value_s = value_e;
        scui_window_move_anima_prepare(&anima);
        scui_window_move_anima_expired(&anima);
        scui_window_move_anima_finish(&anima);
        return;
    }
    scui_anima_create(&anima, &scui_window_mgr.switch_args.anima);
    scui_anima_start(scui_window_mgr.switch_args.anima);
}

/*@brief 窗口切换事件处理回调
 *@param handle 窗口句柄
 *@param inout  true:进入;false:退出;
 */
static void scui_window_move_anima_inout(scui_handle_t handle, bool inout)
{
    /* 获得窗口宽高 */
    scui_point_t    point = scui_window_mgr.switch_args.point;
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 从当前位置到达目标点 */
    int32_t value_s = 0, value_e = 0, value_all = 0;
    
    if (scui_window_mgr.switch_args.dir == scui_opt_dir_to_r) {
        value_s = point.x; value_e = inout ? 0 : +SCUI_HOR_RES; value_all = SCUI_HOR_RES;
    }
    if (scui_window_mgr.switch_args.dir == scui_opt_dir_to_l) {
        value_s = point.x; value_e = inout ? 0 : -SCUI_HOR_RES; value_all = SCUI_HOR_RES;
    }
    if (scui_window_mgr.switch_args.dir == scui_opt_dir_to_d) {
        value_s = point.y; value_e = inout ? 0 : +SCUI_VER_RES; value_all = SCUI_VER_RES;
    }
    if (scui_window_mgr.switch_args.dir == scui_opt_dir_to_u) {
        value_s = point.y; value_e = inout ? 0 : -SCUI_VER_RES; value_all = SCUI_VER_RES;
    }
    
    scui_window_move_anima_auto(value_s, value_e, value_all, 0);
}

/*@brief 窗口切换事件处理回调
 *@param event 事件
 */
static void scui_window_event_switch(scui_event_t *event)
{
    scui_point_t point = scui_window_mgr.switch_args.point;
    
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_window_t *window = widget;
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    
    switch (event->type) {
    case scui_event_ptr_down:
        scui_event_mask_keep(event);
        /* 采用位置检测机制,这会使初始判断条件简单化 */
        scui_window_mgr.switch_args.pos = scui_opt_pos_none;
        /* 水平位置先检测 */
        /* 垂直位置后检测,并且吸收水平位置检测结果 */
        if (event->ptr_c.x <= SCUI_HOR_RES * 1 / 2) scui_window_mgr.switch_args.pos |= scui_opt_pos_l;
        if (event->ptr_c.x >= SCUI_HOR_RES * 1 / 2) scui_window_mgr.switch_args.pos |= scui_opt_pos_r;
        if (event->ptr_c.y <= SCUI_VER_RES * 1 / 2) scui_window_mgr.switch_args.pos |= scui_opt_pos_u;
        if (event->ptr_c.y >= SCUI_VER_RES * 1 / 2) scui_window_mgr.switch_args.pos |= scui_opt_pos_d;
        break;
    case scui_event_ptr_move:
    case scui_event_ptr_fling:
        /* 场景正在切换时,中止更新切换目标 */
        if (scui_window_mgr.switch_args.lock_move) {
            scui_event_mask_over(event);
            SCUI_LOG_INFO("");
            if (scui_window_mgr.switch_args.pos == scui_opt_pos_l)
                scui_window_move_anima_auto(point.x, event->ptr_e.x, SCUI_HOR_RES, 0);
            if (scui_window_mgr.switch_args.pos == scui_opt_pos_r)
                scui_window_move_anima_auto(point.x, event->ptr_e.x - SCUI_HOR_RES, SCUI_HOR_RES, 0);
            if (scui_window_mgr.switch_args.pos == scui_opt_pos_u)
                scui_window_move_anima_auto(point.y, event->ptr_e.y, SCUI_VER_RES, 0);
            if (scui_window_mgr.switch_args.pos == scui_opt_pos_d)
                scui_window_move_anima_auto(point.y, event->ptr_e.y - SCUI_VER_RES, SCUI_VER_RES, 0);
        } else {
            if (scui_window_mgr.switch_args.lock_jump)
                break;
            if (scui_window_mgr.switch_args.pos == scui_opt_pos_none)
                break;
            scui_opt_dir_t event_dir = scui_indev_ptr_dir(event);
            scui_window_switch_type_t switch_type = scui_window_switch_auto;
            scui_handle_t target = SCUI_HANDLE_INVALID;
            SCUI_LOG_INFO("dir:%u", event_dir);
            /* 方向检测与条件加载 */
            if (event_dir == scui_opt_dir_to_r &&
                scui_opt_bits_check(scui_window_mgr.switch_args.pos, scui_opt_pos_l)) { /* 左窗:方向向右 */
                target = window->sibling[2]; switch_type = window->switch_type[2]; point.x = -SCUI_HOR_RES;
            }
            if (event_dir == scui_opt_dir_to_l &&
                scui_opt_bits_check(scui_window_mgr.switch_args.pos, scui_opt_pos_r)) { /* 右窗:方向向左 */
                target = window->sibling[3]; switch_type = window->switch_type[3]; point.x = +SCUI_HOR_RES;
            }
            if (event_dir == scui_opt_dir_to_d &&
                scui_opt_bits_check(scui_window_mgr.switch_args.pos, scui_opt_pos_u)) { /* 上窗:方向向下 */
                target = window->sibling[0]; switch_type = window->switch_type[0]; point.y = -SCUI_VER_RES;
            }
            if (event_dir == scui_opt_dir_to_u &&
                scui_opt_bits_check(scui_window_mgr.switch_args.pos, scui_opt_pos_d)) { /* 下窗:方向向上 */
                target = window->sibling[1]; switch_type = window->switch_type[1]; point.y = +SCUI_VER_RES;
            }
            /* 抓获到运动的目标 */
            if (target != SCUI_HANDLE_INVALID) {
                /* 全局滚动锁定 */
                if (!scui_widget_global_scroll_flag(0x00, &scui_window_mgr.switch_args.key))
                     break;
                
                scui_window_move_anima_tag(0);
                scui_window_mgr.switch_args.lock_move = true;
                scui_window_mgr.switch_args.hold_move = true;
                scui_window_mgr.switch_args.mask_fling = event->type == scui_event_ptr_fling;
                scui_window_mgr.switch_args.dir = event_dir;
                scui_window_mgr.switch_args.pos = event_dir;
                scui_window_mgr.switch_args.pct = 0;
                scui_window_mgr.switch_args.list[0] = widget->myself;
                scui_window_mgr.switch_args.list[1] = target;
                /*  更新交互方向 */
                scui_window_switch_type_update(switch_type, event_dir);
                /* 先释放其他窗口资源 */
                scui_window_switch_hide_without(scui_window_mgr.switch_args.list[0], false);
                scui_widget_show(scui_window_mgr.switch_args.list[1], false);
                scui_widget_move_pos(scui_window_mgr.switch_args.list[1], &point);
                /* 额外做一次校正,应该没意义 */
                point = (scui_point_t){0};
                scui_window_mgr.switch_args.point = point;
                scui_widget_move_pos(scui_window_mgr.switch_args.list[0], &point);
                /* 初始默认启动一次,等待动画被打断 */
                if (event->type == scui_event_ptr_fling)
                    scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], false);
                else {
                    if (scui_window_mgr.switch_args.pos == scui_opt_pos_l)
                        scui_window_move_anima_auto(point.x, event->ptr_e.x, SCUI_HOR_RES, 0);
                    if (scui_window_mgr.switch_args.pos == scui_opt_pos_r)
                        scui_window_move_anima_auto(point.x, event->ptr_e.x - SCUI_HOR_RES, SCUI_HOR_RES, 0);
                    if (scui_window_mgr.switch_args.pos == scui_opt_pos_u)
                        scui_window_move_anima_auto(point.y, event->ptr_e.y, SCUI_VER_RES, 0);
                    if (scui_window_mgr.switch_args.pos == scui_opt_pos_d)
                        scui_window_move_anima_auto(point.y, event->ptr_e.y - SCUI_VER_RES, SCUI_VER_RES, 0);
                }
                scui_event_mask_over(event);
            }
        }
        break;
    case scui_event_ptr_up:
        scui_event_mask_keep(event);
        if (scui_window_mgr.switch_args.lock_move) {
            scui_window_mgr.switch_args.hold_move = false;
            scui_window_move_anima_tag(-1);
            
            if (scui_window_mgr.switch_args.mask_fling) {
                scui_window_mgr.switch_args.mask_fling = false;
                scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], false);
                break;
            }
            if (scui_mabs(point.x, SCUI_HOR_RES) < SCUI_HOR_RES * 1 / 3 &&
                scui_mabs(point.y, SCUI_VER_RES) < SCUI_VER_RES * 1 / 3) {
                if (scui_window_mgr.switch_args.pos == scui_opt_pos_u ||
                    scui_window_mgr.switch_args.pos == scui_opt_pos_l) {
                    scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], true);
                    SCUI_LOG_DEBUG("");
                }
            } else {
                if (scui_window_mgr.switch_args.pos == scui_opt_pos_u ||
                    scui_window_mgr.switch_args.pos == scui_opt_pos_l) {
                    scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], false);
                    SCUI_LOG_DEBUG("");
                }
            }
            if (scui_mabs(point.x, SCUI_HOR_RES) < SCUI_HOR_RES * 2 / 3 &&
                scui_mabs(point.y, SCUI_VER_RES) < SCUI_VER_RES * 2 / 3) {
                if (scui_window_mgr.switch_args.pos == scui_opt_pos_d ||
                    scui_window_mgr.switch_args.pos == scui_opt_pos_r) {
                    scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], false);
                    SCUI_LOG_DEBUG("");
                }
            } else {
                if (scui_window_mgr.switch_args.pos == scui_opt_pos_d ||
                    scui_window_mgr.switch_args.pos == scui_opt_pos_r) {
                    scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], true);
                    SCUI_LOG_DEBUG("");
                }
            }
        }
        break;
    case scui_event_key_click: {
        if (event->key_id != SCUI_WINDOW_SWITCH_KEY_TO_U &&
            event->key_id != SCUI_WINDOW_SWITCH_KEY_TO_D &&
            event->key_id != SCUI_WINDOW_SWITCH_KEY_TO_L &&
            event->key_id != SCUI_WINDOW_SWITCH_KEY_TO_R)
            break;
        
        if (scui_window_mgr.switch_args.lock_jump)
            break;
        if (scui_window_mgr.switch_args.lock_move)
            break;
        
        scui_handle_t target = SCUI_HANDLE_INVALID;
        scui_window_switch_type_t switch_type = scui_window_switch_auto;
        scui_opt_dir_t event_dir = scui_opt_dir_none;
        SCUI_LOG_INFO("key_id:%u", event->key_id);
        /* 方向检测与条件加载 */
        if (event->key_id == SCUI_WINDOW_SWITCH_KEY_TO_R) { /* 左窗:方向向右 */
            event_dir = scui_opt_dir_to_r; target = window->sibling[2];
            switch_type = window->switch_type[2]; point.x = -SCUI_HOR_RES;
        }
        if (event->key_id == SCUI_WINDOW_SWITCH_KEY_TO_L) { /* 右窗:方向向左 */
            event_dir = scui_opt_dir_to_l; target = window->sibling[3];
            switch_type = window->switch_type[3]; point.x = +SCUI_HOR_RES;
        }
        if (event->key_id == SCUI_WINDOW_SWITCH_KEY_TO_D) { /* 上窗:方向向下 */
            event_dir = scui_opt_dir_to_d; target = window->sibling[0];
            switch_type = window->switch_type[0]; point.y = -SCUI_VER_RES;
        }
        if (event->key_id == SCUI_WINDOW_SWITCH_KEY_TO_U) { /* 下窗:方向向上 */
            event_dir = scui_opt_dir_to_u; target = window->sibling[1];
            switch_type = window->switch_type[1]; point.y = +SCUI_VER_RES;
        }
        /* 抓获到运动的目标 */
        if (target != SCUI_HANDLE_INVALID && event_dir != scui_opt_dir_none) {
            /* 全局滚动锁定 */
            if (!scui_widget_global_scroll_flag(0x00, &scui_window_mgr.switch_args.key))
                 break;
            
            scui_window_move_anima_tag(1);
            scui_window_mgr.switch_args.lock_move  = true;
            scui_window_mgr.switch_args.hold_move  = false;
            scui_window_mgr.switch_args.mask_fling = true;
            scui_window_mgr.switch_args.dir = event_dir;
            scui_window_mgr.switch_args.pos = event_dir;
            scui_window_mgr.switch_args.pct = 0;
            scui_window_mgr.switch_args.list[0] = widget->myself;
            scui_window_mgr.switch_args.list[1] = target;
            // 更新交互方向
            scui_window_switch_type_update(switch_type, event_dir);
            /* 先释放其他窗口资源 */
            scui_window_switch_hide_without(scui_window_mgr.switch_args.list[0], false);
            scui_widget_show(scui_window_mgr.switch_args.list[1], false);
            scui_widget_move_pos(scui_window_mgr.switch_args.list[1], &point);
            /* 额外做一次校正,应该没意义 */
            point = (scui_point_t){0};
            scui_window_mgr.switch_args.point = point;
            scui_widget_move_pos(scui_window_mgr.switch_args.list[0], &point);
            /* 初始默认启动一次,等待动画被打断 */
            scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], false);
            SCUI_LOG_DEBUG("");
            
            scui_event_mask_over(event);
        }
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口跳转
 *@param handle 窗口句柄
 *@param type   窗口切换风格
 *@param dir    窗口切换方向
 *@retval 成功失败
 */
bool scui_window_jump(scui_handle_t handle, scui_window_switch_type_t type, scui_opt_dir_t dir)
{
    SCUI_LOG_INFO("");
    
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    if (scui_handle_remap(handle)) {
        scui_widget_t *widget = scui_handle_source_check(handle);
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    } else {
        scui_widget_maker_t *maker = scui_handle_source_check(handle);
        SCUI_ASSERT(maker->parent == SCUI_HANDLE_INVALID);
    }
    
    if (scui_window_mgr.switch_args.lock_jump) {
        SCUI_LOG_WARN("window switching");
        return false;
    }
    if (scui_window_mgr.switch_args.lock_move) {
        SCUI_LOG_WARN("window switching");
        return false;
    }
    if (scui_window_mgr.switch_args.anima != SCUI_HANDLE_INVALID) {
        SCUI_LOG_WARN("window switching");
        return false;
    }
    
    /* 先上锁 */
    if (!scui_widget_global_scroll_flag(0x00, &scui_window_mgr.switch_args.key)) {
         SCUI_LOG_WARN("window switching: %u", scui_window_mgr.switch_args.key);
         return false;
    }
    
    /* 先上锁(标记) */
    scui_window_mgr.switch_args.lock_jump = true;
    
    /* 自适应需要更新窗口切换状态 */
    scui_window_switch_type_update(type, dir);
    type = scui_window_mgr.switch_args.type;
    dir  = scui_window_mgr.switch_args.dir;
    
    /* 清除切换窗口列表,回收除去焦点以外所有其他旧窗口 */
    for (scui_handle_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++) {
        scui_window_mgr.switch_args.list[idx] = SCUI_HANDLE_INVALID;
        if (scui_window_mgr.list[idx] == SCUI_HANDLE_INVALID)
            continue;
        if (scui_window_mgr.list[idx] == scui_window_mgr.active_curr ||
            scui_window_mgr.list[idx] == handle)
            continue;
        scui_widget_t *widget = scui_handle_source_check(scui_window_mgr.list[idx]);
        scui_window_t *window = (void *)widget;
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        
        if (scui_widget_surface_only(widget) && !window->resident)
            scui_widget_hide(scui_window_mgr.list[idx], false);
    }
    
    /* 如果没有焦点窗口 */
    if (scui_window_mgr.active_curr == SCUI_HANDLE_INVALID) {
        scui_widget_show(handle, false);
        scui_window_active(handle);
        scui_window_mgr.switch_args.lock_jump = false;
        scui_widget_global_scroll_flag(0x01, &scui_window_mgr.switch_args.key);
        return true;
    }
    
    /* 新窗口已经是焦点窗口 */
    if (scui_window_mgr.active_curr == handle) {
        scui_widget_show(handle, false);
        scui_window_active(handle);
        scui_window_mgr.switch_args.lock_jump = false;
        scui_widget_global_scroll_flag(0x01, &scui_window_mgr.switch_args.key);
        return true;
    }
    
    #if 1
    // 窗口交互风格自适应
    // 如果目标和当前窗口是临近窗口且指定交互类型
    // 此时的窗口将自适应为到达目标窗口的交互类型
    scui_widget_t *widget_curr = scui_handle_source_check(scui_window_mgr.active_curr);
    scui_window_t *window_curr = (void *)widget_curr;
    for (scui_handle_t idx = 0; idx < 4; idx++)
        if (window_curr->sibling[idx] == handle) {
            type = window_curr->switch_type[idx];
            // 判断方向
            if (idx == 0) dir = scui_opt_dir_to_d;
            if (idx == 1) dir = scui_opt_dir_to_u;
            if (idx == 2) dir = scui_opt_dir_to_r;
            if (idx == 3) dir = scui_opt_dir_to_l;
            /* 自适应需要更新窗口切换状态 */
            scui_window_switch_type_update(type, dir);
            break;
        }
    #endif
    
    /* 无切换效果 */
    if (scui_window_mgr.switch_args.type == scui_window_switch_none) {
        scui_widget_hide(scui_window_mgr.active_curr, false);
        scui_widget_show(handle, false);
        scui_window_active(handle);
        scui_window_mgr.switch_args.lock_jump = false;
        scui_widget_global_scroll_flag(0x01, &scui_window_mgr.switch_args.key);
    } else {
        scui_window_mgr.switch_args.pct = 0;
        scui_window_mgr.switch_args.ofs = 0;
        scui_window_mgr.switch_args.list[0] = scui_window_mgr.active_curr;
        scui_window_mgr.switch_args.list[1] = handle;
        scui_widget_show(scui_window_mgr.active_curr, false);
        scui_widget_show(handle, false);
        
        /* 其他切换效果都需要动画完成 */
        scui_anima_t anima = {0};
        anima.period = SCUI_WINDOW_SWITCH_JUMP_MS;
        scui_window_switch_anima_cfg(&anima);
        
        /* 更新point: */
        scui_point_t point = {0};
        scui_widget_move_pos(scui_window_mgr.switch_args.list[0], &point);
        switch (scui_window_mgr.switch_args.dir) {
        case scui_opt_dir_to_u: point.y = +scui_widget_clip(scui_window_mgr.switch_args.list[0]).h; break;
        case scui_opt_dir_to_d: point.y = -scui_widget_clip(scui_window_mgr.switch_args.list[1]).h; break;
        case scui_opt_dir_to_l: point.x = +scui_widget_clip(scui_window_mgr.switch_args.list[0]).w; break;
        case scui_opt_dir_to_r: point.x = -scui_widget_clip(scui_window_mgr.switch_args.list[1]).w; break;
        default: SCUI_LOG_ERROR("error switch dir 0x%08x", scui_window_mgr.switch_args.dir); break;
        }
        scui_widget_move_pos(scui_window_mgr.switch_args.list[1], &point);
        
        anima.prepare = scui_window_jump_anima_prepare;
        anima.expired = scui_window_jump_anima_expired;
        anima.finish  = scui_window_jump_anima_finish;
        scui_anima_create(&anima, &scui_window_mgr.switch_args.anima);
        scui_anima_start(scui_window_mgr.switch_args.anima);
    }
    return true;
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 */
void scui_window_event_dispatch(scui_event_t *event)
{
    SCUI_LOG_DEBUG("event %u", event->type);
    
    /* 不同的事件处理流程有不同的递归冒泡规则 */
    scui_window_event_switch(event);
}
