
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_ui_maze.h"
#include "app_lv_ui_maze_presenter.h"

#include "Maze.h"

/* 如果需要转储和加载 */
/* 做俩个接口保存这俩个数组即可 */
/* 这里简化则不做该动作 */
static bool app_lv_ui_maze_status = false;
static MazeBlock app_lv_ui_maze_matrix[APP_LV_UI_MAZE_LINE][APP_LV_UI_MAZE_ELEMENT] = {0};
static Maze app_lv_ui_maze = {0};

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_random(uint32_t Number)
{
    return rand() % Number;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_ready(void)
{
    /* 检查一下:如果矩阵为0,则刷新指定数据 */
    /* 检查一下:如果矩阵不为0,则使用原来记录的数据 */
    app_lv_ui_maze_block_t (*matrix)[APP_LV_UI_MAZE_LINE][APP_LV_UI_MAZE_ELEMENT] = (void *)app_lv_ui_maze_matrix;
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_MAZE_LINE; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_MAZE_ELEMENT; idx2++)
        if ((*matrix)[idx1][idx2].h_status != 0 ||
            (*matrix)[idx1][idx2].v_status != 0)
            return app_lv_ui_maze_status;
    /* 配置迷宫参数 */
    app_lv_ui_maze.Matrix   = app_lv_ui_maze_matrix;
    app_lv_ui_maze.Elements = APP_LV_UI_MAZE_ELEMENT;
    app_lv_ui_maze.Lines    = APP_LV_UI_MAZE_LINE;
    app_lv_ui_maze.Random   = app_lv_ui_random;
    app_lv_ui_maze.Allocate = app_mem_alloc;
    app_lv_ui_maze.Free     = app_mem_free;
    /* 设置起始点,结束点,过程点,当前点颜色编号 */
    app_lv_ui_maze.RecordS.Record = 1;
    app_lv_ui_maze.RecordE.Record = 2;
    app_lv_ui_maze.RecordT.Record = 3;
    app_lv_ui_maze.RecordC.Record = 4;
    /* 清除历史路径状态 */
    for (uint32_t Offset = 0; Offset < app_lv_ui_maze.Lines * app_lv_ui_maze.Elements; Offset++)
        app_lv_ui_maze.Matrix[Offset].Record = 0;
    /* 重加载maze */
    return app_lv_ui_maze_status = Maze_Ready(&app_lv_ui_maze);
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_execute(lv_dir_t direct)
{
    MazeWay way = Maze_None;
    
    switch (direct) {
    case LV_DIR_TOP:
        way = Maze_Up;
        break;
    case LV_DIR_BOTTOM:
        way = Maze_Down;
        break;
    case LV_DIR_LEFT:
        way = Maze_Left;
        break;
    case LV_DIR_RIGHT:
        way = Maze_Right;
        break;
    default:
        break;
    }
    
    APP_SYS_ASSERT(way != Maze_None);
    return Maze_Execute(&app_lv_ui_maze, way);
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_matrix(void **matrix)
{
    *matrix = app_lv_ui_maze_matrix;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_clean(void)
{
    app_lv_ui_maze_block_t (*matrix)[APP_LV_UI_MAZE_LINE][APP_LV_UI_MAZE_ELEMENT] = (void *)app_lv_ui_maze_matrix;
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_MAZE_LINE; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_MAZE_ELEMENT; idx2++) {
        (*matrix)[idx1][idx2].h_status = 0;
        (*matrix)[idx1][idx2].v_status = 0;
    }
}

app_lv_ui_maze_presenter_t app_lv_ui_maze_presenter = {
    .ready      = app_lv_ui_ready,
    .execute    = app_lv_ui_execute,
    .get_matrix = app_lv_ui_get_matrix,
    .clean      = app_lv_ui_clean,
};
