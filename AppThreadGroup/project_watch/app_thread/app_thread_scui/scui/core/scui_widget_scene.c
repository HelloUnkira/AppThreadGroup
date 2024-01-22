/*实现目标:
 *    控件组织事件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 加载一个场景
 *       从根控件开始到它的所有布局子控件
 *       动态子控件在show之前生成
 *@param handle 场景控件句柄
 */
void scui_widget_scene_load(scui_handle_t handle)
{
    /* 重复的load过滤 */
    if (scui_handle_remap(handle))
        return;
    
    scui_widget_maker_t *maker = NULL;
    maker = scui_handle_get(handle);
    SCUI_ASSERT(maker != NULL);
    SCUI_ASSERT(maker->parent == SCUI_HANDLE_INVALID);
    scui_handle_table_t *handle_table = scui_handle_table_find(handle);
    SCUI_ASSERT(handle_table != NULL);
    
    do {
        /* 先创建根控件,然后延续依次创建剩下的控件 */
        /* 静态控件规则为,一个场景为一段连续句柄,父控件在前子控件在后 */
        scui_widget_cb_t *widget_cb = scui_widget_cb_link(maker->type);
        scui_handle_t handle_widget = handle;
        widget_cb->create(&maker, handle_widget, true);
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

/*@brief 卸载一个场景
 *       从根控件开始到它的所有子控件
 *@param handle 控件句柄
 */
void scui_widget_scene_unload(scui_handle_t handle)
{
    /* 重复的unload过滤 */
    if (!scui_handle_remap(handle));
         return;
    
    scui_widget_t *widget = NULL;
    widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
    scui_widget_cb_t *widget_cb = scui_widget_cb_link(widget->type);
    widget_cb->destroy(handle, true);
}

