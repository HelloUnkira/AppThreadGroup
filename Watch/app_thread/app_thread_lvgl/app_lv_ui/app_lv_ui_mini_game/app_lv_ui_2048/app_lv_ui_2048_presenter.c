
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_ui_2048.h"
#include "app_lv_ui_2048_presenter.h"

#include "Game2048.h"

/* 如果需要转储和加载 */
/* 做俩个接口保存这俩个数组即可 */
/* 这里简化则不做该动作 */
static uint8_t  app_lv_ui_2048_matrix[APP_LV_UI_2048_NUM * APP_LV_UI_2048_NUM] = {0};
static uint8_t  app_lv_ui_2048_buffer[APP_LV_UI_2048_NUM] = {0};
static Game2048 app_lv_ui_2048 = {0};

/*@brief lvgl ui数据交互回调
 */
static uint32_t app_lv_ui_random(uint32_t number)
{
    return rand() % number;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_ready(uint8_t difficult)
{
    /* 这里简化,默认难度为2 */
    difficult = 2;
    /* 资源绑定 */
    app_lv_ui_2048.Random = app_lv_ui_random;
    app_lv_ui_2048.Matrix = app_lv_ui_2048_matrix;
    app_lv_ui_2048.Buffer = app_lv_ui_2048_buffer;
    app_lv_ui_2048.Scale  = APP_LV_UI_2048_NUM;
    /* 检查一下:如果矩阵为0,则刷新指定数据 */
    /* 检查一下:如果矩阵不为0,则使用原来记录的数据 */
    static bool not_ready_yet = true;
    uint8_t   data = 0;
    uint8_t (*matrix)[APP_LV_UI_2048_NUM][APP_LV_UI_2048_NUM] = app_lv_ui_2048_matrix;
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_2048_NUM; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_2048_NUM; idx2++)
         data |= (*matrix)[idx1][idx2];
    if  (data != 0)
         return;
    /* 重加载2048 */
    return Game2048_Ready(&app_lv_ui_2048, difficult);
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_execute(lv_dir_t direct)
{
    Game2048Way way = Game2048_None;
    
    switch (direct) {
    case LV_DIR_TOP:
        way = Game2048_Up;
        break;
    case LV_DIR_BOTTOM:
        way = Game2048_Down;
        break;
    case LV_DIR_LEFT:
        way = Game2048_Left;
        break;
    case LV_DIR_RIGHT:
        way = Game2048_Right;
        break;
    default:
        break;
    }
    
    APP_SYS_ASSERT(way != Game2048_None);
    return Game2048_Execute(&app_lv_ui_2048, way);
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_get_matrix(void **matrix)
{
    *matrix = app_lv_ui_2048_matrix;
}

app_lv_ui_2048_presenter_t app_lv_ui_2048_presenter = {
    .ready      = app_lv_ui_ready,
    .execute    = app_lv_ui_execute,
    .get_matrix = app_lv_ui_get_matrix,
};
