#ifndef SCUI_STRING_H
#define SCUI_STRING_H

typedef struct {
    scui_widget_t       widget;
    scui_handle_t       font_idx;       // 字库尺寸
    scui_handle_t       text;           // 字符串
    scui_string_args_t  args;           // 字符串绘制参数
    scui_coord_t        unit_ms;        // 滚动单元时间
    scui_coord_t        unit_dx;        // 滚动单元距离
    uint64_t            unit_s:1;       // 滚动模式(单次滚动,结束后不再滚动)
    uint64_t            draw_cache:1;   // 绘制缓存块
    scui_surface_t     *draw_surface;   // 绘制缓存块
    /* 内部域: */
    uint64_t            unit_anima:1;   // 滚动动画标记
    uint64_t            unit_abort:1;   // 滚动中止标记
    uint64_t            unit_over:1;    // 滚动结束
    scui_coord_t        unit_way;       // 滚动单元方向
    scui_coord_t        rcd_ms;
    scui_handle_t       name;           // 字库名字
    uint8_t            *str_utf8;       // 字符串(utf8)
} scui_string_t;

#pragma pack(push, 1)
typedef struct {
    scui_widget_maker_t     widget;
    scui_handle_t           font_idx;       // 字库尺寸
    scui_handle_t           text;           // 字符串
    scui_string_args_t      args;           // 字符串绘制参数
    scui_coord_t            unit_ms;        // 滚动单元时间
    scui_coord_t            unit_dx;        // 滚动单元距离
    uint64_t                unit_s:1;       // 滚动模式(单次滚动,结束后不再滚动)
    uint64_t                draw_cache:1;   // 绘制缓存块
} scui_string_maker_t;
#pragma pack(pop)

/*@brief 字符串控件创建
 *@param maker  字符串控件创建参数
 *@param handle 字符串控件句柄
 *@param layout 通过布局创建
 */
void scui_string_create(scui_string_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 字符串控件销毁
 *@param handle 字符串控件句柄
 */
void scui_string_destroy(scui_handle_t handle);

/*@brief 字符串控件更新文本
 *@param handle 字符串控件句柄
 *@param text   字符串句柄
 */
void scui_string_update_text(scui_handle_t handle, scui_handle_t text);

/*@brief 字符串控件更新文本
 *@param handle   字符串控件句柄
 *@param str_utf8 字符串(utf8)
 */
void scui_string_update_str(scui_handle_t handle, uint8_t *str_utf8);

/*@brief 字符串控件更新文本(重上色)
 *@param 使用#- -#包裹的内容为重上色区域
 *@param handle    字符串控件句柄
 *@param str_utf8  字符串(utf8)
 *@param color_num 重上色颜色数量
 *@param color_ll  重上色颜色列表
 */
void scui_string_update_str_rec(scui_handle_t handle, uint8_t *str_utf8, uint32_t color_num, scui_color_t *color_ll);

/*@brief 字符串控件渐变序列更新
 *@param handle 字符串控件句柄
 *@param grad_s 渐变序列
 *@param grad_n 渐变序列数量
 */
void scui_string_upgrade_grads(scui_handle_t handle, scui_color_t *grad_s, uint32_t grad_n);

/*@brief 字符串控件滚动中止
 *@param handle 字符串控件句柄
 *@param active 中止标记
 */
void scui_string_scroll_abort(scui_handle_t handle, bool abort);

/*@brief 字符串控件滚动结束(单次滚动)
 *@param handle   字符串控件句柄
 *@retval 是否结束
 */
bool scui_string_scroll_over(scui_handle_t handle);

/*@brief 字符串控件事件处理回调
 *@param event 事件
 */
void scui_string_event(scui_event_t *event);

#endif