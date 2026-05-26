#include "lvgl_lib.h"

// 息屏倒计时以及息屏返回主界面倒计时
static lvgl_ctime_t lvgl_ctime = {0};

static void lvgl_ctime_event_cb(lv_event_t *e)
{
    #if LVGL_CTIME_USE == 0
    return;
    #endif
    
    // 非自定义的事件号不能处理
    if (lv_event_get_code(e) != lvgl_view_event_id())
    {
        return;
    }
    
    lvgl_view_event_param_t *param = lv_event_get_param(e);
    switch (param->type)
    {
    case lvgl_view_event_type_sleep_deep_enter: {
        if (lvgl_ctime.dlps_tag)
        {
            return;
        }
        
        LV_LOG_WARN("%s", "lvgl_view_event_type_sleep_deep_enter");
        lvgl_ctime.dlps_tag = true;
        // 关掉监控定时器自己
        lvgl_ctime_pause();
        
        #if 1
        // 实际动作在这个宏包裹
        /* CPU都给关掉, 除去RTC,GSensor和KEY其他全关了 */
        #endif
        break;
    }
    case lvgl_view_event_type_sleep_deep_exit: {
        if (!lvgl_ctime.dlps_tag)
        {
            return;
        }
        
        lvgl_ctime.dlps_tag = false;
        LV_LOG_WARN("%s", "lvgl_view_event_type_sleep_deep_exit");
        // 启用监控定时器自己
        lvgl_ctime_resume();
        
        #if 1
        // 实际动作在这个宏包裹
        // 重新打开CPU, 不再挂起
        #endif
        break;
    }
    case lvgl_view_event_type_sleep_enter: {
        if (lvgl_ctime.sleep_tag)
        {
            return;
        }
        
        lvgl_ctime.sleep_tag = true;
        LV_LOG_WARN("%s", "lvgl_view_event_type_sleep_enter");
        
        lvgl_view_stack_add_by(lvgl_view_id_standby, false,
            lvgl_view_tr_none, LV_DIR_NONE);
            
        #if 1
        // 实际动作在这个宏包裹
        #endif
        break;
    }
    case lvgl_view_event_type_sleep_exit: {
        
        /* 退出dlps和退出sleep是同步进行的 */
        lvgl_view_event_param_t param = {0};
        param.type = lvgl_view_event_type_sleep_deep_exit;
        lvgl_view_event_send(&param);
        
        if (!lvgl_ctime.sleep_tag)
        {
            return;
        }
        
        lvgl_ctime.sleep_tag = false;
        LV_LOG_WARN("%s", "lvgl_view_event_type_sleep_exit");
        
        // 复位倒吊状态
        lvgl_ctime.over = true;
        lvgl_ctime.idle = true;
        
        // 关掉息屏界面
        lvgl_view_stack_del_by(lvgl_view_id_standby,
            lvgl_view_tr_none, LV_DIR_NONE);
            
        #if 1
        // 实际动作在这个宏包裹
        #endif
        break;
    }
    }
}

/* 填0使用默认的状态值, 默认填0即可 */
void lvgl_ctime_reset(uint32_t over_time, uint32_t idle_time)
{
    lvgl_ctime.over_time_bak = over_time != 0 ? over_time :
        lvgl_ctime.over_time_def;
    lvgl_ctime.idle_time_bak = idle_time != 0 ? idle_time :
        lvgl_ctime.idle_time_def;
        
    lvgl_ctime.over = true;
    lvgl_ctime.idle = true;
    lvgl_ctime.over_time = lvgl_ctime.over_time_bak * 10;
    lvgl_ctime.idle_time = lvgl_ctime.idle_time_bak * 10;
}

// 谨慎使用:修改系统默认值(那俩个宏枚举量)
void lvgl_ctime_def_reset(uint32_t over_time, uint32_t idle_time)
{
    lvgl_ctime.over_time_def = over_time != 0 ? over_time :
        LVGL_CTIME_OVER;
    lvgl_ctime.idle_time_def = idle_time != 0 ? idle_time :
        LVGL_CTIME_IDLE;
        
    lvgl_ctime_reset(0, 0);
}

/* 填0使用默认的状态值, 默认填0即可 */
void lvgl_ctime_once_reset(uint32_t over_time, uint32_t idle_time)
{
    lvgl_ctime.once = true;
    lvgl_ctime.over_time_once = over_time != 0 ? over_time :
        lvgl_ctime.over_time_def;
    lvgl_ctime.idle_time_once = idle_time != 0 ? idle_time :
        lvgl_ctime.idle_time_def;
        
    lvgl_ctime.over = true;
    lvgl_ctime.idle = true;
    lvgl_ctime.over_time = lvgl_ctime.over_time_once * 10;
    lvgl_ctime.idle_time = lvgl_ctime.idle_time_once * 10;
}

// 一百毫秒执行一次
void lvgl_ctime_update(lv_timer_t *t)
{
    if (lvgl_ctime.over_time % 10 == 0 &&
        lvgl_ctime.idle_time % 10 == 0)
    LV_LOG_WARN("%d - %d", lvgl_ctime.over_time / 10,
        lvgl_ctime.idle_time / 10);
        
    /* 合并到lvgl流程(未活跃时间小于1秒, 这里复位到开始) */
    if (lv_disp_get_inactive_time(NULL) < 200)
    {
        if (lvgl_ctime.once)
        {
            lvgl_ctime.over_time = lvgl_ctime.over_time_once * 10;
            lvgl_ctime.idle_time = lvgl_ctime.idle_time_once * 10;
        }
        else
        {
            lvgl_ctime.over_time = lvgl_ctime.over_time_bak * 10;
            lvgl_ctime.idle_time = lvgl_ctime.idle_time_bak * 10;
        }
    }
    
    
    
    /* 约减超时等待 */
    if (lvgl_ctime.over_time != 0 &&
        lvgl_ctime.over_time != LVGL_CTIME_OVER_MAX)
    {
        lvgl_ctime.over_time--;
    }
    
    /* 主界面超时等待结束发送休眠请求 */
    if (lvgl_ctime.over_time == 0)
    {
        if (lvgl_ctime.over)
        {
            lvgl_ctime.over = false;
            LV_LOG_WARN("ui idle over enter");
            lvgl_ctime.once = false;
            
            app_module_system_dlps_set(true);
        }
        else
        {
            bool dlps_tag = false;
            
            /* 约减空闲等待 */
            if (lvgl_ctime.idle_time != 0 &&
                lvgl_ctime.idle_time != LVGL_CTIME_IDLE_MAX)
            {
                lvgl_ctime.idle_time--;
            }
            
            /* 空闲等待结束回到主界面 */
            if (lvgl_ctime.idle_time == LVGL_CTIME_IDLE_MAX)
            {
                dlps_tag = true;
            }
            
            if (lvgl_ctime.idle_time == 0)
            {
                if (lvgl_ctime.idle)
                {
                    lvgl_ctime.idle = false;
                    LV_LOG_WARN("ui idle idle home");
                    
                    lvgl_view_stack_reset_by(lvgl_view_id_home_watch, true,
                        lvgl_view_tr_none, LV_DIR_NONE);
                    
                    dlps_tag = true;
                }
            }
            
            if (dlps_tag)
            {
                // 这里甚至已经完全结束了, 可以彻底进入低功耗模式
                // 但这有要求, 唤醒时要保证重新启用定时器
                // 先待定写到此处
                lvgl_view_event_param_t param = {0};
                param.type = lvgl_view_event_type_sleep_deep_enter;
                lvgl_view_event_send(&param);
            }
        }
    }
}

void lvgl_ctime_ready(void)
{
    #if LVGL_CTIME_USE == 0
    return;
    #endif
    
    lvgl_ctime.over = true;
    lvgl_ctime.idle = true;
    lvgl_ctime.over_time = LVGL_CTIME_OVER * 10;
    lvgl_ctime.idle_time = LVGL_CTIME_IDLE * 10;
    lvgl_ctime.over_time_bak = LVGL_CTIME_OVER;
    lvgl_ctime.idle_time_bak = LVGL_CTIME_IDLE;
    lvgl_ctime.over_time_def = LVGL_CTIME_OVER;
    lvgl_ctime.idle_time_def = LVGL_CTIME_IDLE;
    lvgl_ctime.sleep_tag = false;
    
    // 事件响应回调悬挂到目标控件上去
    lv_obj_add_event_cb(lvgl_view_event_obj(), lvgl_ctime_event_cb,
        LV_EVENT_ALL, NULL);
    
    lvgl_ctime.timer = lv_timer_create(
            lvgl_ctime_update, 100, NULL);
    
    // 这种定时器, 一定是常启动的
    lv_timer_resume(lvgl_ctime.timer);
    
    lvgl_ctime_reset(0, 0);
}

void lvgl_ctime_pause(void)
{
    #if LVGL_CTIME_USE == 0
    return;
    #endif
    
    LV_LOG_INFO("%s", __func__);
    lv_timer_pause(lvgl_ctime.timer);
}

void lvgl_ctime_resume(void)
{
    #if LVGL_CTIME_USE == 0
    return;
    #endif
    
    LV_LOG_INFO("%s", __func__);
    lv_timer_resume(lvgl_ctime.timer);
}

void lvgl_ctime_sleep_quit(void)
{
    if (lvgl_ctime.sleep_tag)
    {
        app_module_system_dlps_set(false);
    }
}

bool lvgl_ctime_sleep_tag(void)
{
    return ;
}
