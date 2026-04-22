/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_object_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_object_t *object = widget;
    scui_object_maker_t *object_maker = widget_maker;
    
    /* 必须标记anima,ptr事件 */
    widget_maker->style.sched_anima = true;
    widget_maker->style.indev_ptr   = true;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_object));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    object->prop_num = scui_max(object_maker->prop_num, SCUI_WIDGET_OBJECT_PROP_MIN);
    object->tran_num = scui_max(object_maker->tran_num, SCUI_WIDGET_OBJECT_TRAN_MIN);
    scui_multi_t prop_size = sizeof(scui_object_prop_t) * object->prop_num;
    scui_multi_t tran_size = sizeof(scui_object_tran_t) * object->tran_num;
    object->prop_list = SCUI_MEM_ZALLOC(scui_mem_type_mix, prop_size);
    object->tran_list = SCUI_MEM_ZALLOC(scui_mem_type_mix, tran_size);
    object->state_c = scui_object_state_def;
    object->state_l = scui_object_state_def;
    object->check = object_maker->check;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_object_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    SCUI_MEM_FREE(object->prop_list);
    SCUI_MEM_FREE(object->tran_list);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 对象控件属性检查
 *@param prop 控件属性
 *@retval 有效属性
 */
static bool scui_object_prop_check(scui_object_prop_t *prop)
{
    if (prop->part  > scui_object_part_s  && prop->part  < scui_object_part_e  &&
        prop->state > scui_object_state_s && prop->state < scui_object_state_e &&
        prop->style > scui_object_style_s && prop->style < scui_object_style_e)
        return true;
    
    return false;
}

/*@brief 对象控件移除属性
 *@param handle 控件句柄
 *@param prop   控件属性
 */
void scui_object_prop_del(scui_handle_t handle, scui_object_prop_t *prop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    for (scui_coord_t idx = 0; idx < object->prop_num; idx++) {
        scui_object_prop_t *local_prop = &object->prop_list[idx];
        if (!local_prop->use) continue;
        
        if (local_prop->part  != prop->part  ||
            local_prop->state != prop->state ||
            local_prop->style != prop->style)
            continue;
        
        local_prop->use = false;
        object->prop_now--;
        scui_widget_draw(handle, NULL, false);
        return;
    }
}

/*@brief 对象控件添加属性
 *@param handle 控件句柄
 *@param prop   控件属性
 */
void scui_object_prop_add(scui_handle_t handle, scui_object_prop_t *prop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    /* 先移除旧的再加新的: */
    SCUI_ASSERT(scui_object_prop_check(prop));
    scui_object_prop_del(handle, prop);
    
    for (scui_coord_t idx = 0; idx < object->prop_num; idx++) {
        scui_object_prop_t *local_prop = &object->prop_list[idx];
        if (local_prop->use) continue;
        
       *local_prop = *prop;
        local_prop->use = true;
        object->prop_now++;
        scui_widget_draw(handle, NULL, false);
        return;
    }
    
    SCUI_LOG_ERROR("prop num too small");
    SCUI_ASSERT(false);
}

/*@brief 对象控件同步属性
 *@param handle 控件句柄
 *@param prop   控件属性
 *@retval 成功失败
 */
bool scui_object_prop_sync(scui_handle_t handle, scui_object_prop_t *prop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    for (scui_coord_t idx = 0; idx < object->prop_num; idx++) {
        scui_object_prop_t *local_prop = &object->prop_list[idx];
        if (!local_prop->use) continue;
        
        /* 只检查当前状态 */
        if ((prop->state == 0 && local_prop->state != object->state_c) ||
            (prop->state != 0 && local_prop->state != prop->state))
             continue;
        
        if (local_prop->part  != prop->part ||
            local_prop->style != prop->style)
            continue;
        
       *prop = *local_prop;
        return true;
    }
    
    return false;
}

/*@brief 对象控件过渡检查
 *@param prop 控件过渡
 *@retval 有效过渡
 */
static bool scui_object_tran_check(scui_object_tran_t *tran)
{
    if (tran->part    > scui_object_part_s  && tran->part    < scui_object_part_e  &&
        tran->state_p > scui_object_state_s && tran->state_p < scui_object_state_e &&
        tran->state_n > scui_object_state_s && tran->state_n < scui_object_state_e &&
        tran->style   > scui_object_style_s && tran->style   < scui_object_style_e)
        return true;
    
    return false;
}

/*@brief 对象控件移除过渡
 *@param handle 控件句柄
 *@param tran   控件过渡
 */
void scui_object_tran_del(scui_handle_t handle, scui_object_tran_t *tran)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    for (scui_coord_t idx = 0; idx < object->tran_num; idx++) {
        scui_object_tran_t *local_tran = &object->tran_list[idx];
        if (!local_tran->use) continue;
        
        if (local_tran->part    != tran->part    ||
            local_tran->state_p != tran->state_p ||
            local_tran->state_n != tran->state_n ||
            local_tran->style   != tran->style)
            continue;
        
        local_tran->use = false;
        object->tran_now--;
        return;
    }
}

/*@brief 对象控件添加过渡
 *@param handle 控件句柄
 *@param tran   控件过渡
 */
void scui_object_tran_add(scui_handle_t handle, scui_object_tran_t *tran)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    /* 先移除旧的再加新的: */
    SCUI_ASSERT(scui_object_tran_check(tran));
    scui_object_tran_del(handle, tran);
    
    for (scui_coord_t idx = 0; idx < object->tran_num; idx++) {
        scui_object_tran_t *local_tran = &object->tran_list[idx];
        if (local_tran->use) continue;
        
       *local_tran = *tran;
        local_tran->use = true;
        object->tran_now++;
        
        scui_map_cb_t path = local_tran->path;
        if (path == NULL) path = scui_map_ease_in_out;
        local_tran->path   = path;
        local_tran->tick_d = local_tran->delay + 1;
        local_tran->tick_t = local_tran->time + 1;
        local_tran->pct_c  = 0;
        return;
    }
    
    SCUI_LOG_ERROR("tran num too small");
    SCUI_ASSERT(false);
}

/*@brief 对象控件添加过渡(取属性)
 *@param handle 控件句柄
 *@param tran   控件过渡
 *@retval 缺属性
 */
bool scui_object_tran_add_by(scui_handle_t handle, scui_object_tran_t *tran)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    scui_object_tran_t local_tran = *tran;
    scui_object_prop_t prop_p = {.state = tran->state_p,};
    scui_object_prop_t prop_n = {.state = tran->state_n,};
    prop_p.part = tran->part; prop_p.style = tran->style;
    prop_n.part = tran->part; prop_n.style = tran->style;
    
    if (!scui_object_prop_sync(handle, &prop_p)) return false;
    if (!scui_object_prop_sync(handle, &prop_n)) return false;
    local_tran.data_p = prop_p.data;
    local_tran.data_n = prop_n.data;
    
    scui_object_tran_add(handle, &local_tran);
    return true;
}

/*@brief 对象控件状态获取
 *@param handle 控件句柄
 *@param state  控件状态
 */
void scui_object_state_get(scui_handle_t handle, scui_object_type_t *state)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    *state = object->state_c;
}

/*@brief 对象控件状态设置
 *@param handle 控件句柄
 *@param state  控件状态
 */
void scui_object_state_set(scui_handle_t handle, scui_object_type_t state)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    /* 重复的无效状态切换 */
    if (object->state_c == state)
        return;
    
    /* 关闭所有的过渡,仅启用状态到达过渡 */
    for (scui_coord_t idx = 0; idx < object->tran_num; idx++) {
        scui_object_tran_t *tran = &object->tran_list[idx];
        if (!tran->use) continue;
        
        if (tran->state_p == object->state_c &&
            tran->state_n == state) {
            
            tran->tick_d = 0;
            tran->tick_t = 0;
            continue;
        }
        
        tran->tick_d = tran->delay + 1;
        tran->tick_t = tran->time + 1;
    }
    
    /* 状态更新 */
    object->state_l = object->state_c;
    object->state_c = state;
}

/*@brief 对象控件应用过渡
 *@param handle   控件句柄
 *@param tran_idx 控件过渡索引
 */
static void scui_object_tran_sync(scui_handle_t handle, scui_coord_t tran_idx)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    SCUI_ASSERT(tran_idx < object->tran_num);
    scui_object_tran_t *local_tran = &object->tran_list[tran_idx];
    /* 使用当前的pct_c计算data并且更新到prop_list */
    scui_object_prop_t prop = {0};
    prop.part  = local_tran->part;
    prop.state = local_tran->state_n;
    prop.style = local_tran->style;
    scui_multi_t pct_c = local_tran->pct_c;
    
    SCUI_LOG_INFO("object prop tran:");
    SCUI_LOG_INFO("pct:%3d%%, part:0x%x, state:0x%x, style:0x%x",
        pct_c, prop.part, prop.state, prop.style);
    
    switch (prop.style) {
    case scui_object_style_color_bg:
    case scui_object_style_color_fg: {
        scui_color32_t color32_p = local_tran->data_p.color32;
        scui_color32_t color32_n = local_tran->data_n.color32;
        scui_color32_mix_with(&prop.data.color32, &color32_n, &color32_p, pct_c);
        SCUI_LOG_INFO("color:0x%x", prop.data.color32.full);
        break;
    }
    case scui_object_style_alpha: {
        scui_alpha_t alpha_p = local_tran->data_p.alpha;
        scui_alpha_t alpha_n = local_tran->data_n.alpha;
        prop.data.alpha = scui_map(pct_c, 0, 100, alpha_p, alpha_n);
        SCUI_LOG_INFO("alpha:%d", prop.data.alpha);
        break;
    }
    default:
    case scui_object_style_width:
    case scui_object_style_height:
    case scui_object_style_radius: {
        scui_multi_t number_p = local_tran->data_p.number;
        scui_multi_t number_n = local_tran->data_n.number;
        prop.data.number = scui_map(pct_c, 0, 100, number_p, number_n);
        SCUI_LOG_INFO("number:%d", prop.data.number);
        break;
    }
    }
    
    /* 同步更新prop(直接add即可) */
    scui_object_prop_add(handle, &prop);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_object_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_object_t *object = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        scui_object_tran_t tran_zero = {0};
        /* 遍历所有的样式切换节点,检查所有有效目标 */
        for (scui_coord_t idx = 0; idx < object->tran_num; idx++) {
            scui_object_tran_t *local_tran = &object->tran_list[idx];
            if (!local_tran->use) continue;
            
            /* 跳过旧样式切换节点(打断) */
            if (local_tran->state_n != object->state_c) {
                continue;
            }
            
            /* 延时嘀嗒(不精确,有残差) */
            if (local_tran->tick_d <= local_tran->delay) {
                local_tran->tick_d += event->tick;
                continue;
            }
            /* 进度嘀嗒(不精确,有残差) */
            if (local_tran->tick_t <= local_tran->time) {
                local_tran->tick_t += event->tick;
                /* 使用新值更新当前样式 */
                local_tran->pct_c = local_tran->path(local_tran->tick_t,
                    0, local_tran->time, 0, 100);
                
                scui_object_tran_sync(widget->myself, idx);
                continue;
            }
        }
        break;
    }
    case scui_event_ptr_down:{
        if (!scui_widget_event_inside(event))
             break;
        
        scui_object_state_set(event->object, scui_object_state_pre);
        break;
    }
    case scui_event_ptr_up: {
        if (object->state_c != scui_object_state_pre)
            break;
        
        if (object->check) {
            if (object->state_l == scui_object_state_def) {
                scui_object_state_set(event->object, scui_object_state_chk);
                break;
            }
            if (object->state_l == scui_object_state_chk) {
                scui_object_state_set(event->object, scui_object_state_def);
                break;
            }
        }
        
        scui_object_state_set(event->object, scui_object_state_def);
        break;
    }
    default:
        break;
    }
}
