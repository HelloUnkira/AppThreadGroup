/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件子类型信息匹配
 *@param type 控件子类型
 *@param info 控件子类型信息
 */
static void scui_menial_info_map_find(scui_menial_type_t type, scui_menial_info_t **info)
{
    static const scui_menial_info_t scui_menial_info[scui_menial_type_num] = {
        [scui_menial_type_btn] = {
            .maker   = scui_menial_btn_maker,
            .config  = scui_menial_btn_config,
            .recycle = scui_menial_btn_recycle,
            .invoke  = scui_menial_btn_invoke,
        },
        [scui_menial_type_arc] = {
            .maker   = scui_menial_arc_maker,
            .config  = scui_menial_arc_config,
            .recycle = scui_menial_arc_recycle,
            .invoke  = scui_menial_arc_invoke,
        },
        [scui_menial_type_bar] = {
            .maker   = scui_menial_bar_maker,
            .config  = scui_menial_bar_config,
            .recycle = scui_menial_bar_recycle,
            .invoke  = scui_menial_bar_invoke,
        },
    };
    
    SCUI_ASSERT(info != NULL);
    SCUI_ASSERT(type >  scui_menial_type_none);
    SCUI_ASSERT(type <  scui_menial_type_num);
    *info = &scui_menial_info[type];
}

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_menial_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_menial_t *menial = widget;
    scui_menial_maker_t *menial_maker = widget_maker;
    
    scui_menial_info_t *menial_info = NULL;
    scui_menial_info_map_find(menial_maker->type, &menial_info);
    menial_info->maker(menial_maker);
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_menial));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    /* menial必须依赖SVG */
    #if SCUI_DRAW_USE_THORVG == 0
    SCUI_LOG_ERROR("menial must rely on SVG");
    SCUI_ASSERT(false);
    #endif
    
    /* 资源同步与构造 */
    menial->type = menial_maker->type;
    menial->data = menial_maker->data;
    menial_info->config(menial);
    SCUI_ASSERT(menial->type > scui_menial_type_none);
    SCUI_ASSERT(menial->type < scui_menial_type_num);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_menial_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_menial));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_menial_t *menial = (void *)widget;
    
    /* 资源析构 */
    scui_menial_info_t *menial_info = NULL;
    scui_menial_info_map_find(menial->type, &menial_info);
    menial_info->recycle(menial);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_menial_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_menial_t *menial = (void *)widget;
    
    /* 事件处理回调:子控件 */
    scui_menial_info_t *menial_info = NULL;
    scui_menial_info_map_find(menial->type, &menial_info);
    menial_info->invoke(event);
}
