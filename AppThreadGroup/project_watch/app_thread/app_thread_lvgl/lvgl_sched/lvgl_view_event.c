#include "lvgl_view_sched.h"
extern lvgl_view_sched_t lvgl_view_sched;

static void lvgl_view_event_enc_cb(lv_event_t *e)
{
    uint32_t key = lv_event_get_key(e);
    uint32_t key_id = key & (~LVGL_VIEW_EVENT_ENC_MASK);
    uint32_t key_val = key & LVGL_VIEW_EVENT_ENC_MASK;
    LV_LOG_WARN("enc way:0x%x, diff:%u", key_id, key_val);
    
    lv_disp_trig_activity(NULL);
    /* 界面跳转时屏蔽编码器工作 */
    if (lvgl_view_sched_work())
    {
        LV_LOG_WARN("view switch");
        return;
    }
    
    lv_coord_t enc_way  = 0;
    lv_coord_t enc_step = key_val;
    
    /* 暂定如此, 方向反了交换一下enc_way即可 */
    if (key_id == LVGL_VIEW_EVENT_ENC_BDIR)
    {
        enc_way = +1;
    }
    
    if (key_id == LVGL_VIEW_EVENT_ENC_FDIR)
    {
        enc_way = -1;
    }
    
    if (enc_way == 0)
    {
        return;
    }
    
    // 先发送给界面
    lvgl_view_event_param_t *custom = lvgl_view_sched.event_param;
    custom->type = lvgl_view_event_type_enc;
    custom->stop = false;
    custom->enc.way = key_id;
    custom->enc.step = key_val;
    lv_obj_t *page_c = lv_obj_get_child(lvgl_view_sched.view_c, 0);
    lv_event_send(page_c, lvgl_view_event_id(), custom);
    
    if (custom->stop)
    {
        return;
    }
    
    // 再尝试滚动界面内最高可滚动容器, 由page往下找第一个即可
    // 优先进行垂直滚动匹配, 其次进行水平滚动匹配
    lv_dir_t enc_dir = LV_DIR_NONE;
    
    if (enc_way == -1)
    {
        enc_dir = LV_DIR_TOP | LV_DIR_LEFT;
    }
    
    if (enc_way == +1)
    {
        enc_dir = LV_DIR_BOTTOM | LV_DIR_RIGHT;
    }
    
    LV_ASSERT(enc_dir != LV_DIR_NONE);
    LV_LOG_INFO("enc_dir:%x", enc_dir);
    
    
    /* 判断当前界面是否可以滚动 */
    lvgl_view_config_t *config = lvgl_view_id_config(lvgl_view_sched.page_c);
    
    if (config->enc_s_way != LV_DIR_NONE)
    {
        enc_dir &= config->enc_s_way;
    }
    
    #if 1
    // 滚动条目
    lv_group_t *g = NULL;
    lv_obj_t *obj_scrollable = NULL;
    lv_coord_t obj_dist = 0;
    
    // 光电编码器有个全局的滚动调速, 一个单位移动的距离:
    lv_coord_t obj_diff_x = 3 * enc_step;
    lv_coord_t obj_diff_y = 3 * enc_step;
    
    if ((enc_dir & (LV_DIR_TOP | LV_DIR_LEFT)) != 0)
    {
        if (((enc_dir & LV_DIR_TOP) != 0) &&
            lvgl_view_obj_tree_scrollable(page_c, LV_DIR_TOP, &obj_scrollable, &obj_dist))
        {
            LV_LOG_INFO("obj_scrollable:%p", obj_scrollable);
            lv_obj_scroll_by(obj_scrollable, 0, -LV_MIN(obj_diff_y, obj_dist), true);
            return;
        }
        
        if (((enc_dir & LV_DIR_LEFT) != 0) &&
            lvgl_view_obj_tree_scrollable(page_c, LV_DIR_LEFT, &obj_scrollable, &obj_dist))
        {
            LV_LOG_INFO("obj_scrollable:%p", obj_scrollable);
            lv_obj_scroll_by(obj_scrollable, -LV_MIN(obj_diff_x, obj_dist), 0, true);
            return;
        }
    }
    
    if ((enc_dir & (LV_DIR_BOTTOM | LV_DIR_RIGHT)) != 0)
    {
        if (((enc_dir & LV_DIR_BOTTOM) != 0) &&
            lvgl_view_obj_tree_scrollable(page_c, LV_DIR_BOTTOM, &obj_scrollable, &obj_dist))
        {
            LV_LOG_INFO("obj_scrollable:%p", obj_scrollable);
            lv_obj_scroll_by(obj_scrollable, 0, +LV_MIN(obj_diff_y, obj_dist), true);
            return;
        }
        
        if (((enc_dir & LV_DIR_RIGHT) != 0) &&
            lvgl_view_obj_tree_scrollable(page_c, LV_DIR_RIGHT, &obj_scrollable, &obj_dist))
        {
            LV_LOG_INFO("obj_scrollable:%p", obj_scrollable);
            lv_obj_scroll_by(obj_scrollable, +LV_MIN(obj_diff_x, obj_dist), 0, true);
            return;
        }
    }
    
    #endif
    
    // 滚动窗口
    static uint32_t enc_event_tick_t = 0;
    static uint32_t enc_event_val_t  = 0;
    static lv_dir_t enc_event_dir_t  = 0;
    uint32_t enc_elapse = lv_tick_elaps(enc_event_tick_t);
    enc_event_tick_t = lv_tick_get();
    
    LV_LOG_INFO("enc_way:%d", enc_way);
    LV_LOG_INFO("enc_elapse:%d", enc_elapse);
    
    if (enc_elapse > LVGL_VIEW_EVENT_ENC_SPAN || enc_event_dir_t != enc_way)
    {
        enc_event_dir_t  = enc_dir;
        enc_event_val_t  = enc_step;
    }
    else
    {
        enc_event_val_t += enc_step;
    }
    
    // 连续累积多次滚动, 窗口切换
    if (enc_event_val_t >= LVGL_VIEW_EVENT_ENC_CNT)
    {
        enc_event_val_t = 0;
        // 编码器的窗口切换, 通常都是cover模式的
        // 因为编码器是层级交互, 所以在当前层级相关的界面, 都是平行界面
        
        if ((enc_dir & LV_DIR_TOP) != 0 && config->enc_b && config->page_b != 0)
        {
            LV_LOG_WARN("enc switch");
            lvgl_view_stack_cover(config->page_b);
            return;
        }
        
        if ((enc_dir & LV_DIR_BOTTOM) != 0 && config->enc_t && config->page_t != 0)
        {
            LV_LOG_WARN("enc switch");
            lvgl_view_stack_cover(config->page_t);
            return;
        }
        
        if ((enc_dir & LV_DIR_LEFT) != 0 && config->enc_r && config->page_r != 0)
        {
            LV_LOG_WARN("enc switch");
            lvgl_view_stack_cover(config->page_r);
            return;
        }
        
        if ((enc_dir & LV_DIR_RIGHT) != 0 && config->enc_l && config->page_l != 0)
        {
            LV_LOG_WARN("enc switch");
            lvgl_view_stack_cover(config->page_l);
            return;
        }
    }
    
}

static void lvgl_view_event_key_cb(lv_event_t *e)
{
    uint32_t key = lvgl_view_sched.key_id;
    lv_event_code_t code = lv_event_get_code(e);
    if (LV_EVENT_PRESSED > code || code > LV_EVENT_RELEASED)
    {
        return;
    }
    
    lv_disp_trig_activity(NULL);
    /* 界面跳转时屏蔽按键工作 */
    if (lvgl_view_sched_work())
    {
        LV_LOG_WARN("view switch");
        return;
    }
    
    // 先发送给界面
    lvgl_view_event_param_t *custom = lvgl_view_sched.event_param;
    custom->type = lvgl_view_event_type_key;
    custom->stop = false;
    custom->key.key_id = key;
    custom->key.key_event = code;
    lv_obj_t *page_c = lv_obj_get_child(lvgl_view_sched.view_c, 0);
    lv_event_send(page_c, lvgl_view_event_id(), custom);
    
    if (custom->stop)
    {
        return;
    }
    
    uint32_t stack_nest = lvgl_view_stack_nest();
    uint32_t stack_top  = lvgl_view_stack_top();
    
    switch (code)
    {
    case LV_EVENT_SHORT_CLICKED:
    {
        LV_LOG_WARN("LV_EVENT_SHORT_CLICKED");
        
        if (key == LV_KEY_ENTER)
        {
            // 浮窗跳回它们的主界面, 这里写浮窗的句柄
            // 并不一定是主表盘, 任意节目自己的浮窗都是返回它自己
            if (stack_top == lvgl_view_id_home_panel_t ||
                stack_top == lvgl_view_id_home_panel_b)
            {
                lvgl_view_stack_reset(lvgl_view_id_home_watch, false);
                return;
            }
            
            // 这里可以补充特殊需求, 如主表盘单击休眠(可选), 或者跳转到列表界面
            if (stack_top == lvgl_view_id_home_watch)
            {
                // 跳转到列表界面
                // lvgl_view_stack_add(lvgl_view_id_menu, false);
                return;
            }
            
            // 一个默认流程, 任意界面返回上一层
            if (stack_nest > 1)
            {
                lvgl_view_stack_del(stack_top);
                return;
            }
            else
            {
                // 一级界面回到主表盘
                lvgl_view_stack_reset(lvgl_view_id_home_watch, false);
            }
        }
        
        break;
    }
    case LV_EVENT_LONG_PRESSED: {
        LV_LOG_WARN("LV_EVENT_LONG_PRESSED");
        lvgl_view_sched.key_long_cnt = 0;
        
        break;
    }
    case LV_EVENT_LONG_PRESSED_REPEAT: {
        LV_LOG_WARN("LV_EVENT_LONG_PRESSED_REPEAT");
        lvgl_view_sched.key_long_cnt++;
        
        // 参上
        // 这个可以做延迟计数, 获得任意时长的长按
        if (lvgl_view_sched.key_long_cnt > 10)
        {
        }
        
        // 如任意界面长按跳转到开关机界面(弹窗)
        // 并且退出开关机界面返回到任意节目
        // lvgl_view_stack_add(/* 开关机界面句柄 */, false);
        break;
    }
    }
}

static void lvgl_view_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    switch (code)
    {
    case LV_EVENT_KEY:
    {
        uint32_t key = lv_event_get_key(e);
        LV_LOG_WARN("%d %d", code, key);
        lvgl_view_sched.key_id = key;
        
        #if 1
        
        // 编码器:不再使用或者依赖lv_indev
        // 但是会借用它的流程将事件状态流转到此处
        // 我们需要额外定义俩个特殊键值
        // 不可以与lv_key_t重复即可
        
        if ((key & LVGL_VIEW_EVENT_ENC_BDIR) == LVGL_VIEW_EVENT_ENC_BDIR)
        {
            uint32_t enc_diff = key & LVGL_VIEW_EVENT_ENC_MASK;
            LV_LOG_INFO("enc diff bdir:u", enc_diff);
            lvgl_view_event_enc_cb(e);
            return;
        }
        
        if ((key & LVGL_VIEW_EVENT_ENC_FDIR) == LVGL_VIEW_EVENT_ENC_FDIR)
        {
            uint32_t enc_diff = key & LVGL_VIEW_EVENT_ENC_MASK;
            LV_LOG_INFO("enc diff fdir:u", enc_diff);
            lvgl_view_event_enc_cb(e);
            return;
        }
        
        #endif
    }
    }
    
    // 走默认案件事件回调即可
    lvgl_view_event_key_cb(e);
}

void lvgl_view_event_ready(void)
{
    static lvgl_view_event_param_t lvgl_view_event_param = {0};
    lvgl_view_sched.event_param = &lvgl_view_event_param;
    lvgl_view_sched.event_id = lv_event_register_id();
    
    extern lv_group_t *lvgl_indev_group;
    LV_ASSERT(lvgl_indev_group != NULL);
    
    lv_group_add_obj(lvgl_indev_group, lvgl_view_sched.view_indev);
    lv_group_focus_obj(lvgl_view_sched.view_indev);
    lv_group_focus_freeze(lvgl_indev_group, true);
    
    lv_obj_add_event_cb(lvgl_view_sched.view_indev, lvgl_view_event_cb,
        LV_EVENT_ALL, NULL);
}

void lvgl_view_event_stop(void)
{
    lvgl_view_event_param_t *custom = lvgl_view_sched.event_param;
    custom->stop = true;
}

uint32_t lvgl_view_event_id(void)
{
    return lvgl_view_sched.event_id;
}
