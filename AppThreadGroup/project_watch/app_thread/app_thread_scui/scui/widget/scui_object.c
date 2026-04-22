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
    
    /* 必须标记anima事件 */
    widget_maker->style.sched_anima = true;
    
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
    object->state = scui_object_state_def;
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

/*@brief 对象控件状态更新
 *@param handle 控件句柄
 *@param state  控件状态
 */
void scui_object_state_new(scui_handle_t handle, scui_object_type_t state)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    /* 重复的无效状态切换 */
    if (object->state == state)
        return;
    
    /* 关闭所有的过渡,仅启用状态到达过渡 */
    for (scui_coord_t idx = 0; idx < object->tran_num; idx++) {
        scui_object_tran_t *tran = &object->tran_list[idx];
        if (!tran->use) continue;
        
        if (tran->state_p == object->state &&
            tran->state_n == state) {
            
            tran->tick_d = 0;
            tran->tick_t = 0;
            continue;
        }
        
        tran->tick_d = tran->delay + 1;
        tran->tick_t = tran->time + 1;
    }
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

/*@brief 对象控件添加属性
 *@param handle 控件句柄
 *@param prop   控件属性
 */
void scui_object_prop_add(scui_handle_t handle, scui_object_prop_t *prop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    /* 进行一次查重检查: */
    SCUI_ASSERT(scui_object_prop_check(prop));
    for (scui_coord_t idx = 0; idx < object->prop_num; idx++) {
        scui_object_prop_t *local_prop = &object->prop_list[idx];
        if (local_prop->use) continue;
        
        if (local_prop->part  != prop->part  ||
            local_prop->state != prop->state ||
            local_prop->style != prop->style)
            continue;
        
       *local_prop = *prop;
        scui_widget_draw(handle, NULL, false);
        return;
    }
    
    for (scui_coord_t idx = 0; idx < object->prop_num; idx++) {
        scui_object_prop_t *local_prop = &object->prop_list[idx];
        if (!local_prop->use) continue;
        
       *local_prop = *prop;
        local_prop->use = true;
        scui_widget_draw(handle, NULL, false);
        return;
    }
    
    SCUI_LOG_ERROR("prop num too small");
    SCUI_ASSERT(false);
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
        scui_widget_draw(handle, NULL, false);
        return;
    }
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
        
        if (local_prop->part  != prop->part  ||
            local_prop->state != prop->state ||
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

/*@brief 对象控件添加过渡(同步添加属性)
 *@param handle 控件句柄
 *@param tran   控件过渡
 */
void scui_object_tran_add(scui_handle_t handle, scui_object_tran_t *tran)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    scui_object_prop_t prop_p = {0}, prop_n = {0};
    prop_p.part = tran->part; prop_p.style = tran->style;
    prop_n.part = tran->part; prop_n.style = tran->style;
    prop_p.state = tran->state_p;
    prop_n.state = tran->state_n;
    /* 将tran的prop添加进prop_list */
    scui_object_prop_add(handle, &prop_p);
    scui_object_prop_add(handle, &prop_n);
    
    /* 进行一次查重检查: */
    SCUI_ASSERT(scui_object_tran_check(tran));
    for (scui_coord_t idx = 0; idx < object->tran_num; idx++) {
        scui_object_tran_t *local_tran = &object->tran_list[idx];
        if (local_tran->use) continue;
        
        if (local_tran->part    != tran->part    ||
            local_tran->state_p != tran->state_p ||
            local_tran->state_n != tran->state_n ||
            local_tran->style   != tran->style)
            continue;
        
       *local_tran = *tran;
        scui_map_cb_t path = local_tran->path;
        if (path == NULL) path = scui_map_ease_in_out;
        local_tran->path = path;
        return;
    }
    
    for (scui_coord_t idx = 0; idx < object->tran_num; idx++) {
        scui_object_tran_t *local_tran = &object->tran_list[idx];
        if (!local_tran->use) continue;
        
       *local_tran = *tran;
        local_tran->use = true;
        scui_map_cb_t path = local_tran->path;
        if (path == NULL) path = scui_map_ease_in_out;
        local_tran->path = path;
        return;
    }
    
    SCUI_LOG_ERROR("tran num too small");
    SCUI_ASSERT(false);
}

/*@brief 对象控件移除过渡(同步移除属性)
 *@param handle 控件句柄
 *@param tran   控件过渡
 */
void scui_object_tran_del(scui_handle_t handle, scui_object_tran_t *tran)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    SCUI_ASSERT(tran != NULL);
    SCUI_ASSERT(tran != NULL);
    
    scui_object_prop_t prop_p = {0}, prop_n = {0};
    prop_p.part = tran->part; prop_p.style = tran->style;
    prop_n.part = tran->part; prop_n.style = tran->style;
    prop_p.state = tran->state_p;
    prop_n.state = tran->state_n;
    /* 将tran的prop移除出prop_list */
    scui_object_prop_del(handle, &prop_p);
    scui_object_prop_del(handle, &prop_n);
    
    for (scui_coord_t idx = 0; idx < object->tran_num; idx++) {
        scui_object_tran_t *local_tran = &object->tran_list[idx];
        if (!local_tran->use) continue;
        
        if (local_tran->part    != tran->part    ||
            local_tran->state_p != tran->state_p ||
            local_tran->state_n != tran->state_n ||
            local_tran->style   != tran->style)
            continue;
        
        local_tran->use = false;
        return;
    }
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
    
    switch (prop.style) {
    case scui_object_style_color: {
        scui_color_t color_p = local_tran->data_p.color;
        scui_color_t color_n = local_tran->data_n.color;
        scui_color32_mix_with(&prop.data.color.color_s, &color_p.color_s, &color_n.color_s, pct_c);
        scui_color32_mix_with(&prop.data.color.color_e, &color_p.color_e, &color_n.color_e, pct_c);
        prop.data.color.color_f = color_n.color_f;
        prop.data.color.filter  = color_n.filter;
        break;
    }
    case scui_object_style_alpha: {
        scui_alpha_t alpha_p = local_tran->data_p.alpha;
        scui_alpha_t alpha_n = local_tran->data_n.alpha;
        prop.data.alpha = scui_map(pct_c, 0, 100, alpha_p, alpha_n);
        break;
    }
    default:
    case scui_object_style_width:
    case scui_object_style_height:
    case scui_object_style_radius: {
        scui_multi_t number_p = local_tran->data_p.number;
        scui_multi_t number_n = local_tran->data_n.number;
        prop.data.number = scui_map(pct_c, 0, 100, number_p, number_n);
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
            if (local_tran->state_n != object->state) {
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
    default:
        break;
    }
}
