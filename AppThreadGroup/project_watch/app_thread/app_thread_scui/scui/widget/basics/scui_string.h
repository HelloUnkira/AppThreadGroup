#ifndef SCUI_STRING_H
#define SCUI_STRING_H

typedef struct {
    scui_coord_t       *index_ls;       // 索引起始点序列
    scui_coord_t       *index_le;       // 索引结束点序列
    scui_color_t       *color_ll;       // 范围颜色值序列
    scui_coord_t        color_num;      // 范围数量
} scui_string_rec_t;

typedef struct {
    scui_color_t       *grad_s;         // 字符渐变序列表
    scui_coord_t        grad_n;         // 字符渐变序列表数量
    uintptr_t           grad_w:1;       // 字符颜色渐变方向(0:hor;1:ver;)
} scui_string_grad_t;

typedef struct {
    scui_coord_t        line_mum;       // 行数量
    scui_coord_t       *line_ofs;       // 行索引偏移
    scui_coord_t       *line_width;     // 行当前宽度
} scui_string_typo_t;

typedef struct {
    scui_color_t        color;          // 字符串颜色
    scui_coord_t        line_width;     // 线宽
    scui_string_rec_t  *colors;         // 字符复色序列表
    scui_string_grad_t *grads;          // 字符渐变序列表
    uintptr_t           regrad:1;       // 字符颜色渐变使能
    uintptr_t           recolor:1;      // 字符颜色复色使能
    uintptr_t           line_under:1;   // 下划线
    uintptr_t           line_delete:1;  // 删除线
    uintptr_t           gap_line:7;     // 行间距(<= 127)
    uintptr_t           gap_item:7;     // 字间距(<= 127)
    uintptr_t           gap_none:7;     // 空字符(<= 127)
    uintptr_t           align_hor:2;    // 排版对齐(0:左对齐;1:右对齐;2:中心对齐)
    uintptr_t           align_ver:2;    // 排版对齐(0:上对齐;1:下对齐;2:中心对齐)
    uintptr_t           mode_scroll:2;  // 滚动模式(0:左右滚动;1:轮转滚动)
    uintptr_t           line_multi:1;   // 多行模式(非默认)
    scui_coord_t        size;           // 字库尺寸
    scui_handle_t       name;           // 字库句柄
    scui_handle_t       lang;           // 语言编号
    /* 外部域转内部域: */
    scui_area_t         clip;           // 绘制剪切域
    uint8_t            *utf8;           // 字符串(utf8)
    /* 内部域: */
    scui_string_typo_t *typo;           // 排版信息(多行模式使用)
    scui_coord_t        width;          // 单行模式宽度
    scui_coord_t        height;         // 多行模式高度
    scui_coord_t        limit;          // 滚动限制量
    scui_coord_t        offset;         // 滚动偏移量
    scui_coord_t        number;         // 字符数量
    uint32_t           *unicode;        // 字符串(unicode)
    uintptr_t           update:1;       // 更新标记
} scui_string_args_t;

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t       text;           // 字符串
    scui_handle_t       font_idx;       // 字库索引
    scui_handle_t       lang_type;      // 语言类型
    scui_string_args_t  args;           // 字符串绘制参数
    scui_coord_t        unit_ms;        // 滚动单元时间
    scui_coord_t        unit_dx;        // 滚动单元距离
    uintptr_t           unit_s:1;       // 滚动模式(单次滚动,结束后不再滚动)
    uintptr_t           draw_cache:1;   // 绘制缓存块使能
    /* 内部域: */
    scui_surface_t     *draw_surface;   // 绘制缓存块
    uintptr_t           unit_anima:1;   // 滚动动画标记
    uintptr_t           unit_abort:1;   // 滚动中止标记
    uintptr_t           unit_over:1;    // 滚动结束标记
    scui_coord_t        unit_way;       // 滚动单元方向
    scui_coord_t        rcd_ms;
    uint8_t            *str_utf8;       // 字符串(utf8)
} scui_string_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_handle_t       text;           // 字符串
    scui_handle_t       font_idx;       // 字库索引
    scui_handle_t       lang_type;      // 语言类型
    scui_string_args_t  args;           // 字符串绘制参数
    scui_coord_t        unit_ms;        // 滚动单元时间
    scui_coord_t        unit_dx;        // 滚动单元距离
    uintptr_t           unit_s:1;       // 滚动模式(单次滚动,结束后不再滚动)
    uintptr_t           draw_cache:1;   // 绘制缓存块使能
} scui_string_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 *@param layout     通过布局创建
 */
void scui_string_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_string_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_string_event(scui_event_t *event);

/*@brief 字符串处理
 *@param args 字符串绘制参数
 */
void scui_string_args_process(scui_string_args_t *args);

/*****************************************************************************/
/* utf-8 unicode tools<s>:************************************************** */
/*****************************************************************************/

/*@brief utf8字符转unicode字符
 *@param utf8    utf8字符
 *@param unicode unicode字符
 *@retval utf8字符字节数
 */
uint32_t scui_utf8_to_unicode(uint8_t *utf8, uint32_t *unicode);

/*@brief utf8字符字节数
 *@param utf8 字符(首字符)
 *@retval 字符长度
 */
uint32_t scui_utf8_bytes(uint8_t utf8);

/*@brief utf8字符数量
 *@param utf8 字符串
 *@retval 字符数量
 */
uint32_t scui_utf8_str_num(uint8_t *utf8);

/*@brief utf8字节数量
 *@param utf8 字符串
 *@retval 字节数量
 */
uint32_t scui_utf8_str_bytes(uint8_t *utf8);

/*@brief utf8字符串转为unicode字符串
 *@param unicode  unicode字符串
 *@param utf8     utf8字符串
 *@param utf8_num utf8字符数
 */
void scui_utf8_str_to_unicode(uint8_t *utf8, uint32_t utf8_num, uint32_t *unicode);

/*@brief 后缀匹配
 *@param str    utf8字符串
 *@param suffix utf8字符串
 */
bool scui_utf8_str_match_suffix(uint8_t *str, uint8_t *suffix);

/*****************************************************************************/
/* utf-8 unicode tools<e>:************************************************** */
/*****************************************************************************/

#endif
