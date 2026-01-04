/*实现目标:
 *    控件组织
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件类型检查
 *@param handle 控件句柄
 *@param type   控件类型
 *@retval 控件类型检查
 */
bool scui_widget_type_check(scui_handle_t handle, scui_widget_type_t type)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_widget_type_t widget_type = widget->type;
    
    while (true) {
        // 控件类型相等
        if (widget_type == type)
            return true;
        
        scui_widget_map_t *widget_map = NULL;
        scui_widget_map_find(widget_type, &widget_map);
        if (widget_map == NULL || widget_type == widget_map->base)
            break;
        
        // 检查父控件的可继承属性
        widget_type = widget_map->base;
        scui_widget_map_find(widget_type, &widget_map);
        if (!widget_map->inherit)
             break;
    }
    
    return false;
}

/*@brief 查找控件映射表
 *@param type       控件类型
 *@param widget_map 控件映射表
 */
void scui_widget_map_find(scui_widget_type_t type, scui_widget_map_t **widget_map)
{
    /* 控件组织表 */
    static const scui_widget_map_t scui_widget_map[scui_widget_type_num] = {
        /* 基础控件 */
        [scui_widget_type_window] = {
            .size    = sizeof(scui_window_t),
            .maker   = sizeof(scui_window_maker_t),
            .base    = scui_widget_type_window,
            .make    = scui_window_make,
            .burn    = scui_window_burn,
            .invoke  = NULL,
            .inherit = false,
            .name    = "window",
        },
        [scui_widget_type_custom] = {
            .size    = sizeof(scui_custom_t),
            .maker   = sizeof(scui_custom_maker_t),
            .base    = scui_widget_type_custom,
            .make    = scui_custom_make,
            .burn    = scui_custom_burn,
            .invoke  = NULL,
            .inherit = false,
            .name    = "custom",
        },
        [scui_widget_type_scroll] = {
            .size    = sizeof(scui_scroll_t),
            .maker   = sizeof(scui_scroll_maker_t),
            .base    = scui_widget_type_scroll,
            .make    = scui_scroll_make,
            .burn    = scui_scroll_burn,
            .invoke  = scui_scroll_invoke,
            .inherit = true,
            .name    = "scroll",
        },
        [scui_widget_type_string] = {
            .size    = sizeof(scui_string_t),
            .maker   = sizeof(scui_string_maker_t),
            .base    = scui_widget_type_string,
            .make    = scui_string_make,
            .burn    = scui_string_burn,
            .invoke  = scui_string_invoke,
            .inherit = false,
            .name    = "string",
        },
        [scui_widget_type_linear] = {
            .size    = sizeof(scui_linear_t),
            .maker   = sizeof(scui_linear_maker_t),
            .base    = scui_widget_type_scroll,
            .make    = scui_linear_make,
            .burn    = scui_linear_burn,
            .invoke  = scui_linear_invoke,
            .inherit = true,
            .name    = "linear",
        },
        [scui_widget_type_roller] = {
            .size    = sizeof(scui_roller_t),
            .maker   = sizeof(scui_roller_maker_t),
            .base    = scui_widget_type_linear,
            .make    = scui_roller_make,
            .burn    = scui_roller_burn,
            .invoke  = scui_roller_invoke,
            .inherit = true,
            .name    = "roller",
        },
        [scui_widget_type_xvedio] = {
            .size    = sizeof(scui_xvedio_t),
            .maker   = sizeof(scui_xvedio_maker_t),
            .base    = scui_widget_type_xvedio,
            .make    = scui_xvedio_make,
            .burn    = scui_xvedio_burn,
            .invoke  = scui_xvedio_invoke,
            .inherit = false,
            .name    = "xvedio",
        },
        
        /* 扩展控件 */
        [scui_widget_type_button] = {
            .size    = sizeof(scui_button_t),
            .maker   = sizeof(scui_button_maker_t),
            .base    = scui_widget_type_button,
            .make    = scui_button_make,
            .burn    = scui_button_burn,
            .invoke  = scui_button_invoke,
            .inherit = false,
            .name    = "button",
        },
        [scui_widget_type_chart] = {
            .size    = sizeof(scui_chart_t),
            .maker   = sizeof(scui_chart_maker_t),
            .base    = scui_widget_type_chart,
            .make    = scui_chart_make,
            .burn    = scui_chart_burn,
            .invoke  = scui_chart_invoke,
            .inherit = false,
            .name    = "chart",
        },
    };
    
    SCUI_ASSERT(type < scui_widget_type_num);
    *widget_map = &scui_widget_map[type];
}

/*@brief 清空控件
 *@param handle 控件句柄
 */
void scui_widget_clean(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle_c = widget->child_list[idx];
        scui_widget_destroy(handle_c);
        
        // 调用结束后, 它应该已经被清理了
        SCUI_ASSERT(widget->child_list[idx] == SCUI_HANDLE_INVALID);
    }
}

/*@brief 销毁控件
 *@param handle 控件句柄
 */
void scui_widget_destroy(scui_handle_t handle)
{
    /* 重复的销毁过滤 */
    if (scui_handle_unmap(handle))
        return;
    
    /* 控件默认设置为隐藏 */
    scui_widget_state_show(handle, false);
    
    /* 控件销毁前:控件销毁事件 */
    scui_event_t event = {
        .object     = handle,
        .style.sync = true,
        .type       = scui_event_destroy,
    };
    scui_event_notify(&event);
    
    /* 得到控件实例 */
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_widget_map_t *widget_map = NULL;
    scui_widget_map_find(widget->type, &widget_map);
    /* 销毁流程 */
    widget_map->burn(handle);
    /* 销毁控件实例 */
    SCUI_MEM_FREE(widget);
}
/*@brief 创建控件
 *@param maker  控件构造实例
 *@param handle 控件句柄
 */
void scui_widget_create(void *maker, scui_handle_t *handle)
{
    scui_widget_map_t   *widget_map   = NULL;
    scui_widget_maker_t *widget_maker = maker;
    scui_widget_map_find(widget_maker->type, &widget_map);
    
    /* 创建构造器实例 */
    void *local_maker = SCUI_MEM_ALLOC(scui_mem_type_mix, widget_map->maker);
    memcpy(local_maker, widget_maker, widget_map->maker);
    /* 创建控件实例 */
    scui_widget_t *widget = SCUI_MEM_ZALLOC(scui_mem_type_mix, widget_map->size);
    /* 构造流程 */
    widget_maker = local_maker;
    /* 备注:动态构造器是不知道句柄的 */
    /* 所以动态构造器需要填入非法句柄 */
    // widget_maker->myself = SCUI_HANDLE_INVALID;
    widget_map->make(widget, widget_maker, handle);
    SCUI_MEM_FREE(widget_maker);
    
    /* 控件构建后:控件构建事件 */
    scui_event_t event = {
        .object     = widget->myself,
        .style.sync = true,
        .type       = scui_event_create,
    };
    scui_event_notify(&event);
    
    /* 控件默认设置为显示 */
    scui_widget_state_show(widget->myself, false);
}

/*@brief 创建控件树(句柄映射表)
 *@param handle 根控件句柄
 */
void scui_widget_create_layout_tree(scui_handle_t handle)
{
    if (scui_handle_remap(handle))
        return;
    
    scui_widget_t *widget = NULL;
    scui_widget_maker_t *widget_maker = NULL;
    widget = widget_maker = scui_handle_source_check(handle);
    SCUI_ASSERT(widget_maker->parent == SCUI_HANDLE_INVALID);
    
    scui_handle_table_t *handle_table = scui_handle_table_find(handle);
    SCUI_ASSERT(handle_table != NULL);
    
    do {
        /* 先创建根控件,然后延续依次创建剩下的控件 */
        /* 静态控件规则为,一个场景为一段连续句柄,父控件在前子控件在后 */
        scui_widget_create(widget_maker, &handle);
        /* 迭代到下一个句柄 */
        widget_maker = scui_handle_source(++handle);
        if (widget_maker == NULL)
            break;
        widget = widget_maker;
        /* 一直迭代到下一个根控件句柄前停下 */
        if ((scui_handle_unmap(handle) && widget_maker->parent == SCUI_HANDLE_INVALID) ||
            (scui_handle_remap(handle) && widget->parent == SCUI_HANDLE_INVALID))
            break;
    } while (handle < handle_table->offset + handle_table->number);
}
