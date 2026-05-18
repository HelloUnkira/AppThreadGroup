/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_window_switch_t scui_window_switch = {0};

/*@brief 获取窗口切换风格
 *@param cfg_type 窗口切换风格
 */
void scui_window_switch_get_cfg_type(scui_window_switch_type_t *cfg_type)
{
    *cfg_type = scui_window_switch.cfg_type;
}

/*@brief 设置窗口切换风格
 *@param cfg_type 窗口切换风格
 */
void scui_window_switch_set_cfg_type(scui_window_switch_type_t cfg_type)
{
    scui_window_switch.cfg_type = cfg_type;
}

/*@brief 获取窗口切换方向
 *@param cfg_dir 窗口切换方向
 */
void scui_window_switch_get_cfg_dir(scui_opt_dir_t *cfg_dir)
{
    *cfg_dir = scui_window_switch.cfg_dir;
}

/*@brief 设置窗口切换方向
 *@param cfg_dir 窗口切换方向
 */
void scui_window_switch_set_cfg_dir(scui_opt_dir_t cfg_dir)
{
    scui_window_switch.cfg_dir = cfg_dir;
}

/*@brief 窗口切换风格
 *@retval 窗口切换风格
 */
scui_window_switch_type_t scui_window_switch_type(void)
{
    return scui_window_switch.type;
}

/*@brief 窗口切换方向
 *@retval 窗口切换方向
 */
scui_opt_dir_t scui_window_switch_dir(void)
{
    return scui_window_switch.dir;
}

/*@brief 窗口切换进度
 *@retval 窗口切换进度
 */
scui_coord_t scui_window_switch_pct(void)
{
    return scui_window_switch.pct;
}

/*@brief 窗口切换就绪
 */
static void scui_window_switch_ready(void)
{
    scui_handle_t handle_c = scui_window_switch.list[0];
    scui_handle_t handle_t = scui_window_switch.list[1];
    
    scui_handle_t window_list[SCUI_WINDOW_LIST_LIMIT];
    scui_window_list_sync(window_list);
    
    for (scui_multi_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++) {
        if (window_list[idx] == SCUI_HANDLE_INVALID) continue;
        if (window_list[idx] == handle_c) continue;
        if (window_list[idx] == handle_t) continue;
        /* 忽略这俩个要切换的窗口 */
        
        scui_handle_t  handle_n = window_list[idx];
        scui_widget_t *widget_n = scui_handle_source_check(handle_n);
        scui_window_t *window_n = (void *)widget_n;
        
        /* 不自动回收常驻窗口 */
        if (window_n->resident) continue;
        
        /* 任意窗口或者仅带资源窗口 */
        if (scui_widget_surface_only(widget_n))
            scui_widget_hide(handle_n, false);
    }
    
    /* 加载这俩个需要切换的窗口 */
    if (handle_c != SCUI_HANDLE_INVALID && scui_handle_unmap(handle_c))
        scui_widget_show(handle_c, false);
    if (handle_t != SCUI_HANDLE_INVALID && scui_handle_unmap(handle_t))
        scui_widget_show(handle_t, false);
}

/*@brief 窗口切换完成
 *@param 切换后的窗口
 */
static void scui_window_switch_finish(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_window_t *window = (void *)widget;
    
    scui_handle_t window_list[SCUI_WINDOW_LIST_LIMIT];
    scui_window_list_sync(window_list);
    
    for (scui_multi_t idx = 0; idx < SCUI_WINDOW_LIST_LIMIT; idx++) {
        if (window_list[idx] == SCUI_HANDLE_INVALID) continue;
        if (window_list[idx] == handle) continue;
        
        scui_handle_t  handle_n = window_list[idx];
        scui_widget_t *widget_n = scui_handle_source_check(handle_n);
        scui_window_t *window_n = (void *)widget_n;
        
        /* 不自动回收常驻窗口 */
        if (window_n->resident) continue;
        
        bool sibling_check = false;
        /* 不回收当前窗口的临近窗口 */
        #if SCUI_WINDOW_PRELOAD_USE
        for (scui_coord_t idx_c = 0; idx_c < 4; idx_c++) {
            if (window->sibling[idx_c] != handle_n)
                continue;
            
            sibling_check = true;
            break;
        }
        #endif
        
        if (sibling_check)
            continue;
        
        /* 回收无关联性质窗口 */
        scui_widget_hide(handle_n, true);
    }
    
    /* 发送预加载事件到达指定窗口 */
    scui_event_define(event, handle, false,
        scui_event_window_preload, NULL);
    scui_event_notify(&event);
}

/*@brief 窗口切换风格自动更新
 *@param type 窗口切换风格
 *@param dir  窗口切换方向
 */
static void scui_window_switch_update(scui_window_switch_type_t type, scui_opt_dir_t dir)
{
    scui_window_switch.dir  = dir;
    scui_window_switch.type = type;
    /* 自适应需要更新窗口切换状态 */
    if (scui_window_switch.type == scui_window_switch_auto)
        scui_window_switch.type  = scui_window_switch.cfg_type;
    /* 自适应需要更新窗口切换状态 */
    if (scui_window_switch.dir == scui_opt_dir_none)
        scui_window_switch.dir  = scui_window_switch.cfg_dir;
}

/*@brief 窗口切换动画配置
 *@param anima 动画实例
 */
static void scui_window_switch_anima(scui_anima_t *anima)
{
    switch (scui_window_switch.type) {
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
        /* 非叠加类型的移动可适合超调效果 */
        anima->path = scui_map_overshoot;
        anima->period = anima->period * 3 / 2;
        /* 非叠加类型的移动可适合重力回弹效果 */
        anima->path = scui_map_bounce;
        anima->period = anima->period * 3 / 2;
        break;
    default:
        anima->path = scui_map_linear;
        anima->period = anima->period;
        break;
    }
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
        
        /* 背景窗口虚化 */
        if (scui_window_switch.type == scui_window_switch_cover_in ||
            scui_window_switch.type == scui_window_switch_cover_out) {
            scui_handle_t handle_t = SCUI_HANDLE_INVALID;
            
            if (scui_window_switch.type == scui_window_switch_cover_in)
                handle_t = scui_window_switch.list[0];
            if (scui_window_switch.type == scui_window_switch_cover_out)
                handle_t = scui_window_switch.list[1];
            
            /* 问题:存在之后的窗口重绘失效当前虚化 */
            /* 暂定:锁定窗口绘制, 禁止绘制该控件树 */
            scui_window_draw_lock_set(handle_t, true);
            
            /* 背景窗口虚化 */
            scui_widget_draw(handle_t, NULL, false);
            scui_widget_draw_blur(handle_t, NULL);
            // scui_widget_draw_blur(handle_t, NULL);
            SCUI_LOG_INFO("");
        }
        break;
    }
    case 0x01: {
        if (over_switch) break;
        over_switch = true;
        
        if (scui_window_switch.type == scui_window_switch_cover_in ||
            scui_window_switch.type == scui_window_switch_cover_out) {
            scui_handle_t handle_t = SCUI_HANDLE_INVALID;
            
            if (scui_window_switch.type == scui_window_switch_cover_in)
                handle_t = scui_window_switch.list[0];
            if (scui_window_switch.type == scui_window_switch_cover_out)
                handle_t = scui_window_switch.list[1];
            
            /* 窗口绘制解锁(解锁所有窗口即可) */
            scui_window_draw_lock_set(handle_t, false);
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
    scui_map_cb_t anima_path[5] = {0};
    scui_coord_t anima_speed[5] = SCUI_WINDOW_SWITCH_MOVE_SPD;
    
    anima_path[0] = scui_map_linear;
    anima_path[1] = scui_map_linear;
    anima_path[2] = scui_map_linear;
    anima_path[3] = scui_map_ease_in_out;
    anima_path[4] = scui_map_linear;
    
    for (scui_coord_t idx = 0; idx < 5; idx++) {
        if (scui_window_switch.anima_path[idx]  == NULL)
            scui_window_switch.anima_path[idx]   = anima_path[idx];
        if (scui_window_switch.anima_speed[idx] == 0)
            scui_window_switch.anima_speed[idx]  = anima_speed[idx];
    }
    
    scui_anima_t anima_t = {.period = -1,};
    scui_window_switch_anima(&anima_t);
    scui_window_switch.anima_path[1] = anima_t.path;
    scui_window_switch.anima_path[2] = anima_t.path;
    scui_window_switch.anima_path[3] = anima_t.path;
    scui_window_switch.anima_path[4] = anima_t.path;
}

/*@brief 窗口切换动画配置
 *@param anima_tag 切换类型
 */
static void scui_window_move_anima_tag(scui_coord_t anima_tag)
{
    scui_window_switch.anima_tag  = 0;
    scui_window_switch.anima_tag |= 1 << 3;
    scui_coord_t idx = anima_tag;
    
    if (anima_tag >= 0 && anima_tag < 5)
        scui_window_switch.anima_tag |= 1 << idx;
}

/*@brief 窗口移动动画回调
 */
static void scui_window_move_anima_ready(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief 窗口移动动画回调
 */
static void scui_window_move_anima_expire(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima = instance;
    
    scui_window_move_anima_state(0x02);
    
    scui_widget_t *widget = NULL;
    scui_multi_t pct = scui_abs(anima->value_c) * 100;
    if (scui_opt_bits_check(scui_window_switch.dir, scui_opt_dir_hor))
         pct /= SCUI_HOR_RES;
    if (scui_opt_bits_check(scui_window_switch.dir, scui_opt_dir_ver))
         pct /= SCUI_VER_RES;
    scui_window_switch.pct = pct;
    SCUI_LOG_DEBUG("pct:%d", scui_window_switch.pct);
    
    scui_point_t point = {0};
    if (scui_opt_bits_check(scui_window_switch.dir, scui_opt_dir_hor))
        point.x = anima->value_c;
    if (scui_opt_bits_check(scui_window_switch.dir, scui_opt_dir_ver))
        point.y = anima->value_c;
    scui_window_switch.point = point;
    SCUI_LOG_DEBUG("point:<%d,%d>", point.x, point.y);
    
    /* 更新point */
    scui_widget_move_pos(scui_window_switch.list[0], &point);
    if (scui_window_switch.dir == scui_opt_dir_dtu)
        point.y += scui_widget_clip(scui_window_switch.list[0]).h;
    if (scui_window_switch.dir == scui_opt_dir_utd)
        point.y -= scui_widget_clip(scui_window_switch.list[1]).h;
    if (scui_window_switch.dir == scui_opt_dir_rtl)
        point.x += scui_widget_clip(scui_window_switch.list[0]).w;
    if (scui_window_switch.dir == scui_opt_dir_ltr)
        point.x -= scui_widget_clip(scui_window_switch.list[1]).w;
    scui_widget_move_pos(scui_window_switch.list[1], &point);
    
    /* 有自己的独立buffer,直接refr */
    widget = scui_handle_source_check(scui_window_switch.list[0]);
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
    
    if (scui_window_switch.anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scui_window_switch.anima);
        scui_anima_destroy(scui_window_switch.anima);
        scui_window_switch.anima = SCUI_HANDLE_INVALID;
    }
    
    /* 仅最后的动画帧才进行资源回收 */
    if (scui_window_switch.lock_move)
        return;
    
    scui_window_move_anima_state(0x01);
    
    if (scui_window_switch.pct == 0) {
        scui_window_active(scui_window_switch.list[0]);
        scui_window_switch_finish(scui_window_switch.list[0]);
        scui_window_switch_update(scui_window_switch_move, scui_opt_dir_none);
        scui_window_stack_switch(scui_window_switch.list[0]);
    }
    if (scui_window_switch.pct == 100) {
        scui_window_active(scui_window_switch.list[1]);
        scui_window_switch_finish(scui_window_switch.list[1]);
        scui_window_switch_update(scui_window_switch_move, scui_opt_dir_none);
        scui_window_stack_switch(scui_window_switch.list[1]);
    }
    
    scui_window_switch.lock_move = false;
    scui_window_switch.lock_jump = false;
    scui_widget_scroll_state(0x01);
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
    anima.ready   = scui_window_move_anima_ready;
    anima.expire  = scui_window_move_anima_expire;
    anima.finish  = scui_window_move_anima_finish;
    anima.value_s = value_s;
    anima.value_e = value_e;
    anima.period  = period != 0 ? period : scui_abs(anima.value_e - anima.value_s);
    
    /* 确定当前动画的路径 */
    /* 计算当前动画的周期 */
    scui_window_move_anima_cfg();
    for (scui_multi_t idx = 0; idx < 5; idx++) {
        if ((scui_window_switch.anima_tag & (1 << idx)) == 0)
             continue;
        
        anima.path = scui_window_switch.anima_path[idx];
        anima.period = anima.period * 1000 / scui_window_switch.anima_speed[idx];
        break;
    }
    
    SCUI_LOG_INFO("<%d, %d>", value_s, value_e);
    
    if (scui_window_switch.anima == SCUI_HANDLE_INVALID)
        scui_window_move_anima_state(0x00);
    
    if (scui_window_switch.anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scui_window_switch.anima);
        scui_anima_destroy(scui_window_switch.anima);
        scui_window_switch.anima = SCUI_HANDLE_INVALID;
    }
    if (value_s == value_e) {
        anima.value_c = value_s = value_e;
        scui_window_move_anima_ready(&anima);
        scui_window_move_anima_expire(&anima);
        scui_window_move_anima_finish(&anima);
        return;
    }
    scui_anima_create(&anima, &scui_window_switch.anima);
    scui_anima_start(scui_window_switch.anima);
}

/*@brief 窗口切换事件处理回调
 *@param handle 窗口句柄
 *@param inout  true:进入;false:退出;
 */
static void scui_window_move_anima_inout(scui_handle_t handle, bool inout)
{
    /* 获得窗口宽高 */
    scui_point_t    point = scui_window_switch.point;
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 从当前位置到达目标点 */
    int32_t value_s = 0, value_e = 0, value_all = 0;
    
    if (scui_window_switch.dir == scui_opt_dir_ltr) {
        value_s = point.x; value_e = inout ? 0 : +SCUI_HOR_RES; value_all = SCUI_HOR_RES;
    }
    if (scui_window_switch.dir == scui_opt_dir_rtl) {
        value_s = point.x; value_e = inout ? 0 : -SCUI_HOR_RES; value_all = SCUI_HOR_RES;
    }
    if (scui_window_switch.dir == scui_opt_dir_utd) {
        value_s = point.y; value_e = inout ? 0 : +SCUI_VER_RES; value_all = SCUI_VER_RES;
    }
    if (scui_window_switch.dir == scui_opt_dir_dtu) {
        value_s = point.y; value_e = inout ? 0 : -SCUI_VER_RES; value_all = SCUI_VER_RES;
    }
    
    scui_window_move_anima_auto(value_s, value_e, value_all, 0);
}

/*@brief 窗口切换事件处理回调
 *@param event     事件
 *@param widget    窗口控件实例
 *@param event_dir 事件方向
 *@param anima_tag 动画类型
 *@retval 成功失败
 */
static bool scui_window_switch_event_catch(scui_event_t *event, scui_opt_dir_t event_dir, uint8_t anima_tag)
{
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_window_t *window = (void *)widget;
    scui_point_t   point  = scui_window_switch.point;
    
    scui_handle_t target = SCUI_HANDLE_INVALID;
    scui_window_switch_type_t switch_type = scui_window_switch_auto;
    
    if (scui_window_switch.lock_jump) {
        SCUI_LOG_INFO("window switching");
        return false;
    }
    if (scui_window_switch.lock_move) {
        SCUI_LOG_INFO("window switching");
        return false;
    }
    if (scui_widget_scroll_state(0x02)) {
        SCUI_LOG_INFO("window switching");
        return false;
    }
    
    if (event_dir == scui_opt_dir_ltr && /* 左窗:方向向右 */
        scui_opt_bits_check(scui_window_switch.pos, scui_opt_pos_l)) {
        target = window->sibling[2]; switch_type = window->switch_type[2];
        point.x = -SCUI_HOR_RES;
    }
    if (event_dir == scui_opt_dir_rtl && /* 右窗:方向向左 */
        scui_opt_bits_check(scui_window_switch.pos, scui_opt_pos_r)) {
        target = window->sibling[3]; switch_type = window->switch_type[3];
        point.x = +SCUI_HOR_RES;
    }
    if (event_dir == scui_opt_dir_utd && /* 上窗:方向向下 */
        scui_opt_bits_check(scui_window_switch.pos, scui_opt_pos_u)) {
        target = window->sibling[0]; switch_type = window->switch_type[0];
        point.y = -SCUI_VER_RES;
    }
    if (event_dir == scui_opt_dir_dtu && /* 下窗:方向向上 */
        scui_opt_bits_check(scui_window_switch.pos, scui_opt_pos_d)) {
        target = window->sibling[1]; switch_type = window->switch_type[1];
        point.y = +SCUI_VER_RES;
    }
    
    /* 抓获到运动的目标 */
    SCUI_LOG_INFO("dir:%u", event_dir);
    if (target != SCUI_HANDLE_INVALID && event_dir != scui_opt_dir_none) {
        /* 全局滚动锁定 */
        scui_widget_scroll_state(0x00);
        scui_window_move_anima_tag(anima_tag);
        scui_window_switch.dir = event_dir;
        scui_window_switch.pos = event_dir;
        scui_window_switch.pct = 0;
        scui_window_switch.list[0] = widget->myself;
        scui_window_switch.list[1] = target;
        /* 更新交互方向 */
        scui_window_switch_update(switch_type, event_dir);
        /* 就绪窗口资源 */
        scui_window_switch_ready();
        /* 额外做一次校正 */
        scui_widget_move_pos(scui_window_switch.list[1], &point);
        scui_window_switch.point = point = (scui_point_t){0};
        scui_widget_move_pos(scui_window_switch.list[0], &point);
        
        SCUI_LOG_DEBUG("");
        return true;
    }
    
    return false;
}

/*@brief 窗口切换事件处理回调
 *@param event 事件
 */
void scui_window_switch_event(scui_event_t *event)
{
    /* 不同的事件处理流程有不同的递归冒泡规则 */
    SCUI_LOG_DEBUG("event %u", event->type);
    
    /* 只有活跃窗口才进行窗口切换 */
    if (scui_window_active_curr() != event->object)
        return;
    
    SCUI_ASSERT(scui_widget_type_check(event->object, scui_widget_type_window));
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_window_t *window = (void *)widget;
    /* 只有常规窗口才支持事件响应 */
    if (window->resident) return;
    
    switch (event->type) {
    case scui_event_ptr_down: {
        /* 采用位置检测机制,这会使初始判断条件简单化 */
        scui_window_switch.pos = scui_opt_pos_none;
        /* 水平位置先检测, 垂直位置后检测,并且吸收水平位置检测结果 */
        if (event->ptr_c.x <= SCUI_HOR_RES * 1 / 2) scui_window_switch.pos |= scui_opt_pos_l;
        if (event->ptr_c.x >= SCUI_HOR_RES * 1 / 2) scui_window_switch.pos |= scui_opt_pos_r;
        if (event->ptr_c.y <= SCUI_VER_RES * 1 / 2) scui_window_switch.pos |= scui_opt_pos_u;
        if (event->ptr_c.y >= SCUI_VER_RES * 1 / 2) scui_window_switch.pos |= scui_opt_pos_d;
        break;
    }
    case scui_event_ptr_move:
    case scui_event_ptr_fling: {
        /* 窗口正在切换时,中止更新切换目标 */
        if (!scui_window_switch.lock_move) {
            
            scui_opt_dir_t event_dir = scui_indev_ptr_dir(event);
            if (scui_window_switch_event_catch(event, event_dir, 0)) {
                scui_window_switch.lock_move  = true;
                scui_window_switch.mask_fling = event->type != scui_event_ptr_move;
                scui_event_mask_over(event);
            }
        }
        
        /* 初始默认启动一次,等待动画被打断 */
        if (scui_window_switch.lock_move) {
            scui_point_t point = scui_window_switch.point;
            scui_event_mask_over(event);
            
            if (scui_window_switch.pos == scui_opt_pos_l)
                scui_window_move_anima_auto(point.x, event->ptr_e.x, SCUI_HOR_RES, 0);
            if (scui_window_switch.pos == scui_opt_pos_r)
                scui_window_move_anima_auto(point.x, event->ptr_e.x - SCUI_HOR_RES, SCUI_HOR_RES, 0);
            if (scui_window_switch.pos == scui_opt_pos_u)
                scui_window_move_anima_auto(point.y, event->ptr_e.y, SCUI_VER_RES, 0);
            if (scui_window_switch.pos == scui_opt_pos_d)
                scui_window_move_anima_auto(point.y, event->ptr_e.y - SCUI_VER_RES, SCUI_VER_RES, 0);
        }
        break;
    }
    case scui_event_ptr_up: {
        if (scui_window_switch.lock_move) {
            scui_window_switch.lock_move = false;
            scui_window_move_anima_tag(-1);
            
            if (scui_window_switch.mask_fling) {
                scui_window_switch.mask_fling = false;
                scui_window_move_anima_inout(scui_window_switch.list[0], false);
                break;
            }
            scui_point_t point = scui_window_switch.point;
            if (scui_mabs(point.x, SCUI_HOR_RES) < SCUI_HOR_RES * 1 / 3 &&
                scui_mabs(point.y, SCUI_VER_RES) < SCUI_VER_RES * 1 / 3) {
                if (scui_window_switch.pos == scui_opt_pos_u ||
                    scui_window_switch.pos == scui_opt_pos_l) {
                    scui_window_move_anima_inout(scui_window_switch.list[0], true);
                    SCUI_LOG_DEBUG("");
                }
            } else {
                if (scui_window_switch.pos == scui_opt_pos_u ||
                    scui_window_switch.pos == scui_opt_pos_l) {
                    scui_window_move_anima_inout(scui_window_switch.list[0], false);
                    SCUI_LOG_DEBUG("");
                }
            }
            if (scui_mabs(point.x, SCUI_HOR_RES) < SCUI_HOR_RES * 2 / 3 &&
                scui_mabs(point.y, SCUI_VER_RES) < SCUI_VER_RES * 2 / 3) {
                if (scui_window_switch.pos == scui_opt_pos_d ||
                    scui_window_switch.pos == scui_opt_pos_r) {
                    scui_window_move_anima_inout(scui_window_switch.list[0], false);
                    SCUI_LOG_DEBUG("");
                }
            } else {
                if (scui_window_switch.pos == scui_opt_pos_d ||
                    scui_window_switch.pos == scui_opt_pos_r) {
                    scui_window_move_anima_inout(scui_window_switch.list[0], true);
                    SCUI_LOG_DEBUG("");
                }
            }
        }
        break;
    }
    case scui_event_enc_fdir:
    case scui_event_enc_bdir: {
        
        scui_opt_dir_t event_dir = scui_opt_dir_none;
        SCUI_LOG_INFO("enc_type:%u", event->type);
        /* 方向检测与条件加载 */
        if (event->type == scui_event_enc_fdir) {
            
            if (window->switch_enc_way == scui_opt_dir_hor) {
                if (window->sibling[0] != SCUI_HANDLE_INVALID)
                    event_dir = scui_opt_dir_utd;
                if (window->sibling[2] != SCUI_HANDLE_INVALID)
                    event_dir = scui_opt_dir_ltr;
            }
            if (window->switch_enc_way == scui_opt_dir_ver) {
                if (window->sibling[2] != SCUI_HANDLE_INVALID)
                    event_dir = scui_opt_dir_ltr;
                if (window->sibling[0] != SCUI_HANDLE_INVALID)
                    event_dir = scui_opt_dir_utd;
            }
        }
        if (event->type == scui_event_enc_bdir) {
            
            if (window->switch_enc_way == scui_opt_dir_hor) {
                if (window->sibling[1] != SCUI_HANDLE_INVALID)
                    event_dir = scui_opt_dir_dtu;
                if (window->sibling[3] != SCUI_HANDLE_INVALID)
                    event_dir = scui_opt_dir_rtl;
            }
            if (window->switch_enc_way == scui_opt_dir_ver) {
                if (window->sibling[3] != SCUI_HANDLE_INVALID)
                    event_dir = scui_opt_dir_rtl;
                if (window->sibling[1] != SCUI_HANDLE_INVALID)
                    event_dir = scui_opt_dir_dtu;
            }
        }
        
        /* 给定方向进行窗口切换 */
        scui_window_switch.pos = scui_opt_pos_all;
        if (scui_window_switch_event_catch(event, event_dir, 1)) {
            scui_window_move_anima_inout(scui_window_switch.list[0], false);
            scui_event_mask_over(event);
        }
        
        break;
    }
    case scui_event_key_click: {
        
        scui_opt_dir_t event_dir = scui_opt_dir_none;
        SCUI_LOG_INFO("key_id:%u", event->key_id);
        /* 方向检测与条件加载 */
        if (event->key_id == window->switch_key_id[2]) event_dir = scui_opt_dir_ltr;
        if (event->key_id == window->switch_key_id[3]) event_dir = scui_opt_dir_rtl;
        if (event->key_id == window->switch_key_id[0]) event_dir = scui_opt_dir_utd;
        if (event->key_id == window->switch_key_id[1]) event_dir = scui_opt_dir_dtu;
        
        /* 给定方向进行窗口切换 */
        scui_window_switch.pos = scui_opt_pos_all;
        if (scui_window_switch_event_catch(event, event_dir, 2)) {
            scui_window_move_anima_inout(scui_window_switch.list[0], false);
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
bool scui_window_switch_jump(scui_handle_t handle, scui_window_switch_type_t type, scui_opt_dir_t dir)
{
    SCUI_LOG_INFO("");
    
    SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
    scui_handle_t handle_a = scui_window_active_curr();
    
    #if 0
    /* 检查参数正确与否的断言(可选) */
    void *widget_src = scui_handle_source_check(handle);
    bool  window_isw = ((scui_widget_t *)widget_src)->type == scui_widget_type_window;
    bool  window_ism = ((scui_widget_maker_t *)widget_src)->type == scui_widget_type_window;
    SCUI_ASSERT(window_isw || window_ism);
    #endif
    
    if (scui_window_switch.lock_jump) {
        SCUI_LOG_INFO("window switching");
        return false;
    }
    if (scui_window_switch.lock_move) {
        SCUI_LOG_INFO("window switching");
        return false;
    }
    if (scui_widget_scroll_state(0x02)) {
        SCUI_LOG_INFO("window switching");
        return false;
    }
    
    /* 先上锁(标记) */
    scui_window_switch.lock_jump = true;
    
    /* 如果没有焦点窗口 */
    /* 新窗口已经是焦点窗口 */
    if (handle_a == SCUI_HANDLE_INVALID || handle_a == handle) {
        scui_window_switch.list[0] = SCUI_HANDLE_INVALID;
        scui_window_switch.list[1] = handle;
        scui_window_switch_ready();
        scui_window_active(handle);
        scui_window_switch_finish(handle);
        scui_window_switch.lock_jump = false;
        return true;
    }
    
    scui_widget_t *widget_a = scui_handle_source_check(handle_a);
    scui_window_t *window_a = (void *)widget_a;
    SCUI_ASSERT(!window_a->resident);
    /* 只有常规窗口才支持跳转 */
    
    /* 自适应需要更新窗口切换状态 */
    scui_window_switch_update(type, dir);
    type = scui_window_switch.type;
    dir  = scui_window_switch.dir;
    
    #if 1
    /* 窗口交互风格自适应 */
    /* 如果目标和当前窗口是临近窗口且指定交互类型 */
    /* 此时的窗口将自适应为到达目标窗口的交互类型 */
    for (scui_multi_t idx = 0; idx < 4; idx++) {
        if (window_a->sibling[idx] != handle)
            continue;
        
        /* 更新类型和判断方向 */
        type = window_a->switch_type[idx];
        if (idx == 0) dir = scui_opt_dir_utd;
        if (idx == 1) dir = scui_opt_dir_dtu;
        if (idx == 2) dir = scui_opt_dir_ltr;
        if (idx == 3) dir = scui_opt_dir_rtl;
        
        /* 自适应需要更新窗口切换状态 */
        scui_window_switch_update(type, dir);
        break;
    }
    #endif
    
    /* 无切换效果 */
    if (scui_window_switch.type == scui_window_switch_none) {
        scui_window_switch.list[0] = SCUI_HANDLE_INVALID;
        scui_window_switch.list[1] = handle;
        scui_window_switch_ready();
        scui_window_active(handle);
        scui_window_switch_finish(handle);
        scui_window_switch.lock_jump = false;
        return true;
    }
    
    /* 全局滚动锁定 */
    scui_widget_scroll_state(0x00);
    scui_point_t point = scui_window_switch.point;
    
    scui_window_move_anima_tag(4);
    scui_window_switch.dir = scui_window_switch.dir;
    scui_window_switch.pos = scui_window_switch.dir;
    scui_window_switch.pct = 0;
    scui_window_switch.list[0] = handle_a;
    scui_window_switch.list[1] = handle;
    /* 就绪窗口资源 */
    scui_window_switch_ready();
    /* 额外做一次校正 */
    scui_widget_move_pos(scui_window_switch.list[1], &point);
    scui_window_switch.point = point = (scui_point_t){0};
    scui_widget_move_pos(scui_window_switch.list[0], &point);
    /* 初始默认启动一次,等待动画被打断 */
    scui_window_move_anima_inout(scui_window_switch.list[0], false);
    SCUI_LOG_DEBUG("");
    return true;
}
