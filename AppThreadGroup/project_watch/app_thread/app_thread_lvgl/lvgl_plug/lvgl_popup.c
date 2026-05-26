#include "lvgl_lib.h"

/* 弹窗事件界面丢弃表 */
/* 弹窗事件界面默认按优先级加入到显示中去, 但是这里可以填表选择丢弃的指定弹窗 */
static const bool lvgl_popup_discard[lvgl_view_event_type_popup_num][lvgl_view_event_type_popup_num] =
{
    // [当前显示的弹窗界面对应的全局事件][现在需要判定的弹窗界面对应的全局事件] = 是否丢弃
    
};

/* 全局弹窗事件映射表 */
static const lvgl_popup_t lvgl_popup_table[lvgl_view_event_type_popup_num] =
{
    // [事件] = {.page_id = 界面ID号, .prio = 优先级(数字越大优先级越高[0, 0xFF])},
};

/* 界面弹窗退出 */
void lvgl_popup_quit(uint32_t type)
{
    if (lvgl_view_event_type_popup_s >= type)
    {
        return;
    }
    
    if (lvgl_view_event_type_popup_e <= type)
    {
        return;
    }
    
    type = type - lvgl_view_event_type_popup_s - 1;
    uint32_t page_id = lvgl_popup_table[type].page_id;
    
    if (page_id != 0)
    {
        lvgl_view_stack_del(page_id);
    }
}

// show_page_id > page_id --- return true;
static bool lvgl_popup_page_prio_cb(uint32_t show_page_id, uint32_t page_id)
{
    LV_LOG_INFO("");
    
    /* 息屏状态下的AOD界面, 默认最高优先级 */
    /* 理论上没有任何弹窗优先级高于它, 因为弹窗显示前需要退出息屏状态 */
    if (show_page_id == lvgl_view_id_standby)
    {
        return true;
    }
    
    /* 查优先级信息 */
    uint32_t show_page_id_prio = 0;
    
    for (uint32_t idx = 0; idx < lvgl_view_arr_len(lvgl_popup_table); idx++)
    {
        if (lvgl_popup_table[idx].page_id != show_page_id)
        {
            continue;
        }
        
        show_page_id_prio = lvgl_popup_table[idx].prio;
        break;
    }
    
    /* 常规界面默认优先级为0, 任意弹窗都可覆盖它 */
    if (show_page_id_prio == 0)
    {
        return false;
    }
    
    /* 查优先级信息 */
    uint32_t page_id_prio = 0;
    
    for (uint32_t idx = 0; idx < lvgl_view_arr_len(lvgl_popup_table); idx++)
    {
        if (lvgl_popup_table[idx].page_id != page_id)
        {
            continue;
        }
        
        page_id_prio = lvgl_popup_table[idx].prio;
        break;
    }
    
    // 没有逆向映射, 查询效率有点低
    return show_page_id_prio > page_id_prio;
}

/* 界面弹窗 */
static void lvgl_popup_event_proc_cb(uint32_t type)
{
    if (type >= lvgl_view_event_type_popup_num)
    {
        return;
    }
    
    /* 检查需要弹窗的界面优先级情况 */
    uint32_t priority_cur = 0;
    uint32_t page_id_cur = lvgl_view_id_unknown;
    
    for (uint32_t idx = 0; idx < lvgl_view_arr_len(lvgl_popup_table); idx++)
    {
        if (idx != type)
        {
            continue;
        }
        
        page_id_cur  = lvgl_popup_table[idx].page_id;
        priority_cur = lvgl_popup_table[idx].prio;
        break;
    }
    
    // 断言:没有找到事件的对应界面ID号, 事件弹窗优先级大于0
    LV_ASSERT(page_id_cur != 0);
    LV_ASSERT(priority_cur > 0 && priority_cur <= 0xFF);
    
    // 倒着查, 如果遇到一个高优先级事件与当前低优先级事件匹配到, 确认是否丢弃该事件
    for (uint32_t top = lvgl_view_stack_nest(); top > 0; top--)
    {
    
        /* 查优先级信息 */
        uint32_t priority_top = 0;
        uint32_t stack_top = lvgl_view_stack_node(top);
        uint32_t event_top = 0;
        
        // 息屏特殊界面, 绕过它
        if (stack_top == lvgl_view_id_standby)
        {
            continue;
        }
        
        // 先查这个事件是否在优先级表中, 如果没有退出此流程
        for (uint32_t idx = 0; idx < lvgl_view_arr_len(lvgl_popup_table); idx++)
        {
            if (lvgl_popup_table[idx].page_id != stack_top)
            {
                continue;
            }
            
            event_top = idx;
            priority_top = lvgl_popup_table[idx].prio;
            break;
        }
        
        /* 遇到了低优先级界面, 查询结束 */
        if (priority_top <= priority_cur)
        {
            break;
        }
        
        // 低优先级弹窗, 检查是否丢弃
        if (lvgl_popup_discard[event_top][type])
        {
            return;
        }
    }
    
    if (lvgl_ctime_sleep_tag())
    {
        // 待定中:提醒事件不要动画
        lvgl_view_stack_add_prio_by(page_id_cur, false,
            lvgl_view_tr_none, LV_DIR_NONE,
            lvgl_popup_page_prio_cb);
    }
    else
    {
        // 按优先级压入栈中
        lvgl_view_stack_add_prio(page_id_cur, false,
            lvgl_popup_page_prio_cb);
    }
}

static void lvgl_popup_event_cb(lv_event_t *e)
{
    // 非自定义的事件号不能处理
    if (lv_event_get_code(e) != lvgl_view_event_id())
    {
        return;
    }
    
    lvgl_view_event_param_t *param = lv_event_get_param(e);
    
    if (lvgl_view_event_type_popup_s >= param->type)
    {
        return;
    }
    
    if (lvgl_view_event_type_popup_e <= param->type)
    {
        return;
    }
    
    // 响应事件弹窗
    uint32_t type = param->type - lvgl_view_event_type_popup_s - 1;
    lvgl_popup_event_proc_cb(type);
    
    
    
    // 默认提醒都要唤醒设备
    lvgl_ctime_sleep_quit();
    
    // 默认的外源事件都更新一次时间, 保持活跃态
    if (!lvgl_ctime_sleep_tag())
    {
        lv_disp_trig_activity(NULL);
    }
}

void lvgl_popup_ready(void)
{
    // 事件响应回调悬挂到目标控件上去
    lv_obj_add_event_cb(lvgl_view_event_obj(), lvgl_popup_event_cb,
        LV_EVENT_ALL, NULL);
}

void lvgl_popup_notify(uint32_t type)
{
    if (lvgl_view_event_type_popup_s >= type)
    {
        return;
    }
    
    if (lvgl_view_event_type_popup_e <= type)
    {
        return;
    }
    
    lvgl_view_event_param_t param = {0};
    param.type = type;
    lvgl_view_event_send(&param);
}

