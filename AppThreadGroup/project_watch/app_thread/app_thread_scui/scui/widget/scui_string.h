#ifndef SCUI_STRING_H
#define SCUI_STRING_H

typedef struct {
    scui_coord_t *index_ls;
    scui_coord_t *index_le;
    scui_color_t *color_ll;
    uint32_t      color_num;
} scui_string_rec_t;

typedef struct {
    uint32_t            line_mum;
    uint32_t           *line_ofs;
    uint32_t           *line_width;
} scui_string_typo_t;

typedef struct {
    scui_color_t        color;          // 字符串颜色
    scui_string_rec_t  *colors;         // 字符颜色值表
    scui_color_t       *grad_s;         // 字符渐变序列表
    uint32_t            grad_n;         // 字符渐变序列表数量
    uint64_t            grad_w:1;       // 字符颜色渐变方向(0:hor;1:ver;)
    uint64_t            regrad:1;       // 字符颜色渐变使能
    uint64_t            recolor:1;      // 字符颜色值表使能
    uint64_t            gap_line:7;     // 行间距(<= 127)
    uint64_t            gap_item:7;     // 字间距(<= 127)
    uint64_t            gap_none:7;     // 空字符(<= 127)
    uint64_t            align_hor:2;    // 排版对齐(0:左对齐;1:右对齐;2:中心对齐)
    uint64_t            align_ver:2;    // 排版对齐(0:上对齐;1:下对齐;2:中心对齐)
    uint64_t            mode_scroll:2;  // 滚动模式(0:左右滚动;1:轮转滚动)
    uint64_t            line_multi:1;   // 多行模式(非默认)
    /* 外部域转内部域: */
    scui_area_t         clip;           // 绘制剪切域
    scui_handle_t       name;           // 字库文字句柄
    uint8_t            *utf8;           // 字符串(utf8)
    /* 内部域: */
    scui_string_typo_t *typo;           // 排版信息(多行模式使用)
    scui_coord_t        width;          // 单行模式宽度
    scui_coord_t        height;         // 多行模式高度
    scui_coord_t        limit;          // 滚动限制量
    scui_coord_t        offset;         // 滚动偏移量
    uint32_t            number;         // 字符数量
    uint32_t           *unicode;        // 字符串(unicode)
    uint64_t            update:1;       // 更新标记
} scui_string_args_t;

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
