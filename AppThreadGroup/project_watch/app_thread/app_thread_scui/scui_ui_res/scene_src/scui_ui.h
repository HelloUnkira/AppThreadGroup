#ifndef SCUI_UI_H
#define SCUI_UI_H

/*****************************************************************************/
/* ui的一些配置参数 */
#define SCUI_UI_SCROLL_BAR_STOP_TIME            (750)
#define SCUI_UI_SCROLL_BAR_FADE_TIME            (750)
#define SCUI_UI_POPUP_STOP_TIME                 (1000)
#define SCUI_UI_POPUP_FADE_TIME                 (750)
/*****************************************************************************/
//
//
//
/*****************************************************************************/
/* 主题:蜂窝(所有图标为正方形图标) */
#define SCUI_UI_HONEYCOMB_EDGE_MODE             (0)     // 边界模式(0:圆屏; 1:方屏)
#define SCUI_UI_HONEYCOMB_LIST_NUM              (37)    // 图标数量
#define SCUI_UI_HONEYCOMB_OFS_DEF               (2)     // 默认图标尺寸偏移
#define SCUI_UI_HONEYCOMB_OFS_MIN               (0)     // 最小缩小图标尺寸偏移
#define SCUI_UI_HONEYCOMB_OFS_MAX               (8)     // 最大放大图标尺寸偏移
#define SCUI_UI_HONEYCOMB_SPACE                 (8)     // 图标间隙
/* 主题:蜂窝 初始布局构建参数 */
scui_coord_t scui_ui_honeycomb_dif_x(scui_handle_t ofs_cur);
scui_coord_t scui_ui_honeycomb_dif_y(scui_handle_t ofs_cur);
scui_coord_t scui_ui_honeycomb_mid_x(scui_handle_t ofs_cur);
scui_coord_t scui_ui_honeycomb_mid_y(scui_handle_t ofs_cur);
/* 主题:蜂窝初始布局 */
void scui_ui_honeycomb_list_layout(scui_point_t *list_layout, scui_handle_t ofs_cur);
/*****************************************************************************/
/* 主题:瀑布(所有图标为正方形图标) */
#define SCUI_UI_WATERFALL_OFS_MIN               (3)     // 最小缩小图标尺寸偏移
#define SCUI_UI_WATERFALL_OFS_MAX               (7)     // 最大放大图标尺寸偏移
#define SCUI_UI_WATERFALL_LINE_SPACE            (8)     // 图标垂直间隙
/*****************************************************************************/
/* 主题:波轮(所有图标为正方形图标) */
#define SCUI_UI_THEMEWHEEL_OFS_MIN              (0)     // 图标尺寸偏移
#define SCUI_UI_THEMEWHEEL_OFS_MAX              (5)     // 图标尺寸偏移
#define SCUI_UI_THEMEWHEEL_ANGLE_UNIT           (24)    // 一圈数量 == 360 / angle_unit
#define SCUI_UI_THEMEWHEEL_ANGLE_CELL           (6)     // 动画颗粒度,一般不改
#define SCUI_UI_THEMEWHEEL_IMAGE_ARROW          (scui_image_prj_image_src_repeat_arrow_05_backbmp)
/*****************************************************************************/
/* 主题:扩散(所有图标为正方形图标) */
#define SCUI_UI_SPREAD_OFS_MIN                  (0)     // 图标尺寸偏移
#define SCUI_UI_SPREAD_OFS_MAX                  (6)     // 图标尺寸偏移
#define SCUI_UI_SPREAD_ANGLE_UNIT               (45)    // 一圈数量 == 360 / angle_unit
#define SCUI_UI_SPREAD_ANGLE_CELL               (8)     // 动画颗粒度,一般不改
#define SCUI_UI_SPREAD_RAD_FADE                 (5)     // 消失圆半径
#define SCUI_UI_SPREAD_RAD_MIN                  ((SCUI_DRV_HOR_RES - 10 * 2) / 2 / 4)   /* 小圈圆半径(静止时) */
#define SCUI_UI_SPREAD_DIS_MAX                  ((SCUI_DRV_HOR_RES - 10 * 2) / 2 / 2)   /* 小圈圆到大圈圆距离(静止时) */
#define SCUI_UI_SPREAD_FLING_MODE               (1)     // 0:normal;1:circle;
#define SCUI_UI_SPREAD_FLING_WAY                (0)     // 0:any;1:hori;2:vert;
/*****************************************************************************/
//
//
//
/*****************************************************************************/
typedef enum {
    scui_ui_scene_list_type_list_scale,
    scui_ui_scene_list_type_list_arc,
    scui_ui_scene_list_type_honeycomb,
    scui_ui_scene_list_type_waterfall,
    scui_ui_scene_list_type_themewheel,
    scui_ui_scene_list_type_spread,
} scui_ui_scene_list_type_t;

/* 应用列表,设置列表,主题等使用 */
extern scui_handle_t  scui_ui_scene_list_num;
extern scui_handle_t *scui_ui_scene_list_image;
extern scui_handle_t *scui_ui_scene_list_text;
extern scui_handle_t *scui_ui_scene_list_jump;
void scui_ui_scene_list_cfg(scui_ui_scene_list_type_t type);
/*****************************************************************************/
//
//
//
/*****************************************************************************/
/*@brief 窗口关联配置
 *@param event 事件
 */
void scui_ui_scene_link_cfg(scui_event_t *event);
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
/* 全局弧形滚动条效果 */
void scui_ui_bar_arc_pct(scui_handle_t pct);
void scui_ui_bar_arc_reset(scui_handle_t handle);

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_bar_arc_event_proc(scui_event_t *event);
/*****************************************************************************/

#endif
