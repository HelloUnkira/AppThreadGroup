/*实现目标:
 *    控件组织
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/* 控件数据布局表 */
static uint8_t *scui_widget_layout = NULL;
static uint32_t scui_widget_layout_size = 0;

/* 函数映射布局表 */
static scui_event_cb_t *scui_event_cb_layout = NULL;
static uint32_t scui_event_cb_layout_num = 0;

/* 图片信息布局表 */
static scui_image_t *scui_image_layout = NULL;
static uint32_t scui_image_layout_num = 0;

/* 控件组织表 */
static const scui_widget_ogz_t scui_widget_ogz[scui_widget_type_num] = {
    [scui_widget_type_custom].paint   = NULL,   // scui_custom_paint
    [scui_widget_type_custom].create  = NULL,   // scui_custom_create_by_layout
    [scui_widget_type_custom].destroy = NULL,   // scui_custom_destroy
    [scui_widget_type_window].paint   = NULL,
    [scui_widget_type_window].create  = NULL,
    [scui_widget_type_window].destroy = NULL,
};

/* 控件组织尺寸表 */
static const uint32_t scui_widget_ogz_size[scui_widget_type_num] = {
    // sizeof(scui_custom_t),
    // sizeof(scui_window_t),
    0,
};

uint32_t scui_widget_ogz_create_by_layout(scui_handle_t handle, uint32_t *offset)
{
    scui_widget_t *widget = scui_widget_layout + *offset;
    *offset += scui_widget_ogz_size[widget->type];
    
    // return scui_widget_ogz[widget->type].create(widget, handle);
}

uint32_t scui_widget_ogz_destroy(scui_handle_t handle, bool from_parent)
{
}

uint32_t scui_widget_ogz_scene_load(scui_handle_t handle)
{
}

uint32_t scui_widget_ogz_scene_unload(scui_handle_t handle)
{
}
