/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*****************************************************************************/
/* 应用列表,设置列表等使用 */
scui_handle_t  scui_ui_scene_list_num   = 0;
scui_handle_t *scui_ui_scene_list_image = NULL;
scui_handle_t *scui_ui_scene_list_text  = NULL;
scui_handle_t *scui_ui_scene_list_jump  = NULL;

void scui_ui_scene_list_cfg(uint8_t type)
{
    switch (type) {
    case 0x00: {
        
        static scui_handle_t list_image[] = {
            scui_image_prj_image_src_00_theme_icon_01_spo2_03_02png,
            scui_image_prj_image_src_00_theme_icon_00_heart_03_02png,
            scui_image_prj_image_src_00_theme_icon_00_heart_03_02png,
            scui_image_prj_image_src_00_theme_icon_00_heart_03_02png,
            scui_image_prj_image_src_00_theme_icon_00_heart_03_02png,
            scui_image_prj_image_src_00_theme_icon_00_heart_03_02png,
            scui_image_prj_image_src_00_theme_icon_00_heart_03_02png,
            scui_image_prj_image_src_00_theme_icon_00_heart_03_02png,
            scui_image_prj_image_src_00_theme_icon_00_heart_03_02png,
            scui_image_prj_image_src_00_theme_icon_00_heart_03_02png,
            scui_image_prj_image_src_00_theme_icon_01_spo2_03_02png,
        };
        static scui_handle_t list_text[] = {
            SCUI_MULTI_LANG_0X002f,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X0030,
            SCUI_MULTI_LANG_0X002f,
        };
        static scui_handle_t list_jump[] = {
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
            SCUI_UI_SCENE_6,
        };
        scui_ui_scene_list_num   = scui_arr_len(list_image);
        scui_ui_scene_list_image = list_image;
        scui_ui_scene_list_text  = list_text;
        scui_ui_scene_list_jump  = list_jump;
        SCUI_ASSERT(scui_arr_len(list_image) == scui_arr_len(list_text));
        SCUI_ASSERT(scui_arr_len(list_image) == scui_arr_len(list_jump));
        break;
    }
    default:
        SCUI_ASSERT(false);
        break;
    }
}

/*****************************************************************************/
/*@brief 窗口关联配置
 *@param event 事件
 */
void scui_ui_scene_link_cfg(scui_event_t *event)
{
    // 仅焦点切换时才可进行
    if (event->type != scui_event_focus_get ||
       !scui_widget_event_check_prepare(event))
        return;
    
    /* 窗口属性参数配置(浮动窗口) */
    scui_window_float_t float_cfg = {0};
    scui_window_float_cfg_get(&float_cfg);
    float_cfg.main = event->object;
    float_cfg.list[0] = SCUI_HANDLE_INVALID;
    float_cfg.list[1] = SCUI_HANDLE_INVALID;
    float_cfg.list[2] = SCUI_HANDLE_INVALID;
    float_cfg.list[3] = SCUI_HANDLE_INVALID;
    
    switch (event->object) {
    case SCUI_UI_SCENE_HOME:
        float_cfg.list[0] = SCUI_UI_SCENE_FLOAT_1;
        float_cfg.list[1] = SCUI_UI_SCENE_FLOAT_2;
        float_cfg.list[2] = SCUI_UI_SCENE_FLOAT_3;
        float_cfg.list[3] = SCUI_UI_SCENE_FLOAT_4;
        break;
    default:
        break;
    }
    scui_window_float_cfg_set(&float_cfg);
    
    /* 窗口属性参数配置(场景管理) */
    scui_window_cfg_t window_cfg = {0};
    scui_window_cfg_get(event->object, &window_cfg);
    window_cfg.sibling[0] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[1] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[2] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[3] = SCUI_HANDLE_INVALID;
    
    switch (event->object) {
    case SCUI_UI_SCENE_HOME:
        window_cfg.sibling[2] = SCUI_UI_SCENE_6;
        window_cfg.sibling[3] = SCUI_UI_SCENE_1;
        break;
    case SCUI_UI_SCENE_1:
        window_cfg.sibling[0] = SCUI_UI_SCENE_TEST;
        window_cfg.sibling[1] = SCUI_UI_SCENE_CUBE;
        window_cfg.sibling[2] = SCUI_UI_SCENE_HOME;
        window_cfg.sibling[3] = SCUI_UI_SCENE_2;
        break;
    case SCUI_UI_SCENE_2:
        window_cfg.sibling[0] = SCUI_UI_SCENE_LANTERN;
        window_cfg.sibling[1] = SCUI_UI_SCENE_SOCCER;
        window_cfg.sibling[2] = SCUI_UI_SCENE_1;
        window_cfg.sibling[3] = SCUI_UI_SCENE_3;
        break;
    case SCUI_UI_SCENE_3:
        window_cfg.sibling[0] = SCUI_UI_SCENE_BUTTERFLY;
        window_cfg.sibling[2] = SCUI_UI_SCENE_2;
        window_cfg.sibling[3] = SCUI_UI_SCENE_4;
        break;
    case SCUI_UI_SCENE_4:
        window_cfg.sibling[2] = SCUI_UI_SCENE_3;
        window_cfg.sibling[3] = SCUI_UI_SCENE_5;
        break;
    case SCUI_UI_SCENE_5:
        window_cfg.sibling[2] = SCUI_UI_SCENE_4;
        window_cfg.sibling[3] = SCUI_UI_SCENE_6;
        break;
    case SCUI_UI_SCENE_6:
        window_cfg.sibling[2] = SCUI_UI_SCENE_5;
        window_cfg.sibling[3] = SCUI_UI_SCENE_HOME;
        break;
    
    case SCUI_UI_SCENE_TEST:
        window_cfg.sibling[1] = SCUI_UI_SCENE_1;
        break;
    case SCUI_UI_SCENE_CUBE:
        window_cfg.sibling[0] = SCUI_UI_SCENE_1;
        break;
    
    case SCUI_UI_SCENE_LANTERN:
        window_cfg.sibling[1] = SCUI_UI_SCENE_2;
        break;
    case SCUI_UI_SCENE_SOCCER:
        window_cfg.sibling[0] = SCUI_UI_SCENE_2;
        break;
    
    case SCUI_UI_SCENE_BUTTERFLY:
        window_cfg.sibling[1] = SCUI_UI_SCENE_3;
        break;
    default:
        break;
    }
    scui_window_cfg_set(event->object, &window_cfg);
}

/*****************************************************************************/
