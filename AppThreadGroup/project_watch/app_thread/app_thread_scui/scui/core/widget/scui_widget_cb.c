/*实现目标:
 *    控件组织回调
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 查找控件映射表
 *@param type 控件类型
 *@retval 控件映射表
 */
scui_widget_cb_t * scui_widget_cb_find(uint32_t type)
{
    /* 控件组织表 */
    static const scui_widget_cb_t scui_widget_cb[scui_widget_type_num] = {
        [scui_widget_type_window].event   = (scui_widget_cb_event_t)    NULL,
        [scui_widget_type_window].create  = (scui_widget_cb_create_t)   scui_window_create,
        [scui_widget_type_window].destroy = (scui_widget_cb_destroy_t)  scui_window_destroy,
        [scui_widget_type_custom].event   = (scui_widget_cb_event_t)    NULL,
        [scui_widget_type_custom].create  = (scui_widget_cb_create_t)   scui_custom_create,
        [scui_widget_type_custom].destroy = (scui_widget_cb_destroy_t)  scui_custom_destroy,
        
    };
    
    SCUI_ASSERT(type < scui_widget_type_num);
    return &scui_widget_cb[type];
}

/*@brief 通过映射表调用创建一个控件
 *@param handle 控件句柄
 */
void scui_widget_cb_create(scui_handle_t handle)
{
    /* 重复的创建过滤 */
    if (scui_handle_remap(handle))
        return;
    
    scui_widget_maker_t *maker = NULL;
    maker = scui_handle_get(handle);
    SCUI_ASSERT(maker != NULL);
    scui_widget_cb_t *widget_cb = scui_widget_cb_find(maker->type);
    widget_cb->create(maker, &handle, true);
}

/*@brief 通过映射表调用销毁一个控件
 *@param handle 控件句柄
 */
void scui_widget_cb_destroy(scui_handle_t handle)
{
    /* 重复的销毁过滤 */
    if (!scui_handle_remap(handle));
         return;
    
    scui_widget_t *widget = NULL;
    widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    scui_widget_cb_t *widget_cb = scui_widget_cb_find(widget->type);
    widget_cb->destroy(handle);
}

/*@brief 通过映射表调用加载一个控件树
 *       从根控件开始到它的所有子控件(动态子控件在show之前生成)
 *@param handle 根控件句柄
 */
void scui_widget_cb_load(scui_handle_t handle)
{
    scui_widget_maker_t *maker = NULL;
    maker = scui_handle_get(handle);
    SCUI_ASSERT(maker != NULL);
    SCUI_ASSERT(maker->parent == SCUI_HANDLE_INVALID);
    
    scui_handle_table_t *handle_table = scui_handle_table_find(handle);
    SCUI_ASSERT(handle_table != NULL);
    
    do {
        /* 先创建根控件,然后延续依次创建剩下的控件 */
        /* 静态控件规则为,一个场景为一段连续句柄,父控件在前子控件在后 */
        scui_widget_cb_create(handle);
        /* 迭代到下一个句柄 */
        handle++;
        maker = scui_handle_get(handle);
        SCUI_ASSERT(maker != NULL);
        SCUI_ASSERT(!scui_handle_remap(handle));
        /* 一直迭代到下一个根控件句柄前停下 */
        if (maker->parent == SCUI_HANDLE_INVALID)
            break;
    } while (handle < handle_table->offset + handle_table->number);
}

/*@brief 通过映射表调用卸载一个控件树
 *       从根控件开始到它的所有子控件
 *@param handle 根控件句柄
 */
void scui_widget_cb_unload(scui_handle_t handle)
{
    scui_widget_t *widget = NULL;
    widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    scui_widget_cb_destroy(handle);
}
