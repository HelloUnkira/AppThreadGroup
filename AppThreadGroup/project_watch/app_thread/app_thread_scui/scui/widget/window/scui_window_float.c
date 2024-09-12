/*实现目标:
 *    窗口管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_window_float_t scui_window_float = {0};

/*@brief 窗口配置参数获取
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_float_cfg_get(scui_window_float_t *cfg)
{
    *cfg = scui_window_float;
}

/*@brief 窗口配置参数设置
 *@param handle 窗口控件句柄
 *@param cfg    配置参数
 */
void scui_window_float_cfg_set(scui_window_float_t *cfg)
{
    scui_window_float = *cfg;
    
    bool float_flag = false;
    /* 断言检查: */
    for (uint8_t idx = 0; idx < 4; idx++) {
        scui_handle_t handle = scui_window_float.list[idx];
        if (handle != SCUI_HANDLE_INVALID) {
            scui_widget_t *widget = scui_handle_get(handle);
            scui_widget_maker_t *maker = (void *)widget;
            SCUI_ASSERT(widget != NULL);
            
            /* 窗口至少要给定ptr事件响应标记 */
            if (scui_handle_remap(handle)) {
                SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
                SCUI_ASSERT(widget->style.indev_ptr);
            } else {
                SCUI_ASSERT(maker->parent == SCUI_HANDLE_INVALID);
                SCUI_ASSERT(maker->style.indev_ptr);
            }
            float_flag = true;
        }
    }
    if (float_flag) {
        scui_handle_t handle = scui_window_float.main;
        SCUI_ASSERT(handle != SCUI_HANDLE_INVALID);
        scui_widget_t *widget = scui_handle_get(handle);
        scui_widget_maker_t *maker = (void *)widget;
        SCUI_ASSERT(widget != NULL);
        
        /* 窗口至少要给定ptr事件响应标记 */
        if (scui_handle_remap(handle)) {
            SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
            SCUI_ASSERT(widget->style.indev_ptr);
        } else {
            SCUI_ASSERT(maker->parent == SCUI_HANDLE_INVALID);
            SCUI_ASSERT(maker->style.indev_ptr);
        }
    }
}

/*@brief 窗口正在工作中
 *@retval 正在工作中
 */
bool scui_window_float_running(void)
{
    return scui_window_float.cover ||
          !scui_widget_event_scroll_flag(0x02, &scui_window_float.key);
}

/*@brief 窗口移动伴随透明度更新
 *@param handle 窗口控件句柄
 *@param point  窗口移动坐标
 */
void scui_window_float_move_with_alpha(scui_handle_t handle, scui_point_t *point)
{
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    /* ??? */
    if (scui_handle_unmap(handle))
        return;
    /* 移动窗口 */
    scui_widget_move_pos(handle, point);
    /* 获得窗口宽高 */
    scui_area_t clip = scui_widget_clip(handle);
    SCUI_ASSERT(clip.x >= -clip.w && clip.x <= +clip.w);
    SCUI_ASSERT(clip.y >= -clip.h && clip.y <= +clip.h);
    /* 根据动作方向确定透明度 */
    scui_alpha_t alpha = scui_alpha_pct100;
    if (scui_window_float.pos == scui_opt_pos_l ||
        scui_window_float.pos == scui_opt_pos_r)
        alpha -= scui_abs(clip.x) * (scui_coord_t)scui_alpha_pct100 / clip.w;
    if (scui_window_float.pos == scui_opt_pos_u ||
        scui_window_float.pos == scui_opt_pos_d)
        alpha -= scui_abs(clip.y) * (scui_coord_t)scui_alpha_pct100 / clip.h;
    /* 更新透明度 */
    widget->surface->alpha = alpha;
    /* 有自己的独立buffer,直接refr */
    if (scui_widget_surface_only(widget))
        scui_widget_refr(handle, false);
    else
        scui_widget_draw(handle, NULL, false);
}

/*@brief 窗口浮动动画回调
 */
void scui_window_float_anima_start(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief 窗口浮动动画回调
 */
void scui_window_float_anima_ready(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima  = instance;
    scui_handle_t handle = scui_window_float.target;
    scui_coord_t hor_res = scui_disp_get_hor_res();
    scui_coord_t ver_res = scui_disp_get_ver_res();
    
    scui_area_t   clip = scui_widget_clip(handle);
    scui_point_t point = {.x = clip.x,.y = clip.y,};
    
    /* 默认已经覆盖,如果位置不对则已经回退 */
    if ((scui_window_float.pos == scui_opt_pos_l && point.x == -clip.w) ||
        (scui_window_float.pos == scui_opt_pos_u && point.y == -clip.h) ||
        (scui_window_float.pos == scui_opt_pos_r && point.x == +clip.w) ||
        (scui_window_float.pos == scui_opt_pos_d && point.y == +clip.h)) {
         scui_window_active(scui_window_float.main);
         scui_window_stack_cover(scui_window_float.main);
         scui_widget_hide(handle, true);
         // 主窗口虚化
         #if SCUI_WIDGET_ANIMA_ABORT_BY_SCROLL
         scui_widget_draw(scui_window_float.main, NULL, false);
         #endif
    }
    
    if (scui_window_float.anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scui_window_float.anima);
        scui_anima_destroy(scui_window_float.anima);
        scui_window_float.anima = SCUI_HANDLE_INVALID;
    }
    
    if (!scui_window_float.hold)
         scui_widget_event_scroll_flag(0x01, &scui_window_float.key);
}

/*@brief 窗口浮动动画回调
 */
void scui_window_float_anima_expired(void *instance)
{
    SCUI_LOG_INFO("");
    scui_anima_t *anima  = instance;
    scui_handle_t handle = scui_window_float.target;
    scui_area_t   clip   = scui_widget_clip(handle);
    
    scui_point_t point = {.x = clip.x,.y = clip.y,};
    if ((scui_window_float.dir & scui_opt_dir_hor) != 0)
        point.x = anima->value_c;
    if ((scui_window_float.dir & scui_opt_dir_ver) != 0)
        point.y = anima->value_c;
    
    SCUI_LOG_INFO("<%d, %d>", point.x, point.y);
    scui_window_float_move_with_alpha(handle, &point);
}

/*@brief 窗口浮动动画自动化
 */
void scui_window_float_anima_auto(int32_t value_s, int32_t value_e, uint32_t peroid)
{
    scui_anima_t anima = {0};
    anima.start   = scui_window_float_anima_start;
    anima.ready   = scui_window_float_anima_ready;
    anima.expired = scui_window_float_anima_expired;
    anima.value_s = value_s;
    anima.value_e = value_e;
    anima.peroid  = peroid != 0 ? peroid : scui_abs(anima.value_e - anima.value_s) / 2;
    
    SCUI_LOG_INFO("<%d, %d>", value_s, value_e);
    
    if (scui_window_float.anima != SCUI_HANDLE_INVALID) {
        scui_anima_stop(scui_window_float.anima);
        scui_anima_destroy(scui_window_float.anima);
        scui_window_float.anima = SCUI_HANDLE_INVALID;
    }
    if (value_s == value_e) {
        anima.value_c = value_s = value_e;
        scui_window_float_anima_start(&anima);
        scui_window_float_anima_expired(&anima);
        scui_window_float_anima_ready(&anima);
        return;
    }
    scui_anima_create(&anima, &scui_window_float.anima);
    scui_anima_start(scui_window_float.anima);
}

/*@brief 窗口浮动动画回调
 *@param handle 窗口句柄
 *@param inout  true:进入;false:退出;
 */
void scui_window_float_anima_inout(scui_handle_t handle, bool inout)
{
    scui_coord_t hor_res = scui_disp_get_hor_res();
    scui_coord_t ver_res = scui_disp_get_ver_res();
    scui_area_t clip = scui_widget_clip(handle);
    SCUI_ASSERT(clip.x >= -clip.w && clip.x <= +clip.w);
    SCUI_ASSERT(clip.y >= -clip.h && clip.y <= +clip.h);
    
    /* 从当前位置到达目标点 */
    int32_t value_s = 0;
    int32_t value_e = 0;
    if (scui_window_float.dir == scui_opt_dir_to_r) {
        value_s = clip.x;
        value_e = inout ? 0 : -clip.w;
    }
    if (scui_window_float.dir == scui_opt_dir_to_l) {
        value_s = clip.x;
        value_e = inout ? 0 : +hor_res;
    }
    if (scui_window_float.dir == scui_opt_dir_to_d) {
        value_s = clip.y;
        value_e = inout ? 0 : -clip.h;
    }
    if (scui_window_float.dir == scui_opt_dir_to_u) {
        value_s = clip.y;
        value_e = inout ? 0 : +ver_res;
    }
    
    scui_window_float_anima_auto(value_s, value_e, 0);
}

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 */
void scui_window_float_event_grasp_show(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    scui_widget_show(scui_window_float.main, false);
}

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 */
void scui_window_float_event_grasp_hide(scui_event_t *event)
{
    if (!scui_widget_event_check_finish(event))
         return;
    
}

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 */
void scui_window_float_event_grasp_key(scui_event_t *event)
{
    if (!scui_widget_event_check_execute(event))
         return;
    
    if (event->type == scui_event_key_click) {
        /* 全局滚动锁定 */
        if (!scui_widget_event_scroll_flag(0x00, &scui_window_float.key))
             return;
        scui_window_float_anima_inout(event->object, false);
        scui_widget_event_mask_over(event);
    }
}

/*@brief 窗口浮动事件抓取回调
 *@param event 事件
 */
void scui_window_float_event_grasp_ptr(scui_event_t *event)
{
    if (!scui_widget_event_check_execute(event))
         return;
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    /* 获得窗口宽高 */
    scui_area_t clip = scui_widget_clip(handle);
    SCUI_ASSERT(clip.x >= -clip.w && clip.x <= +clip.w);
    SCUI_ASSERT(clip.y >= -clip.h && clip.y <= +clip.h);
    
    switch (event->type) {
    case scui_event_ptr_down: {
        scui_widget_event_mask_keep(event);
        scui_window_float.cover = true;
        break;
    }
    case scui_event_ptr_move: {
        /* 全局滚动锁定 */
        if (!scui_widget_event_scroll_flag(0x00, &scui_window_float.key))
             break;
        scui_widget_event_mask_keep(event);
        if (scui_window_float.target != handle)
            break;
        int32_t value_s = 0;
        int32_t value_e = 0;
        if ((scui_window_float.dir & scui_opt_dir_ver) != 0) {
            value_s = clip.y;
            if (scui_window_float.dir == scui_opt_dir_to_d)
                value_e = event->ptr_e.y - clip.h;
            if (scui_window_float.dir == scui_opt_dir_to_u)
                value_e = event->ptr_e.y;
        }
        if ((scui_window_float.dir & scui_opt_dir_hor) != 0) {
            value_s = clip.x;
            if (scui_window_float.dir == scui_opt_dir_to_r)
                value_e = event->ptr_e.x - clip.w;
            if (scui_window_float.dir == scui_opt_dir_to_l)
                value_e = event->ptr_e.x;
        }
        scui_window_float_anima_auto(value_s, value_e, 0);
        scui_widget_event_mask_over(event);
        scui_window_float.hold = true;
        break;
    }
    case scui_event_ptr_up: {
        scui_widget_event_mask_keep(event);
        if (scui_window_float.target != handle)
            break;
        if (scui_window_float.cover) {
            // 没有移动时,不做回弹
            if (!scui_window_float.hold)
                 break;
            
            if (scui_mabs(clip.x, clip.w) < clip.w * 2 / 3 &&
                scui_mabs(clip.y, clip.h) < clip.h * 2 / 3) {
                if (scui_window_float.pos == scui_opt_pos_u ||
                    scui_window_float.pos == scui_opt_pos_l) {
                    scui_window_float_anima_inout(handle, false);
                    SCUI_LOG_DEBUG("");
                }
            } else {
                if (scui_window_float.pos == scui_opt_pos_u ||
                    scui_window_float.pos == scui_opt_pos_l) {
                    scui_window_float_anima_inout(handle, true);
                    SCUI_LOG_DEBUG("");
                }
            }
            if (scui_mabs(clip.x, clip.w) < clip.w * 1 / 3 &&
                scui_mabs(clip.y, clip.h) < clip.h * 1 / 3) {
                if (scui_window_float.pos == scui_opt_pos_d ||
                    scui_window_float.pos == scui_opt_pos_r) {
                    scui_window_float_anima_inout(handle, true);
                    SCUI_LOG_DEBUG("");
                }
            } else {
                if (scui_window_float.pos == scui_opt_pos_d ||
                    scui_window_float.pos == scui_opt_pos_r) {
                    scui_window_float_anima_inout(handle, false);
                    SCUI_LOG_DEBUG("");
                }
            }
        } else {
            if (scui_mabs(clip.x, clip.w) < clip.w * 1 / 3 &&
                scui_mabs(clip.y, clip.h) < clip.h * 1 / 3) {
                if (scui_window_float.pos == scui_opt_pos_u ||
                    scui_window_float.pos == scui_opt_pos_l) {
                    scui_window_float_anima_inout(handle, false);
                    SCUI_LOG_DEBUG("");
                }
            } else {
                if (scui_window_float.pos == scui_opt_pos_u ||
                    scui_window_float.pos == scui_opt_pos_l) {
                    scui_window_float_anima_inout(handle, true);
                    SCUI_LOG_DEBUG("");
                }
            }
            if (scui_mabs(clip.x, clip.w) < clip.w * 2 / 3 &&
                scui_mabs(clip.y, clip.h) < clip.h * 2 / 3) {
                if (scui_window_float.pos == scui_opt_pos_d ||
                    scui_window_float.pos == scui_opt_pos_r) {
                    scui_window_float_anima_inout(handle, true);
                    SCUI_LOG_DEBUG("");
                }
            } else {
                if (scui_window_float.pos == scui_opt_pos_d ||
                    scui_window_float.pos == scui_opt_pos_r) {
                    scui_window_float_anima_inout(handle, false);
                    SCUI_LOG_DEBUG("");
                }
            }
        }
        scui_window_float.hold = false;
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口浮动事件检查回调
 *@param event 事件
 */
void scui_window_float_event_check_ptr(scui_event_t *event)
{
    if (!scui_widget_event_check_execute(event))
         return;
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    /* 获得窗口宽高 */
    scui_area_t clip = scui_widget_clip(handle);
    SCUI_ASSERT(clip.x >= -clip.w && clip.x <= +clip.w);
    SCUI_ASSERT(clip.y >= -clip.h && clip.y <= +clip.h);
    
    switch (event->type) {
    case scui_event_ptr_down: {
        scui_widget_event_mask_keep(event);
        scui_window_float.cover = false;
        /* 采用位置检测机制,这会使初始判断条件简单化 */
        scui_window_float.pos = scui_opt_pos_none;
        /* 水平位置先检测 */
        if (event->ptr_c.x <= clip.w * 1 / 2)
            scui_window_float.pos |= scui_opt_pos_l;
        if (event->ptr_c.x >= clip.w * 1 / 2)
            scui_window_float.pos |= scui_opt_pos_r;
        /* 垂直位置后检测,并且吸收水平位置检测结果 */
        if (event->ptr_c.y <= clip.h * 1 / 2)
            scui_window_float.pos |= scui_opt_pos_u;
        if (event->ptr_c.y >= clip.h * 1 / 2)
            scui_window_float.pos |= scui_opt_pos_d;
        break;
    }
    case scui_event_ptr_move: {
        scui_widget_event_mask_keep(event);
        if (scui_window_float.pos == scui_opt_pos_none)
            break;
        scui_point_t point = {0};
        scui_opt_dir_t event_dir = scui_indev_ptr_dir(event);
        scui_window_float.target = SCUI_HANDLE_INVALID;
        /* 方向检测与条件加载 */
        if (event_dir == scui_opt_dir_to_r &&
           (scui_window_float.pos & scui_opt_pos_l) != 0) { /* 左浮窗:方向向右 */
            scui_window_float.target = scui_window_float.list[2];
            point.x = -clip.w;
        }
        if (event_dir == scui_opt_dir_to_l &&
           (scui_window_float.pos & scui_opt_pos_r) != 0) { /* 右浮窗:方向向左 */
            scui_window_float.target = scui_window_float.list[3];
            point.x = +clip.w;
        }
        if (event_dir == scui_opt_dir_to_d &&
           (scui_window_float.pos & scui_opt_pos_u) != 0) { /* 上浮窗:方向向下 */
            scui_window_float.target = scui_window_float.list[0];
            point.y = -clip.h;
        }
        if (event_dir == scui_opt_dir_to_u &&
           (scui_window_float.pos & scui_opt_pos_d) != 0) { /* 下浮窗:方向向上 */
            scui_window_float.target = scui_window_float.list[1];
            point.y = +clip.h;
        }
        /* 抓获到运动的目标 */
        if (scui_window_float.target != SCUI_HANDLE_INVALID) {
            /* 全局滚动锁定 */
            if (!scui_widget_event_scroll_flag(0x00, &scui_window_float.key))
                 break;
            /* 先释放其他窗口资源 */
            scui_window_list_hide_without(handle, false);
            scui_widget_show(scui_window_float.target, false);
            scui_window_active(scui_window_float.target);
            scui_window_stack_cover(scui_window_float.target);
            /* 移动到所在边界然后真实重绘 */
            scui_window_float_move_with_alpha(scui_window_float.target, &point);
            
            // 主窗口虚化
            #if SCUI_WIDGET_ANIMA_ABORT_BY_SCROLL
            scui_widget_draw(handle, NULL, false);
            scui_event_t event_draw = {0};
            event_draw.object = handle;
            event_draw.type = scui_event_draw;
            // 移除跟主窗口相关所有绘制事件
            while (scui_event_dequeue(&event_draw, true));
            /* 主窗口虚化 */
            scui_widget_draw_blur(handle, NULL);
            SCUI_LOG_INFO("");
            #endif
            
            scui_window_float.main  = handle;
            scui_window_float.dir   = event_dir;
            scui_window_float.pos   = event_dir;
            scui_window_float.cover = false;
            scui_window_float.hold  = true;
            /* 初始默认启动一次,等待动画被打断 */
            int32_t value_s = 0;
            int32_t value_e = 0;
            if ((scui_window_float.dir & scui_opt_dir_ver) != 0) {
                value_s = point.y;
                if (scui_window_float.dir == scui_opt_dir_to_d)
                    value_e = event->ptr_e.y - clip.h;
                if (scui_window_float.dir == scui_opt_dir_to_u)
                    value_e = event->ptr_e.y;
            }
            if ((scui_window_float.dir & scui_opt_dir_hor) != 0) {
                value_s = point.x;
                if (scui_window_float.dir == scui_opt_dir_to_r)
                    value_e = event->ptr_e.x - clip.w;
                if (scui_window_float.dir == scui_opt_dir_to_l)
                    value_e = event->ptr_e.x;
            }
            scui_window_float_anima_auto(value_s, value_e, 0);
            scui_widget_event_mask_over(event);
        }
        break;
    }
    case scui_event_ptr_fling:
        break;
    case scui_event_ptr_up:
        break;
    default:
        break;
    }
}
