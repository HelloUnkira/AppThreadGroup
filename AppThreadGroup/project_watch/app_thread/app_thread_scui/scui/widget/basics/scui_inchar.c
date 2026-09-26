/*实现目标:
 *    控件(输入字符)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_inchar_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 继承对象 */
    scui_string_t *string = widget;
    scui_string_maker_t *string_maker = widget_maker;
    /* 本类对象 */
    scui_inchar_t *inchar = widget;
    scui_inchar_maker_t *inchar_maker = widget_maker;
    
    /* 必须标记anima事件 */
    widget_maker->style.sched_anima = true;
    
    /* 限制: 单色文本输入(禁用绘制缓存/级联上色/复色/划线) */
    string_maker->draw_cache   = false;
    string_maker->args.regrad  = false;
    string_maker->args.recolor = false;
    string_maker->args.line_under  = false;
    string_maker->args.line_delete = false;
    
    /* 构造派生控件实例 */
    scui_string_make(string, string_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_inchar));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    SCUI_ASSERT(inchar_maker->limit > 0);
    inchar->limit = inchar_maker->limit;
    inchar->index = inchar_maker->index;
    inchar->color = inchar_maker->color;
    inchar->hide  = inchar_maker->hide;
    
    /* 方向标记(按对齐方向初始化: 右对齐=前向; 左/中对齐=后向) */
    inchar->prev = (string_maker->args.align_hor == 1);
    
    /* 资源分配与参数同步(utf8按最大字符宽度构建) */
    inchar->str_utf8  = SCUI_MEM_ZALLOC(scui_mem_type_mix, (inchar->limit + 1) * 4);
    inchar->str_uni   = SCUI_MEM_ZALLOC(scui_mem_type_mix, (inchar->limit + 1) * sizeof(uint32_t));
    inchar->ofs_utf8  = SCUI_MEM_ZALLOC(scui_mem_type_mix, (inchar->limit + 1) * sizeof(scui_coord_t));
    inchar->str_width = SCUI_MEM_ZALLOC(scui_mem_type_mix, (inchar->limit + 1) * sizeof(scui_coord_t));
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_inchar_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    /* 释放本类资源 */
    SCUI_MEM_FREE(inchar->str_uni);
    SCUI_MEM_FREE(inchar->str_utf8);
    SCUI_MEM_FREE(inchar->ofs_utf8);
    SCUI_MEM_FREE(inchar->str_width);
    
    /* 析构派生控件实例 */
    scui_string_burn(widget->myself);
}

/*@brief 输入字符控件方向标记
 *@param handle 输入字符控件句柄
 *@param prev   方向标记(前向:真;后向:假)
 */
void scui_inchar_char_ins_way(scui_handle_t handle, bool prev)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    inchar->prev = prev;
}

/*@brief 输入字符控件指定位置输入字符
 *@param handle   输入字符控件句柄
 *@param index    字符索引
 *@param str_utf8 字符(utf8字符串)
 */
void scui_inchar_char_ins(scui_handle_t handle, scui_coord_t index, uint8_t *str_utf8)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    /* 字符(utf8字符串)长度 */
    uint32_t str_num = scui_utf8_str_num(str_utf8);
    uint32_t str_bytes = scui_utf8_str_bytes(str_utf8);
    SCUI_ASSERT(str_num != 0);
    
    /* 最大字符数量(越界断言) */
    SCUI_ASSERT(inchar->str_num + (scui_coord_t)str_num <= inchar->limit);
    
    /* 插入位置(越界夹取) */
    if (index < 0) index = 0;
    if (index > inchar->str_num) index = inchar->str_num;
    
    /* 插入位置字节偏移(面向数组) */
    scui_coord_t ofs_ins = inchar->ofs_utf8[index];
    scui_coord_t str_bytes_c = scui_utf8_str_bytes(inchar->str_utf8);
    memmove(inchar->str_utf8 + ofs_ins + str_bytes, inchar->str_utf8 + ofs_ins, str_bytes_c - ofs_ins + 1);
    memcpy(inchar->str_utf8 + ofs_ins, str_utf8, str_bytes);
    
    /* 字符数组就地维护(偏移/宽度/unicode) */
    memmove(&inchar->ofs_utf8[index + str_num], &inchar->ofs_utf8[index], (inchar->str_num - index + 1) * sizeof(scui_coord_t));
    for (scui_coord_t idx = index + str_num; idx <= inchar->str_num + (scui_coord_t)str_num; idx++) inchar->ofs_utf8[idx] += str_bytes;
    
    memmove(&inchar->str_uni[index + str_num], &inchar->str_uni[index], (inchar->str_num - index) * sizeof(uint32_t));
    memmove(&inchar->str_width[index + str_num], &inchar->str_width[index], (inchar->str_num - index) * sizeof(scui_coord_t));
    
    uint8_t *utf8_p = str_utf8;
    for (scui_coord_t idx = 0; idx < str_num; idx++) {
        uint32_t unicode = 0;
        uint32_t utf8_len = scui_utf8_to_uni(utf8_p, &unicode);
        SCUI_ASSERT(utf8_len != 0);
        inchar->str_uni[index + idx] = unicode;
        
        /* 字符宽度(glyph就地读取, 永续保存) */
        scui_cache_glyph_unit_t glyph_unit = {0};
        glyph_unit.size = string->args.size;
        glyph_unit.name = string->args.name;
        glyph_unit.glyph.space_width = string->args.gap_none;
        glyph_unit.glyph.unicode_letter = unicode;
        scui_cache_glyph_load(&glyph_unit);
        scui_cache_glyph_unload(&glyph_unit);
        inchar->str_width[index + idx] = glyph_unit.glyph.bitmap == NULL ? string->args.gap_none : glyph_unit.glyph.adv_w;
        
        inchar->ofs_utf8[index + idx] = ofs_ins + (utf8_p - str_utf8);
        utf8_p += utf8_len;
    }
    inchar->str_num += (scui_coord_t)str_num;
    
    /* 光标(插入后到达最新字符索引, 保持稳定顺序序列) */
    inchar->index = index + (scui_coord_t)str_num - 1;
    
    /* 刷新排版(utf8更新) */
    scui_string_update_str(handle, inchar->str_utf8);
    /* 刷新光标区域(重算) */
    scui_inchar_cursor_set(handle, inchar->index);
}

/*@brief 输入字符控件指定位置替换字符
 *@param handle   输入字符控件句柄
 *@param index    字符索引(定点替换本身)
 *@param str_utf8 字符(utf8字符串)
 */
void scui_inchar_char_rep(scui_handle_t handle, scui_coord_t index, uint8_t *str_utf8)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    /* 字符(utf8字符串)长度(定点替换单字符) */
    uint32_t str_num = scui_utf8_str_num(str_utf8);
    uint32_t str_bytes = scui_utf8_str_bytes(str_utf8);
    SCUI_ASSERT(str_num == 1);
    
    /* 替换位置(越界夹取/空文本不替换) */
    if (index < 0) index = 0;
    if (index >= inchar->str_num)
        return;
    
    /* 替换位置字节偏移与新旧字节差(面向数组) */
    scui_coord_t ofs_rep = inchar->ofs_utf8[index];
    uint32_t old_bytes = scui_utf8_bytes(inchar->str_utf8[ofs_rep]);
    scui_coord_t bytes_diff = (scui_coord_t)str_bytes - (scui_coord_t)old_bytes;
    if (bytes_diff != 0) {
        scui_coord_t str_bytes_c = scui_utf8_str_bytes(inchar->str_utf8);
        memmove(inchar->str_utf8 + ofs_rep + str_bytes,
            inchar->str_utf8 + ofs_rep + old_bytes, str_bytes_c - ofs_rep - old_bytes + 1);
    }
    memcpy(inchar->str_utf8 + ofs_rep, str_utf8, str_bytes);
    
    /* 字符数组就地维护(后续偏移随字节差修正) */
    for (scui_coord_t idx = index + 1; idx <= inchar->str_num; idx++) inchar->ofs_utf8[idx] += bytes_diff;
    
    /* 字符信息原位更新(unicode/宽度) */
    uint32_t unicode = 0;
    uint32_t utf8_len = scui_utf8_to_uni(str_utf8, &unicode);
    SCUI_ASSERT(utf8_len != 0);
    inchar->str_uni[index] = unicode;
    
    scui_cache_glyph_unit_t glyph_unit = {0};
    glyph_unit.size = string->args.size;
    glyph_unit.name = string->args.name;
    glyph_unit.glyph.space_width = string->args.gap_none;
    glyph_unit.glyph.unicode_letter = unicode;
    scui_cache_glyph_load(&glyph_unit);
    scui_cache_glyph_unload(&glyph_unit);
    inchar->str_width[index] = glyph_unit.glyph.bitmap == NULL ? string->args.gap_none : glyph_unit.glyph.adv_w;
    
    /* 光标(原地替换, 光标不变, 重算显示区域) */
    inchar->index = index;
    
    /* 刷新排版(utf8更新) */
    scui_string_update_str(handle, inchar->str_utf8);
    /* 刷新光标区域(重算) */
    scui_inchar_cursor_set(handle, inchar->index);
}

/*@brief 输入字符控件指定位置删除字符
 *@param handle 输入字符控件句柄
 *@param index  字符索引(定点删除本身)
 */
void scui_inchar_char_rem(scui_handle_t handle, scui_coord_t index)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    /* 删除位置(越界夹取) */
    if (index < 0) index = 0;
    if (index >= inchar->str_num)
        return;
    
    /* 删除字节数与偏移(面向数组) */
    scui_coord_t ofs_del = inchar->ofs_utf8[index];
    uint32_t del_bytes = scui_utf8_bytes(inchar->str_utf8[ofs_del]);
    scui_coord_t str_bytes_c = scui_utf8_str_bytes(inchar->str_utf8);
    memmove(inchar->str_utf8 + ofs_del, inchar->str_utf8 + ofs_del + del_bytes, str_bytes_c - ofs_del - del_bytes + 1);
    
    /* 字符数组就地维护(偏移/宽度/unicode) */
    for (scui_coord_t idx = index + 1; idx <= inchar->str_num; idx++) inchar->ofs_utf8[idx] -= del_bytes;
    memmove(&inchar->ofs_utf8[index], &inchar->ofs_utf8[index + 1], (inchar->str_num - index) * sizeof(scui_coord_t));
    memmove(&inchar->str_uni[index], &inchar->str_uni[index + 1], (inchar->str_num - index - 1) * sizeof(uint32_t));
    memmove(&inchar->str_width[index], &inchar->str_width[index + 1], (inchar->str_num - index - 1) * sizeof(scui_coord_t));
    inchar->str_num--;
    
    /* 光标保持原位(和替换一致), 仅端点删除(末尾)收敛到新边界 */
    if (inchar->str_num > 0 && inchar->index >= inchar->str_num)
        inchar->index = inchar->str_num - 1;
    
    /* 刷新排版(utf8更新) */
    scui_string_update_str(handle, inchar->str_utf8);
    /* 刷新光标区域(重算) */
    scui_inchar_cursor_set(handle, inchar->index);
}

/*@brief 输入字符控件输入字符(换算为指定位置插入)
 *@param handle   输入字符控件句柄
 *@param str_utf8 字符(utf8字符串)
 */
void scui_inchar_char_add(scui_handle_t handle, uint8_t *str_utf8)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    /* 光标基准(隐藏模式固定末尾) */
    scui_coord_t cursor_idx = inchar->hide ? inchar->str_num : inchar->index;
    if (cursor_idx > inchar->str_num) cursor_idx = inchar->str_num;
    if (cursor_idx < 0) cursor_idx = 0;
    
    /* 插入位置(方向标记: 前向;后向) */
    scui_coord_t ins_idx = inchar->prev ? cursor_idx : cursor_idx + 1;
    if (ins_idx > inchar->str_num) ins_idx = inchar->str_num;
    
    scui_inchar_char_ins(handle, ins_idx, str_utf8);
}

/*@brief 输入字符控件删除字符
 *@param handle 输入字符控件句柄
 */
void scui_inchar_char_del(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    /* 删除位置(隐藏模式固定末尾, 常规光标位置) */
    scui_coord_t del_idx = inchar->hide ? inchar->str_num - 1 : inchar->index;
    if (inchar->str_num <= 0 || del_idx < 0) return;
    
    scui_inchar_char_rem(handle, del_idx);
}

/*@brief 输入字符控件字符数量
 *@param handle 输入字符控件句柄
 *@param num    字符数量
 */
void scui_inchar_char_num(scui_handle_t handle, scui_coord_t *num)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    *num = inchar->str_num;
}

/*@brief 输入字符控件utf8字节数量
 *@param handle 输入字符控件句柄
 *@param num    utf8字节数量
 */
void scui_inchar_str_num(scui_handle_t handle, scui_coord_t *num)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    *num = (scui_coord_t)scui_utf8_str_bytes(inchar->str_utf8);
}

/*@brief 输入字符控件取出utf8
 *@param handle   输入字符控件句柄
 *@param str_utf8 字符串(utf8)
 */
void scui_inchar_str_utf8(scui_handle_t handle, uint8_t *str_utf8)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    memcpy(str_utf8, inchar->str_utf8, scui_utf8_str_bytes(inchar->str_utf8) + 1);
}

/*@brief 输入字符控件光标显示
 *@param handle 输入字符控件句柄
 *@param hide   隐藏标记(隐藏时固定末尾编辑)
 */
void scui_inchar_cursor(scui_handle_t handle, bool hide)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    inchar->hide = hide;
    scui_widget_draw(handle, NULL, false, 0);
}

/*@brief 输入字符控件设置光标
 *@param handle 输入字符控件句柄
 *@param index  光标字符索引
 */
void scui_inchar_cursor_set(scui_handle_t handle, scui_coord_t index)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    /* 光标索引(有字符时落在字符索引位, 无字符时0) */
    if (inchar->str_num > 0) inchar->index = scui_clamp(index, 0, inchar->str_num - 1);
    else inchar->index = 0;
    
    /* 光标区域计算(排版参数) */
    if (string->args.name != SCUI_HANDLE_INVALID) {
        /* 从字库中提取行高 */
        scui_cache_font_unit_t font_unit = {0};
        font_unit.name = string->args.name;
        font_unit.size = string->args.size;
        scui_cache_font_load(&font_unit);
        scui_coord_t line_height = scui_font_line_height(font_unit.font);
        scui_cache_font_unload(&font_unit);
        
        scui_coord_t cursor_x = 0;
        scui_coord_t cursor_y = 0;
        scui_coord_t cursor_w = 0;
        
        if (string->args.line_multi && string->args.typo != NULL) {
            /* 多行: 光标行定位 */
            scui_string_typo_t *typo = string->args.typo;
            scui_coord_t line_idx = 0;
            for (scui_coord_t idx_line = 0; idx_line < typo->line_num; idx_line++) {
                line_idx = idx_line;
                if (inchar->index <= typo->line_ofs_e[idx_line])
                    break;
            }
            
            /* 行内列宽累加(字符宽度数组) */
            for (scui_coord_t idx = typo->line_ofs_s[line_idx]; idx < inchar->index; idx++)
                cursor_x += inchar->str_width[idx] + string->args.gap_item;
            
            /* 行内水平对齐 */
            scui_coord_t line_w = typo->line_width[line_idx];
            if (string->args.align_hor == 1) cursor_x += (widget->clip.w - line_w);
            if (string->args.align_hor == 2) cursor_x += (widget->clip.w - line_w) / 2;
            
            /* 行定位与垂直滚动偏移 */
            cursor_y = line_idx * (line_height + string->args.gap_line) + string->args.offset;
            
            /* 光标宽度(空文本为字号/空格宽; 末尾取前一字符宽) */
            cursor_w = inchar->str_num == 0 ?
                (string->args.size > 0 ? string->args.size : string->args.gap_none) :
                (inchar->index < inchar->str_num ? inchar->str_width[inchar->index] :
                (inchar->index > 0 ? inchar->str_width[inchar->index - 1] :
                (string->args.size > 0 ? string->args.size : string->args.gap_none)));
        } else {
            /* 单行: 列宽累加(字符宽度数组) */
            for (scui_coord_t idx = 0; idx < inchar->index; idx++)
                cursor_x += inchar->str_width[idx] + string->args.gap_item;
            
            /* 水平滚动偏移 */
            cursor_x += string->args.offset;
            
            /* 水平对齐(文本不超宽时生效) */
            scui_coord_t limit = string->args.width - widget->clip.w;
            if (limit <= 0) {
                if (string->args.align_hor == 1) cursor_x += -limit;
                if (string->args.align_hor == 2) cursor_x += -limit / 2;
            }
            
            /* 垂直对齐 */
            if (string->args.align_ver == 1) cursor_y = (widget->clip.h - line_height);
            if (string->args.align_ver == 2) cursor_y = (widget->clip.h - line_height) / 2;
            
            /* 光标宽度(空文本为字号/空格宽; 末尾取前一字符宽) */
            cursor_w = inchar->str_num == 0 ?
                (string->args.size > 0 ? string->args.size : string->args.gap_none) :
                (inchar->index < inchar->str_num ? inchar->str_width[inchar->index] :
                (inchar->index > 0 ? inchar->str_width[inchar->index - 1] :
                (string->args.size > 0 ? string->args.size : string->args.gap_none)));
        }
        
        inchar->area.x = cursor_x;
        inchar->area.y = cursor_y;
        inchar->area.w = cursor_w;
        inchar->area.h = line_height;
    }
    
    scui_widget_draw(handle, NULL, false, 0);
}

/*@brief 输入字符控件获取光标
 *@param handle 输入字符控件句柄
 *@param index  光标字符索引
 */
void scui_inchar_cursor_get(scui_handle_t handle, scui_coord_t *index)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_inchar));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    *index = inchar->index;
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_inchar_invoke(scui_event_t *event)
{
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_string_t *string = (void *)widget;
    scui_inchar_t *inchar = (void *)widget;
    
    switch (event->type) {
    case scui_event_create: {
        /* 光标初始化(默认index==0) */
        scui_inchar_cursor_set(widget->myself, 0);
        break;
    }
    case scui_event_draw_graph: {
        if (inchar->hide) break;
        /* 光标呼吸(仅显示时) */
        
        /* 光标背景先绘制, 文本后绘制 */
        if (string->args.name != SCUI_HANDLE_INVALID) {
            /* 呼吸明暗(三角波: 谷值1/3, 峰值原值) */
            /* 纯色填充只取控件alpha, 呼吸使用RGB明暗而非透明度 */
            scui_multi_t wave = (scui_multi_t)inchar->bre_time;
            wave = wave * 100 / SCUI_WIDGET_INCHAR_BREATH_TIME;
            if (wave > 50) wave = 100 - wave;
            
            scui_multi_t factor = 25 + wave;
            scui_color_t color = inchar->color;
            color.color.ch.r = (uint8_t)(color.color.ch.r * factor / 75);
            color.color.ch.g = (uint8_t)(color.color.ch.g * factor / 75);
            color.color.ch.b = (uint8_t)(color.color.ch.b * factor / 75);
            scui_widget_draw_color(widget->myself, &inchar->area, color);
        }
        
        break;
    }
    case scui_event_anima_elapse: {
        if (inchar->hide) break;
        /* 光标呼吸(仅显示时) */
        
        inchar->bre_time += event->tick;
        inchar->bre_time %= SCUI_WIDGET_INCHAR_BREATH_TIME;
        scui_widget_draw(widget->myself, &inchar->area, false, 0);
        break;
    }
    default:
        break;
    }
    
    /* 基类处理 */
    scui_string_invoke(event);
}
