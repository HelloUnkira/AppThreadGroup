
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_check_time.h"
#include "app_lv_style.h"
#include "app_lv_event_ui.h"
#include "app_lv_ui_scene.h"

#include "app_lv_ui_tetris.h"
#include "app_lv_ui_tetris_presenter.h"

#include "app_lv_ui_clock_presenter.h"

typedef struct {
    lv_anim_t  anim;
    lv_obj_t  *scene;
    lv_obj_t  *time;
    lv_anim_t  erase_anim;
    lv_obj_t  *erase_line;
    double     erase_value;
    lv_obj_t  *lbl_matrix[APP_LV_UI_TETRIS_LINE][APP_LV_UI_TETRIS_ELEMENT];
    lv_coord_t lbl_matrix_row_dsc[APP_LV_UI_TETRIS_LINE + 1];
    lv_coord_t lbl_matrix_col_dsc[APP_LV_UI_TETRIS_ELEMENT + 1];
    lv_obj_t  *lbl_graph[APP_LV_UI_TETRIS_SCALE][APP_LV_UI_TETRIS_SCALE];
    lv_coord_t lbl_graph_row_dsc[APP_LV_UI_TETRIS_SCALE + 1];
    lv_coord_t lbl_graph_col_dsc[APP_LV_UI_TETRIS_SCALE + 1];
    uint64_t   speed:8;
    uint64_t   count:8;
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
        
        switch (dir) {
        case LV_DIR_LEFT:
            app_lv_ui_tetris_presenter.move_left();
            break;
        case LV_DIR_RIGHT:
            app_lv_ui_tetris_presenter.move_right();
            break;
        case LV_DIR_TOP:
            app_lv_ui_tetris_presenter.rotate_left();
            break;
        case LV_DIR_BOTTOM:
            app_lv_ui_tetris_presenter.rotate_right();
            break;
        default:
            break;
        }
        return;
    }
    /* 其他事件不做重定向 */
    app_lv_event_ui_default(e);
}

/*@brief 界面自定义事件回调
*/
static void app_lv_ui_local_list_btn_cb(lv_event_t *e)
{
    uintptr_t btn_idx = (uintptr_t)lv_event_get_user_data(e);
    switch (btn_idx)
    {
    case (uintptr_t)(-1): {
        /* 重新就绪检查 */
        app_lv_ui_tetris_presenter.clean();
        app_lv_ui_tetris_presenter.ready();
        break;
    }
    case (uintptr_t)(-2): {
        app_lv_ui_tetris_presenter.move_left();
        break;
    }
    case (uintptr_t)(-3): {
        app_lv_ui_tetris_presenter.move_right();
        break;
    }
    case (uintptr_t)(-4): {
        app_lv_ui_tetris_presenter.execute();
        break;
    }
    case (uintptr_t)(-5): {
        app_lv_ui_tetris_presenter.rotate_left();
        break;
    }
    case (uintptr_t)(-6): {
        app_lv_ui_tetris_presenter.rotate_right();
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
        lv_palette_main(LV_PALETTE_GREY),
        lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_GREEN),
        lv_palette_main(LV_PALETTE_RED),
        // lv_palette_main(LV_PALETTE_YELLOW),
        lv_palette_main(LV_PALETTE_ORANGE),
        lv_palette_main(LV_PALETTE_PINK),
        lv_palette_main(LV_PALETTE_PURPLE),
        lv_palette_main(LV_PALETTE_DEEP_PURPLE),
        lv_palette_main(LV_PALETTE_BROWN),
        lv_palette_main(LV_PALETTE_AMBER),
        lv_palette_main(LV_PALETTE_CYAN),
        lv_palette_main(LV_PALETTE_INDIGO),
        lv_palette_main(LV_PALETTE_TEAL),
        lv_palette_main(LV_PALETTE_LIME),
    };
    /* 暂停 */
    bool run_status = false;
    app_lv_ui_tetris_presenter.get_status(&run_status);
    if (!run_status)
        return;
    /* 矩阵刷新 */
    uint32_t scale = 0, off_x = 0, off_y = 0;
    app_lv_ui_tetris_block_t (*matrix)[APP_LV_UI_TETRIS_LINE][APP_LV_UI_TETRIS_ELEMENT] = NULL;
    app_lv_ui_tetris_block_t (*graph)[APP_LV_UI_TETRIS_SCALE][APP_LV_UI_TETRIS_SCALE] = NULL;
    app_lv_ui_tetris_presenter.get_matrix((void **)&matrix);
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_TETRIS_LINE; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_TETRIS_ELEMENT; idx2++)
        if ((*matrix)[idx1][idx2].status)
            lv_obj_set_style_bg_color(app_lv_ui_res_local->lbl_matrix[idx2][idx1], color_idx[(*matrix)[idx1][idx2].record + 1], 0);
        else
            lv_obj_set_style_bg_color(app_lv_ui_res_local->lbl_matrix[idx2][idx1], color_idx[0], 0);
    /* 当前图元刷新 */
    app_lv_ui_tetris_presenter.get_graph((void **)&graph, &scale, &off_x, &off_y);
    for (uint8_t idx1 = 0; idx1 < scale; idx1++)
    for (uint8_t idx2 = 0; idx2 < scale; idx2++)
        if ((*graph)[idx1][idx2].status)
            lv_obj_set_style_bg_color(app_lv_ui_res_local->lbl_matrix[idx2 + off_x][idx1 + off_y], color_idx[(*graph)[idx1][idx2].record + 1], 0);
    /* 下一图元刷新 */
    app_lv_ui_tetris_presenter.get_graph_next((void **)&graph, &scale);
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_TETRIS_SCALE; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_TETRIS_SCALE; idx2++)
        lv_obj_set_style_bg_color(app_lv_ui_res_local->lbl_graph[idx2][idx1], color_idx[0], 0);
    for (uint8_t idx1 = 0; idx1 < scale; idx1++)
    for (uint8_t idx2 = 0; idx2 < scale; idx2++)
        if ((*graph)[idx1][idx2].status)
            lv_obj_set_style_bg_color(app_lv_ui_res_local->lbl_graph[idx2][idx1], color_idx[(*graph)[idx1][idx2].record + 1], 0);
    /* 进行消去动画 */
    bool    erase_line_val = false;
    uint8_t erase_line_number = 0;
    /* 检查矩阵有几个消去行 */
    for (int16_t idx1 = APP_LV_UI_TETRIS_LINE - 1; idx1 >= 0; idx1--) {
        erase_line_val = true;
    for (int16_t idx2 = APP_LV_UI_TETRIS_ELEMENT - 1; idx2 >= 0; idx2--)
        if (!(*matrix)[idx1][idx2].status) {
            erase_line_val = false;
            break;
        }
        if (erase_line_val)
            erase_line_number++;
    }
    if (erase_line_number == 0)
        app_lv_ui_res_local->erase_value = 0.0;
    if (erase_line_number != 0) {
        /* 第一次满足消去特效时重置计数器以同步 */
        if (app_lv_ui_res_local->erase_value >= -0.001 &&
            app_lv_ui_res_local->erase_value <= +0.001)
            app_lv_ui_res_local->count = 0;
        /* 满足消去行,实现淡出动画 */
        bool    line_val = true;
        uint8_t line_idx = APP_LV_UI_TETRIS_LINE - 1;
        app_lv_ui_res_local->erase_value += 100.0 / (float)app_lv_ui_res_local->speed;
        for (uint8_t line = 0; line < erase_line_number; line++) {
            /* 查找可消去的行 */
            for (int16_t idx1 = line_idx; idx1 > 0; idx1--) {
                    line_val = true;
            for (int16_t idx2 = APP_LV_UI_TETRIS_ELEMENT - 1; idx2 > 0; idx2--)
                if (!(*matrix)[idx1][idx2].status) {
                    line_val = false;
                    break;
                }
                if (line_val) {
                    line_idx = idx1;
                    break;
                }
            }
            for (uint8_t idx2 = 0; idx2 < APP_LV_UI_TETRIS_ELEMENT; idx2++) {
                 uint8_t mix = (float)app_lv_ui_res_local->erase_value * 2.55;
                 lv_obj_t *obj = app_lv_ui_res_local->lbl_matrix[idx2][line_idx];
                 lv_color_t color = lv_color_mix(color_idx[0], lv_obj_get_style_bg_color(obj, 0), mix);
                 lv_obj_set_style_bg_color(app_lv_ui_res_local->lbl_matrix[idx2][line_idx], color, 0);
            }
            line_idx--;
        }
    }
    /* 递进到下一次 */
    if (app_lv_ui_res_local->count++ % app_lv_ui_res_local->speed == 0) {
        run_status = app_lv_ui_tetris_presenter.execute();
        app_lv_ui_tetris_presenter.set_status(&run_status);
    }
    /*  */
    uint32_t erase_line = 0;
    app_lv_ui_tetris_presenter.get_erase_line(&erase_line);
    lv_label_set_text_fmt(app_lv_ui_res_local->erase_line, "Erase Line:%d", erase_line);
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_tetris_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        /* 禁用默认事件响应,事件重定向使用 */
        app_lv_event_ui_default_config(NULL, false, NULL);
        app_lv_event_ui_default_config(NULL, true,  app_lv_event_ui_default_redirect);
        /* 界面常亮 */
        app_lv_check_time_exec(false);
        /* 默认顶部风格 */
        lv_obj_t *title_box = NULL, *title = NULL;
        title_box = app_lv_style_title(app_lv_ui_res_local->scene, &app_lv_ui_res_local->time, &title);
        lv_label_set_text(title, "Tetris");
        /* 创建第一行数据 */
        app_lv_ui_res_local->erase_line = app_lv_style_label_title(app_lv_ui_res_local->scene);
        lv_obj_set_size(app_lv_ui_res_local->erase_line, app_lv_style_hor_pct(50), app_lv_style_ver_pct(15));
        lv_obj_set_style_pad_top(app_lv_ui_res_local->erase_line, app_lv_style_ver_pct(5), 0);
        lv_obj_set_style_text_color(app_lv_ui_res_local->erase_line, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_set_style_text_align(app_lv_ui_res_local->erase_line, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align_to(app_lv_ui_res_local->erase_line, title_box, LV_ALIGN_OUT_BOTTOM_LEFT, app_lv_style_hor_pct(5), 0);
        lv_label_set_text(app_lv_ui_res_local->erase_line, "Erase Line:0");
        /* 初始化参数 */
        app_lv_ui_res_local->lbl_matrix_row_dsc[APP_LV_UI_TETRIS_LINE]    = LV_GRID_TEMPLATE_LAST;
        app_lv_ui_res_local->lbl_matrix_col_dsc[APP_LV_UI_TETRIS_ELEMENT] = LV_GRID_TEMPLATE_LAST;
        for (uint32_t idx = 0; idx < APP_LV_UI_TETRIS_LINE;     app_lv_ui_res_local->lbl_matrix_row_dsc[idx] = app_lv_style_hor_pct(5), idx++);
        for (uint32_t idx = 0; idx < APP_LV_UI_TETRIS_ELEMENT;  app_lv_ui_res_local->lbl_matrix_col_dsc[idx] = app_lv_style_ver_pct(5), idx++);
        app_lv_ui_res_local->lbl_graph_row_dsc[APP_LV_UI_TETRIS_SCALE]    = LV_GRID_TEMPLATE_LAST;
        app_lv_ui_res_local->lbl_graph_col_dsc[APP_LV_UI_TETRIS_SCALE] = LV_GRID_TEMPLATE_LAST;
        for (uint32_t idx = 0; idx < APP_LV_UI_TETRIS_SCALE; app_lv_ui_res_local->lbl_graph_row_dsc[idx] = app_lv_style_hor_pct(5), idx++);
        for (uint32_t idx = 0; idx < APP_LV_UI_TETRIS_SCALE; app_lv_ui_res_local->lbl_graph_col_dsc[idx] = app_lv_style_ver_pct(5), idx++);
        /* 创建显示矩阵 */
        lv_obj_t *matrix = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(matrix);
        lv_obj_set_size(matrix, app_lv_style_hor_pct(75), app_lv_style_ver_pct(75));
        lv_obj_set_style_layout(matrix, LV_LAYOUT_GRID, 0);
        lv_obj_set_style_pad_row(matrix, 0, 0);
        lv_obj_set_style_pad_column(matrix, 0, 0);
        lv_obj_set_style_grid_row_dsc_array(matrix, app_lv_ui_res_local->lbl_matrix_row_dsc, 0);
        lv_obj_set_style_grid_column_dsc_array(matrix, app_lv_ui_res_local->lbl_matrix_col_dsc, 0);
        lv_obj_align_to(matrix, app_lv_ui_res_local->erase_line, LV_ALIGN_OUT_BOTTOM_LEFT, 0, app_lv_style_ver_pct(10));
        for (uint8_t idx1 = 0; idx1 < APP_LV_UI_TETRIS_LINE; idx1++)
        for (uint8_t idx2 = 0; idx2 < APP_LV_UI_TETRIS_ELEMENT; idx2++) {
            app_lv_ui_res_local->lbl_matrix[idx1][idx2] = app_lv_style_btn_block(matrix, 1, 1, 1);
            lv_obj_set_grid_cell(app_lv_ui_res_local->lbl_matrix[idx1][idx2], LV_GRID_ALIGN_STRETCH, idx1, 1,
                                                                              LV_GRID_ALIGN_STRETCH, idx2, 1);
        }
        /* 创建显示矩阵 */
        lv_obj_t *graph = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(graph);
        lv_obj_set_size(graph, app_lv_style_hor_pct(25), app_lv_style_ver_pct(25));
        lv_obj_set_style_layout(graph, LV_LAYOUT_GRID, 0);
        lv_obj_set_style_pad_row(graph, 0, 0);
        lv_obj_set_style_pad_column(graph, 0, 0);
        lv_obj_set_style_grid_row_dsc_array(graph, app_lv_ui_res_local->lbl_graph_row_dsc, 0);
        lv_obj_set_style_grid_column_dsc_array(graph, app_lv_ui_res_local->lbl_graph_col_dsc, 0);
        lv_obj_align_to(graph, title_box, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
        for (uint8_t idx1 = 0; idx1 < APP_LV_UI_TETRIS_SCALE; idx1++)
        for (uint8_t idx2 = 0; idx2 < APP_LV_UI_TETRIS_SCALE; idx2++) {
            app_lv_ui_res_local->lbl_graph[idx1][idx2] = app_lv_style_label_block(graph, 1, 1, 1);
            lv_obj_set_grid_cell(app_lv_ui_res_local->lbl_graph[idx1][idx2], LV_GRID_ALIGN_STRETCH, idx1, 1,
                                                                             LV_GRID_ALIGN_STRETCH, idx2, 1);
        }
        /* 初始化列表,右部 */
        lv_obj_t *list = lv_list_create(app_lv_ui_res_local->scene);
        app_lv_style_object(list);
        lv_obj_set_size(list, app_lv_style_hor_pct(20), app_lv_style_ver_pct(60));
        lv_obj_set_style_pad_row(list, app_lv_style_ver_pct(1), 0);
        /* 固定按钮 */
        const char *btn_text[] = {LV_SYMBOL_REFRESH, LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, LV_SYMBOL_DOWN, LV_SYMBOL_MINUS, LV_SYMBOL_PLUS};
        for (uint8_t idx = 0; idx < sizeof(btn_text) / sizeof(btn_text[0]); idx++) {
            lv_obj_t *list_btn = app_lv_style_btn(list);
            lv_obj_add_event_cb(list_btn, app_lv_ui_local_list_btn_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)(0 - (idx + 1)));
            lv_obj_set_size(list_btn, app_lv_style_hor_pct(15), app_lv_style_ver_pct(8));
            lv_obj_t *list_lbl = app_lv_style_label(list_btn);
            lv_obj_align(list_lbl, LV_ALIGN_CENTER, 0, 0);
            lv_label_set_text_static(list_lbl, btn_text[idx]);
        }
        /* 更新列表布局 */
        lv_obj_align_to(list, matrix, LV_ALIGN_OUT_RIGHT_MID, app_lv_style_hor_pct(1), 0);
        /* 初始计数, 设置速度(可以额外制作控件修改speed) */
        app_lv_ui_res_local->count = 0;
        app_lv_ui_res_local->speed = 30;
        /* 重新就绪检查 */
        app_lv_ui_tetris_presenter.ready();
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 100, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_tetris_hide(void *scene)
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
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_tetris = {
    /* 场景资源节点 */
    .show = app_lv_ui_tetris_show,
    .hide = app_lv_ui_tetris_hide,
};
