
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

typedef struct {
    lv_anim_t  anim;
    lv_obj_t  *scene;
    lv_obj_t  *time;
    lv_obj_t  *lbl_matrix1[APP_LV_UI_MAZE_LINE][APP_LV_UI_MAZE_ELEMENT];
    lv_obj_t  *lbl_matrix2[APP_LV_UI_MAZE_LINE][APP_LV_UI_MAZE_ELEMENT];
    lv_coord_t lbl_matrix_row_dsc[APP_LV_UI_MAZE_LINE + 1];
    lv_coord_t lbl_matrix_col_dsc[APP_LV_UI_MAZE_ELEMENT + 1];
    bool       refr;    /* 迷宫本体最开始刷一次即可 */
    bool       move;    /* 产生动作才刷新像素块 */
    bool       run;
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

/*@brief 界面默认事件响应回调(本地重定向)
 */
static void app_lv_event_ui_default_redirect(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    /* 手势事件重定向为指定动作 */
    if (code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        APP_SYS_LOG_INFO("LV_EVENT_GESTURE:%x", dir);
        /* 结束了重置 */
        if (!app_lv_ui_res_local->run) {
             app_lv_ui_maze_presenter.clean();
             app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.ready();
             app_lv_ui_res_local->refr = true;
             app_lv_ui_res_local->move = true;
             return;
        }
        switch (dir) {
        case LV_DIR_LEFT:
            app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.execute(LV_DIR_LEFT);
            app_lv_ui_res_local->move = true;
            break;
        case LV_DIR_RIGHT:
            app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.execute(LV_DIR_RIGHT);
            app_lv_ui_res_local->move = true;
            break;
        case LV_DIR_TOP:
            app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.execute(LV_DIR_TOP);
            app_lv_ui_res_local->move = true;
            break;
        case LV_DIR_BOTTOM:
            app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.execute(LV_DIR_BOTTOM);
            app_lv_ui_res_local->move = true;
            break;
        default:
            break;
        }
        return;
    }
    /* 其他事件不做重定向 */
    app_lv_event_ui_default_cb(e);
}

/*@brief 界面自定义事件回调
*/
static void app_lv_ui_local_list_btn_cb(lv_event_t *e)
{
    /* 结束了重置 */
    if (!app_lv_ui_res_local->run) {
         app_lv_ui_maze_presenter.clean();
         app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.ready();
         app_lv_ui_res_local->refr = true;
         return;
    }
    uintptr_t btn_idx = (uintptr_t)lv_event_get_user_data(e);
    switch (btn_idx)
    {
    case (uintptr_t)(-1): {
        /* 重新就绪检查 */
        app_lv_ui_maze_presenter.clean();
        app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.ready();
        app_lv_ui_res_local->refr = true;
        app_lv_ui_res_local->move = true;
        break;
    }
    case (uintptr_t)(-2): {
        app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.execute(LV_DIR_TOP);
        app_lv_ui_res_local->move = true;
        break;
    }
    case (uintptr_t)(-3): {
        app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.execute(LV_DIR_LEFT);
        app_lv_ui_res_local->move = true;
        break;
    }
    case (uintptr_t)(-4): {
        app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.execute(LV_DIR_RIGHT);
        app_lv_ui_res_local->move = true;
        break;
    }
    case (uintptr_t)(-5): {
        app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.execute(LV_DIR_BOTTOM);
        app_lv_ui_res_local->move = true;
        break;
    }
    default:
        break;
    }
}

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    /* 左上角系统时钟更新 */
    char format_clock_1[20] = {0};
    app_lv_ui_clock_presenter.format_clock_1(format_clock_1);
    lv_label_set_text(app_lv_ui_res_local->time, format_clock_1);
    /* 数据走色 */
    lv_color_t color_idx[] = {
        lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_GREEN),
        lv_palette_main(LV_PALETTE_RED),
        lv_palette_main(LV_PALETTE_ORANGE),
        lv_palette_main(LV_PALETTE_PINK),
        lv_palette_main(LV_PALETTE_PURPLE),
    };
    /* 矩阵刷新 */
    app_lv_ui_maze_block_t (*matrix)[APP_LV_UI_MAZE_LINE][APP_LV_UI_MAZE_ELEMENT] = NULL;
    app_lv_ui_maze_presenter.get_matrix((void **)&matrix);
    /* 迷宫本体只需要刷新一次 */
    if (app_lv_ui_res_local->refr) {
        app_lv_ui_res_local->refr = false;
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_MAZE_LINE; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_MAZE_ELEMENT; idx2++) {
        lv_obj_set_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx1][idx2], LV_BORDER_SIDE_NONE, 0);
        lv_obj_set_style_border_color(app_lv_ui_res_local->lbl_matrix1[idx1][idx2], lv_color_white(), 0);
        if (idx1 == 0)
            lv_obj_set_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx1][idx2],
            lv_obj_get_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx1][idx2], 0) | LV_BORDER_SIDE_LEFT, 0);
        if (idx2 == 0)
            lv_obj_set_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx1][idx2],
            lv_obj_get_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx1][idx2], 0) | LV_BORDER_SIDE_TOP, 0);
        if (idx1 == APP_LV_UI_MAZE_LINE - 1)
            lv_obj_set_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx1][idx2],
            lv_obj_get_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx1][idx2], 0) | LV_BORDER_SIDE_RIGHT, 0);
        if (idx2 == APP_LV_UI_MAZE_ELEMENT - 1)
            lv_obj_set_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx1][idx2],
            lv_obj_get_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx1][idx2], 0) | LV_BORDER_SIDE_BOTTOM, 0);
    }
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_MAZE_LINE; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_MAZE_ELEMENT; idx2++) {
        if ((*matrix)[idx1][idx2].h_status != 0) {
            lv_obj_set_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx2][idx1], 
            lv_obj_get_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx2][idx1], 0) | LV_BORDER_SIDE_BOTTOM, 0);
            if (idx1 != APP_LV_UI_MAZE_LINE - 1) {
            lv_obj_set_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx2][idx1 + 1], 
            lv_obj_get_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx2][idx1 + 1], 0) | LV_BORDER_SIDE_TOP, 0);
            }
        }
        if ((*matrix)[idx1][idx2].v_status != 0) {
            lv_obj_set_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx2][idx1], 
            lv_obj_get_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx2][idx1], 0) | LV_BORDER_SIDE_RIGHT, 0);
            if (idx2 != APP_LV_UI_MAZE_ELEMENT - 1) {
            lv_obj_set_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx2 + 1][idx1], 
            lv_obj_get_style_border_side(app_lv_ui_res_local->lbl_matrix1[idx2 + 1][idx1], 0) | LV_BORDER_SIDE_LEFT, 0);
            }
        }
    }
    }
    /* 迷宫本体只需要刷新一次 */
    if (app_lv_ui_res_local->move) {
        app_lv_ui_res_local->move = false;
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_MAZE_LINE; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_MAZE_ELEMENT; idx2++)
        if ((*matrix)[idx1][idx2].record)
            lv_obj_set_style_bg_color(app_lv_ui_res_local->lbl_matrix2[idx2][idx1], color_idx[(*matrix)[idx1][idx2].record + 1], 0);
        else
            lv_obj_set_style_bg_color(app_lv_ui_res_local->lbl_matrix2[idx2][idx1], lv_color_black(), 0);
    }
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_maze_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        /* 禁用默认事件响应,事件重定向使用 */
        app_lv_event_ui_default_config(NULL, false, NULL);
        app_lv_event_ui_default_config(NULL, true,  app_lv_event_ui_default_redirect);
        /* 界面常亮 */
        app_lv_check_time_exec(false);
        /* 默认顶部风格 */
        lv_obj_t *title_box = NULL, *title = NULL;
        title_box = app_lv_style_title(app_lv_ui_res_local->scene, &app_lv_ui_res_local->time, &title);
        lv_label_set_text(title, "Maze");
        /* 初始化参数 */
        app_lv_ui_res_local->lbl_matrix_row_dsc[APP_LV_UI_MAZE_LINE]    = LV_GRID_TEMPLATE_LAST;
        app_lv_ui_res_local->lbl_matrix_col_dsc[APP_LV_UI_MAZE_ELEMENT] = LV_GRID_TEMPLATE_LAST;
        for (uint32_t idx = 0; idx < APP_LV_UI_MAZE_LINE;     app_lv_ui_res_local->lbl_matrix_row_dsc[idx] = app_lv_style_hor_pct(5), idx++);
        for (uint32_t idx = 0; idx < APP_LV_UI_MAZE_ELEMENT;  app_lv_ui_res_local->lbl_matrix_col_dsc[idx] = app_lv_style_ver_pct(5), idx++);
        /* 创建显示矩阵 */
        lv_obj_t *matrix = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(matrix);
        lv_obj_set_size(matrix, app_lv_style_hor_pct(85), app_lv_style_ver_pct(70));
        lv_obj_set_style_layout(matrix, LV_LAYOUT_GRID, 0);
        lv_obj_set_style_pad_row(matrix, 0, 0);
        lv_obj_set_style_pad_column(matrix, 0, 0);
        lv_obj_set_style_grid_row_dsc_array(matrix, app_lv_ui_res_local->lbl_matrix_row_dsc, 0);
        lv_obj_set_style_grid_column_dsc_array(matrix, app_lv_ui_res_local->lbl_matrix_col_dsc, 0);
        lv_obj_align_to(matrix, title_box, LV_ALIGN_OUT_BOTTOM_MID, 0, app_lv_style_ver_pct(1));
        for (uint8_t idx1 = 0; idx1 < APP_LV_UI_MAZE_LINE; idx1++)
        for (uint8_t idx2 = 0; idx2 < APP_LV_UI_MAZE_ELEMENT; idx2++) {
            app_lv_ui_res_local->lbl_matrix2[idx1][idx2] = app_lv_style_btn_block2(matrix, &app_lv_ui_res_local->lbl_matrix1[idx1][idx2], 2, 1, 0);
            lv_obj_set_grid_cell(app_lv_ui_res_local->lbl_matrix1[idx1][idx2], LV_GRID_ALIGN_STRETCH, idx1, 1, LV_GRID_ALIGN_STRETCH, idx2, 1);
        }
        /* 初始化列表,右部 */
        lv_obj_t *list  = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(list);
        lv_obj_set_size(list, LV_HOR_RES, app_lv_style_ver_pct(15));
        lv_obj_set_style_pad_all(list, 10, 0);
        lv_obj_set_style_layout(list, LV_LAYOUT_FLEX, 0);
        lv_obj_set_style_flex_flow(list, LV_FLEX_FLOW_ROW_WRAP, 0);
        lv_obj_set_style_flex_main_place(list, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
        lv_obj_set_style_pad_column(list, app_lv_style_hor_pct(1), 0);
        /* 固定按钮 */
        const char *btn_text[] = {LV_SYMBOL_REFRESH, LV_SYMBOL_UP, LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, LV_SYMBOL_DOWN};
        for (uint8_t idx = 0; idx < sizeof(btn_text) / sizeof(btn_text[0]); idx++) {
            lv_obj_t *list_btn = app_lv_style_btn(list);
            lv_obj_add_event_cb(list_btn, app_lv_ui_local_list_btn_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)(0 - (idx + 1)));
            lv_obj_set_size(list_btn, app_lv_style_hor_pct(15), app_lv_style_ver_pct(8));
            lv_obj_t *list_lbl = app_lv_style_label(list_btn);
            lv_obj_align(list_lbl, LV_ALIGN_CENTER, 0, 0);
            lv_label_set_text_static(list_lbl, btn_text[idx]);
        }
        /* 更新列表布局 */
        lv_obj_align_to(list, matrix, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 重新就绪检查 */
        app_lv_ui_res_local->run  = app_lv_ui_maze_presenter.ready();
        app_lv_ui_res_local->refr = true;
        app_lv_ui_res_local->move = true;
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 10, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_maze_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化显示动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 启用默认事件响应,事件重定向取消 */
        app_lv_event_ui_default_config(NULL, false, app_lv_event_ui_default_redirect);
        app_lv_event_ui_default_config(NULL, true,  NULL);
        /* 界面恢复 */
        app_lv_check_time_reset(0, 0);
        app_lv_check_time_exec(true);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        ((app_lv_scene_t *)scene)->root = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_maze = {
    /* 场景资源节点 */
    .show = app_lv_ui_maze_show,
    .hide = app_lv_ui_maze_hide,
};
