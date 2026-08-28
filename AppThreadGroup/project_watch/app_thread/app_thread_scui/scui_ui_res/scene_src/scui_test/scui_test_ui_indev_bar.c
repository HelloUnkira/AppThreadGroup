/*实现目标:
 *    测试(indev bar)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#define SCUI_UI_INDEV_BAR_KEEP      10  /* 保留打印行数 */
#define SCUI_UI_INDEV_BAR_MARK_LEN  48  /* 单行标记长度 */

static struct {
    char  (*line)[SCUI_UI_INDEV_BAR_MARK_LEN];  /* 标记序列 */
    char   *text_str;                           /* 文本缓冲 */
    uint8_t line_cnt;                           /* 标记行数 */
} * scui_ui_res_local = NULL;

/*@brief 文本框追加打印标记(保留最近10次)
 *@param mark 标记文本
 */
static void scui_test_ui_indev_bar_print(const char *mark)
{
    /* 行数达到上限,前移丢弃最旧 */
    if (scui_ui_res_local->line_cnt >= SCUI_UI_INDEV_BAR_KEEP) {
        for (uint8_t idx = 1; idx < SCUI_UI_INDEV_BAR_KEEP; idx++)
            memcpy(scui_ui_res_local->line[idx - 1],
                scui_ui_res_local->line[idx], SCUI_UI_INDEV_BAR_MARK_LEN);
    } else {
        scui_ui_res_local->line_cnt++;
    }
    /* 新建标记写入尾部 */
    snprintf(scui_ui_res_local->line[scui_ui_res_local->line_cnt - 1],
        SCUI_UI_INDEV_BAR_MARK_LEN, "%s", mark);
    
    /* 拼接文本, 换行分隔 */
    char *str = scui_ui_res_local->text_str;
    char *end = scui_ui_res_local->text_str +
        SCUI_UI_INDEV_BAR_KEEP * SCUI_UI_INDEV_BAR_MARK_LEN;
    for (uint8_t idx = 0; idx < scui_ui_res_local->line_cnt; idx++) {
        int len = snprintf(str, end - str, "%s%s", scui_ui_res_local->line[idx],
            idx + 1 < scui_ui_res_local->line_cnt ? "\n" : "");
        str += len;
    }
    scui_string_update_str(SCUI_UI_SCENE_TEST_UI_INDEV_BAR_2,
        (uint8_t *)scui_ui_res_local->text_str);
}

/*@brief 长条事件回调(ptr捕获注入bar, bar事件打印)
 *@param event 事件
 */
void scui_test_ui_indev_bar_strip_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_ptr_down:
    case scui_event_ptr_hold:
    case scui_event_ptr_up: {
        /* 输出原始坐标点与按下状态 */
        scui_indev_state_t state = scui_indev_state_press;
        if (event->type == scui_event_ptr_up)
            state = scui_indev_state_release;
        
        /* 组存坐标转条内相对坐标 */
        scui_point_t point = event->ptr_c;
        scui_widget_switch_point(event->object, &point);
        SCUI_LOG_INFO("pos_x:%d pos_y:%d state:%d", point.x, point.y, state);
        
        /* 注入bar输入数据 */
        scui_indev_data_t indev_data = {0};
        indev_data.type        = scui_indev_type_bar;
        indev_data.state       = state;
        indev_data.bar.bar_pos = point.y;
        scui_indev_notify(&indev_data);
        break;
    }
    case scui_event_bar_move:
    case scui_event_bar_fling: {
        const char *name = NULL;
        switch (event->type) {
        case scui_event_bar_move:  name = "move";  break;
        case scui_event_bar_fling: name = "fling"; break;
        }
        char mark[SCUI_UI_INDEV_BAR_MARK_LEN];
        snprintf(mark, sizeof(mark), "%s(way:%d,diff:%d)",
            name, event->bar_way, event->bar_diff);
        scui_test_ui_indev_bar_print(mark);
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_indev_bar_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        scui_ui_res_local->line_cnt = 0;
        scui_ui_res_local->line = SCUI_MEM_ALLOC(scui_mem_type_user,
            SCUI_UI_INDEV_BAR_KEEP * SCUI_UI_INDEV_BAR_MARK_LEN);
        scui_ui_res_local->text_str = SCUI_MEM_ALLOC(scui_mem_type_user,
            SCUI_UI_INDEV_BAR_KEEP * SCUI_UI_INDEV_BAR_MARK_LEN);
        break;
    }
    case scui_event_destroy: {
        SCUI_MEM_FREE(scui_ui_res_local->line);
        SCUI_MEM_FREE(scui_ui_res_local->text_str);
        break;
    }
    default:
        break;
    }
}