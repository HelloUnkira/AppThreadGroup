#include "app_lv_view_sched.h"

static void app_lv_view_config_create_empty(lv_obj_t *page);
static void app_lv_view_config_destroy_empty(lv_obj_t *page);
static void app_lv_view_config_focus_get_empty(lv_obj_t *page);
static void app_lv_view_config_focus_lost_empty(lv_obj_t *page);

/*****************************************************************************************/
/* 默认配置信息部分--------------------------------------------------------------------- */
/*****************************************************************************************/
static const app_lv_view_config_t app_lv_view_config_list_def[app_lv_view_id_num] = {
    [app_lv_view_id_home_watch] = {
    },
};
/*****************************************************************************************/
/* 默认配置信息部分--------------------------------------------------------------------- */
/*****************************************************************************************/
static app_lv_view_config_t *app_lv_view_config_list = NULL;

void app_lv_view_id_ready(void)
{
    size_t size = sizeof(app_lv_view_config_t) * (app_lv_view_id_num + 1);
    
    // 内存常驻表
    app_lv_view_config_list = lv_mem_alloc(size);
    lv_memcpy(app_lv_view_config_list, app_lv_view_config_list_def, size);
}

void app_lv_view_id_adjust(uint32_t type, app_lv_view_config_t *config)
{
    if (type > app_lv_view_id_unknown &&
        type < app_lv_view_id_num) {
        
        app_lv_view_config_list[type].create = config->create;
        app_lv_view_config_list[type].page_l = config->page_l;
        app_lv_view_config_list[type].page_r = config->page_r;
        app_lv_view_config_list[type].page_t = config->page_t;
        app_lv_view_config_list[type].page_b = config->page_b;
    }
}

app_lv_view_config_t *app_lv_view_id_inst(uint32_t type)
{
    LV_ASSERT(type > app_lv_view_id_unknown);
    LV_ASSERT(type < app_lv_view_id_num);
    
    // 多检查检查
    LV_ASSERT(app_lv_view_config_list[type].create != NULL);
    return &app_lv_view_config_list[type];
}

/*****************************************************************************************/
static void app_lv_view_config_create_empty(lv_obj_t *page)
{
}
static void app_lv_view_config_destroy_empty(lv_obj_t *page)
{
}
static void app_lv_view_config_focus_get_empty(lv_obj_t *page)
{
}
static void app_lv_view_config_focus_lost_empty(lv_obj_t *page)
{
}
