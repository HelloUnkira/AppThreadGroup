
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_scene.h"
#include "app_lv_ui_event.h"
#include "app_lv_ui_style.h"
#include "app_lv_ui_scene.h"

#include "app_lv_ui_calculator.h"
#include "app_lv_ui_calculator_presenter.h"

#include "app_lv_ui_clock_presenter.h"

#define APP_LV_RES_USE_BTNMATRIX   1

typedef struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *time;
    lv_obj_t *tips;
    lv_obj_t *retval;
    lv_obj_t *expr;
    lv_obj_t *list_btn;
#if APP_LV_RES_USE_BTNMATRIX
    lv_obj_t *btnmatrix;
#endif
} app_lv_ui_res_local_t;

static app_lv_ui_res_local_t *app_lv_ui_res_local = NULL;

#define APP_LV_UI_TEXT_MAX_LEN  127

#if APP_LV_RES_USE_BTNMATRIX
const char *app_lv_ui_res_btnmatrix_str[] = {
    /* 操作数表 */
    "<", ">", "=", "\n",
    "C", "0", "X", "\n",
    "1", "2", "3", "\n",
    "4", "5", "6", "\n",
    "7", "8", "9", "\n",
    "(", ".", ")", "\n",
    /* 操作符表 */
    "+", "-", "%",      "\n",
    "*", "/", ",",      "\n",
    "e", "**", "pai",   "\n",
    "!", "!!", "c(,)",  "\n",
    /* 扩展操作函数表 */
    "sqrt()",   "log()",    "log10()",  "\n",
    "cos()",    "sin()",    "tan()",    "\n",
    "cosh()",   "sinh()",   "tanh()",   "\n",
    "acos()",   "asin()",   "atan()",   "\n",
    "atan2(,)", "exp()",    "pow(,)",   "\n",
    "fmod(,)",  "ceil()",   "floor()",  "",
};
#else
const char *app_lv_ui_res_btnlist_str[] = {
    /* 操作数表 */
    "<", ">", "=",
    "C", "0", "X",
    "1", "2", "3",
    "4", "5", "6",
    "7", "8", "9",
    "(", ".", ")",
    /* 操作符表 */
    "+", "-", "%",
    "*", "/", ",",
    "e", "**", "pai",
    "!", "!!", "c(,)",
    /* 扩展操作函数表 */
    "sqrt()",   "log()",    "log10()",
    "cos()",    "sin()",    "tan()",
    "cosh()",   "sinh()",   "tanh()",
    "acos()",   "asin()",   "atan()",
    "atan2(,)", "exp()",    "pow(,)",
    "fmod(,)",  "ceil()",   "floor()",
};

static const uint8_t app_lv_ui_res_btnlist_str_size = sizeof(app_lv_ui_res_btnlist_str) /
                                                      sizeof(app_lv_ui_res_btnlist_str[0]);
#endif

/*@brief 界面自定义事件回调
*/
#if APP_LV_RES_USE_BTNMATRIX
static void app_lv_ui_res_btnmatrix_str_cb(lv_event_t *e)
#else
static void app_lv_ui_res_btnlist_str_cb(lv_event_t *e)
#endif
{
#if APP_LV_RES_USE_BTNMATRIX
    uint16_t btn_id = lv_btnmatrix_get_selected_btn(app_lv_ui_res_local->btnmatrix);
    const char *str = lv_btnmatrix_get_btn_text(app_lv_ui_res_local->btnmatrix, btn_id);
#else
    const char *str = lv_event_get_user_data(e);
#endif
    
    if (str == NULL)
        return;
    /* 弹回到最开始 */
    lv_obj_scroll_to_y(app_lv_ui_res_local->list_btn, 0, LV_ANIM_ON);
    
    /* 光标左移 */
    if (!strcmp(str, "<")) {
        lv_textarea_cursor_left(app_lv_ui_res_local->expr);
        return;
    }
    /* 光标右移 */
    if (!strcmp(str, ">")) {
        lv_textarea_cursor_right(app_lv_ui_res_local->expr);
        return;
    }
    /* 计算 */
    if (!strcmp(str, "=")) {
        double result = 0.0;
        char err[128] = {0};
        char expr[APP_LV_UI_TEXT_MAX_LEN + 1] = {0};
        strcpy(expr,lv_textarea_get_text(app_lv_ui_res_local->expr));
        if (app_lv_ui_calculator_presenter.math_expression(expr, &result, err)) {
            char ret_str[20] = {0};
            sprintf(ret_str, "%.8lf", result);
            lv_label_set_text(app_lv_ui_res_local->retval, ret_str);
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:...");
        } else {
            lv_label_set_text(app_lv_ui_res_local->retval, "0.000");
            lv_label_set_text(app_lv_ui_res_local->tips, err);
        }
        return;
    }
    /* 退格 */
    if (!strcmp(str, "X")) {
        lv_textarea_del_char(app_lv_ui_res_local->expr);
        return;
    }
    /* 清空 */
    if (!strcmp(str, "C")) {
        lv_label_set_text(app_lv_ui_res_local->retval, "0.000");
        lv_label_set_text(app_lv_ui_res_local->tips, "Tips:...");
        lv_textarea_set_text(app_lv_ui_res_local->expr, "");
        return;
    }
    /* 其他字符追加 */
    lv_textarea_add_text(app_lv_ui_res_local->expr, str);
    /* 特殊字符追加提示 */
    /* 常见数学符号省略 */
    {
        if (0);
        else if (!strcmp(str, "e"))
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:e == 2.7182818....");
        else if (!strcmp(str, "pai"))
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:pai == 3.1415926....");
        else if (!strcmp(str, "**"))
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:x**y == pow(x,y) -> power");
        else if (!strcmp(str, "!"))
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:x! is factorial of full permutation");
        else if (!strcmp(str, "!!"))
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:x!! is 2 factorial");
        else if (!strcmp(str, "c(,)"))
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:c(k,n) is combination");
        else if (!strcmp(str, "atan2(,)"))
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:atan2(x,y) high precision atan(x/y)");
        else if (!strcmp(str, "fmod(,)"))
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:fmod(x,y) == x%y");
        else if (!strcmp(str, "ceil()"))
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:ceil(x) == [x]");
        else if (!strcmp(str, "floor()"))
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:floor() == [x] + 1");
        else
            lv_label_set_text(app_lv_ui_res_local->tips, "Tips:...");
    }
    /* 如果匹配到了括号,光标回退一个格子 */
    if (strchr(str, '(') != NULL && strchr(str, ')') != NULL) {
        lv_textarea_cursor_left(app_lv_ui_res_local->expr);
        /* 如果匹配到了逗号,光标回退一个格子 */
        if (strchr(str, ',') != NULL)
            lv_textarea_cursor_left(app_lv_ui_res_local->expr);
    }
}

/*@brief 界面动画定时器回调
 */
static void app_lv_ui_local_anim_handler(void *para, int32_t value)
{
    /* 左上角系统时钟更新 */
    lv_label_set_text_fmt(app_lv_ui_res_local->time, "%s %.2u:%.2u",
                          app_lv_ui_clock_presenter.is_24() ? "" :
                          app_lv_ui_clock_presenter.is_am() ? "AM" :
                          app_lv_ui_clock_presenter.is_pm() ? "PM" : "",
                          app_lv_ui_clock_presenter.get_hour(),
                          app_lv_ui_clock_presenter.get_minute());
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_calculator_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(app_lv_ui_res_local_t));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_ui_style_scene();
        /* 默认顶部风格 */
        lv_obj_t *title_box = NULL, *title = NULL;
        title_box = app_lv_ui_style_title(app_lv_ui_res_local->scene, &app_lv_ui_res_local->time, &title);
        lv_label_set_text(title, "Calculator");
        /* 第一列:提示,结果 */
        lv_obj_t *list = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_ui_style_object(list);
        lv_obj_set_size(list, LV_HOR_RES, 30);
        lv_obj_align_to(list, title_box, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        app_lv_ui_res_local->tips = app_lv_ui_style_label_title(list);
        lv_obj_set_size(app_lv_ui_res_local->tips, LV_HOR_RES - 120, 30);
        lv_obj_set_style_text_color(app_lv_ui_res_local->tips, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_set_style_text_align(app_lv_ui_res_local->tips, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_align(app_lv_ui_res_local->tips, LV_ALIGN_LEFT_MID, 20, 0);
        lv_label_set_text(app_lv_ui_res_local->tips, "Tips:...");
        app_lv_ui_res_local->retval = app_lv_ui_style_label_title(list);
        lv_obj_set_size(app_lv_ui_res_local->retval, 80, 30);
        lv_obj_set_style_text_color(app_lv_ui_res_local->retval, lv_palette_main(LV_PALETTE_GREEN), 0);
        lv_obj_set_style_text_align(app_lv_ui_res_local->retval, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_align(app_lv_ui_res_local->retval, LV_ALIGN_RIGHT_MID, -20, 0);
        lv_label_set_text(app_lv_ui_res_local->retval, "0.000");
        /* 第二列:表达式 */
        app_lv_ui_res_local->expr = app_lv_ui_style_textarea(app_lv_ui_res_local->scene);
        lv_obj_set_size(app_lv_ui_res_local->expr, LV_HOR_RES - 40, 35);
        lv_obj_set_style_text_align(app_lv_ui_res_local->expr, LV_TEXT_ALIGN_RIGHT, 0);
        lv_textarea_set_max_length(app_lv_ui_res_local->expr, APP_LV_UI_TEXT_MAX_LEN);
        lv_textarea_set_placeholder_text(app_lv_ui_res_local->expr, "Expr:...   ");
        lv_obj_align_to(app_lv_ui_res_local->expr, list, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        #if APP_LV_RES_USE_BTNMATRIX
        app_lv_ui_res_local->list_btn = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_ui_style_object(app_lv_ui_res_local->list_btn);
        lv_obj_set_size(app_lv_ui_res_local->list_btn, LV_HOR_RES, 60 * 3 - 20);
        lv_obj_align_to(app_lv_ui_res_local->list_btn, app_lv_ui_res_local->expr, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        app_lv_ui_res_local->btnmatrix = app_lv_ui_style_btnmatrix(app_lv_ui_res_local->list_btn, app_lv_ui_res_btnmatrix_str);
        lv_obj_set_size(app_lv_ui_res_local->btnmatrix, LV_HOR_RES, 30 * 16);
        lv_obj_add_event_cb(app_lv_ui_res_local->btnmatrix, app_lv_ui_res_btnmatrix_str_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_set_style_width(app_lv_ui_res_local->btnmatrix, LV_HOR_RES / 3 - 20, LV_PART_ITEMS);
        lv_obj_set_style_height(app_lv_ui_res_local->btnmatrix, 20, LV_PART_ITEMS);
        lv_obj_align(app_lv_ui_res_local->btnmatrix, LV_ALIGN_TOP_MID, 0, 0);
        #else
        /* 三列:操作数与操作符 */
        app_lv_ui_res_local->list_btn = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_ui_style_object(app_lv_ui_res_local->list_btn);
        lv_obj_set_style_pad_all(app_lv_ui_res_local->list_btn, 10, 0);
        lv_obj_set_style_layout(app_lv_ui_res_local->list_btn, LV_LAYOUT_FLEX, 0);
        lv_obj_set_style_flex_flow(app_lv_ui_res_local->list_btn, LV_FLEX_FLOW_ROW_WRAP, 0);
        lv_obj_set_style_flex_main_place(app_lv_ui_res_local->list_btn, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
        lv_obj_set_style_anim_time(app_lv_ui_res_local->list_btn, 1000, LV_PART_SCROLLBAR);
        lv_obj_set_size(app_lv_ui_res_local->list_btn, LV_HOR_RES, 60 * 3 - 20);
        lv_obj_align_to(app_lv_ui_res_local->list_btn, app_lv_ui_res_local->expr, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
        for (uint8_t idx = 0; idx < app_lv_ui_res_btnlist_str_size; idx++) {
            lv_obj_t *btn = app_lv_ui_style_btn(app_lv_ui_res_local->list_btn);
            lv_obj_set_size(btn, LV_HOR_RES / 3 - 20, 30 - 10);
            lv_obj_add_event_cb(btn, app_lv_ui_res_btnlist_str_cb, LV_EVENT_CLICKED, (void *)app_lv_ui_res_btnlist_str[idx]);
            lv_obj_t *lab = app_lv_ui_style_label(btn);
            lv_label_set_text(lab, app_lv_ui_res_btnlist_str[idx]);
            lv_obj_align(lab, LV_ALIGN_CENTER, 0, 0);
        }
        #endif
        /* 初始化显示动画 */
        lv_anim_init(&app_lv_ui_res_local->anim);
        lv_anim_set_var(&app_lv_ui_res_local->anim, app_lv_ui_res_local->scene);
        lv_anim_set_exec_cb(&app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler);
        lv_anim_set_repeat_count(&app_lv_ui_res_local->anim, 0);
        lv_anim_set_values(&app_lv_ui_res_local->anim, 0, 100);
        lv_anim_set_time(&app_lv_ui_res_local->anim, 1000);
        lv_anim_start(&app_lv_ui_res_local->anim);
    }
    app_lv_scene_calculator.self = app_lv_ui_res_local == NULL ? NULL :
                                   app_lv_ui_res_local->scene;
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_calculator_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
    app_lv_scene_calculator.self = app_lv_ui_res_local == NULL ? NULL :
                                   app_lv_ui_res_local->scene;
}

app_lv_scene_t app_lv_scene_calculator = {
    /* 场景资源节点 */
    .show = app_lv_ui_calculator_show,
    .hide = app_lv_ui_calculator_hide,
};
