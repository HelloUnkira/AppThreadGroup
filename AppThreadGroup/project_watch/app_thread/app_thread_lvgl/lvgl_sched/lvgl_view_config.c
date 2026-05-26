#include "lvgl_lib.h"

/*****************************************************************************************/
/* 默认配置信息部分--------------------------------------------------------------------- */
/*****************************************************************************************/
const lvgl_view_config_t lvgl_view_config_list_def[lvgl_view_id_num] =
{
    [lvgl_view_id_standby] = {
        .create         = lvgl_ui_standby_create,
        .destroy        = lvgl_ui_standby_destroy,
        .focus_get      = lvgl_ui_standby_focus_get,
        .focus_lost     = lvgl_ui_standby_focus_lost,
    },
    [lvgl_view_id_home_watch] = {
        .create         = lvgl_ui_home_watch_create,
        .destroy        = lvgl_ui_home_watch_destroy,
        .focus_get      = lvgl_ui_home_watch_focus_get,
        .focus_lost     = lvgl_ui_home_watch_focus_lost,
        
        .page_l = lvgl_view_id_nega,
        .page_r = lvgl_view_id_card1,
        
        .page_t = lvgl_view_id_home_panel_t,
        .page_b = lvgl_view_id_home_panel_b,
        .transform_t = lvgl_view_tr_cover_in,
        .transform_b = lvgl_view_tr_cover_in,
        .enc_t = true,
        .enc_b = true,
    },
    [lvgl_view_id_home_panel_t] = {
        .create         = lvgl_ui_home_panel_t_create,
        .destroy        = lvgl_ui_home_panel_t_destroy,
        .focus_get      = lvgl_ui_home_panel_t_focus_get,
        .focus_lost     = lvgl_ui_home_panel_t_focus_lost,
        
        .page_b = lvgl_view_id_home_watch,
        .transform_b = lvgl_view_tr_cover_out,
        .enc_b = true,
    },
    [lvgl_view_id_home_panel_b] = {
        .create         = lvgl_ui_home_panel_b_create,
        .destroy        = lvgl_ui_home_panel_b_destroy,
        .focus_get      = lvgl_ui_home_panel_b_focus_get,
        .focus_lost     = lvgl_ui_home_panel_b_focus_lost,
        
        .page_t = lvgl_view_id_home_watch,
        .transform_t = lvgl_view_tr_cover_out,
        .enc_t = true,
    },
    [lvgl_view_id_nega] = {
        .create         = lvgl_ui_nega_create,
        .destroy        = lvgl_ui_nega_destroy,
        .focus_get      = lvgl_ui_nega_focus_get,
        .focus_lost     = lvgl_ui_nega_create,
        
        .page_r = lvgl_view_id_home_watch,
    },
    [lvgl_view_id_card1] = {
        .create         = lvgl_ui_card1_create,
        .destroy        = lvgl_ui_card1_destroy,
        .focus_get      = lvgl_ui_card1_focus_get,
        .focus_lost     = lvgl_ui_card1_create,
        
        .page_l = lvgl_view_id_home_watch,
        .page_r = lvgl_view_id_card2,
    },
    [lvgl_view_id_card2] = {
        .create         = lvgl_ui_card2_create,
        .destroy        = lvgl_ui_card2_destroy,
        .focus_get      = lvgl_ui_card2_focus_get,
        .focus_lost     = lvgl_ui_card2_create,
        
        .page_l = lvgl_view_id_card1,
        .page_r = lvgl_view_id_nega,
    },
};
/*****************************************************************************************/
/* 默认配置信息部分--------------------------------------------------------------------- */
/*****************************************************************************************/

