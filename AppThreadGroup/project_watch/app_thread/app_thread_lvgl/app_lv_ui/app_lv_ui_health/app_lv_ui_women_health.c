
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

static struct {
    lv_anim_t anim;
    lv_obj_t *scene;
    lv_obj_t *time;
    lv_obj_t *img;
    lv_obj_t *txt;
    lv_obj_t *calendar;
    lv_obj_t *btnmatrix;
    uint16_t  btn_cnt;
    uint16_t  btn_id_s;
    uint16_t  btn_id_e;
} *app_lv_ui_res_local = NULL;

static const char *app_lv_ui_res_week[] = {"S", "M", "T", "W", "T", "F", "S"};

/*@brief 界面自定义事件回调
 */
static void app_lv_ui_btnmatrix_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t *dsc = lv_event_get_param(e);
    switch (lv_event_get_code(e)) {
    case LV_EVENT_DRAW_PART_BEGIN: {
        /* 目标绘制部 */
        if (dsc->part != LV_PART_ITEMS) {
            break;
        }
        /* 染色 */
        uint8_t day_s = 0, day_e = 0;
        uint16_t btn_ofs = app_lv_ui_res_local->btn_id_s;
        /* 默认不显示边界 */
        dsc->rect_dsc->border_opa = LV_OPA_TRANSP;
        dsc->rect_dsc->border_width = 0;
        /* 默认渐变底色 */
        dsc->rect_dsc->bg_opa = LV_OPA_TRANSP;
        dsc->rect_dsc->bg_color = lv_color_black();
        dsc->rect_dsc->bg_grad.dir = LV_GRAD_DIR_VER;
        dsc->rect_dsc->bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_BLUE);
        dsc->rect_dsc->bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_GREEN);
        dsc->rect_dsc->bg_grad.stops_count = 2;
        /* 月经期 */
        app_lv_ui_presenter_women_health.get_menstrual(&day_s, &day_e);
        if (dsc->id >= btn_ofs + day_s && dsc->id <= btn_ofs + day_e) {
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
            dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_PINK);
            dsc->rect_dsc->bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_PINK);
            dsc->rect_dsc->bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_PINK);
        }
        /* 易孕期 */
        app_lv_ui_presenter_women_health.get_pregnancy(&day_s, &day_e);
        if (dsc->id >= btn_ofs + day_s && dsc->id <= btn_ofs + day_e) {
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
            dsc->rect_dsc->bg_color = lv_palette_main(LV_PALETTE_PURPLE);
            dsc->rect_dsc->bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_PURPLE);
            dsc->rect_dsc->bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_PURPLE);
        }
        uint16_t t_year = 0;
        uint8_t t_month = 0;
        app_lv_ui_presenter_women_health.get_dtime(&t_year, &t_month);
        /* 今天:边界框 */
        uint8_t day   = app_lv_ui_presenter_clock.get_day();
        uint8_t month = app_lv_ui_presenter_clock.get_month();
        uint16_t year = app_lv_ui_presenter_clock.get_year();
        if (t_month == month && t_year == year && dsc->id == day + btn_ofs) {
            dsc->rect_dsc->border_opa = LV_OPA_COVER;
            dsc->rect_dsc->border_color = lv_palette_main(LV_PALETTE_RED);
            dsc->rect_dsc->border_width += 1;
        }
        /*  */
        break;
    }
    case LV_EVENT_DRAW_PART_END: {
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
    app_lv_ui_presenter_clock.format_clock_1(format_clock_1);
    lv_label_set_text(app_lv_ui_res_local->time, format_clock_1);
    /*  */
    uint8_t day   = app_lv_ui_presenter_clock.get_day();
    uint8_t month = app_lv_ui_presenter_clock.get_month();
    uint16_t year = app_lv_ui_presenter_clock.get_year();
    uint16_t t_year = 0;
    uint8_t t_month = 0;
    app_lv_ui_presenter_women_health.get_dtime(&t_year, &t_month);
    if (t_month == month && t_year == year) {
        uint8_t day_s = 0, day_e = 0;
        /* 安全期 */
        lv_obj_set_style_text_color(app_lv_ui_res_local->txt, lv_palette_main(LV_PALETTE_LIGHT_GREEN), 0);
        lv_label_set_text_static(app_lv_ui_res_local->txt, app_lv_lang_str_find(APP_LV_LANG_0X0113));
        /*  */
        app_lv_ui_presenter_women_health.get_menstrual(&day_s, &day_e);
        /* 月经期 */
        if (day_s <= day && day <= day_e) {
            lv_obj_set_style_text_color(app_lv_ui_res_local->txt, lv_palette_main(LV_PALETTE_PINK), 0);
            lv_label_set_text_static(app_lv_ui_res_local->txt, app_lv_lang_str_find(APP_LV_LANG_0X0111));
        }
        /* 易孕期 */
        app_lv_ui_presenter_women_health.get_pregnancy(&day_s, &day_e);
        if (day_s <= day && day <= day_e) {
            lv_obj_set_style_text_color(app_lv_ui_res_local->txt, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
            lv_label_set_text_static(app_lv_ui_res_local->txt, app_lv_lang_str_find(APP_LV_LANG_0X0112));
        }
    }
}

/*@brief     界面显示
 *@param[in] scene 场景
 */
static void app_lv_ui_women_health_show(void *scene)
{
    if (app_lv_ui_res_local == NULL) {
        app_lv_ui_res_local  = lv_mem_alloc(sizeof(*app_lv_ui_res_local));
        /* 初始化场景 */
        app_lv_ui_res_local->scene = app_lv_style_scene();
        ((app_lv_scene_t *)scene)->root = app_lv_ui_res_local->scene;
        /* 默认顶部风格 */
        lv_obj_t *title_box = lv_obj_create(app_lv_ui_res_local->scene);
        app_lv_style_object(title_box);
        lv_obj_set_size(title_box, LV_HOR_RES, app_lv_style_ver_pct(12));
        lv_obj_align(title_box, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_t *title_lab = lv_label_create(title_box);
        app_lv_style_object(title_lab);
        lv_obj_add_flag(title_lab, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(title_lab, app_lv_event_ui_backtrack_cb, LV_EVENT_CLICKED, NULL);
        lv_obj_set_style_text_font(title_lab, app_lv_ui_font(36), 0);
        lv_label_set_text_static(title_lab, LV_SYMBOL_BACKSPACE);
        lv_obj_align(title_lab, LV_ALIGN_LEFT_MID, app_lv_style_hor_pct(5), 0);
        lv_obj_t *title_txt = lv_label_create(title_box);
        app_lv_style_object(title_txt);
        lv_obj_set_width(title_txt, app_lv_style_hor_pct(50));
        lv_obj_set_style_text_align(title_txt, LV_TEXT_ALIGN_LEFT, 0);
        lv_label_set_text_static(title_txt, app_lv_lang_str_find(APP_LV_LANG_0X0110));
        lv_obj_align_to(title_txt, title_lab, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
        app_lv_ui_res_local->time = lv_label_create(title_box);
        app_lv_style_object(app_lv_ui_res_local->time);
        lv_obj_align(app_lv_ui_res_local->time, LV_ALIGN_RIGHT_MID, -app_lv_style_hor_pct(5), 0);
        /* 绘制图片 */
        app_lv_ui_res_local->img = lv_img_create(app_lv_ui_res_local->scene);
        app_lv_style_object(app_lv_ui_res_local->img);
        lv_img_set_src(app_lv_ui_res_local->img, app_lv_pic_str_find(APP_LV_PIC_03_WOMEN_HEALTH_FLOWER_PNG));
        lv_obj_align_to(app_lv_ui_res_local->img, title_box, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 绘制文本 */
        app_lv_ui_res_local->txt = lv_label_create(app_lv_ui_res_local->scene);
        app_lv_style_object(app_lv_ui_res_local->txt);
        lv_obj_set_width(app_lv_ui_res_local->txt, app_lv_style_hor_pct(80));
        lv_obj_align_to(app_lv_ui_res_local->txt, app_lv_ui_res_local->img, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 绘制日历表 */
        app_lv_ui_res_local->calendar = lv_calendar_create(app_lv_ui_res_local->scene);
        app_lv_style_object(app_lv_ui_res_local->calendar);
        lv_obj_set_style_border_width(app_lv_ui_res_local->calendar, 0, 0);
        lv_obj_align_to(app_lv_ui_res_local->calendar, app_lv_ui_res_local->txt, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
        /* 日历表是矩阵,为之自定义风格 */
        app_lv_ui_res_local->btn_cnt   = 0;
        app_lv_ui_res_local->btn_id_s  = 0;
        app_lv_ui_res_local->btn_id_e  = 0;
        app_lv_ui_res_local->btnmatrix = lv_calendar_get_btnmatrix(app_lv_ui_res_local->calendar);
        lv_obj_set_style_radius(app_lv_ui_res_local->btnmatrix, 45, LV_PART_ITEMS);
        lv_obj_set_style_text_font(app_lv_ui_res_local->btnmatrix, app_lv_ui_font(16), LV_PART_ITEMS);
        lv_obj_add_event_cb(app_lv_ui_res_local->btnmatrix, app_lv_ui_btnmatrix_cb, LV_EVENT_ALL, NULL);
        /* 日历的当月起始和结束 */
        for (uint16_t btn_id = 0; true; btn_id++) {
            const char *txt = lv_btnmatrix_get_btn_text(app_lv_ui_res_local->btnmatrix, btn_id);
            if (txt == NULL)
                break;
            app_lv_ui_res_local->btn_cnt++;
            if (strcmp(txt, "1") == 0) {
                /* 第一个1是当月起始 */
                if (app_lv_ui_res_local->btn_id_s == 0)
                    app_lv_ui_res_local->btn_id_s  = btn_id;
                else
                /* 第二个1是下月起始 */
                if (app_lv_ui_res_local->btn_id_e == 0)
                    app_lv_ui_res_local->btn_id_e  = btn_id - 1;
            }
        }
        app_lv_ui_res_local->btn_cnt -= 7;
        /* 初始化显示动画 */
        app_lv_style_object_anim(app_lv_ui_res_local->scene,
                                &app_lv_ui_res_local->anim, app_lv_ui_local_anim_handler,
                                 LV_ANIM_REPEAT_INFINITE, 0, 3, 1000);
    }
}

/*@brief     界面隐藏
 *@param[in] scene 场景
 */
static void app_lv_ui_women_health_hide(void *scene)
{
    if (app_lv_ui_res_local != NULL) {
        /* 反初始化动画 */
        lv_anim_del(app_lv_ui_res_local->scene, app_lv_ui_local_anim_handler);
        /* 反初始化场景 */
        lv_obj_del(app_lv_ui_res_local->scene);
        ((app_lv_scene_t *)scene)->root = NULL;
        lv_mem_free(app_lv_ui_res_local);
        app_lv_ui_res_local = NULL;
    }
}

app_lv_scene_t app_lv_ui_women_health = {
    /* 场景资源节点 */
    .show = app_lv_ui_women_health_show,
    .hide = app_lv_ui_women_health_hide,
};
