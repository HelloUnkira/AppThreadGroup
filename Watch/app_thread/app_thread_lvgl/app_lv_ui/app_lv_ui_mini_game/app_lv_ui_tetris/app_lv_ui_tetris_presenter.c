
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_ui_tetris.h"
#include "app_lv_ui_tetris_presenter.h"

#include "Tetris.h"

/* 如果需要转储和加载 */
/* 做俩个接口保存这俩个数组即可 */
/* 这里简化则不做该动作 */
static bool app_lv_ui_tetris_status = false;
static TetrisBlock app_lv_ui_tetris_matrix[APP_LV_UI_TETRIS_LINE][APP_LV_UI_TETRIS_ELEMENT] = {0};
static Tetris app_lv_ui_tetris = {0};

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_get_type(void)
{
    return rand() % TetrisGraph_GetNumber();
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_set_record(uint32_t type, TetrisBlock *block)
{
    block->Record = type;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_ready(void)
{
    app_lv_ui_tetris.Elements  = APP_LV_UI_TETRIS_ELEMENT;
    app_lv_ui_tetris.Lines     = APP_LV_UI_TETRIS_LINE;
    app_lv_ui_tetris.Matrix    = (TetrisBlock *)app_lv_ui_tetris_matrix;
    app_lv_ui_tetris.EraseLine = 0;
    app_lv_ui_tetris.GetType   = app_lv_ui_get_type;
    app_lv_ui_tetris.SetRecord = app_lv_ui_set_record;
    /* 检查一下:如果矩阵为0,则刷新指定数据 */
    /* 检查一下:如果矩阵不为0,则使用原来记录的数据 */
    static bool not_ready_yet = true;
    uint8_t data = 0;
    app_lv_ui_tetris_block_t (*matrix)[APP_LV_UI_TETRIS_LINE][APP_LV_UI_TETRIS_ELEMENT] = (void *)app_lv_ui_tetris_matrix;
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_TETRIS_LINE; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_TETRIS_ELEMENT; idx2++)
         data |= (*matrix)[idx1][idx2].status;
    if  (data != 0)
         return app_lv_ui_tetris_status;
    /* 重加载tetris */
    app_lv_ui_tetris_status = Tetris_Ready(&app_lv_ui_tetris);
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_execute(void)
{
    return Tetris_Iterate(&app_lv_ui_tetris);
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_move_step(void)
{
    return Tetris_MoveStep(&app_lv_ui_tetris);
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_move_left(void)
{
    return Tetris_MoveLeft(&app_lv_ui_tetris);
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_move_right(void)
{
    return Tetris_MoveRight(&app_lv_ui_tetris);
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_rotate_left(void)
{
    return Tetris_RotateLeft(&app_lv_ui_tetris);
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_rotate_right(void)
{
    return Tetris_RotateRight(&app_lv_ui_tetris);
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_matrix(void **matrix)
{
    *matrix = app_lv_ui_tetris_matrix;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_graph(void **graph, uint32_t *scale, uint32_t *x, uint32_t *y)
{
    *graph = app_lv_ui_tetris.Graph;
    *scale = app_lv_ui_tetris.Scale;
    *x     = app_lv_ui_tetris.PosX;
    *y     = app_lv_ui_tetris.PosY;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_graph_next(void **graph, uint32_t *scale)
{
    *graph = app_lv_ui_tetris.GraphNext;
    *scale = app_lv_ui_tetris.ScaleNext;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_erase_line(uint32_t *erase_line)
{
    *erase_line = app_lv_ui_tetris.EraseLine;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_set_status(bool *status)
{
    app_lv_ui_tetris_status = *status;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_get_status(bool *status)
{
    *status = app_lv_ui_tetris_status;
}


app_lv_ui_tetris_presenter_t app_lv_ui_tetris_presenter = {
    .ready          = app_lv_ui_ready,
    .execute        = app_lv_ui_execute,
    .move_step      = app_lv_ui_move_step,
    .move_left      = app_lv_ui_move_left,
    .move_right     = app_lv_ui_move_right,
    .rotate_left    = app_lv_ui_rotate_left,
    .rotate_right   = app_lv_ui_rotate_right,
    .get_matrix     = app_lv_ui_get_matrix,
    .get_graph      = app_lv_ui_get_graph,
    .get_graph_next = app_lv_ui_get_graph_next,
    .get_erase_line = app_lv_ui_get_erase_line,
    .set_status     = app_lv_ui_get_set_status,
    .get_status     = app_lv_ui_get_get_status,
};
