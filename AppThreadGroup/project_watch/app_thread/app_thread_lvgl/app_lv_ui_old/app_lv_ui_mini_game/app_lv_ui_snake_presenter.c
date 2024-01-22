
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

#include "Snake.h"

/* 如果需要转储和加载 */
/* 做俩个接口保存这俩个数组 */
/* 贪吃蛇需要额外保存蛇身并且生成蛇身的部分小逻辑 */
/* 这涉及到对双链表的部分操作,贪吃蛇游戏本身没附带该部分 */
/* 这里简化则不做该动作 */
static bool app_lv_ui_snake_status = false;
static SnakeBlock app_lv_ui_snake_matrix[APP_LV_UI_SNAKE_LINE][APP_LV_UI_SNAKE_ELEMENT] = {0};
static Snake app_lv_ui_snake_inst = {0};

/* 贪吃蛇地图暂时没看见可以动态生成的算法,直接使用多个内置地图代表难度即可,这里只用一个 */
static bool app_lv_ui_snake_map_1[APP_LV_UI_SNAKE_LINE][APP_LV_UI_SNAKE_ELEMENT] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
    {0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,},
    {0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,},
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,},
    {0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,},
    {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,},
    {0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,},
    {0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,},
    {0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
};

/*@brief lvgl ui数据交互回调
 */
void app_lv_ui_func_local_create(uint32_t size, void **pointer)
{
    *pointer = app_mem_alloc(size);
}

/*@brief lvgl ui数据交互回调
 */
void app_lv_ui_func_local_destroy(void *pointer)
{
    app_mem_free(pointer);
}

/*@brief lvgl ui数据交互回调
 */
void app_lv_ui_func_local_set_record1(SnakeNode *old_head, SnakeNode *node)
{
    old_head->Record = 0;   /* 旧节点设置为蛇身 */
    node->Record = 1;       /* 新节点设置为蛇头 */
}

/*@brief lvgl ui数据交互回调
 */
void app_lv_ui_func_local_set_record2(SnakeNode *new_tail, SnakeNode *node)
{
    node->Record = 0;       /* 旧节点设置为蛇身 */
    new_tail->Record = 2;   /* 新节点设置为蛇尾 */
}

/*@brief lvgl ui数据交互回调
 */
void app_lv_ui_func_local_next_touch(uint32_t *x, uint32_t *y)
{
    *x = rand() % APP_LV_UI_SNAKE_ELEMENT;
    *y = rand() % APP_LV_UI_SNAKE_LINE;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_func_local_ready(void)
{
    /* 检查一下:如果矩阵为0,则刷新指定数据 */
    /* 检查一下:如果矩阵不为0,则使用原来记录的数据 */
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_SNAKE_LINE; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_SNAKE_ELEMENT; idx2++)
        if (app_lv_ui_snake_matrix[idx1][idx2].Status != 0)
            return app_lv_ui_snake_status;
    /* 配置snake参数 */
    app_lv_ui_snake_inst.Matrix     = app_lv_ui_snake_matrix;
    app_lv_ui_snake_inst.Width      = APP_LV_UI_SNAKE_ELEMENT;
    app_lv_ui_snake_inst.Height     = APP_LV_UI_SNAKE_LINE;
    app_lv_ui_snake_inst.Create     = app_lv_ui_func_local_create;
    app_lv_ui_snake_inst.Destroy    = app_lv_ui_func_local_destroy;
    app_lv_ui_snake_inst.SetRecord1 = app_lv_ui_func_local_set_record1;
    app_lv_ui_snake_inst.SetRecord2 = app_lv_ui_func_local_set_record2;
    app_lv_ui_snake_inst.NextTouch  = app_lv_ui_func_local_next_touch;
    /* 使用map生成matrix */
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_SNAKE_LINE; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_SNAKE_ELEMENT; idx2++)
        app_lv_ui_snake_matrix[idx1][idx2].Status = app_lv_ui_snake_map_1[idx1][idx2];
    /* 重加载snake */
    return app_lv_ui_snake_status = Snake_Ready(&app_lv_ui_snake_inst);
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_func_local_execute(lv_dir_t direct)
{
    SnakeWay way = Snake_None;
    
    switch (direct) {
    case LV_DIR_TOP:
        way = Snake_Up;
        break;
    case LV_DIR_BOTTOM:
        way = Snake_Down;
        break;
    case LV_DIR_LEFT:
        way = Snake_Left;
        break;
    case LV_DIR_RIGHT:
        way = Snake_Right;
        break;
    default:
        break;
    }
    
    APP_SYS_ASSERT(way != Snake_None);
    return Snake_Execute(&app_lv_ui_snake_inst, way);
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_func_local_clean(void)
{
    /* 回收贪吃蛇蛇身 */
    while (DL_List_GetHead(&app_lv_ui_snake_inst.List) != NULL) {
        DL_Node *Target = DL_List_GetHead(&app_lv_ui_snake_inst.List);
        DL_List_Remove(&app_lv_ui_snake_inst.List, Target);
        free(DL_GetOwner(SnakeNode, Node, Target));
    }
    /* 清空矩阵 */
    for (uint8_t idx1 = 0; idx1 < APP_LV_UI_SNAKE_LINE; idx1++)
    for (uint8_t idx2 = 0; idx2 < APP_LV_UI_SNAKE_ELEMENT; idx2++)
        app_lv_ui_snake_matrix[idx1][idx2].Status = 0;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_func_local_get_matrix(void **matrix)
{
    *matrix = app_lv_ui_snake_matrix;
}

/*@brief lvgl ui数据交互回调
 */
static bool app_lv_ui_func_local_get_node(uint32_t count, app_lv_ui_snake_node_t *node)
{
    uint32_t current = 0;
    Snake_Traverse_Backward(&app_lv_ui_snake_inst, Node) {
        if (current == count) {
            node->pos_x  = Node->PosX;
            node->pos_y  = Node->PosY;
            node->record = Node->Record;
            return true;
        }
        current++;
    }
    return false;
}

/*@brief lvgl ui数据交互回调
 */
static void app_lv_ui_func_local_get_food(app_lv_ui_snake_node_t *node)
{
    node->pos_x = app_lv_ui_snake_inst.TouchPosX;
    node->pos_y = app_lv_ui_snake_inst.TouchPosY;
}

app_lv_ui_snake_presenter_t app_lv_ui_snake_presenter = {
    .ready          = app_lv_ui_func_local_ready,
    .execute        = app_lv_ui_func_local_execute,
    .clean          = app_lv_ui_func_local_clean,
    .get_matrix     = app_lv_ui_func_local_get_matrix,
    .get_node       = app_lv_ui_func_local_get_node,
    .get_food       = app_lv_ui_func_local_get_food,
};
