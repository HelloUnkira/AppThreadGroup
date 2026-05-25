#include "lvgl_view_sched.h"
lvgl_view_sched_t lvgl_view_sched = {0};

/*@brief 滑动方向检查
 */
static lv_dir_t lvgl_view_slide_way(void)
{
    lv_coord_t dist_x =
        lvgl_view_dist(lvgl_view_sched.touch_last.x, lvgl_view_sched.touch_curr.x);
    lv_coord_t dist_y = lvgl_view_dist(lvgl_view_sched.touch_last.y,
            lvgl_view_sched.touch_curr.y);
    // 如果系统没消抖, 用这个, 否则默认系统消抖即可, 此时将它改为1
    const lv_coord_t dist_jitter = 5;
    
    // 继续检查, 现在还不确定距离
    if (dist_x < dist_jitter && dist_y < dist_jitter)
    {
        return LV_DIR_NONE;
    }
    
    if (dist_x > dist_y)
    {
        if (lvgl_view_sched.touch_last.x > lvgl_view_sched.touch_curr.x)
        {
            return LV_DIR_LEFT;
        }
        
        if (lvgl_view_sched.touch_last.x < lvgl_view_sched.touch_curr.x)
        {
            return LV_DIR_RIGHT;
        }
    }
    
    if (dist_y > dist_x)
    {
        if (lvgl_view_sched.touch_last.y > lvgl_view_sched.touch_curr.y)
        {
            return LV_DIR_TOP;
        }
        
        if (lvgl_view_sched.touch_last.y < lvgl_view_sched.touch_curr.y)
        {
            return LV_DIR_BOTTOM;
        }
    }
    
    return LV_DIR_NONE;
}

static void lvgl_view_event_bubble(lv_obj_t *obj)
{
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
    
    for (uint32_t id = 0; id < lv_obj_get_child_cnt(obj); id++)
    {
        lv_obj_t *child = lv_obj_get_child(obj, id);
        lvgl_view_event_bubble(child);
    }
}

static void lvgl_view_obj_scroll_lock(lv_obj_t *obj, bool lock)
{
    // 待定中使用?往基类对象塞个变量去备份呢???
    if (lock)
    {
        lv_obj_allocate_spec_attr(obj);
        // obj->spec_attr->scroll_flag = 0;
        
        if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE))
        {
            // obj->spec_attr->scroll_flag = LV_OBJ_FLAG_SCROLLABLE;
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        }
    }
    else
    {
        lv_obj_allocate_spec_attr(obj);
        
        // if (obj->spec_attr->scroll_flag == LV_OBJ_FLAG_SCROLLABLE)
        //     lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        
        // obj->spec_attr->scroll_flag = 0;
    }
    
    for (uint32_t id = 0; id < lv_obj_get_child_cnt(obj); id++)
    {
        lv_obj_t *child = lv_obj_get_child(obj, id);
        lvgl_view_obj_scroll_lock(child, lock);
    }
}

static bool lvgl_view_obj_scrollable(lv_obj_t *obj, lv_dir_t dir,
    lv_obj_t **obj_scrollable, lv_coord_t *obj_dist)
{
    // 滚动条件已经达成
    if (lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE) &&
        (lv_obj_get_scroll_dir(obj) & dir) != 0)
    {
    
        // LV_LOG_WARN("obj:%p", obj);
        // LV_LOG_WARN("scroll_dir:%d", lv_obj_get_scroll_dir(obj));
        lv_coord_t scroll_delta = 0;
        
        // 检查是否可以实际滚动, 如果可以计算是否具备滚动条件
        scroll_delta = lv_obj_get_scroll_bottom(obj);
        
        if (dir == LV_DIR_TOP && scroll_delta > 0)
        {
            // LV_LOG_WARN("scroll_b:%d", scroll_delta);
            if (obj_scrollable != NULL)
            {
                *obj_scrollable = obj;
            }
            
            if (obj_dist != NULL)
            {
                *obj_dist = scroll_delta;
            }
            
            return true;
        }
        
        scroll_delta = lv_obj_get_scroll_top(obj);
        
        if (dir == LV_DIR_BOTTOM && scroll_delta > 0)
        {
            // LV_LOG_WARN("scroll_t:%d", scroll_deltas);
            if (obj_scrollable != NULL)
            {
                *obj_scrollable = obj;
            }
            
            if (obj_dist != NULL)
            {
                *obj_dist = scroll_delta;
            }
            
            return true;
        }
        
        scroll_delta = lv_obj_get_scroll_right(obj);
        
        if (dir == LV_DIR_LEFT && scroll_delta > 0)
        {
            // LV_LOG_WARN("scroll_r:%d", scroll_delta);
            if (obj_scrollable != NULL)
            {
                *obj_scrollable = obj;
            }
            
            if (obj_dist != NULL)
            {
                *obj_dist = scroll_delta;
            }
            
            return true;
        }
        
        scroll_delta = lv_obj_get_scroll_left(obj);
        
        if (dir == LV_DIR_RIGHT && scroll_delta > 0)
        {
            // LV_LOG_WARN("scroll_l:%d", scroll_delta);
            if (obj_scrollable != NULL)
            {
                *obj_scrollable = obj;
            }
            
            if (obj_dist != NULL)
            {
                *obj_dist = scroll_delta;
            }
            
            return true;
        }
    }
    
    return false;
}

// 向上冒泡到父控件
bool lvgl_view_obj_bubble_scrollable(lv_obj_t *obj, lv_dir_t dir,
    lv_obj_t **obj_scrollable, lv_coord_t *obj_dist)
{
    if (lvgl_view_obj_scrollable(obj, dir, obj_scrollable, obj_dist))
    {
        return true;
    }
    
    lv_obj_t *parent = lv_obj_get_parent(obj);
    
    if (parent == NULL)
    {
        return false;
    }
    
    return lvgl_view_obj_bubble_scrollable(parent, dir, obj_scrollable, obj_dist);
}

// 向下沉底到子控件
bool lvgl_view_obj_tree_scrollable(lv_obj_t *obj, lv_dir_t dir,
    lv_obj_t **obj_scrollable, lv_coord_t *obj_dist)
{
    if (lvgl_view_obj_scrollable(obj, dir, obj_scrollable, obj_dist))
    {
        return true;
    }
    
    for (uint32_t id = 0; id < lv_obj_get_child_cnt(obj); id++)
    {
        lv_obj_t *child = lv_obj_get_child(obj, id);
        
        if (lvgl_view_obj_tree_scrollable(child, dir, obj_scrollable, obj_dist))
        {
            return true;
        }
    }
    
    return false;
}

static void lvgl_view_src_make(void)
{
    #if LVGL_VIEW_CVS_USE
    LV_LOG_WARN("snapshot view_c:\n");
    LV_LOG_WARN("snapshot view_t:\n");
    lv_snapshot_take_to_buf(lvgl_view_sched.view_c, lvgl_view_sched.view_cvs_cf,
        &lvgl_view_sched.view_sps_c, lvgl_view_sched.buffer_c,
        lvgl_view_sched.buffer_size);
    lv_snapshot_take_to_buf(lvgl_view_sched.view_t, lvgl_view_sched.view_cvs_cf,
        &lvgl_view_sched.view_sps_t, lvgl_view_sched.buffer_t,
        lvgl_view_sched.buffer_size);
    LV_LOG_WARN("snapshot over\n");
    // 显示滑动特效控件, 隐藏原始界面
    lv_obj_clear_flag(lvgl_view_sched.view_cvs, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lvgl_view_sched.view_c, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lvgl_view_sched.view_t, LV_OBJ_FLAG_HIDDEN);
    #endif
}

static void lvgl_view_src_burn(void)
{
    #if LVGL_VIEW_CVS_USE
    // 隐藏显示滑动特效控件, 显示原始界面(view_t被回收了)
    lv_obj_clear_flag(lvgl_view_sched.view_c, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(lvgl_view_sched.view_cvs, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_pos(lvgl_view_sched.view_c, 0, 0);
    #endif
}

static void lvgl_view_page_config(uint32_t page)
{
    if (lvgl_view_sched.view_c != NULL)
    {
        lvgl_view_config_t *config = lvgl_view_id_config(lvgl_view_sched.page_c);
        lv_obj_t *page_c = lv_obj_get_child(lvgl_view_sched.view_c, 0);
        config->destroy(page_c);
        
        lv_obj_del(lvgl_view_sched.view_c);
        lvgl_view_sched.view_c = NULL;
        
        lvgl_view_sched.move_lock = false;
        lvgl_view_sched.slide_dir = LV_DIR_NONE;
    }
    
    if (lvgl_view_sched.view_t != NULL)
    {
        lvgl_view_config_t *config = lvgl_view_id_config(lvgl_view_sched.page_t);
        lv_obj_t *page_t = lv_obj_get_child(lvgl_view_sched.view_t, 0);
        config->destroy(page_t);
        
        lv_obj_del(lvgl_view_sched.view_t);
        lvgl_view_sched.view_t = NULL;
    }
    
    
    lvgl_view_sched.page_c = page;
    lvgl_view_config_t *config = lvgl_view_id_config(lvgl_view_sched.page_c);
    
    /* 主显示未创建, 创建主显示(待定中...) */
    if (lvgl_view_sched.view_c == NULL)
    {
        lv_obj_t *view = lv_obj_create(lvgl_view_sched.view_root);
        
        if (lvgl_view_sched.view_event_cb != NULL)
        {
            lv_obj_add_event_cb(view, lvgl_view_sched.view_event_cb, LV_EVENT_ALL, view);
        }
        
        lv_obj_set_size(view, LV_HOR_RES, LV_VER_RES);
        lv_obj_clear_flag(view, LV_OBJ_FLAG_SCROLLABLE);
        lvgl_view_sched.view_c = view;
        
        lv_obj_t *page = lv_obj_create(view);
        lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
        lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
        config->create(page);
        lvgl_view_event_bubble(view);
    }
}

static void lvgl_view_slide_anim_exec_cb(void *var, int32_t val)
{
    // 这里需要限速
    #if 0
    static uint32_t prev_tick = 0;
    static const uint32_t fps = 60;
    uint32_t fps_tick = 1000 / fps;
    
    if (lv_tick_elaps(prev_tick) < fps_tick)
    {
        return;
    }
    
    fps_tick = lv_tick_get();
    #endif
    
    
    
    lv_point_t point_c = {0};
    int32_t pct = LV_ABS(val) * 100;
    
    if ((lvgl_view_sched.slide_dir & LV_DIR_HOR) != 0)
    {
        pct /= LV_HOR_RES;
        point_c.x = val;
    }
    
    if ((lvgl_view_sched.slide_dir & LV_DIR_VER) != 0)
    {
        pct /= LV_VER_RES;
        point_c.y = val;
    }
    
    lv_point_t point_t = point_c;
    
    if (lvgl_view_sched.slide_dir == LV_DIR_TOP)
    {
        point_t.y += LV_VER_RES;
    }
    
    if (lvgl_view_sched.slide_dir == LV_DIR_BOTTOM)
    {
        point_t.y -= LV_VER_RES;
    }
    
    if (lvgl_view_sched.slide_dir == LV_DIR_LEFT)
    {
        point_t.x += LV_HOR_RES;
    }
    
    if (lvgl_view_sched.slide_dir == LV_DIR_RIGHT)
    {
        point_t.x -= LV_HOR_RES;
    }
    
    lvgl_view_sched.pct = pct;
    lvgl_view_sched.point_c = point_c;
    lvgl_view_sched.point_t = point_t;
    
    // LV_LOG_WARN("val:%d, pct:%d", val, pct);
    
    // 发起一次异步调度(这里要异步吗，这里对吗)
    // 这里的异步存在争议点, 存在ready后资源已经回收, 还在执行transform的可能
    // lv_async_call_cancel(lvgl_view_transform_async_cb, NULL);
    // lv_async_call(lvgl_view_transform_async_cb, NULL);
    
    lvgl_view_transform_async_cb(NULL);
    // 这里的递归调度可以直接同步
    // 因为它的调度链的源头来源于lv_anim
    // 而不是普通的时间调度的调度链
}

static void lvgl_view_slide_anim_start_cb(lv_anim_t *a)
{
    // LV_LOG_WARN("");
    // lvgl_view_transform_async_start_cb();
    lvgl_view_sched.anim = a;
}

static void lvgl_view_slide_anim_ready_cb(lv_anim_t *a)
{
    // LV_LOG_WARN("");
    lvgl_view_sched.anim = NULL;
    
    // 仅仅最后一次帧动画才回收资源
    if (lvgl_view_sched.move_lock)
    {
        return;
    }
    
    // LV_LOG_WARN("pct:%d", lvgl_view_sched.pct);
    
    bool recycle = false;
    
    if (lvgl_view_sched.pct == 0)
    {
        lvgl_view_transform_reset();
        lvgl_view_config_t *config = lvgl_view_id_config(lvgl_view_sched.page_t);
        lv_obj_t *page_t = lv_obj_get_child(lvgl_view_sched.view_t, 0);
        config->destroy(page_t);
        
        lv_obj_del(lvgl_view_sched.view_t);
        lvgl_view_sched.page_c = lvgl_view_sched.page_c;
        lvgl_view_sched.view_c = lvgl_view_sched.view_c;
        lvgl_view_stack_update(lvgl_view_sched.page_c, 0);
        lvgl_view_obj_scroll_lock(lvgl_view_sched.view_c, false);
        LV_ASSERT(lvgl_view_sched.view_c != NULL);
        lv_obj_set_pos(lvgl_view_sched.view_c, 0, 0);
        lvgl_view_sched.page_t = 0;
        lvgl_view_sched.view_t = NULL;
        recycle = true;
    }
    
    if (lvgl_view_sched.pct == 100)
    {
        lvgl_view_transform_reset();
        lvgl_view_config_t *config = lvgl_view_id_config(lvgl_view_sched.page_c);
        lv_obj_t *page_c = lv_obj_get_child(lvgl_view_sched.view_c, 0);
        config->destroy(page_c);
        
        lv_obj_del(lvgl_view_sched.view_c);
        lvgl_view_sched.page_c = lvgl_view_sched.page_t;
        lvgl_view_sched.view_c = lvgl_view_sched.view_t;
        lvgl_view_stack_update(lvgl_view_sched.page_c, 0);
        LV_ASSERT(lvgl_view_sched.view_c != NULL);
        lv_obj_set_pos(lvgl_view_sched.view_c, 0, 0);
        lvgl_view_sched.page_t = 0;
        lvgl_view_sched.view_t = NULL;
        recycle = true;
    }
    
    if (recycle)
    {
        lvgl_view_src_burn();
        //
        lvgl_view_sched.slide_dir = LV_DIR_NONE;
        lvgl_view_sched.anim_lock = false;
        lvgl_view_sched.move_lock = false;
        lvgl_view_sched.jump_lock = false;
    }
}

static void lvgl_view_slide_anim_auto(int32_t val_s, int32_t val_e,
    int32_t val_all, int32_t period)
{
    // 将之前的动画删除掉
    lvgl_view_sched.anim_lock = true;
    lv_anim_del(lvgl_view_sched.view_root, NULL);
    
    if (period == 0)
    {
        period = LV_ABS(val_e - val_s);
    }
    
    // LV_LOG_WARN("val_s:%d, val_e:%d", val_s, val_e);
    
    if (val_s == val_e)
    {
        lvgl_view_slide_anim_start_cb(NULL);
        lvgl_view_slide_anim_exec_cb(NULL, val_s);
        lvgl_view_slide_anim_ready_cb(NULL);
        lvgl_view_sched.anim_lock = false;
        return;
    }
    
    // 启动一个新的动画
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, lvgl_view_sched.view_root);
    lv_anim_set_values(&anim, val_s, val_e);
    lv_anim_set_path_cb(&anim,  lv_anim_path_linear);
    lv_anim_set_exec_cb(&anim,  lvgl_view_slide_anim_exec_cb);
    lv_anim_set_start_cb(&anim, lvgl_view_slide_anim_start_cb);
    lv_anim_set_ready_cb(&anim, lvgl_view_slide_anim_ready_cb);
    // lv_anim_set_early_apply(&anim, false); // 不跟手
    lv_anim_set_repeat_count(&anim, 0);
    
    // 这里的跟手速度可以适当调一下，可以加快或者变慢
    period = period * LVGL_VIEW_ANIM_SPEED / LV_HOR_RES;
    lv_anim_set_time(&anim, period);
    
    if (lvgl_view_sched.anim_path_cb != NULL)
    {
        lv_anim_set_path_cb(&anim, lvgl_view_sched.anim_path_cb);
    }
    
    lvgl_view_sched.anim = lv_anim_start(&anim);
}

static void lvgl_view_slide_anim_inout(bool inout)
{
    lv_point_t point_c = lvgl_view_sched.point_c;
    
    /* 从当前位置到达目标点 */
    int32_t value_s = 0, value_e = 0, value_all = 0;
    
    if (lvgl_view_sched.slide_dir == LV_DIR_TOP)
    {
        value_s = point_c.y;
        value_e = inout ? 0 : -LV_VER_RES;
        value_all = LV_VER_RES;
    }
    
    if (lvgl_view_sched.slide_dir == LV_DIR_BOTTOM)
    {
        value_s = point_c.y;
        value_e = inout ? 0 : +LV_VER_RES;
        value_all = LV_VER_RES;
    }
    
    if (lvgl_view_sched.slide_dir == LV_DIR_LEFT)
    {
        value_s = point_c.x;
        value_e = inout ? 0 : -LV_HOR_RES;
        value_all = LV_HOR_RES;
    }
    
    if (lvgl_view_sched.slide_dir == LV_DIR_RIGHT)
    {
        value_s = point_c.x;
        value_e = inout ? 0 : +LV_HOR_RES;
        value_all = LV_HOR_RES;
    }
    
    lvgl_view_slide_anim_auto(value_s, value_e, value_all, 0);
}

/*@brief 滑动监听事件回调
 */
static void lvgl_view_slide_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    
    switch (code)
    {
    case LV_EVENT_PRESSED:
    {
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &lvgl_view_sched.touch_last);
        lvgl_view_sched.touch_press = lvgl_view_sched.touch_last;
        lvgl_view_sched.touch_curr  = lvgl_view_sched.touch_last;
        lvgl_view_sched.touch_dist  = lvgl_view_sched.touch_last;
        lvgl_view_sched.touch_skip  = false;
        break;
    }
    
    case LV_EVENT_PRESSING:
    {
        lv_indev_t *indev = lv_indev_get_act();
        lv_indev_get_point(indev, &lvgl_view_sched.touch_curr);
        lv_disp_trig_activity(NULL);
        
        // 第一次滑动事件监听
        if (!lvgl_view_sched.move_lock)
        {
            lv_dir_t slide_dir = lvgl_view_slide_way();
            
            // 子控件滚动不做窗口切换
            if (lvgl_view_sched.view_c != NULL)
            {
                if (lvgl_view_obj_bubble_scrollable(obj, slide_dir, NULL, NULL))
                {
                    lvgl_view_sched.touch_skip = true;
                    // LV_LOG_WARN("view inner scrollable now");
                    break;
                }
                
                // LV_LOG_WARN("view inner scrollable not find");
            }
            
            // 子控件滚动过不做窗口切换
            if (lvgl_view_sched.touch_skip)
            {
                LV_LOG_WARN("view switch");
                break;
            }
            
            if (lvgl_view_sched.anim_lock)
            {
                LV_LOG_WARN("view switch");
                break;
            }
            
            if (lvgl_view_sched.jump_lock)
            {
                LV_LOG_WARN("view switch");
                break;
            }
            
            if (lvgl_view_sched.page_c != 0 && slide_dir != LV_DIR_NONE)
            {
                // 更新到对应的界面的回调, 创建需要使用到的目标界面
                lvgl_view_config_t *config_c = lvgl_view_id_config(lvgl_view_sched.page_c);
                lvgl_view_config_t *config_t = NULL;
                LV_ASSERT(config_c->create  != NULL);
                LV_ASSERT(config_c->destroy != NULL);
                uint32_t transform = 0;
                
                
                lv_point_t point_t = {0};
                
                // 如果不存在对应界面的创建函数, 那意味着无法到达
                if (config_c->page_r != 0 && slide_dir == LV_DIR_LEFT)
                {
                    lvgl_view_sched.page_t = config_c->page_r;
                    transform = config_c->transform_r;
                    point_t.x += LV_HOR_RES;
                    config_t = lvgl_view_id_config(config_c->page_r);
                }
                
                if (config_c->page_l != 0 && slide_dir == LV_DIR_RIGHT)
                {
                    lvgl_view_sched.page_t = config_c->page_l;
                    transform = config_c->transform_l;
                    point_t.x -= LV_HOR_RES;
                    config_t = lvgl_view_id_config(config_c->page_l);
                }
                
                if (config_c->page_t != 0 && slide_dir == LV_DIR_BOTTOM)
                {
                    lvgl_view_sched.page_t = config_c->page_t;
                    transform = config_c->transform_t;
                    point_t.y -= LV_VER_RES;
                    config_t = lvgl_view_id_config(config_c->page_t);
                }
                
                if (config_c->page_b != 0 && slide_dir == LV_DIR_TOP)
                {
                    lvgl_view_sched.page_t = config_c->page_b;
                    transform = config_c->transform_b;
                    point_t.y += LV_VER_RES;
                    config_t = lvgl_view_id_config(config_c->page_b);
                }
                
                if (config_t != NULL)
                {
                    LV_ASSERT(config_t->create  != NULL);
                    LV_ASSERT(config_t->destroy != NULL);
                    #if 0
                    
                    /* 主显示未创建, 创建主显示(待定中...) */
                    if (lvgl_view_sched.view_c == NULL)
                    {
                        lv_obj_t *view = lv_obj_create(lvgl_view_sched.view_root);
                        
                        if (lvgl_view_sched.view_event_cb != NULL)
                        {
                            lv_obj_add_event_cb(view, lvgl_view_sched.view_event_cb, LV_EVENT_ALL, view);
                        }
                        
                        lv_obj_set_size(view, LV_HOR_RES, LV_VER_RES);
                        lv_obj_clear_flag(view, LV_OBJ_FLAG_SCROLLABLE);
                        lvgl_view_sched.view_c = view;
                        lv_obj_t *page = lv_obj_create(view);
                        lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
                        lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
                        create_c(page);
                        lvgl_view_event_bubble(view);
                    }
                    
                    #else
                    LV_ASSERT(lvgl_view_sched.view_c != NULL);
                    #endif
                    lvgl_view_obj_scroll_lock(lvgl_view_sched.view_c, true);
                    
                    /* 目标显示未创建, 创建目标显示 */
                    if (lvgl_view_sched.view_t == NULL)
                    {
                        lv_obj_t *view = lv_obj_create(lvgl_view_sched.view_root);
                        
                        if (lvgl_view_sched.view_event_cb != NULL)
                        {
                            lv_obj_add_event_cb(view, lvgl_view_sched.view_event_cb, LV_EVENT_ALL, view);
                        }
                        
                        lv_obj_set_pos(view, point_t.x, point_t.y);
                        lv_obj_set_size(view, LV_HOR_RES, LV_VER_RES);
                        lv_obj_clear_flag(view, LV_OBJ_FLAG_SCROLLABLE);
                        lvgl_view_sched.view_t = view;
                        
                        lv_obj_t *page = lv_obj_create(view);
                        lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
                        lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
                        config_t->create(page);
                        lvgl_view_event_bubble(view);
                    }
                    
                    /* 现在将移动锁予以标记 */
                    lvgl_view_sched.move_lock = true;
                    lvgl_view_src_make();
                    //
                    lvgl_view_sched.pct = 0;
                    lvgl_view_sched.point_c = (lv_point_t)
                    {
                        0
                    };
                    lvgl_view_sched.point_t = (lv_point_t)
                    {
                        0
                    };
                    
                    // 做一下通配处理
                    if (transform == lvgl_view_tr_none ||
                        transform == lvgl_view_tr_auto)
                    {
                        transform  = lvgl_view_sched.slide_type_cfg;
                    }
                    
                    lvgl_view_sched.slide_dir = slide_dir;
                    lvgl_view_sched.transform_type = transform;
                    LV_ASSERT(lvgl_view_sched.slide_dir & LV_DIR_ALL);
                    LV_ASSERT(lvgl_view_sched.transform_type > lvgl_view_tr_single_s);
                    LV_ASSERT(lvgl_view_sched.transform_type < lvgl_view_tr_single_e);
                    
                }
            }
        }
        
        lv_point_t point = lvgl_view_sched.touch_curr;
        lv_point_t point_c = lvgl_view_sched.point_c;
        
        // 持续滑动中
        if (lvgl_view_sched.move_lock)
        {
            // 启用一个动画, 对画布进行图形变换
            
            #if 0
            /* 落点跟随风格:动画永远带着界面跑到手指落点 */
            if (lvgl_view_sched.slide_dir == LV_DIR_RIGHT)
            {
                lvgl_view_slide_anim_auto(point_c.x, point.x, LV_HOR_RES, 0);
            }
            
            if (lvgl_view_sched.slide_dir == LV_DIR_LEFT)
            {
                lvgl_view_slide_anim_auto(point_c.x, point.x - LV_HOR_RES, LV_HOR_RES, 0);
            }
            
            if (lvgl_view_sched.slide_dir == LV_DIR_BOTTOM)
            {
                lvgl_view_slide_anim_auto(point_c.y, point.y, LV_VER_RES, 0);
            }
            
            if (lvgl_view_sched.slide_dir == LV_DIR_TOP)
            {
                lvgl_view_slide_anim_auto(point_c.y, point.y - LV_VER_RES, LV_VER_RES, 0);
            }
            
            #else
            /* 位移风格:动画只补充移位 */
            lv_point_t offset = {0};
            offset.x = point_c.x + lvgl_view_sched.touch_curr.x -
                lvgl_view_sched.touch_dist.x;
            offset.y = point_c.y + lvgl_view_sched.touch_curr.y -
                lvgl_view_sched.touch_dist.y;
            
            /* 将上一次动画中没有用完的位移取出 */
            if (lvgl_view_sched.anim != 0)
            {
                int32_t val_c = lvgl_view_sched.anim->current_value;
                int32_t val_s = lvgl_view_sched.anim->start_value;
                int32_t val_e = lvgl_view_sched.anim->end_value;
            
                // 这里偷懒, 直接从anim取未完成位移, 正常应该本地保存一个备份的
                if ((lvgl_view_sched.slide_dir & LV_DIR_HOR) != 0)
                {
                    offset.x += val_e - val_c;
                }
            
                if ((lvgl_view_sched.slide_dir & LV_DIR_VER) != 0)
                {
                    offset.y += val_e - val_c;
                }
            }
            
            
            if (lvgl_view_sched.slide_dir == LV_DIR_RIGHT)
            {
                offset.x = LV_CLAMP(0, offset.x, +LV_HOR_RES);
                lvgl_view_slide_anim_auto(point_c.x, offset.x, LV_HOR_RES, 0);
            }
            
            if (lvgl_view_sched.slide_dir == LV_DIR_LEFT)
            {
                offset.x = LV_CLAMP(-LV_HOR_RES, offset.x, 0);
                lvgl_view_slide_anim_auto(point_c.x, offset.x, LV_HOR_RES, 0);
            }
            
            if (lvgl_view_sched.slide_dir == LV_DIR_BOTTOM)
            {
                offset.y = LV_CLAMP(0, offset.y, +LV_VER_RES);
                lvgl_view_slide_anim_auto(point_c.y, offset.y, LV_VER_RES, 0);
            }
            
            if (lvgl_view_sched.slide_dir == LV_DIR_TOP)
            {
                offset.y = LV_CLAMP(-LV_VER_RES, offset.y, 0);
                lvgl_view_slide_anim_auto(point_c.y, offset.y, LV_VER_RES, 0);
            }
            
            lvgl_view_sched.touch_dist = lvgl_view_sched.touch_curr;
            #endif
        }
        
        break;
    }
    
    case LV_EVENT_RELEASED:
    {
        if (lvgl_view_sched.move_lock)
        {
            lvgl_view_sched.move_lock = false;
            
            lv_indev_t *indev = lv_indev_get_act();
            lv_indev_get_point(indev, &lvgl_view_sched.touch_curr);
            lv_point_t point = lvgl_view_sched.touch_curr;
            
            #if 1
            lv_area_t area_c = {0};
            area_c.x1 = LV_MAX(lvgl_view_sched.point_c.x, 0);
            area_c.y1 = LV_MAX(lvgl_view_sched.point_c.y, 0);
            area_c.x2 = LV_MIN(LV_HOR_RES, LV_HOR_RES + lvgl_view_sched.point_c.x) - 1;
            area_c.y2 = LV_MIN(LV_VER_RES, LV_VER_RES + lvgl_view_sched.point_c.y) - 1;
            int32_t w = lv_area_get_width(&area_c);
            int32_t h = lv_area_get_height(&area_c);
            bool inout = ((w * h) > ((int32_t)LV_HOR_RES * (int32_t)LV_VER_RES / 2));
            lvgl_view_slide_anim_inout(inout);
            
            #else
            
            if (lvgl_view_mabs(point.x, LV_HOR_RES) < LV_HOR_RES * 1 / 3 &&
                lvgl_view_mabs(point.y, LV_VER_RES) < LV_VER_RES * 1 / 3)
            {
                if (lvgl_view_sched.slide_dir == LV_DIR_BOTTOM ||
                    lvgl_view_sched.slide_dir == LV_DIR_RIGHT)
                {
                    lvgl_view_slide_anim_inout(true);
                }
            }
            else
            {
                if (lvgl_view_sched.slide_dir == LV_DIR_BOTTOM ||
                    lvgl_view_sched.slide_dir == LV_DIR_RIGHT)
                {
                    lvgl_view_slide_anim_inout(false);
                }
            }
            
            if (lvgl_view_mabs(point.x, LV_HOR_RES) < LV_HOR_RES * 2 / 3 &&
                lvgl_view_mabs(point.y, LV_VER_RES) < LV_VER_RES * 2 / 3)
            {
                if (lvgl_view_sched.slide_dir == LV_DIR_TOP ||
                    lvgl_view_sched.slide_dir == LV_DIR_LEFT)
                {
                    lvgl_view_slide_anim_inout(false);
                }
            }
            else
            {
                if (lvgl_view_sched.slide_dir == LV_DIR_TOP ||
                    lvgl_view_sched.slide_dir == LV_DIR_LEFT)
                {
                    lvgl_view_slide_anim_inout(true);
                }
            }
            
            #endif
        }
        
        break;
    }
    
    default:
        break;
    }
}

bool lvgl_view_sched_work(void)
{
    return lvgl_view_sched.anim_lock || lvgl_view_sched.move_lock
        || lvgl_view_sched.jump_lock;
}

bool lvgl_view_sched_jump(uint32_t page, uint32_t transform_type, lv_dir_t dir)
{
    // LV_LOG_WARN("");
    
    LV_ASSERT(page != 0);
    
    // if (lvgl_view_sched.touch_skip) {LV_LOG_WARN("view idle.."); return false;}
    if (lvgl_view_sched.anim_lock)
    {
        LV_LOG_WARN("view switch");
        return false;
    }
    
    if (lvgl_view_sched.move_lock)
    {
        LV_LOG_WARN("view switch");
        return false;
    }
    
    if (lvgl_view_sched.jump_lock)
    {
        LV_LOG_WARN("view switch");
        return false;
    }
    
    // 开始锁定
    lvgl_view_sched.jump_lock = true;
    
    /* 无动画界面跳转 */
    if (lvgl_view_sched.page_c == 0 || lvgl_view_sched.page_c == page ||
        transform_type == lvgl_view_tr_none)
    {
        lvgl_view_page_config(page);
        lvgl_view_sched.jump_lock = false;
        return true;
    }
    
    // 如果目标界面和当前界面有临近关系且自适应
    // 应当使用跟手切换动作
    LV_ASSERT(lvgl_view_sched.page_c != 0);
    LV_ASSERT(lvgl_view_sched.view_c != NULL);
    
    if (transform_type == lvgl_view_tr_auto)
    {
        lvgl_view_config_t *config = lvgl_view_id_config(lvgl_view_sched.page_c);
        
        if (config->page_l == page)
        {
            transform_type  = config->transform_l;
            dir = LV_DIR_RIGHT;
        }
        
        if (config->page_r == page)
        {
            transform_type  = config->transform_r;
            dir = LV_DIR_LEFT;
        }
        
        if (config->page_t == page)
        {
            transform_type  = config->transform_t;
            dir = LV_DIR_BOTTOM;
        }
        
        if (config->page_b == page)
        {
            transform_type  = config->transform_b;
            dir = LV_DIR_TOP;
        }
        
        if (transform_type == lvgl_view_tr_none)
        {
            transform_type  = lvgl_view_tr_auto;
        }
    }
    
    if (transform_type == lvgl_view_tr_auto)
    {
        transform_type  = lvgl_view_sched.jump_type_cfg;
        dir = lvgl_view_sched.jump_dir_cfg;
    }
    
    if (dir == LV_DIR_NONE)
    {
        dir = lvgl_view_sched.jump_dir_cfg;
    }
    
    lvgl_view_sched.page_t = page;
    lvgl_view_sched.slide_dir = dir;
    lvgl_view_sched.transform_type = transform_type;
    
    LV_ASSERT(lvgl_view_sched.page_t != 0);
    LV_ASSERT(lvgl_view_sched.slide_dir != LV_DIR_NONE);
    
    LV_ASSERT(lvgl_view_sched.transform_type > lvgl_view_tr_single_s);
    LV_ASSERT(lvgl_view_sched.transform_type < lvgl_view_tr_single_e);
    
    // 更新起始位置
    lv_point_t point_t = {0};
    
    if (lvgl_view_sched.slide_dir == LV_DIR_LEFT)
    {
        point_t.x += LV_HOR_RES;
    }
    
    if (lvgl_view_sched.slide_dir == LV_DIR_RIGHT)
    {
        point_t.x -= LV_HOR_RES;
    }
    
    if (lvgl_view_sched.slide_dir == LV_DIR_BOTTOM)
    {
        point_t.y -= LV_VER_RES;
    }
    
    if (lvgl_view_sched.slide_dir == LV_DIR_TOP)
    {
        point_t.y += LV_VER_RES;
    }
    
    /* 目标显示未创建, 创建目标显示 */
    lvgl_view_config_t *config_t = lvgl_view_id_config(page);
    
    if (lvgl_view_sched.view_t == NULL)
    {
        lv_obj_t *view = lv_obj_create(lvgl_view_sched.view_root);
        
        if (lvgl_view_sched.view_event_cb != NULL)
        {
            lv_obj_add_event_cb(view, lvgl_view_sched.view_event_cb, LV_EVENT_ALL, NULL);
        }
        
        lv_obj_set_pos(view, point_t.x, point_t.y);
        lv_obj_set_size(view, LV_HOR_RES, LV_VER_RES);
        lv_obj_clear_flag(view, LV_OBJ_FLAG_SCROLLABLE);
        lvgl_view_sched.view_t = view;
        
        lv_obj_t *page = lv_obj_create(view);
        lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES);
        lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
        config_t->create(page);
        lvgl_view_event_bubble(view);
    }
    
    LV_ASSERT(lvgl_view_sched.view_t != NULL);
    
    
    /* 现在将移动锁予以标记 */
    lvgl_view_sched.move_lock = false;
    lvgl_view_src_make();
    //
    lvgl_view_sched.pct = 0;
    lvgl_view_sched.point_c = (lv_point_t)
    {
        0
    };
    lvgl_view_sched.point_t = (lv_point_t)
    {
        0
    };
    // 无需多盐, 这里一定是退出当前界面
    lvgl_view_slide_anim_inout(false);
    return true;
}

void lvgl_view_sched_ready(void)
{
    LV_LOG_WARN("view manager ready\n\n");
    lvgl_view_sched.view_indev = lv_obj_create(lv_scr_act());
    lvgl_view_sched.view_root = lv_obj_create(lv_scr_act());
    lv_obj_set_size(lvgl_view_sched.view_indev, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_size(lvgl_view_sched.view_root, LV_HOR_RES, LV_VER_RES);
    lv_obj_refr_size(lvgl_view_sched.view_indev);
    lv_obj_refr_size(lvgl_view_sched.view_root);
    
    lv_obj_clear_flag(lvgl_view_sched.view_indev, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(lvgl_view_sched.view_root, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(lvgl_view_sched.view_indev, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(lvgl_view_sched.view_root, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(lvgl_view_sched.view_root, lvgl_view_slide_event_cb,
        LV_EVENT_ALL, NULL);
        
    #if LVGL_VIEW_CVS_USE
    lvgl_view_sched.view_cvs_cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
    lvgl_view_sched.view_cvs = lv_canvas_create(lvgl_view_sched.view_root);
    lv_obj_set_size(lvgl_view_sched.view_cvs, LV_HOR_RES, LV_VER_RES);
    lv_obj_refr_size(lvgl_view_sched.view_cvs);
    
    // 现在开始进行画布映射, 生成渲染资源
    // 创建三个画布, 俩个用于page映射, 一个输出变形结果
    // 备注:::这里有必要优化, buffer_n可以直达送显的buffer的,暂未继续深入研究
    size_t buffer_size = lv_snapshot_buf_size_needed(lvgl_view_sched.view_root,
            lvgl_view_sched.view_cvs_cf);
    lvgl_view_sched.buffer_c = lv_mem_alloc(buffer_size);
    lvgl_view_sched.buffer_t = lv_mem_alloc(buffer_size);
    lvgl_view_sched.buffer_n = lv_mem_alloc(buffer_size);
    lvgl_view_sched.buffer_size = buffer_size;
    
    // 画布绑定输出常驻并且隐藏
    lv_canvas_set_buffer(lvgl_view_sched.view_cvs, lvgl_view_sched.buffer_n,
        LV_HOR_RES, LV_VER_RES, lvgl_view_sched.view_cvs_cf);
    lv_obj_add_flag(lvgl_view_sched.view_cvs, LV_OBJ_FLAG_HIDDEN);
    #endif
    
    // 内存常驻表
    size_t size = sizeof(lvgl_view_config_t) * (lvgl_view_id_num + 1);
    lvgl_view_sched.list_config = lv_mem_alloc(size);
    
    // 初始同步默认配置条目
    // 默认的配置表, 运行过程中可以通过接口更新它的副本
    extern const lvgl_view_config_t lvgl_view_config_list_def[];
    lv_memcpy(lvgl_view_sched.list_config, lvgl_view_config_list_def, size);
}

void lvgl_view_sched_config_slide(uint32_t type)
{
    lvgl_view_sched.slide_type_cfg = type;
}

void lvgl_view_sched_config_jump(uint32_t type, lv_dir_t dir)
{
    lvgl_view_sched.jump_type_cfg = type;
    lvgl_view_sched.jump_dir_cfg  = dir;
}

void lvgl_view_event_cb_register(lv_event_cb_t view_event_cb)
{
    lvgl_view_sched.view_event_cb = view_event_cb;
}

void lvgl_view_id_adjust(uint32_t type, lvgl_view_config_t *config)
{
    LV_ASSERT(type > lvgl_view_id_unknown);
    LV_ASSERT(type < lvgl_view_id_num);
    
    lvgl_view_sched.list_config[type].create = config->create;
    lvgl_view_sched.list_config[type].page_l = config->page_l;
    lvgl_view_sched.list_config[type].page_r = config->page_r;
    lvgl_view_sched.list_config[type].page_t = config->page_t;
    lvgl_view_sched.list_config[type].page_b = config->page_b;
}

lvgl_view_config_t *lvgl_view_id_config(uint32_t type)
{
    LV_ASSERT(type > lvgl_view_id_unknown);
    LV_ASSERT(type < lvgl_view_id_num);
    
    // 多检查检查
    LV_ASSERT(lvgl_view_sched.list_config[type].create     != NULL);
    LV_ASSERT(lvgl_view_sched.list_config[type].destroy    != NULL);
    LV_ASSERT(lvgl_view_sched.list_config[type].focus_get  != NULL);
    LV_ASSERT(lvgl_view_sched.list_config[type].focus_lost != NULL);
    
    return &lvgl_view_sched.list_config[type];
}

