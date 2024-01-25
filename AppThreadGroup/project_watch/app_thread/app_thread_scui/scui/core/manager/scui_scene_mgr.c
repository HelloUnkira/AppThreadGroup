/*实现目标:
 *    场景管理器
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_scene_mgr_t scui_scene_mgr = {0};

/*@brief 场景管理器混合根控件列表
 *       将所有根控件画布混合到绘制画布上
 *       场景管理器会有特殊的处理
 *       用于处理画布级别特效流程
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_scene_mgr_mix_list(scui_widget_t **list, uint32_t num)
{
    /*  */
}

/*@brief 场景管理器排序根控件列表
 *@param list 根控件列表
 *@param num  根控件数量
 */
void scui_scene_mgr_sort_list(scui_widget_t **list, uint32_t num)
{
    /* 要用稳定排序,这里使用冒泡排序 */
    for (uint32_t idx_i = 0; idx_i < num; idx_i++)
    for (uint32_t idx_j = 0; idx_j + 1 < num - idx_i; idx_j++) {
        scui_widget_t *widget_1 = list[idx_j];
        scui_widget_t *widget_2 = list[idx_j + 1];
        SCUI_ASSERT(widget_1->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(widget_2->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(widget_1->myself));
        SCUI_ASSERT(scui_handle_remap(widget_2->myself));
        
        SCUI_ASSERT(widget_1->type == scui_widget_type_window);
        SCUI_ASSERT(widget_2->type == scui_widget_type_window);
        scui_window_t *window_1 = (scui_window_t *)widget_1;
        scui_window_t *window_2 = (scui_window_t *)widget_2;
        
        if (window_1->level > window_2->level) {
            scui_widget_t *widget_t = list[idx_j];
            list[idx_j] = list[idx_j + 1];
            list[idx_j + 1] = widget_t;
        }
    }
}

/*@brief 场景管理器混合画布
 *       将所有独立画布混合到绘制画布上
 *       将所有无独立画布就地渲染
 */
void scui_scene_mgr_mix_surface(void)
{
    uint32_t scene_list_num = 0;
    scui_widget_t *scene_list[SCUI_SCENE_MGR_LIMIT] = {0};
    
    /* 第一轮混合:处理所有独立画布 */
    for (uint32_t idx = 0; idx < SCUI_SCENE_MGR_LIMIT; idx++) {
        if (scui_scene_mgr.scene[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t  handle = scui_scene_mgr.scene[idx];
        scui_widget_t *widget = scui_handle_get(handle);
        SCUI_ASSERT(scui_handle_remap(handle));
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        
        if (scui_widget_surface_only(widget))
            scene_list[scene_list_num++] = widget;
    }
    scui_scene_mgr_sort_list(scene_list, scene_list_num);
    scui_scene_mgr_mix_list(scene_list, scene_list_num);
    scene_list_num = 0;
    
    /* 第二轮混合:处理所有独立画布 */
    for (uint32_t idx = 0; idx < SCUI_SCENE_MGR_LIMIT; idx++) {
        if (scui_scene_mgr.scene[idx] == SCUI_HANDLE_INVALID)
            continue;
        scui_handle_t  handle = scui_scene_mgr.scene[idx];
        scui_widget_t *widget = scui_handle_get(handle);
        SCUI_ASSERT(scui_handle_remap(handle));
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        
        if (!scui_widget_surface_only(widget))
             scene_list[scene_list_num++] = widget;
    }
    scui_scene_mgr_sort_list(scene_list, scene_list_num);
    
    for (uint32_t idx = 0; idx < scene_list_num; idx++) {
        scui_widget_t *widget = scene_list[idx];
        scui_handle_t  handle = widget->myself;
        SCUI_ASSERT(widget->parent == SCUI_HANDLE_INVALID);
        SCUI_ASSERT(scui_handle_remap(handle));
        
        scui_widget_draw(handle, true);
    }
}

/*@brief 场景管理器获得活跃场景句柄
 *@retval 场景句柄
 */
scui_handle_t scui_scene_mgr_active_get(void)
{
    return scui_scene_mgr.scene_active;
}

/*@brief 场景管理器设置活跃场景句柄
 *@param handle 场景句柄
 */
void scui_scene_mgr_active_set(scui_handle_t handle)
{
    scui_scene_mgr.scene_active = handle;
}

/*@brief 控件默认事件处理回调
 *@param event 事件
 *@retval 事件状态
 */
scui_event_retval_t scui_scene_mgr_event_dispatch(scui_event_t *event)
{
    return scui_event_retval_quit;
}
