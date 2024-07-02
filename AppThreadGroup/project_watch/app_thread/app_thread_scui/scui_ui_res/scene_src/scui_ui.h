#ifndef SCUI_UI_H
#define SCUI_UI_H

/* 主题:蜂窝(所有图标为正方形图标) */
#define SCUI_UI_HONEYCOMB_EDGE_MODE             (0)     // 边界模式(0:圆屏; 1:方屏)
#define SCUI_UI_HONEYCOMB_LIST_NUM              (37)    // 图标数量
#define SCUI_UI_HONEYCOMB_OFS_DEF               (2)     // 默认图标尺寸偏移
#define SCUI_UI_HONEYCOMB_OFS_MIN               (0)     // 最小缩小图标尺寸偏移
#define SCUI_UI_HONEYCOMB_OFS_MAX               (8)     // 最大放大图标尺寸偏移
#define SCUI_UI_HONEYCOMB_SPACE                 (8)     // 图标间隙

// 蜂窝初始布局构建参数
scui_coord_t scui_ui_honeycomb_dif_x(scui_handle_t ofs_cur);
scui_coord_t scui_ui_honeycomb_dif_y(scui_handle_t ofs_cur);
scui_coord_t scui_ui_honeycomb_mid_x(scui_handle_t ofs_cur);
scui_coord_t scui_ui_honeycomb_mid_y(scui_handle_t ofs_cur);

// 蜂窝信息,蜂窝初始布局
extern const scui_handle_t scui_ui_honeycomb_list_image[SCUI_UI_HONEYCOMB_LIST_NUM];
extern const scui_handle_t scui_ui_honeycomb_list_jump[SCUI_UI_HONEYCOMB_LIST_NUM];
void scui_ui_honeycomb_list_layout(scui_point_t *list_layout, scui_handle_t ofs_cur);



/* 应用列表,设置列表等使用 */
extern scui_handle_t  scui_ui_scene_list_num;
extern scui_handle_t *scui_ui_scene_list_image;
extern scui_handle_t *scui_ui_scene_list_text;
extern scui_handle_t *scui_ui_scene_list_jump;
void scui_ui_scene_list_cfg(uint8_t type);

/*@brief 窗口关联配置
 *@param event 事件
 */
void scui_ui_scene_link_cfg(scui_event_t *event);

/* 全局弹窗效果 */
void scui_ui_scene_popup_exec(scui_handle_t text, uint8_t *string);

/* 全局弧形滚动条效果 */
void scui_ui_bar_arc_pct(scui_handle_t pct);
void scui_ui_bar_arc_reset(scui_handle_t handle);

/*@全局弧形滚动条: 控件事件响应回调
 *@param event 事件
 */
void scui_ui_bar_arc_event_proc(scui_event_t *event);

#endif
