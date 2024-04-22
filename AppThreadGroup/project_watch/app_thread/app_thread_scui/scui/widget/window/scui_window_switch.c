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
void scui_window_switch_type_update(scui_window_switch_type_t type, scui_event_dir_t dir)
{
    scui_window_mgr.switch_args.dir  = dir;
    scui_window_mgr.switch_args.type = type;
    /* 自适应需要更新窗口切换状态 */
    if (type == scui_window_switch_auto) {
        scui_window_mgr.switch_args.dir  = scui_window_mgr.switch_args.cfg_dir;
        scui_window_mgr.switch_args.type = scui_window_mgr.switch_args.cfg_type;
    }
}

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_start(void *instance)
{
    SCUI_LOG_INFO("");
    scui_window_mgr.switch_args.pct = 0;
    scui_window_mgr.switch_args.ofs = 0;
    
    scui_point_t point = (scui_point_t){0};
    switch (scui_window_mgr.switch_args.type) {
    case scui_window_switch_center_in:
    case scui_window_switch_center_out: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_pct100;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_pct0;
        scui_widget_refr_pos(scui_window_mgr.switch_args.list[0], &point);
        scui_widget_refr_pos(scui_window_mgr.switch_args.list[1], &point);
        break;
    }
    case scui_window_switch_zoom1:
    case scui_window_switch_zoom2: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_pct100;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_pct0;
    }
    case scui_window_switch_normal: {
        scui_widget_refr_pos(scui_window_mgr.switch_args.list[0], &point);
        switch (scui_window_mgr.switch_args.dir) {
        case scui_event_dir_to_u:
            point.y = +scui_widget_attr_clip(scui_window_mgr.switch_args.list[0]).h;
            break;
        case scui_event_dir_to_d:
            point.y = -scui_widget_attr_clip(scui_window_mgr.switch_args.list[1]).h;
            break;
        case scui_event_dir_to_l:
            point.x = +scui_widget_attr_clip(scui_window_mgr.switch_args.list[0]).w;
            break;
        case scui_event_dir_to_r:
            point.x = -scui_widget_attr_clip(scui_window_mgr.switch_args.list[1]).w;
            break;
        default:
            SCUI_LOG_ERROR("error switch dir 0x%08x", scui_window_mgr.switch_args.dir);
            break;
        }
        scui_widget_refr_pos(scui_window_mgr.switch_args.list[1], &point);
        break;
    }
    default:
        SCUI_LOG_ERROR("error switch type 0x%08x", scui_window_mgr.switch_args.type);
        break;
    }
    
    /* 有自己的独立buffer,直接refr */
    scui_widget_t *widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    
    if (scui_widget_surface_only(widget))
        scui_widget_refr(widget->myself, true);
    else
        scui_widget_draw(widget->myself, NULL, true);
}

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_ready(void *instance)
{
    SCUI_LOG_INFO("");
    
    switch (scui_window_mgr.switch_args.type) {
    case scui_window_switch_center_in:
    case scui_window_switch_center_out:
    case scui_window_switch_zoom1:
    case scui_window_switch_zoom2: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_pct0;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_pct100;
    }
    case scui_window_switch_normal: {
        scui_window_mgr.switch_args.pct = 100;
        scui_point_t point = (scui_point_t){0};
        scui_window_active(scui_window_mgr.switch_args.list[1]);
        scui_widget_refr_pos(scui_window_mgr.switch_args.list[1], &point);
        scui_widget_hide(scui_window_mgr.switch_args.list[0], true);
        scui_widget_refr(scui_window_mgr.switch_args.list[1], true);
        break;
    }
    default:
        SCUI_LOG_ERROR("error switch type 0x%08x", scui_window_mgr.switch_args.type);
        break;
    }
    
    SCUI_ASSERT(scui_window_mgr.switch_args.anima != SCUI_HANDLE_INVALID);
    scui_anima_destroy(scui_window_mgr.switch_args.anima);
    scui_window_mgr.switch_args.anima = SCUI_HANDLE_INVALID;
    scui_window_mgr.switch_args.lock  = false;
}

/*@brief 窗口跳转动画回调
 */
void scui_window_jump_anima_expired(void *instance)
{
    SCUI_LOG_DEBUG("");
    scui_anima_t *anima = instance;
    
    scui_multi_t ofs = 0;
    if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0)
         ofs = scui_disp_get_ver_res();
    if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0)
         ofs = scui_disp_get_hor_res();
    
    ofs *= anima->value_c;
    ofs /= 100;
    
    scui_window_mgr.switch_args.pct = anima->value_c;
    scui_window_mgr.switch_args.ofs = ofs;
    
    scui_point_t point = (scui_point_t){0};
    switch (scui_window_mgr.switch_args.type) {
    case scui_window_switch_center_in:
    case scui_window_switch_center_out: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_by_percent(100 - scui_window_mgr.switch_args.pct);
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_by_percent(scui_window_mgr.switch_args.pct);
        scui_widget_refr_pos(scui_window_mgr.switch_args.list[0], &point);
        scui_widget_refr_pos(scui_window_mgr.switch_args.list[1], &point);
        break;
    }
    case scui_window_switch_zoom1:
    case scui_window_switch_zoom2: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_by_percent(100 - scui_window_mgr.switch_args.pct);
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_by_percent(scui_window_mgr.switch_args.pct);
    }
    case scui_window_switch_normal: {
        scui_point_t point = (scui_point_t){0};
        switch (scui_window_mgr.switch_args.dir) {
        case scui_event_dir_to_u:
            point.y = -scui_window_mgr.switch_args.ofs;
            scui_widget_refr_pos(scui_window_mgr.switch_args.list[0], &point);
            point.y += +scui_widget_attr_clip(scui_window_mgr.switch_args.list[0]).h;
            scui_widget_refr_pos(scui_window_mgr.switch_args.list[1], &point);
            break;
        case scui_event_dir_to_d:
            point.y = +scui_window_mgr.switch_args.ofs;
            scui_widget_refr_pos(scui_window_mgr.switch_args.list[0], &point);
            point.y += -scui_widget_attr_clip(scui_window_mgr.switch_args.list[1]).h;
            scui_widget_refr_pos(scui_window_mgr.switch_args.list[1], &point);
            break;
        case scui_event_dir_to_l:
            point.x = -scui_window_mgr.switch_args.ofs;
            scui_widget_refr_pos(scui_window_mgr.switch_args.list[0], &point);
            point.x += +scui_widget_attr_clip(scui_window_mgr.switch_args.list[0]).w;
            scui_widget_refr_pos(scui_window_mgr.switch_args.list[1], &point);
            break;
        case scui_event_dir_to_r:
            point.x = +scui_window_mgr.switch_args.ofs;
            scui_widget_refr_pos(scui_window_mgr.switch_args.list[0], &point);
            point.x += -scui_widget_attr_clip(scui_window_mgr.switch_args.list[1]).w;
            scui_widget_refr_pos(scui_window_mgr.switch_args.list[1], &point);
            break;
        default:
            SCUI_LOG_ERROR("error switch dir 0x%08x", scui_window_mgr.switch_args.dir);
            break;
        }
        break;
    }
    default:
        SCUI_LOG_ERROR("error switch type 0x%08x", scui_window_mgr.switch_args.type);
        break;
    }
    
    /* 有自己的独立buffer,直接refr */
    scui_widget_t *widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    if (scui_widget_surface_only(widget))
        scui_widget_refr(widget->myself, true);
    else
        scui_widget_draw(widget->myself, NULL, true);
}

/*@brief 窗口移动动画回调
 */
void scui_window_move_anima_start(void *instance)
{
    SCUI_LOG_INFO("");
    
    scui_window_switch_type_t type = scui_window_mgr.switch_args.cfg_type;
    scui_event_dir_t dir = scui_window_mgr.switch_args.dir;
    scui_window_switch_type_update(type, dir);
}

/*@brief 窗口移动动画回调
 */
void scui_window_move_anima_ready(void *instance)
{
    SCUI_LOG_INFO("");
    
    switch (scui_window_mgr.switch_args.type) {
    case scui_window_switch_center_in:
    case scui_window_switch_center_out:
    case scui_window_switch_zoom1:
    case scui_window_switch_zoom2:
    case scui_window_switch_normal: {
        if (scui_window_mgr.switch_args.pct == 0) {
            scui_window_active(scui_window_mgr.switch_args.list[0]);
            scui_widget_hide(scui_window_mgr.switch_args.list[1], true);
        }
        if (scui_window_mgr.switch_args.pct == 100) {
            scui_window_active(scui_window_mgr.switch_args.list[1]);
            scui_widget_hide(scui_window_mgr.switch_args.list[0], true);
        }
        break;
    }
    default:
        SCUI_LOG_ERROR("error switch type 0x%08x", scui_window_mgr.switch_args.type);
        break;
    }
    
    if (scui_window_mgr.switch_args.anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scui_window_mgr.switch_args.anima);
        scui_anima_destroy(scui_window_mgr.switch_args.anima);
        scui_window_mgr.switch_args.anima = SCUI_HANDLE_INVALID;
    }
    
    if (!scui_window_mgr.switch_args.hold_move) {
         scui_window_mgr.switch_args.lock_move = false;
         scui_window_mgr.switch_args.lock  = false;
    }
}

/*@brief 窗口移动动画回调
 */
void scui_window_move_anima_expired(void *instance)
{
    SCUI_LOG_DEBUG("");
    scui_anima_t *anima = instance;
    
    scui_multi_t pct = scui_abs(anima->value_c) * 100;
    if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0)
         pct /= scui_disp_get_ver_res();
    if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0)
         pct /= scui_disp_get_hor_res();
    
    scui_window_mgr.switch_args.pct = pct;
    scui_window_mgr.switch_args.ofs = anima->value_c;
    SCUI_LOG_DEBUG("pct:%d", scui_window_mgr.switch_args.pct);
    
    switch (scui_window_mgr.switch_args.type) {
    case scui_window_switch_center_in:
    case scui_window_switch_center_out: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_by_percent(100 - scui_window_mgr.switch_args.pct);
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_by_percent(scui_window_mgr.switch_args.pct);
        scui_point_t point = (scui_point_t){0};
        scui_widget_refr_pos(scui_window_mgr.switch_args.list[0], &point);
        scui_widget_refr_pos(scui_window_mgr.switch_args.list[1], &point);
        break;
    }
    case scui_window_switch_zoom1:
    case scui_window_switch_zoom2: {
        scui_widget_t *widget = NULL;
        widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
        widget->surface.alpha = scui_alpha_by_percent(100 - scui_window_mgr.switch_args.pct);
        widget = scui_handle_get(scui_window_mgr.switch_args.list[1]);
        widget->surface.alpha = scui_alpha_by_percent(scui_window_mgr.switch_args.pct);
    }
    case scui_window_switch_normal: {
        scui_handle_t handle = scui_window_mgr.switch_args.list[0];
        scui_area_t   clip   = scui_widget_attr_clip(handle);
        
        scui_point_t point = {.x = clip.x,.y = clip.y,};
        if ((scui_window_mgr.switch_args.dir & scui_event_dir_hor) != 0)
            point.x = anima->value_c;
        if ((scui_window_mgr.switch_args.dir & scui_event_dir_ver) != 0)
            point.y = anima->value_c;
        /* 移动窗口 */
        scui_widget_refr_pos(scui_window_mgr.switch_args.list[0], &point);
        if (scui_window_mgr.switch_args.dir == scui_event_dir_to_u)
            point.y += scui_widget_attr_clip(scui_window_mgr.switch_args.list[0]).h;
        if (scui_window_mgr.switch_args.dir == scui_event_dir_to_d)
            point.y -= scui_widget_attr_clip(scui_window_mgr.switch_args.list[1]).h;
        if (scui_window_mgr.switch_args.dir == scui_event_dir_to_l)
            point.x += scui_widget_attr_clip(scui_window_mgr.switch_args.list[0]).w;
        if (scui_window_mgr.switch_args.dir == scui_event_dir_to_r)
            point.x -= scui_widget_attr_clip(scui_window_mgr.switch_args.list[1]).w;
        /* 移动窗口 */
        scui_widget_refr_pos(scui_window_mgr.switch_args.list[1], &point);
        break;
    }
    default:
        SCUI_LOG_ERROR("error switch type 0x%08x", scui_window_mgr.switch_args.type);
        break;
    }
    
    /* 有自己的独立buffer,直接refr */
    scui_widget_t *widget = scui_handle_get(scui_window_mgr.switch_args.list[0]);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    if (scui_widget_surface_only(widget))
        scui_widget_refr(widget->myself, true);
    else
        scui_widget_draw(widget->myself, NULL, true);
}

/*@brief 窗口移动动画自动化
 */
void scui_window_move_anima_auto(int32_t value_s, int32_t value_e, uint32_t peroid)
{
    scui_anima_t anima = {0};
    anima.start   = scui_window_move_anima_start;
    anima.ready   = scui_window_move_anima_ready;
    anima.expired = scui_window_move_anima_expired;
    anima.value_s = value_s;
    anima.value_e = value_e;
    anima.peroid  = peroid != 0 ? peroid : scui_abs(anima.value_e - anima.value_s) / 2;
    
    SCUI_LOG_INFO("<%d, %d>", value_s, value_e);
    
    if (scui_window_mgr.switch_args.anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scui_window_mgr.switch_args.anima);
        scui_anima_destroy(scui_window_mgr.switch_args.anima);
        scui_window_mgr.switch_args.anima = SCUI_HANDLE_INVALID;
    }
    if (value_s == value_e) {
        anima.value_c = value_s = value_e;
        scui_window_move_anima_start(&anima);
        scui_window_move_anima_expired(&anima);
        scui_window_move_anima_ready(&anima);
        return;
    }
    scui_anima_create(&anima, &scui_window_mgr.switch_args.anima);
    scui_anima_start(scui_window_mgr.switch_args.anima);
}

/*@brief 窗口切换事件处理回调
 *@param handle 窗口句柄
 *@param inout  true:进入;false:退出;
 */
void scui_window_move_anima_inout(scui_handle_t handle, bool inout)
{
    scui_coord_t hor_res = scui_disp_get_hor_res();
    scui_coord_t ver_res = scui_disp_get_ver_res();
    scui_area_t clip = scui_widget_attr_clip(handle);
    SCUI_ASSERT(clip.x >= -clip.w && clip.x <= +clip.w);
    SCUI_ASSERT(clip.y >= -clip.h && clip.y <= +clip.h);
    
    /* 从当前位置到达目标点 */
    int32_t value_s = 0;
    int32_t value_e = 0;
    if (scui_window_mgr.switch_args.dir == scui_event_dir_to_r) {
        value_s = clip.x;
        value_e = inout ? 0 : +hor_res;
    }
    if (scui_window_mgr.switch_args.dir == scui_event_dir_to_l) {
        value_s = clip.x;
        value_e = inout ? 0 : -clip.w;
    }
    if (scui_window_mgr.switch_args.dir == scui_event_dir_to_d) {
        value_s = clip.y;
        value_e = inout ? 0 : +ver_res;
    }
    if (scui_window_mgr.switch_args.dir == scui_event_dir_to_u) {
        value_s = clip.y;
        value_e = inout ? 0 : -clip.h;
    }
    
    scui_window_move_anima_auto(value_s, value_e, 0);
}

/*@brief 窗口切换事件处理回调
 *@param event 事件
 */
void scui_window_switch_event(scui_event_t *event)
{
    scui_widget_t *widget = scui_handle_get(event->object);
    scui_window_t *window = widget;
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    /* 获得窗口宽高 */
    scui_area_t clip = scui_widget_attr_clip(widget->myself);
    SCUI_ASSERT(clip.x >= -clip.w && clip.x <= +clip.w);
    SCUI_ASSERT(clip.y >= -clip.h && clip.y <= +clip.h);
    
    switch (event->type) {
    case scui_event_ptr_down:
        event->style.result |= 0x01;
        /* 采用位置检测机制,这会使初始判断条件简单化 */
        scui_window_mgr.switch_args.pos = scui_event_pos_none;
        /* 水平位置先检测 */
        if (event->ptr_c.x <= clip.w * 1 / 2)
            scui_window_mgr.switch_args.pos |= scui_event_pos_l;
        if (event->ptr_c.x >= clip.w * 1 / 2)
            scui_window_mgr.switch_args.pos |= scui_event_pos_r;
        /* 垂直位置后检测,并且吸收水平位置检测结果 */
        if (event->ptr_c.y <= clip.h * 1 / 2)
            scui_window_mgr.switch_args.pos |= scui_event_pos_u;
        if (event->ptr_c.y >= clip.h * 1 / 2)
            scui_window_mgr.switch_args.pos |= scui_event_pos_d;
        break;
    case scui_event_ptr_move:
    case scui_event_ptr_fling:
        event->style.result |= 0x02;
        /* 场景正在切换时,中止更新切换目标 */
        if (scui_window_mgr.switch_args.lock_move) {
            SCUI_LOG_INFO("");
            if (scui_window_mgr.switch_args.pos == scui_event_pos_l)
                scui_window_move_anima_auto(clip.x, event->ptr_e.x, 0);
            if (scui_window_mgr.switch_args.pos == scui_event_pos_r)
                scui_window_move_anima_auto(clip.x, event->ptr_e.x - clip.w, 0);
            if (scui_window_mgr.switch_args.pos == scui_event_pos_u)
                scui_window_move_anima_auto(clip.y, event->ptr_e.y, 0);
            if (scui_window_mgr.switch_args.pos == scui_event_pos_d)
                scui_window_move_anima_auto(clip.y, event->ptr_e.y - clip.h, 0);
        } else {
            if (scui_window_mgr.switch_args.lock)
                break;
            if (scui_window_mgr.switch_args.pos == scui_event_pos_none)
                break;
            scui_point_t point = {0};
            scui_event_dir_t event_dir = scui_indev_ptr_dir(event);
            scui_handle_t target = SCUI_HANDLE_INVALID;
            SCUI_LOG_INFO("dir:%u", event_dir);
            /* 方向检测与条件加载 */
            if (event_dir == scui_event_dir_to_r &&
               (scui_window_mgr.switch_args.pos & scui_event_pos_l) != 0) { /* 左窗:方向向右 */
                target = window->cfg.sibling[2];
                point.x = -clip.w;
            }
            if (event_dir == scui_event_dir_to_l &&
               (scui_window_mgr.switch_args.pos & scui_event_pos_r) != 0) { /* 右窗:方向向左 */
                target = window->cfg.sibling[3];
                point.x = +clip.w;
            }
            if (event_dir == scui_event_dir_to_d &&
               (scui_window_mgr.switch_args.pos & scui_event_pos_u) != 0) { /* 上窗:方向向下 */
                target = window->cfg.sibling[0];
                point.y = -clip.h;
            }
            if (event_dir == scui_event_dir_to_u &&
               (scui_window_mgr.switch_args.pos & scui_event_pos_d) != 0) { /* 下窗:方向向上 */
                target = window->cfg.sibling[1];
                point.y = +clip.h;
            }
            /* 抓获到运动的目标 */
            if (target != SCUI_HANDLE_INVALID) {
                scui_window_mgr.switch_args.lock = true;
                scui_window_mgr.switch_args.lock_move = true;
                scui_window_mgr.switch_args.hold_move = true;
                scui_window_mgr.switch_args.flag_fling = event->type == scui_event_ptr_fling;
                scui_window_mgr.switch_args.dir = event_dir;
                scui_window_mgr.switch_args.pos = event_dir;
                scui_window_mgr.switch_args.list[0] = widget->myself;
                scui_window_mgr.switch_args.list[1] = target;
                /* 先释放其他窗口资源 */
                scui_window_hide_without(scui_window_mgr.switch_args.list[0], false);
                scui_widget_show(scui_window_mgr.switch_args.list[1], false);
                scui_widget_refr_pos(scui_window_mgr.switch_args.list[1], &point);
                /* 初始默认启动一次,等待动画被打断 */
                if (event->type == scui_event_ptr_fling)
                    scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], false);
                else {
                if (scui_window_mgr.switch_args.pos == scui_event_pos_l)
                    scui_window_move_anima_auto(clip.x, event->ptr_e.x, 0);
                if (scui_window_mgr.switch_args.pos == scui_event_pos_r)
                    scui_window_move_anima_auto(clip.x, event->ptr_e.x - clip.w, 0);
                if (scui_window_mgr.switch_args.pos == scui_event_pos_u)
                    scui_window_move_anima_auto(clip.y, event->ptr_e.y, 0);
                if (scui_window_mgr.switch_args.pos == scui_event_pos_d)
                    scui_window_move_anima_auto(clip.y, event->ptr_e.y - clip.h, 0);
                }
            }
        }
        break;
    case scui_event_ptr_up:
        event->style.result |= 0x01;
        if (scui_window_mgr.switch_args.lock_move) {
            scui_window_mgr.switch_args.hold_move = false;
            if (scui_window_mgr.switch_args.flag_fling) {
                scui_window_mgr.switch_args.flag_fling = false;
                scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], false);
                break;
            }
            if (scui_mabs(clip.x, clip.w) < clip.w * 1 / 3 &&
                scui_mabs(clip.y, clip.h) < clip.h * 1 / 3) {
                if (scui_window_mgr.switch_args.pos == scui_event_pos_u ||
                    scui_window_mgr.switch_args.pos == scui_event_pos_l) {
                    scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], true);
                    SCUI_LOG_DEBUG("");
                }
            } else {
                if (scui_window_mgr.switch_args.pos == scui_event_pos_u ||
                    scui_window_mgr.switch_args.pos == scui_event_pos_l) {
                    scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], false);
                    SCUI_LOG_DEBUG("");
                }
            }
            if (scui_mabs(clip.x, clip.w) < clip.w * 2 / 3 &&
                scui_mabs(clip.y, clip.h) < clip.h * 2 / 3) {
                if (scui_window_mgr.switch_args.pos == scui_event_pos_d ||
                    scui_window_mgr.switch_args.pos == scui_event_pos_r) {
                    scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], false);
                    SCUI_LOG_DEBUG("");
                }
            } else {
                if (scui_window_mgr.switch_args.pos == scui_event_pos_d ||
                    scui_window_mgr.switch_args.pos == scui_event_pos_r) {
                    scui_window_move_anima_inout(scui_window_mgr.switch_args.list[0], true);
                    SCUI_LOG_DEBUG("");
                }
            }
        }
        break;
    default:
        break;
    }
}
