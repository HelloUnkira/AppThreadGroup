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
    
    object->state_c = scui_object_state_def;
    object->state_l = scui_object_state_def;
    object->check   = object_maker->check;
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_object_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    /* 回收可能存在的资源 */
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
        object->prop_cur--;
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
    
    #if 0
    /* 唯一实例:以旧换新: */
    SCUI_ASSERT(scui_object_prop_check(prop));
    scui_object_prop_del(handle, prop);
    #else
    /* 唯一实例:原地更新 */
    for (scui_coord_t idx = 0; idx < object->prop_num; idx++) {
        scui_object_prop_t *local_prop = &object->prop_list[idx];
        if (!local_prop->use) continue;
        
        if (local_prop->part  != prop->part  ||
            local_prop->state != prop->state ||
            local_prop->style != prop->style)
            continue;
        
       *local_prop = *prop;
        local_prop->use = true;
        scui_widget_draw(handle, NULL, false);
        return;
    }
    #endif
    
    /* 动态调整资源大小(只需扩充即可) */
    if (object->prop_cur + 1 > object->prop_num) {
        scui_coord_t prop_num  = object->prop_num + SCUI_WIDGET_OBJECT_PROP_STEP;
        scui_multi_t prop_size = prop_num * sizeof(scui_object_prop_t);
        scui_object_prop_t *prop_list = SCUI_MEM_ZALLOC(scui_mem_type_mix, prop_size);
        prop_size = object->prop_num * sizeof(scui_object_prop_t);
        memcpy(prop_list, object->prop_list, prop_size);
        SCUI_MEM_FREE(object->prop_list);
        object->prop_list = prop_list;
        object->prop_num  = prop_num;
    }
    
    SCUI_ASSERT(object->prop_list != NULL);
    for (scui_coord_t idx = 0; idx < object->prop_num; idx++) {
        scui_object_prop_t *local_prop = &object->prop_list[idx];
        if (local_prop->use) continue;
        
       *local_prop = *prop;
        local_prop->use = true;
        object->prop_cur++;
        scui_widget_draw(handle, NULL, false);
        return;
    }
    
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
        object->tran_cur--;
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
    
    #if 0
    /* 唯一实例:以旧换新: */
    SCUI_ASSERT(scui_object_tran_check(tran));
    scui_object_tran_del(handle, tran);
    #else
    /* 唯一实例:原地更新 */
    for (scui_coord_t idx = 0; idx < object->tran_num; idx++) {
        scui_object_tran_t *local_tran = &object->tran_list[idx];
        if (!local_tran->use) continue;
        
        if (local_tran->part    != tran->part    ||
            local_tran->state_p != tran->state_p ||
            local_tran->state_n != tran->state_n ||
            local_tran->style   != tran->style)
            continue;
        
       *local_tran = *tran;
        
        scui_map_cb_t path = local_tran->path;
        if (path == NULL) path = scui_map_ease_in_out;
        local_tran->path   = path;
        local_tran->tick_d = local_tran->delay + 1;
        local_tran->tick_t = local_tran->time + 1;
        local_tran->pct_c  = 0;
        return;
    }
    #endif
    
    /* 动态调整资源大小(只需扩充即可) */
    if (object->tran_cur + 1 > object->tran_num) {
        scui_coord_t tran_num  = object->tran_num + SCUI_WIDGET_OBJECT_PROP_STEP;
        scui_multi_t tran_size = tran_num * sizeof(scui_object_tran_t);
        scui_object_tran_t *tran_list = SCUI_MEM_ZALLOC(scui_mem_type_mix, tran_size);
        tran_size = object->tran_num * sizeof(scui_object_tran_t);
        memcpy(tran_list, object->tran_list, tran_size);
        SCUI_MEM_FREE(object->tran_list);
        object->tran_list = tran_list;
        object->tran_num  = tran_num;
    }
    
    SCUI_ASSERT(object->tran_list != NULL);
    for (scui_coord_t idx = 0; idx < object->tran_num; idx++) {
        scui_object_tran_t *local_tran = &object->tran_list[idx];
        if (local_tran->use) continue;
        
       *local_tran = *tran;
        local_tran->use = true;
        object->tran_cur++;
        
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
    case scui_object_style_crect_color:
    case scui_object_style_crect_color_grad:
    case scui_object_style_color_bg:
    case scui_object_style_color_fg: {
        scui_color32_t color32_p = local_tran->data_p.color32;
        scui_color32_t color32_n = local_tran->data_n.color32;
        scui_color32_mix_with(&prop.data.color32, &color32_n, &color32_p, pct_c);
        SCUI_LOG_INFO("color:0x%x", prop.data.color32.full);
        break;
    }
    case scui_object_style_crect_alpha:
    case scui_object_style_alpha: {
        scui_alpha_t alpha_p = local_tran->data_p.alpha;
        scui_alpha_t alpha_n = local_tran->data_n.alpha;
        prop.data.alpha = scui_map(pct_c, 0, 100, alpha_p, alpha_n);
        SCUI_LOG_INFO("alpha:%d", prop.data.alpha);
        break;
    }
    case scui_object_style_crect_width:
    case scui_object_style_crect_radius:
    case scui_object_style_width:
    case scui_object_style_height:
    case scui_object_style_radius: {
        scui_multi_t number_p = local_tran->data_p.number;
        scui_multi_t number_n = local_tran->data_n.number;
        prop.data.number = scui_map(pct_c, 0, 100, number_p, number_n);
        SCUI_LOG_INFO("number:%d", prop.data.number);
        break;
    }
    default:
        break;
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

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* 自定义的经典效果: */

#if SCUI_DRAW_USE_THORVG
/* 矢量绘图引擎(thorvg): */
#include "thorvg_capi.h"

static void scui_object_tvg_crect_cb(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->graph.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->graph.dst_clip;
    scui_area_t    *src_area    = &draw_dsc->graph.src_area;
    scui_alpha_t    src_alpha   =  draw_dsc->graph.src_alpha;
    scui_color_t    src_color   =  draw_dsc->graph.src_color;
    scui_coord_t    src_width   =  draw_dsc->graph.src_width;
    scui_coord_t    src_radius  =  draw_dsc->graph.src_radius;
    scui_coord_t    src_shadow  =  draw_dsc->graph.src_shadow;
    scui_sbitfd_t   src_grad_w  =  draw_dsc->graph.src_grad_w;
    scui_sbitfd_t   src_grad    =  draw_dsc->graph.src_grad;
    /* draw dsc args<e> */
    
    scui_coord_t src_radius_max = scui_min(src_area->w, src_area->h) / 2;
    if (src_radius > src_radius_max || src_radius < 0)
        src_radius = src_radius_max;
    
    Tvg_Canvas  *canvas = draw_dsc->graph.src_tvg_canvas;
    scui_point_t offset = draw_dsc->graph.src_tvg_offset;
    
    /* 绘制要求:仅绘制在src_area中 */
    /* 所以描边部分需要动态调整一下 */
    
    scui_color32_t c_s = src_color.color_s;
    scui_color32_t c_e = src_color.color_e;
    Tvg_Color_Stop color2[] = {
        {0.0, .r = c_s.ch.r, .g = c_s.ch.g, .b = c_s.ch.b, .a = c_s.ch.a,},
        {1.0, .r = c_e.ch.r, .g = c_e.ch.g, .b = c_e.ch.b, .a = c_e.ch.a,},
    };
    
    scui_coord_t sw = src_width;
    scui_coord_t x = src_area->x - offset.x;
    scui_coord_t y = src_area->y - offset.y;
    scui_coord_t w = src_area->w;
    scui_coord_t h = src_area->h;
    scui_coord_t r = src_radius;
    
    if (src_shadow) {
        if (src_width == 0 || src_width >= src_radius) {
            SCUI_LOG_ERROR("unsupport fill grad");
            return;
        }
        
        struct {
            scui_point_t move_to;
            scui_point_t line_to[4];
            scui_point_t grad_to[2];
        } line_4[4] = {0};
        
        line_4[0].move_to    = (scui_point_t){x + r, y};
        line_4[0].line_to[0] = (scui_point_t){x + w - r, y};
        line_4[0].line_to[1] = (scui_point_t){x + w - r, y + sw};
        line_4[0].line_to[2] = (scui_point_t){x + r, y + sw};
        line_4[0].line_to[3] = (scui_point_t){x + r, y};
        line_4[0].grad_to[0] = line_4[0].line_to[2];
        line_4[0].grad_to[1] = line_4[0].line_to[3];
        
        line_4[1].move_to    = (scui_point_t){x + r, y + h - sw};
        line_4[1].line_to[0] = (scui_point_t){x + w - r, y + h - sw};
        line_4[1].line_to[1] = (scui_point_t){x + w - r, y + h};
        line_4[1].line_to[2] = (scui_point_t){x + r, y + h};
        line_4[1].line_to[3] = (scui_point_t){x + r, y + h - sw};
        line_4[1].grad_to[0] = line_4[0].line_to[3];
        line_4[1].grad_to[1] = line_4[0].line_to[2];
        
        line_4[2].move_to    = (scui_point_t){x, y + r};
        line_4[2].line_to[0] = (scui_point_t){x + sw, y + r};
        line_4[2].line_to[1] = (scui_point_t){x + sw, y + h - r};
        line_4[2].line_to[2] = (scui_point_t){x, y + h - r};
        line_4[2].line_to[3] = (scui_point_t){x, y + r};
        line_4[2].grad_to[0] = line_4[0].line_to[0];
        line_4[2].grad_to[1] = line_4[0].line_to[3];
        
        line_4[3].move_to    = (scui_point_t){x + w, y + r};
        line_4[3].line_to[0] = (scui_point_t){x + w - sw, y + r};
        line_4[3].line_to[1] = (scui_point_t){x + w - sw, y + h - r};
        line_4[3].line_to[2] = (scui_point_t){x + w, y + h - r};
        line_4[3].line_to[3] = (scui_point_t){x + w, y + r};
        line_4[3].grad_to[0] = line_4[0].line_to[0];
        line_4[3].grad_to[1] = line_4[0].line_to[3];
        
        for (scui_coord_t idx = 0; idx < 4; idx++) {
            scui_coord_t x = line_4[idx].move_to.x;
            scui_coord_t y = line_4[idx].move_to.y;
            
            Tvg_Paint *paint = tvg_shape_new();
            tvg_paint_set_opacity(paint, src_alpha);
            tvg_shape_move_to(paint, x, y);
            for (scui_coord_t idx_i = 0; idx_i < 4; idx_i++) {
                x = line_4[idx].line_to[idx_i].x;
                y = line_4[idx].line_to[idx_i].y;
                tvg_shape_line_to(paint,x , y);
            }
            
            scui_coord_t x1 = line_4[3].grad_to[0].x;
            scui_coord_t y1 = line_4[3].grad_to[0].y;
            scui_coord_t x2 = line_4[3].grad_to[1].x;
            scui_coord_t y2 = line_4[3].grad_to[1].y;
            Tvg_Gradient* grad = tvg_linear_gradient_new();
            tvg_linear_gradient_set(grad, x1, y1, x2, y2);
            tvg_gradient_set_color_stops(grad, color2, 2);
            tvg_shape_set_linear_gradient(paint, grad);
            tvg_canvas_push(canvas, paint);
        }
        
        struct {
            scui_point_t center;
            scui_point_t angle;
        } arc_4[4] = {0};
        
        arc_4[0].center = (scui_point_t){x + w - r, y + r};
        arc_4[0].angle  = (scui_point_t){270, 90};
        arc_4[1].center = (scui_point_t){x + w - r, y + h - r};
        arc_4[1].angle  = (scui_point_t){0, 90};
        arc_4[2].center = (scui_point_t){x + r, y + h - r};
        arc_4[2].angle  = (scui_point_t){90, 90};
        arc_4[3].center = (scui_point_t){x + r, y + r};
        arc_4[3].angle  = (scui_point_t){180, 90};
        
        
        color2[0].offset = (r - sw) * 1.0f / r;
        for (scui_coord_t idx = 0; idx < 4; idx++) {
            scui_coord_t c_x = arc_4[idx].center.x;
            scui_coord_t c_y = arc_4[idx].center.y;
            scui_coord_t a_s = arc_4[idx].angle.x;
            scui_coord_t a_l = arc_4[idx].angle.y;
            
            Tvg_Paint *paint = tvg_shape_new();
            tvg_paint_set_opacity(paint, src_alpha);
            tvg_shape_append_arc(paint, c_x, c_y, r, a_s, a_l, 1);
            Tvg_Gradient* grad = tvg_radial_gradient_new();
            tvg_radial_gradient_set(grad, c_x, c_y, r);
            tvg_gradient_set_color_stops(grad, color2, 2);
            tvg_shape_set_radial_gradient(paint, grad);
            tvg_canvas_push(canvas, paint);
        }
        
    } else {
        
        /* 固定:取用画笔并配置信息 */
        Tvg_Paint *paint = tvg_shape_new();
        tvg_paint_set_opacity(paint, src_alpha);
        
        uint8_t x_a = src_color.color.ch.a;
        uint8_t x_r = src_color.color.ch.r;
        uint8_t x_g = src_color.color.ch.g;
        uint8_t x_b = src_color.color.ch.b;
        
        if (src_width == 0 || src_width >= src_radius) {
            
            tvg_shape_move_to(paint, x + r, y);
            tvg_shape_line_to(paint, x + w - r, y);
            tvg_shape_line_to(paint, x + w - r, y + r);
            tvg_shape_line_to(paint, x + r, y + r);
            tvg_shape_line_to(paint, x + r, y);
            
            tvg_shape_move_to(paint, x + r, y + h - r);
            tvg_shape_line_to(paint, x + w - r, y + h - r);
            tvg_shape_line_to(paint, x + w - r, y + h);
            tvg_shape_line_to(paint, x + r, y + h);
            tvg_shape_line_to(paint, x + r, y + h - r);
            
            tvg_shape_move_to(paint, x, y + r);
            tvg_shape_line_to(paint, x + w, y + r);
            tvg_shape_line_to(paint, x + w, y + h - r);
            tvg_shape_line_to(paint, x, y + h - r);
            tvg_shape_line_to(paint, x, y + r);
            
            tvg_shape_append_arc(paint, x + w - r, y + r, r, 270, 90, 1);
            tvg_shape_append_arc(paint, x + w - r, y + h - r, r, 0, 90, 1);
            tvg_shape_append_arc(paint, x + r, y + h - r, r, 90, 90, 1);
            tvg_shape_append_arc(paint, x + r, y + r, r, 180, 90, 1);
            
            tvg_shape_set_fill_color(paint, x_r, x_g, x_b, x_a);
        } else {
            /* 调整一下边界 */
            r -= sw / 2;
            x += sw / 2;
            y += sw / 2;
            w -= sw;
            h -= sw;
            
            tvg_shape_move_to(paint, x + r, y);
            tvg_shape_line_to(paint, x + w - r, y);
            tvg_shape_append_arc(paint, x + w - r, y + r, r, 270, 90, 0);
            tvg_shape_line_to(paint, x + w, y + h - r);
            tvg_shape_append_arc(paint, x + w - r, y + h - r, r, 0, 90, 0);
            tvg_shape_line_to(paint, x + r, y + h);
            tvg_shape_append_arc(paint, x + r, y + h - r, r, 90, 90, 0);
            tvg_shape_line_to(paint, x, y + r);
            tvg_shape_append_arc(paint, x + r, y + r, r, 180, 90, 0);
            
            tvg_shape_set_stroke_color(paint, x_r, x_g, x_b, x_a);
            tvg_shape_set_stroke_width(paint, src_width);
        }
        
        if (src_grad) {
            Tvg_Gradient *grad = tvg_linear_gradient_new();
            if (src_grad_w) tvg_linear_gradient_set(grad, x, y, x, y + h);
            else tvg_linear_gradient_set(grad, x, y, x + w, y);
            tvg_gradient_set_color_stops(grad, color2, 2);
            
            if (src_width == 0 || src_width >= src_radius)
                tvg_shape_set_linear_gradient(paint, grad);
            else tvg_shape_set_stroke_linear_gradient(paint, grad);
        }
        
        tvg_canvas_push(canvas, paint);
    }
}
#endif

/*@brief 对象控件绘制圆角矩形
 *@param handle 对象控件句柄
 *@param clip   剪切域
 *@param prop   属性(part;state)
 *@retval 成功失败
 */
bool scui_object_draw_crect(scui_handle_t handle, scui_area_t *clip, scui_object_prop_t *prop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    scui_object_type_t src_style[] = {
        scui_object_style_crect_alpha,
        scui_object_style_crect_color,
        scui_object_style_crect_width,
        scui_object_style_crect_radius,
        
        scui_object_style_crect_color_grad,
        scui_object_style_crect_multi,
    };
    
    /* 从属性列表取属性 */
    scui_object_data_t src_data[scui_arr_len(src_style)] = {0};
    scui_object_data_t local_data_zero = {0};
    scui_object_prop_t local_prop = {
        .part  = prop->part,
        .state = prop->state,
    };
    for (scui_coord_t idx = 0; idx < scui_arr_len(src_style); idx++) {
        local_prop.data  = local_data_zero;
        local_prop.style = src_style[idx];
        if (idx <= 3 && !scui_object_prop_sync(handle, &local_prop)) {
            SCUI_LOG_ERROR("lose crect style %d", idx);
            return false;
        }
        
        src_data[idx] = local_prop.data;
    }
    
    scui_alpha_t   alpha   = src_data[0].alpha;
    scui_color32_t color_s = src_data[1].color32;
    scui_coord_t   width   = src_data[2].number;
    scui_coord_t   radius  = src_data[3].number;
    scui_color32_t color_e = src_data[4].color32;
    bool shadow = src_data[5].multi.shadow;
    bool grad_w = src_data[5].multi.grad_w;
    bool grad   = src_data[5].multi.grad;
    
    alpha = scui_alpha_mix(alpha, widget->alpha);
    scui_color_t color = {
        .color   = color_s,
        .color_s = color_s,
        .color_e = color_e,
    };
    
    /* tvg draw cb */
    scui_draw_dsc_t draw_dsc = {0};
    draw_dsc.type = scui_draw_type_pixel_tvg;
    draw_dsc.graph.src_area   = *clip;
    draw_dsc.graph.src_width  = width;
    draw_dsc.graph.src_radius = radius;
    draw_dsc.graph.src_shadow = shadow;
    draw_dsc.graph.src_grad_w = grad_w;
    draw_dsc.graph.src_grad   = grad;
    #if SCUI_DRAW_USE_THORVG
    draw_dsc.graph.src_tvg_cb = scui_object_tvg_crect_cb;
    #endif
    
    scui_widget_draw_graph(widget->myself, NULL,
        alpha, color, &draw_dsc);
    
    return true;
}

/*@brief 对象控件绘制圆角矩形
 *@param handle 对象控件句柄
 *@param clip   剪切域
 *@param prop   属性(state)
 *@retval 成功失败
 */
bool scui_object_draw_crect4(scui_handle_t handle, scui_area_t *clip, scui_object_prop_t *prop)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    scui_object_type_t src_part[] = {
        scui_object_part_crect_bg,
        scui_object_part_crect_bd,
        scui_object_part_crect_ol,
        scui_object_part_crect_sd,
    };
    
    bool draw_retval = true;
    scui_object_prop_t local_prop = *prop;
    for (scui_coord_t idx = 0; idx < scui_arr_len(src_part); idx++) {
        local_prop.part  = src_part[idx];
        
        scui_object_state_get(widget->myself, &local_prop.state);
        if (scui_object_draw_crect(widget->myself, clip, &local_prop))
            continue;
        
        draw_retval = false;
    }
    
    return draw_retval;
}

/*@brief 对象控件添加经典圆角矩形属性
 *@param handle 对象控件句柄
 *@param crect  圆角矩形属性
 */
void scui_object_prop_crect(scui_handle_t handle, scui_object_crect_t *crect)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    SCUI_ASSERT(crect->index < 4);
    scui_coord_t src_radius[4] = {[0] = crect->radius,};
    for (scui_coord_t idx = 0; idx + 1 < 4; idx++) {
        src_radius[idx + 1]  = src_radius[idx];
        src_radius[idx + 1] += crect->width[idx + 1];
    }
    
    scui_object_type_t src_style[] = {
        scui_object_style_crect_alpha,
        scui_object_style_crect_color,
        scui_object_style_crect_width,
        scui_object_style_crect_radius,
        
        scui_object_style_crect_color_grad,
        scui_object_style_crect_multi,
    };
    scui_object_data_t src_data[scui_arr_len(src_style)] = {
        [0].alpha   = crect->alpha[crect->index],
        [1].color32 = crect->color[crect->index].color_s,
        [2].number  = crect->width[crect->index],
        [3].number  = src_radius[crect->index],
        /*  */
        [4].color32 = crect->color[crect->index].color_e,
        [5].multi.shadow = crect->index == 3,
        [5].multi.grad_w = crect->grad_w,
        [5].multi.grad   = crect->grad,
    };
    
    scui_object_prop_t local_prop = {
        .part  = crect->part,
        .state = crect->state,
    };
    for (scui_coord_t idx = 0; idx < scui_arr_len(src_style); idx++) {
        local_prop.style = src_style[idx];
        local_prop.data  = src_data[idx];
        scui_object_prop_add(widget->myself, &local_prop);
    }
}

/*@brief 对象控件添加经典圆角矩形属性
 *@param handle 对象控件句柄
 *@param crect  圆角矩形属性
 */
void scui_object_prop_crect4(scui_handle_t handle, scui_object_crect_t *crect)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_object));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_object_t *object = (void *)widget;
    
    scui_object_type_t src_part[] = {
        scui_object_part_crect_bg,
        scui_object_part_crect_bd,
        scui_object_part_crect_ol,
        scui_object_part_crect_sd,
    };
    
    scui_object_crect_t local_crect = *crect;
    for (scui_coord_t idx = 0; idx < scui_arr_len(src_part); idx++) {
        local_crect.part  = src_part[idx];
        local_crect.index = idx;
        
        scui_object_prop_crect4(handle, &local_crect);
    }
}
