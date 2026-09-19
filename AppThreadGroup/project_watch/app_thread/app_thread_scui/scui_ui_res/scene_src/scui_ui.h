#ifndef SCUI_UI_H
#define SCUI_UI_H

/*****************************************************************************/
/* 开机进测试界面 */
#define SCUI_UI_TEST                            (1)
/*****************************************************************************/
/* ui的一些配置参数 */
#define SCUI_UI_SCROLL_BAR_STOP_TIME            (750)
#define SCUI_UI_SCROLL_BAR_FADE_TIME            (750)
#define SCUI_UI_POPUP_ANIM_TIME                 (750)
#define SCUI_UI_POPUP_WAIT_TIME                 (1000)
/*****************************************************************************/
//
//
//
/*****************************************************************************/
typedef enum {
    scui_ui_scene_list_type_s,
    scui_ui_scene_list_type_list_scale,
    scui_ui_scene_list_type_list_arc,
    scui_ui_scene_list_type_honeycomb,
    scui_ui_scene_list_type_waterfall,
    scui_ui_scene_list_type_themewheel,
    scui_ui_scene_list_type_spread,
    scui_ui_scene_list_type_e,
} scui_ui_scene_list_type_t;

/* 应用列表,设置列表,主题等使用: 一张表(image/text/jump 同序合一) */
typedef struct {
    scui_handle_t image;
    scui_handle_t text;
    scui_handle_t jump;
} scui_ui_scene_list_item_t;

extern scui_handle_t  scui_ui_scene_list_num;
extern const scui_ui_scene_list_item_t *scui_ui_scene_list;
void scui_ui_scene_list_cfg(scui_ui_scene_list_type_t type);
/*****************************************************************************/
//
//
//
/*****************************************************************************/
typedef enum {
    scui_ui_scene_mini_card_type_daily_exercise,
    scui_ui_scene_mini_card_type_music_control,
    scui_ui_scene_mini_card_type_hr,
    scui_ui_scene_mini_card_type_spo2,
    scui_ui_scene_mini_card_type_stress,
    scui_ui_scene_mini_card_type_weather,
    scui_ui_scene_mini_card_type_alarm,
    scui_ui_scene_mini_card_type_sleep,
    scui_ui_scene_mini_card_type_sport_record,
    scui_ui_scene_mini_card_type_women_health,
    scui_ui_scene_mini_card_type_compass,
    scui_ui_scene_mini_card_type_altimeter,
    scui_ui_scene_mini_card_type_stopwatch,
    scui_ui_scene_mini_card_type_countdown,
    scui_ui_scene_mini_card_type_world_time,
    scui_ui_scene_mini_card_type_num,
} scui_ui_scene_mini_card_type_t;

/* 小卡片类型表配置 */
extern scui_handle_t scui_ui_scene_mini_card_num;
extern scui_handle_t scui_ui_scene_mini_card_type[];
extern scui_handle_t scui_ui_scene_mini_card_jump[];
void scui_ui_scene_mini_card_cfg(void);
/*****************************************************************************/
//
//
//
/*****************************************************************************/
//
//
//
/*****************************************************************************/

/*@brief 控件事件响应构建
 *@param handle   父控件句柄
 *@param w_h      卡片宽或高
 *@param pos      位置(上/下/左/右)
 *@param event_cb 事件回调
 */
void scui_ui_scene_xfloat_create(scui_handle_t handle, scui_coord_t w_h,
    scui_opt_pos_t pos, scui_event_cb_t event_cb);
/*****************************************************************************/
//
//
//
/*****************************************************************************/
/* 全局弹窗效果 */
void scui_ui_scene_popup_exec(scui_handle_t text, uint8_t *string);
/*****************************************************************************/
//
//
//
/*****************************************************************************/
/* 弧形滚动条效果 */
typedef struct {
    scui_coord_t  bar_pct;
    scui_coord_t  bar_wait;
    scui_coord_t  bar_alpha;
    scui_handle_t bar_handle;
} scui_ui_bar_arc_t;

void scui_ui_bar_arc_reset(scui_ui_bar_arc_t *bar_arc);
void scui_ui_bar_arc_event_proc(scui_ui_bar_arc_t *bar_arc, scui_event_t *event);
/*****************************************************************************/
//
//
//
/*****************************************************************************/
bool scui_ui_scene_test(void);
/*****************************************************************************/
// 回退到上一层
void scui_ui_scene_return(void);
/*****************************************************************************/

#endif
