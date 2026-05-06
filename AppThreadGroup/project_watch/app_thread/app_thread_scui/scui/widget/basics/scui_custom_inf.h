#ifndef SCUI_CUSTOM_INF_H
#define SCUI_CUSTOM_INF_H

/*@brief 自定义控件文本资源回收
 *@param handle 控件句柄
 */
void scui_custom_text_recycle(scui_handle_t handle);

/*@brief 自定义控件获得自定义参数集实例
 *@param handle 控件句柄
 *@param data   自定义参数集实例
 */
void scui_custom_data_inst(scui_handle_t handle, scui_custom_data_t **data);

/*@brief 自定义控件文本绘制(string控件更合适)
 *       备注:偷懒用的,不建议使用,性能低下
 *@param handle 控件句柄
 *@param args   绘制参数(scui_string_args_t)
 *@param text   文本句柄
 */
void scui_custom_draw_text(scui_handle_t handle, void *args, scui_handle_t text);

/*@brief 自定义控件条形码更新
 *@param handle 控件句柄
 *@param data   url字符串
 *@param size   url字符串长度
 */
void scui_custom_update_barcode(scui_handle_t handle);

/*@brief 自定义控件二维码更新
 *@param handle 控件句柄
 *@param data   url字符串
 *@param size   url字符串长度
 */
void scui_custom_update_qrcode(scui_handle_t handle);

/*@brief 自定义控件
 *@param handle 自定义控件句柄
 *@param clip   绘制区域
 *@param data   自定义参数集
 */
#define SCUI_CUSTOM_DRAW_INF_DECLARE(name)  void name(scui_handle_t handle, \
    scui_area_t *clip, scui_custom_data_t *data)
/*****************************************************************************/
SCUI_CUSTOM_DRAW_INF_DECLARE(scui_custom_draw_slider);
SCUI_CUSTOM_DRAW_INF_DECLARE(scui_custom_draw_spinner);
SCUI_CUSTOM_DRAW_INF_DECLARE(scui_custom_draw_indicator);
SCUI_CUSTOM_DRAW_INF_DECLARE(scui_custom_draw_ring_edge);
SCUI_CUSTOM_DRAW_INF_DECLARE(scui_custom_draw_image_text);
SCUI_CUSTOM_DRAW_INF_DECLARE(scui_custom_draw_image_crect4);
/*****************************************************************************/



/* scui_custom_data_t->slider */
#define scui_custom_data_config_slider(custom_data_v,                       \
    bar_v, color_bar_v, edge_v, color_edge_v,                               \
    vmin_v, vmax_v, cmin_v, cmax_v, dist_v, way_v)                          \
do {                                                                        \
    custom_data_v->slider.bar        = bar_v;                               \
    custom_data_v->slider.edge       = edge_v;                              \
    custom_data_v->slider.color_bar  = color_bar_v;                         \
    custom_data_v->slider.color_edge = color_edge_v;                        \
    custom_data_v->slider.vmin       = vmin_v;                              \
    custom_data_v->slider.vmax       = vmax_v;                              \
    custom_data_v->slider.cmin       = cmin_v;                              \
    custom_data_v->slider.cmax       = cmax_v;                              \
    custom_data_v->slider.dist       = dist_v;                              \
    custom_data_v->slider.way        = way_v;                               \
} while (0)                                                                 \

/* scui_custom_data_t->spinner */
#define scui_custom_data_config_spinner(custom_data_v,                      \
    spinner_v, color_v, edge_v, percent_v, angle_s_v, angle_l_v, way_v)     \
do {                                                                        \
    custom_data_v->spinner.spinner = spinner_v;                             \
    custom_data_v->spinner.edge    = edge_v;                                \
    custom_data_v->spinner.color   = color_v;                               \
    custom_data_v->spinner.percent = percent_v;                             \
    custom_data_v->spinner.angle_s = angle_s_v;                             \
    custom_data_v->spinner.angle_l = angle_l_v;                             \
    custom_data_v->spinner.way     = way_v;                                 \
} while (0)                                                                 \

/* scui_custom_data_t->indicator */
#define scui_custom_data_config_indicator(custom_data_v,                    \
    wait_v, color_wait_v, focus_v, color_focus_v,                           \
    count_v, index_v, span_v, way_v)                                        \
do {                                                                        \
    custom_data_v->indicator.wait        = wait_v;                          \
    custom_data_v->indicator.focus       = focus_v;                         \
    custom_data_v->indicator.color_wait  = color_wait_v;                    \
    custom_data_v->indicator.color_focus = color_focus_v;                   \
    custom_data_v->indicator.count       = count_v;                         \
    custom_data_v->indicator.index       = index_v;                         \
    custom_data_v->indicator.span        = span_v;                          \
    custom_data_v->indicator.way         = way_v;                           \
} while (0)                                                                 \

/* scui_custom_data_t->ring_edge */
#define scui_custom_data_config_ring_edge(custom_data_v,                    \
    center_v, image_v, color_v, radius_v, angle_v)                          \
do {                                                                        \
    custom_data_v->ring_edge.image  = image_v;                              \
    custom_data_v->ring_edge.color  = color_v;                              \
    custom_data_v->ring_edge.center = center_v;                             \
    custom_data_v->ring_edge.radius = radius_v;                             \
    custom_data_v->ring_edge.angle  = angle_v;                              \
} while (0)                                                                 \

/* scui_custom_data_t->image_text */
#define scui_custom_data_config_ring_edge(custom_data_v,                    \
    image_v, color_v, span_v, num_v, way_v)                                 \
do {                                                                        \
    custom_data_v->image_text.image = image_v;                              \
    custom_data_v->image_text.color = color_v;                              \
    custom_data_v->image_text.span  = span_v;                               \
    custom_data_v->image_text.way   = way_v;                                \
    custom_data_v->image_text.num   = num_v;                                \
} while (0)                                                                 \
    
    /* scui_custom_data_t->image_crect4 */
#define scui_custom_data_config_image_crect4(custom_data_v,                 \
    image_v, color_v, delta_v)                                              \
do {                                                                        \
    custom_data_v->image_crect4.image[0] = image_v[0];                      \
    custom_data_v->image_crect4.image[1] = image_v[1];                      \
    custom_data_v->image_crect4.image[2] = image_v[2];                      \
    custom_data_v->image_crect4.image[3] = image_v[3];                      \
    custom_data_v->image_crect4.color = color_v;                            \
    custom_data_v->image_crect4.delta = delta_v;                            \
} while (0)                                                                 \



#endif
