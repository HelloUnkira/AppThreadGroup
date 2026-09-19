/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2    /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 全局主题列表: image/text/jump 同序合一, 所有 theme 共用 */
static const scui_ui_scene_list_item_t list[] = {
    { scui_image_prj_theme_menu_ai,              SCUI_LANG_IDX_0X0009,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_alipay,          SCUI_LANG_IDX_0X010b,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_baidu,           SCUI_LANG_IDX_0X0030,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_book,            SCUI_LANG_IDX_0X0030,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_bp,              SCUI_LANG_IDX_0X0040,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_brte,            SCUI_LANG_IDX_0X00fe,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_calc,            SCUI_LANG_IDX_0X0138,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_calendar,        SCUI_LANG_IDX_0X0181,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_camera,          SCUI_LANG_IDX_0X0136,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_card,            SCUI_LANG_IDX_0X0030,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_clock,           SCUI_LANG_IDX_0X0181,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_cmps,            SCUI_LANG_IDX_0X011c,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_contacts,        SCUI_LANG_IDX_0X008b,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_data,            SCUI_LANG_IDX_0X0030,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_game,            SCUI_LANG_IDX_0X0030,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_hr,              SCUI_LANG_IDX_0X0040,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_lefun,           SCUI_LANG_IDX_0X0030,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_lefunAI,         SCUI_LANG_IDX_0X0030,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_msg,             SCUI_LANG_IDX_0X00c9,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_music,           SCUI_LANG_IDX_0X010b,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_pwd,             SCUI_LANG_IDX_0X0185,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_phone,           SCUI_LANG_IDX_0X008b,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_photo,           SCUI_LANG_IDX_0X0136,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_physiology,      SCUI_LANG_IDX_0X0040,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_pres,            SCUI_LANG_IDX_0X0030,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_recording,       SCUI_LANG_IDX_0X0030,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_restart,         SCUI_LANG_IDX_0X019b,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_restore,         SCUI_LANG_IDX_0X019b,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_set,             SCUI_LANG_IDX_0X0151,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_Shutdown,        SCUI_LANG_IDX_0X019b,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_sleep,           SCUI_LANG_IDX_0X004f,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_Somatosensory,   SCUI_LANG_IDX_0X0030,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_spo2,            SCUI_LANG_IDX_0X0040,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_sport,           SCUI_LANG_IDX_0X01c7,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_sport_rcd,       SCUI_LANG_IDX_0X01c5,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_sw,              SCUI_LANG_IDX_0X013f,      SCUI_UI_SCENE_NONE },
    { scui_image_prj_theme_menu_stylus,          SCUI_LANG_IDX_0X0030,      SCUI_UI_SCENE_NONE },
};

scui_handle_t  scui_ui_scene_list_num = 0;
const scui_ui_scene_list_item_t *scui_ui_scene_list = NULL;

void scui_ui_scene_list_cfg(scui_ui_scene_list_type_t type)
{
    (void)type;
    scui_ui_scene_list_num = scui_arr_len(list);
    scui_ui_scene_list     = list;
}


/*****************************************************************************/
scui_handle_t scui_ui_scene_mini_card_num = 0;
scui_handle_t scui_ui_scene_mini_card_type[scui_ui_scene_mini_card_type_num] = {0};
scui_handle_t scui_ui_scene_mini_card_jump[scui_ui_scene_mini_card_type_num] = {0};

void scui_ui_scene_mini_card_cfg(void)
{
    // type list default:
    for (uint32_t idx = 0; idx < scui_ui_scene_mini_card_type_num;idx++) {
         scui_ui_scene_mini_card_type[idx] = idx;
         scui_ui_scene_mini_card_jump[idx] = SCUI_UI_SCENE_NONE;
    }
    scui_ui_scene_mini_card_num = scui_ui_scene_mini_card_type_num;
    
    // 从 scui_presenter的接口中读取系统保存的mini card配置项
    // ......
}

/*****************************************************************************/
bool scui_ui_scene_test(void)
{
    scui_handle_t stack_top  = 0;
    scui_window_stack_top(&stack_top);
    
    static scui_handle_t test_ui_list[] = {
        SCUI_UI_SCENE_TEST_UI_GRAPH,
        SCUI_UI_SCENE_TEST_UI_INDEV_BAR,
        SCUI_UI_SCENE_TEST_UI_INDEV_ENC,
        SCUI_UI_SCENE_TEST_UI_INDEV_KEY,
        SCUI_UI_SCENE_TEST_UI_INDEV_PTR,
        SCUI_UI_SCENE_TEST_UI_LAYOUT,
        SCUI_UI_SCENE_TEST_UI_LIST,
        SCUI_UI_SCENE_TEST_UI_MAIN,
        SCUI_UI_SCENE_TEST_UI_MISC,
        SCUI_UI_SCENE_TEST_UI_OBJECT,
        SCUI_UI_SCENE_TEST_UI_RING,
        SCUI_UI_SCENE_TEST_UI_ROLLER,
        SCUI_UI_SCENE_TEST_UI_SCROLL,
        SCUI_UI_SCENE_TEST_UI_STRING,
        SCUI_UI_SCENE_TEST_UI_SYMBOL,
        SCUI_UI_SCENE_TEST_UI_XIMAGE,
    };
    
    for (scui_handle_t idx = 0; idx < scui_arr_len(test_ui_list); idx++)
        if (stack_top == test_ui_list[idx]) return true;
    
    return false;
}

/*****************************************************************************/
void scui_ui_scene_return(void)
{
    scui_handle_t stack_nest = 0;
    scui_handle_t stack_top  = 0;
    scui_window_stack_nest(&stack_nest);
    scui_window_stack_top(&stack_top);
    
    if (stack_nest > 1) {
        // 如果是测试界面内的
        if (scui_ui_scene_test()) {
            scui_window_stack_del_by(SCUI_HANDLE_INVALID,
                scui_window_switch_none, scui_opt_dir_none);
            return;
        }
        
        // 返回上一层
        scui_window_stack_del(SCUI_HANDLE_INVALID);
        return;
    } else {
        // 如果是测试界面内的
        if (scui_ui_scene_test()) {
            scui_window_stack_reset_by(SCUI_UI_SCENE_TEST_UI_MAIN,
                scui_window_switch_none, scui_opt_dir_none, false);
            return;
        }
        
        // 回到主界面
        scui_window_stack_reset(SCUI_UI_SCENE_HOME, false);
        return;
    }
}
