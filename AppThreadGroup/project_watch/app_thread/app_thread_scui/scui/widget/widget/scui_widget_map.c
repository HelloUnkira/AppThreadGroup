/*实现目标:
 *    控件组织
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_widget_cb_maker_t scui_widget_cb_maker = NULL;
static scui_widget_cb_apply_t scui_widget_cb_apply = NULL;

/*@brief 控件配置回调注册
 *@param maker 控件配置回调
 */
void scui_widget_cb_maker_register(scui_widget_cb_maker_t maker)
{
    scui_widget_cb_maker = maker;
}

/*@brief 控件应用回调注册
 *@param apply 控件应用回调
 */
void scui_widget_cb_apply_register(scui_widget_cb_apply_t apply)
{
    scui_widget_cb_apply = apply;
}

/*@brief 控件树检查
 *@param handle 控件句柄
 */
void scui_widget_tree_check(scui_handle_t handle)
{
    SCUI_LOG_INFO("widget %u", handle);
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 递归迭代它的孩子列表 */
    scui_widget_child_list_btra(widget, idx)
    scui_widget_tree_check(widget->child_list[idx]);
}

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
        /* 控件类型相等 */
        if (widget_type == type)
            return true;
        
        scui_widget_map_t *widget_map = NULL;
        scui_widget_map_find(widget_type, &widget_map);
        if (widget_map == NULL || widget_type == widget_map->base)
            break;
        
        /* 检查父控件的可继承属性 */
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
            .invoke  = scui_window_invoke,
            .inherit = false,
            .name    = "window",
        },
        [scui_widget_type_custom] = {
            .size    = sizeof(scui_custom_t),
            .maker   = sizeof(scui_custom_maker_t),
            .base    = scui_widget_type_custom,
            .make    = scui_custom_make,
            .burn    = scui_custom_burn,
            .invoke  = scui_custom_invoke,
            .inherit = true,
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
        
        /* 扩展控件 */
        [scui_widget_type_roller] = {
            .size    = sizeof(scui_roller_t),
            .maker   = sizeof(scui_roller_maker_t),
            .base    = scui_widget_type_scroll,
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
        [scui_widget_type_xwatch] = {
            .size    = sizeof(scui_xwatch_t),
            .maker   = sizeof(scui_xwatch_maker_t),
            .base    = scui_widget_type_xwatch,
            .make    = scui_xwatch_make,
            .burn    = scui_xwatch_burn,
            .invoke  = scui_xwatch_invoke,
            .inherit = false,
            .name    = "xwatch",
        },
        
        /* 可选控件 */
        [scui_widget_type_object] = {
            .size    = sizeof(scui_object_t),
            .maker   = sizeof(scui_object_maker_t),
            .base    = scui_widget_type_object,
            .make    = scui_object_make,
            .burn    = scui_object_burn,
            .invoke  = scui_object_invoke,
            .inherit = true,
            .name    = "object",
        },
        [scui_widget_type_menial] = {
            .size    = sizeof(scui_menial_t),
            .maker   = sizeof(scui_menial_maker_t),
            .base    = scui_widget_type_object,
            .make    = scui_menial_make,
            .burn    = scui_menial_burn,
            .invoke  = scui_menial_invoke,
            .inherit = false,
            .name    = "menial",
        },
    };
    
    SCUI_ASSERT(type < scui_widget_type_num);
    *widget_map = &scui_widget_map[type];
}

/*@brief 销毁所有子控件
 *@param handle 控件句柄
 */
void scui_widget_clean(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 递归销毁自己的孩子(一直销毁第一个即可) */
    while (widget->child_now != 0) {
        scui_handle_t handle_c = widget->child_list[0];
        scui_widget_destroy(handle_c);
    }
}

/*@brief 控件默认配置(make前)
 *@param maker 控件构造器实例
 *@param type  控件类型
 */
void scui_widget_ready(void *maker, scui_widget_type_t type)
{
    SCUI_ASSERT(scui_widget_cb_maker != NULL);
    scui_widget_cb_maker(maker, type);
}

/*@brief 销毁控件
 *@param handle 控件句柄
 */
void scui_widget_destroy(scui_handle_t handle)
{
    /* 重复的销毁过滤 */
    if (scui_handle_unmap(handle))
        return;
    
    scui_widget_t *widget = scui_handle_source_check(handle);
    
    /* 设置控件状态为隐藏 */
    scui_widget_state_view(widget->myself, false, false);
    
    /* 控件销毁前:控件销毁事件 */
    scui_event_define(event, widget->myself, true, scui_event_destroy, NULL);
    scui_event_notify(&event);
    
    /* 得到控件实例 */
    scui_widget_map_t *widget_map = NULL;
    scui_widget_map_find(widget->type, &widget_map);
    /* 销毁流程 */
    widget_map->burn(widget->myself);
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
    
    /* 创建构造器实例(配置-同步) */
    maker = SCUI_MEM_ALLOC(scui_mem_type_mix, widget_map->maker);
    memcpy(maker, widget_maker, widget_map->maker);
    widget_maker = maker;
    
    /* 创建控件实例 */
    scui_widget_t *widget = SCUI_MEM_ZALLOC(scui_mem_type_mix, widget_map->size);
    /* 备注:动态构造器是不知道句柄的 */
    /* 所以动态构造器需要填入非法句柄 */
    /* widget_maker->myself = SCUI_HANDLE_INVALID; */
    widget_map->make(widget, widget_maker, handle);
    SCUI_MEM_FREE(widget_maker);
    
    /* 控件创建后:控件应用回调 */
    SCUI_ASSERT(scui_widget_cb_apply != NULL);
    scui_widget_cb_apply(widget->myself);
    
    /* 控件构建后:控件构建事件 */
    scui_event_define(event, widget->myself, true, scui_event_create, NULL);
    scui_event_notify(&event);
    
    /* 设置控件状态为显示 */
    scui_widget_state_view(widget->myself, true, false);
}

/*@brief 创建控件树(句柄映射表)
 *@param handle 根控件句柄
 */
void scui_widget_layout_tree(scui_handle_t handle)
{
    if (scui_handle_remap(handle))
        return;
    
    const scui_widget_json_val_t *val = NULL;
    scui_widget_json_key_t *key = scui_handle_source_check(handle);
    val = scui_widget_json_key_find(key, scui_widget_json_field_widget_parent);
    SCUI_ASSERT(val->handle == SCUI_HANDLE_INVALID);
    
    scui_handle_table_t *handle_table = scui_handle_table_find(handle);
    SCUI_ASSERT(handle_table != NULL);
    
    do {
        /* 先创建根控件,然后延续依次创建剩下的控件 */
        /* 静态控件规则为,一个窗口为一段连续句柄,父控件在前子控件在后 */
        val = scui_widget_json_key_find(key, scui_widget_json_field_widget_type);
        scui_widget_type_t type = (scui_widget_type_t)val->handle;
        SCUI_ASSERT(type > scui_widget_type_unknown);
        SCUI_ASSERT(type < scui_widget_type_num);
        
        scui_widget_map_t *widget_map = NULL;
        scui_widget_map_find(type, &widget_map);
        scui_widget_maker_t *widget_maker = SCUI_MEM_ALLOC(scui_mem_type_mix, widget_map->maker);
        memset(widget_maker, 0, widget_map->maker);
        
        scui_widget_ready(widget_maker, type);
        for (scui_handle_t i = 0; i < key->num; i++) {
            if (key->cfg[i] == NULL) continue;
            
            key->cfg[i](widget_maker, (void *)&key->val[i]);
        }
        
        scui_widget_create(widget_maker, &handle);
        SCUI_MEM_FREE(widget_maker);
        handle++;
        /* 迭代到下一个句柄 */
        if (handle  >= handle_table->offset + handle_table->number) break;
        void *source = scui_handle_source(handle);
        if  (source == NULL) break;
        
        /* 一直迭代到下一个根控件句柄前停下 */
        if (scui_handle_unmap(handle)) {
            key = (scui_widget_json_key_t *)source;
            val = scui_widget_json_key_find(key, scui_widget_json_field_widget_parent);
            if (val->handle == SCUI_HANDLE_INVALID)
                break;
        } else {
            scui_widget_t *widget_c = (scui_widget_t *)source;
            if (widget_c->parent == SCUI_HANDLE_INVALID)
                break;
        }
    } while (true);
}

/*@brief  控件快速访问字段值查找
 *@param  key   控件配置表键
 *@param  field 字段id
 *@retval 字段值
 */
const scui_widget_json_val_t * scui_widget_json_key_find(const scui_widget_json_key_t *key, scui_widget_json_field_t field)
{
    if (field < scui_widget_json_field_num) return &key->val[field];
    SCUI_ASSERT(false); return NULL;
}
