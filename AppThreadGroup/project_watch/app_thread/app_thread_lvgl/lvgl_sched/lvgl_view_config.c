#include "lvgl_view_sched.h"

static void lvgl_view_config_create_empty(lv_obj_t *page);
static void lvgl_view_config_destroy_empty(lv_obj_t *page);
static void lvgl_view_config_focus_get_empty(lv_obj_t *page);
static void lvgl_view_config_focus_lost_empty(lv_obj_t *page);

/*****************************************************************************************/
/* 默认配置信息部分--------------------------------------------------------------------- */
/*****************************************************************************************/
static const lvgl_view_config_t lvgl_view_config_list_def[lvgl_view_id_num] = {
    [lvgl_view_id_home_watch] = {
    },
};
/*****************************************************************************************/
/* 默认配置信息部分--------------------------------------------------------------------- */
/*****************************************************************************************/
static lvgl_view_config_t *lvgl_view_config_list = NULL;

void lvgl_view_id_ready(void)
{
    size_t size = sizeof(lvgl_view_config_t) * (lvgl_view_id_num + 1);
    
    // 内存常驻表
    lvgl_view_config_list = lv_mem_alloc(size);
    lv_memcpy(lvgl_view_config_list, lvgl_view_config_list_def, size);
}

void lvgl_view_id_adjust(uint32_t type, lvgl_view_config_t *config)
{
    if (type > lvgl_view_id_unknown &&
        type < lvgl_view_id_num) {
        
        lvgl_view_config_list[type].create = config->create;
        lvgl_view_config_list[type].page_l = config->page_l;
        lvgl_view_config_list[type].page_r = config->page_r;
        lvgl_view_config_list[type].page_t = config->page_t;
        lvgl_view_config_list[type].page_b = config->page_b;
    }
}

lvgl_view_config_t *lvgl_view_id_inst(uint32_t type)
{
    LV_ASSERT(type > lvgl_view_id_unknown);
    LV_ASSERT(type < lvgl_view_id_num);
    
    // 多检查检查
    LV_ASSERT(lvgl_view_config_list[type].create != NULL);
    return &lvgl_view_config_list[type];
}

/*****************************************************************************************/
static void lvgl_view_config_create_empty(lv_obj_t *page)
{
}
static void lvgl_view_config_destroy_empty(lv_obj_t *page)
{
}
static void lvgl_view_config_focus_get_empty(lv_obj_t *page)
{
}
static void lvgl_view_config_focus_lost_empty(lv_obj_t *page)
{
}
