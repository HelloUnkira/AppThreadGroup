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
void scui_obj_chk_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 对象继承序列 <基类 - 本类> */
    scui_widget_t  *widget  = inst;
    scui_object_t  *object  = (void *)widget;
    scui_obj_btn_t *obj_btn = (void *)widget;
    scui_obj_chk_t *obj_chk = (void *)widget;
    /* 对象构造器继承序列 <基类 - 本类> */
    scui_widget_maker_t  *widget_maker  = inst_maker;
    scui_object_maker_t  *object_maker  = (void *)widget_maker;
    scui_obj_btn_maker_t *obj_btn_maker = (void *)widget_maker;
    scui_obj_chk_maker_t *obj_chk_maker = (void *)widget_maker;
    
    /* 构造派生控件实例 */
    scui_obj_btn_make(obj_btn, obj_btn_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_obj_chk));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* 资源同步与构造 */
    obj_chk->fixed     = obj_chk_maker->fixed;
    obj_chk->state     = obj_chk_maker->state;
    obj_chk->font      = obj_chk_maker->font;
    obj_chk->lang      = obj_chk_maker->lang;
    obj_chk->sym_def   = obj_chk_maker->sym_def;
    obj_chk->sym_chk   = obj_chk_maker->sym_chk;
    obj_chk->sym_color = obj_chk_maker->sym_color;
    
    /* symbol语言族(定位符号字库) */
    if (obj_chk->lang == SCUI_HANDLE_INVALID)
        obj_chk->lang  = scui_lang_type_symbol;
    
    /* 初始状态: make结束自行指定(none=默认def) */
    if (obj_chk->state != scui_object_type_none)
        scui_object_state_set(*handle, obj_chk->state);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_obj_chk_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_chk));
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_btn_t *obj_btn = (void *)widget;
    scui_obj_chk_t *obj_chk = (void *)widget;
    
    /* 析构派生控件实例 */
    scui_obj_btn_burn(widget->myself);
}

/*@brief 对象控件标记获取
 *@param handle 控件句柄
 *retval 对象控件标记
 */
bool scui_obj_chk_fixed(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_chk));
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_btn_t *obj_btn = (void *)widget;
    scui_obj_chk_t *obj_chk = (void *)widget;
    
    return obj_chk->fixed;
}

/*@brief 对象控件状态获取(特殊语义)
 *@param handle 控件句柄
 *@param state  对象控件状态
 */
void scui_obj_chk_state(scui_handle_t handle, scui_object_type_t *state)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_obj_chk));
    scui_widget_t  *widget  = scui_handle_source_check(handle);
    scui_object_t  *object  = (void *)widget;
    scui_obj_btn_t *obj_btn = (void *)widget;
    scui_obj_chk_t *obj_chk = (void *)widget;
    
    if (obj_chk->fixed) *state = obj_chk->state;
    else scui_object_state_get(handle, state);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_obj_chk_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t  *widget  = scui_handle_source_check(event->object);
    scui_object_t  *object  = (void *)widget;
    scui_obj_btn_t *obj_btn = (void *)widget;
    scui_obj_chk_t *obj_chk = (void *)widget;
    
    /* 固定标记: 不响应点击(拦截ptr事件) */
    if (obj_chk->fixed && scui_event_type_ptr(event->type)) return;
    
    /* 基类处理(按钮事件/过渡动画推进) */
    scui_obj_btn_invoke(event);
    
    switch (event->type) {
    case scui_event_layout: {
        /* 自动宽高: 以符号宽高最大值作为正方形尺寸 */
        if (!widget->state.layout_w && !widget->state.layout_h) break;
        if (obj_chk->font == SCUI_HANDLE_INVALID) break;
        
        scui_handle_t font = scui_font_name_match(obj_chk->font, obj_chk->lang);
        if (font == SCUI_HANDLE_INVALID) break;
        
        scui_coord_t size = 0;
        if (obj_chk->sym_def) {
            uint32_t sym_code = scui_symbol_code((uint8_t *)obj_chk->sym_def);
            scui_area_t sym_area = scui_symbol_area(font, sym_code);
            scui_coord_t sym_max = sym_area.w > sym_area.h ? sym_area.w : sym_area.h;
            if (sym_max > size) size = sym_max;
        }
        if (obj_chk->sym_chk) {
            uint32_t sym_code = scui_symbol_code((uint8_t *)obj_chk->sym_chk);
            scui_area_t sym_area = scui_symbol_area(font, sym_code);
            scui_coord_t sym_max = sym_area.w > sym_area.h ? sym_area.w : sym_area.h;
            if (sym_max > size) size = sym_max;
        }
        
        if (size <= 0) break;
        
        /* 圆角内切: 取base的radius, 保证symbol被完全包裹 */
        scui_object_prop_t prop = {
            .part  = scui_object_part_rect_bg,
            .form  = scui_object_form_rect_base,
            .style = scui_object_style_rect_radius,
        };
        scui_coord_t radius = 0;
        if (scui_object_prop_sync(event->object, &prop))
            radius = prop.data.number;
        size += radius * 2;
        scui_widget_adjust_size(event->object, size, size);
        break;
    }
    case scui_event_draw_graph: {
        
        /* 符号绘制: 按状态(def/chk) */
        scui_object_type_t state = scui_object_type_none;
        scui_object_state_get(event->object, &state);
        
        const uint8_t *code = obj_chk->sym_def;
        if (state == scui_object_state_chk) code = obj_chk->sym_chk;
        if (obj_chk->font == SCUI_HANDLE_INVALID) break;
        if (code == NULL) break;
        
        uint32_t sym_code = scui_symbol_code((uint8_t *)code);
        scui_handle_t sym_font = scui_font_name_match(obj_chk->font, obj_chk->lang);
        
        /* symbol alpha: 直接取base的alpha(跟随base透明度) */
        scui_object_prop_t sym_prop = {
            .part  = scui_object_part_rect_bg,
            .form  = scui_object_form_rect_base,
            .state = state,
            .style = scui_object_style_rect_alpha,
        };
        scui_color_t sym_color = obj_chk->sym_color;
        if (scui_object_prop_sync(event->object, &sym_prop)) {
            sym_color.color_s.ch.a = sym_prop.data.alpha;
            sym_color.color_e.ch.a = sym_prop.data.alpha;
        }
        
        /* 符号居中(相对控件) */
        scui_area_t sym_area   = scui_symbol_area(sym_font, sym_code);
        scui_area_t sym_target = {
            .x = (widget->clip.w - sym_area.w) / 2,
            .y = (widget->clip.h - sym_area.h) / 2,
            .w = sym_area.w,
            .h = sym_area.h,
        };
        
        scui_widget_draw_symbol(event->object, &sym_target, NULL,
            sym_color, sym_font, sym_code);
        break;
    }
    default:
        break;
    }
}
