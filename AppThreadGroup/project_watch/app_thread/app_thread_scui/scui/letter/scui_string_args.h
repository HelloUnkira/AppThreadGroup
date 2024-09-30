#ifndef SCUI_STRING_ARGS_H
#define SCUI_STRING_ARGS_H

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
