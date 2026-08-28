/*实现目标:
 *    测试(indev ptr)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#define SCUI_UI_INDEV_PTR_KEEP      10  /* 保留打印行数 */
#define SCUI_UI_INDEV_PTR_MARK_LEN  48  /* 单行标记长度 */

static struct {
    char  (*line)[SCUI_UI_INDEV_PTR_MARK_LEN];  /* 标记序列 */
    char   *text_str;                           /* 文本缓冲 */
    uint8_t line_cnt;                           /* 标记行数 */
} * scui_ui_res_local = NULL;

/*@brief 文本框追加打印标记(保留最近10次)
 *@param mark 标记文本
 */
static void scui_test_ui_indev_ptr_print(const char *mark)
{
    /* 行数达到上限,前移丢弃最旧 */
    if (scui_ui_res_local->line_cnt >= SCUI_UI_INDEV_PTR_KEEP) {
        for (uint8_t idx = 1; idx < SCUI_UI_INDEV_PTR_KEEP; idx++)
            memcpy(scui_ui_res_local->line[idx - 1],
                scui_ui_res_local->line[idx], SCUI_UI_INDEV_PTR_MARK_LEN);
    } else {
        scui_ui_res_local->line_cnt++;
    }
    /* 新建标记写入尾部 */
    snprintf(scui_ui_res_local->line[scui_ui_res_local->line_cnt - 1],
        SCUI_UI_INDEV_PTR_MARK_LEN, "%s", mark);
    
    /* 拼接文本, 换行分隔 */
    char *str = scui_ui_res_local->text_str;
    char *end = scui_ui_res_local->text_str +
        SCUI_UI_INDEV_PTR_KEEP * SCUI_UI_INDEV_PTR_MARK_LEN;
    for (uint8_t idx = 0; idx < scui_ui_res_local->line_cnt; idx++) {
        int len = snprintf(str, end - str, "%s%s", scui_ui_res_local->line[idx],
            idx + 1 < scui_ui_res_local->line_cnt ? "\n" : "");
        str += len;
    }
    scui_string_update_str(SCUI_UI_SCENE_TEST_UI_INDEV_PTR_2,
        (uint8_t *)scui_ui_res_local->text_str);
}

/*@brief 长条事件回调(ptr事件直接打印)
 *@param event 事件
 */
void scui_test_ui_indev_ptr_strip_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_ptr_down: {
        scui_ui_res_local->line_cnt = 0;
        char mark[SCUI_UI_INDEV_PTR_MARK_LEN];
        snprintf(mark, sizeof(mark), "down(x:%d,y:%d)",
            event->ptr_c.x, event->ptr_c.y);
        scui_test_ui_indev_ptr_print(mark);
        break;
    }
    case scui_event_ptr_hold:
    case scui_event_ptr_click:
    case scui_event_ptr_up: {
        const char *name = NULL;
        switch (event->type) {
        case scui_event_ptr_hold:  name = "hold";  break;
        case scui_event_ptr_click: name = "click"; break;
        case scui_event_ptr_up:    name = "up";    break;
        }
        char mark[SCUI_UI_INDEV_PTR_MARK_LEN];
        snprintf(mark, sizeof(mark), "%s(x:%d,y:%d)", name,
            event->ptr_c.x, event->ptr_c.y);
        scui_test_ui_indev_ptr_print(mark);
        break;
    }
    case scui_event_ptr_fling: {
        char mark[SCUI_UI_INDEV_PTR_MARK_LEN];
        snprintf(mark, sizeof(mark), "fling(%d,%d->%d,%d)",
            event->ptr_s.x, event->ptr_s.y, event->ptr_e.x, event->ptr_e.y);
        scui_test_ui_indev_ptr_print(mark);
        break;
    }
    case scui_event_ptr_move: {
        char mark[SCUI_UI_INDEV_PTR_MARK_LEN];
        snprintf(mark, sizeof(mark), "move(%d,%d->%d,%d,v:%d)",
            event->ptr_s.x, event->ptr_s.y, event->ptr_e.x, event->ptr_e.y, event->ptr_v);
        scui_test_ui_indev_ptr_print(mark);
        break;
    }
    default:
        break;
    }
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_indev_ptr_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        scui_ui_res_local->line_cnt = 0;
        scui_ui_res_local->line = SCUI_MEM_ALLOC(scui_mem_type_user,
            SCUI_UI_INDEV_PTR_KEEP * SCUI_UI_INDEV_PTR_MARK_LEN);
        scui_ui_res_local->text_str = SCUI_MEM_ALLOC(scui_mem_type_user,
            SCUI_UI_INDEV_PTR_KEEP * SCUI_UI_INDEV_PTR_MARK_LEN);
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