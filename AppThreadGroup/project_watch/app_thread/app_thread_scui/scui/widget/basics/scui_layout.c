/*实现目标:
 *    控件(布局)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 子控件登记项定位
 *@param layout 布局控件实例
 *@param handle 子控件句柄
 *@retval 登记项
 */
static scui_layout_item_node_t *scui_layout_item_claim(scui_layout_t *layout, scui_handle_t handle)
{
    if (layout->item.list == NULL)
        return NULL;
    
    for (scui_handle_t idx = 0; idx < layout->item.num; idx++) {
        scui_layout_item_node_t *node = &layout->item.list[idx];
        if (node->use && node->handle == handle) return node;
        if (node->use) continue;
        
        node->use    = true;
        node->handle = handle;
        return node;
    }
    
    return NULL;
}

/*@brief 子控件登记项查找(只查)
 *@param layout 布局控件实例
 *@param handle 子控件句柄
 *@retval 登记项
 */
static scui_layout_item_node_t *scui_layout_item_find(scui_layout_t *layout, scui_handle_t handle)
{
    if (layout->item.list == NULL)
        return NULL;
    
    for (scui_handle_t idx = 0; idx < layout->item.num; idx++) {
        scui_layout_item_node_t *node = &layout->item.list[idx];
        if (node->use && node->handle == handle)
            return node;
    }
    
    return NULL;
}

/*@brief 子控件登记项定位
 *@param layout 布局控件实例
 *@param handle 子控件句柄
 *@retval 登记项
 */
static scui_layout_flex_node_t *scui_layout_flex_claim(scui_layout_t *layout, scui_handle_t handle)
{
    if (layout->flex.list == NULL)
        return NULL;
    
    for (scui_handle_t idx = 0; idx < layout->flex.num; idx++) {
        scui_layout_flex_node_t *node = &layout->flex.list[idx];
        if (node->use && node->handle == handle) return node;
        if (node->use) continue;
        
        node->use    = true;
        node->handle = handle;
        return node;
    }
    
    return NULL;
}

/*@brief 子控件登记项查找(只查)
 *@param layout 布局控件实例
 *@param handle 子控件句柄
 *@retval 登记项
 */
static scui_layout_flex_node_t *scui_layout_flex_find(scui_layout_t *layout, scui_handle_t handle)
{
    if (layout->flex.list == NULL)
        return NULL;
    
    for (scui_handle_t idx = 0; idx < layout->flex.num; idx++) {
        scui_layout_flex_node_t *node = &layout->flex.list[idx];
        if (node->use && node->handle == handle)
            return node;
    }
    
    return NULL;
}

/*@brief auto布局执行
 *@param layout 布局控件实例
 */
static void scui_layout_item_exec(scui_layout_t *layout)
{
    scui_widget_t *widget = &layout->widget;
    scui_handle_t  handle =  widget->myself;
    if (layout->item.num == 0) return;
    
    scui_coord_t wide = 0;
    scui_coord_t tall = 0;
    /* 阶段1: 统计子控件最大宽高 */
    scui_widget_child_list_btra(widget, child_idx) {
        scui_handle_t   handle_c = widget->child_list[child_idx];
        scui_widget_t  *widget_c = scui_handle_source_check(handle_c);
        scui_layout_item_node_t *node = scui_layout_item_find(layout, handle_c);
        
        scui_coord_t wide_c = widget_c->clip.w;
        scui_coord_t tall_c = widget_c->clip.h;
        if (node == NULL && widget_c->clip.x > widget->clip.x)
            wide_c += widget_c->clip.x - widget->clip.x;
        if (node == NULL && widget_c->clip.y > widget->clip.y)
            tall_c += widget_c->clip.y - widget->clip.y;
            
        if (wide_c > wide) wide = wide_c;
        if (tall_c > tall) tall = tall_c;
    }
    
    /* 阶段2: 更新自动尺寸(尺寸确定后align才有正确参考) */
    scui_coord_t width  = widget->state.layout_w ? wide : widget->clip.w;
    scui_coord_t height = widget->state.layout_h ? tall : widget->clip.h;
    if (width != widget->clip.w || height != widget->clip.h)
        scui_widget_adjust_size(handle, width, height);
        
    /* 阶段3: 父尺寸已确定, 再执行登记node对齐 */
    scui_widget_child_list_btra(widget, child_idx) {
        scui_handle_t handle_c = widget->child_list[child_idx];
        scui_layout_item_node_t *node = scui_layout_item_find(layout, handle_c);
        if (node == NULL) continue;
        
        scui_widget_align_pos(handle_c, node->handle_t, node->align, &node->offset);
    }
}

/*@brief 布局控件子控件对齐
 *@param handle   布局控件句柄
 *@param handle_t 对齐目标控件(为空相对父)
 *@param handle_c 子控件句柄
 *@param align    对齐
 *@param offset   偏移
 */
void scui_layout_item_align(scui_handle_t handle, scui_handle_t handle_t,
    scui_handle_t handle_c, scui_align_t align, scui_point_t *offset)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_layout));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_layout_t *layout = (void *)widget;
    
    if (layout->type != scui_layout_type_item) {
        SCUI_LOG_WARN("unmatch type");
        return;
    }
    
    /* 对齐目标为空则相对父 */
    if (handle_t == SCUI_HANDLE_INVALID) handle_t = handle;
    SCUI_ASSERT(handle_t == handle || scui_widget_parent(handle_t) == handle);
    
    SCUI_ASSERT(scui_widget_parent(handle_c) == handle);
    scui_layout_item_node_t *node = scui_layout_item_claim(layout, handle_c);
    if (node == NULL) return;
    
    node->handle_t = handle_t;
    node->align    = align;
    if (offset != NULL) node->offset = *offset;
    
    /* 标记布局更新 */
    scui_widget_layout_refr(handle);
}

/*@brief flex布局执行
 *@param layout 布局控件实例
 */
static void scui_layout_flex_exec(scui_layout_t *layout)
{
    scui_widget_t *widget = &layout->widget;
    scui_handle_t  handle =  widget->myself;
    if (layout->flex.num == 0) return;
    
    /* 主/副轴: 间距与均分(span/evenly均为水平/垂直, 按way映射) */
    bool tag_v = layout->flex.way;
    scui_coord_t main_span = tag_v ? layout->flex.span.y : layout->flex.span.x;
    scui_coord_t vice_span = tag_v ? layout->flex.span.x : layout->flex.span.y;
    bool even_main = tag_v ? layout->flex.evenly.y : layout->flex.evenly.x;
    bool even_vice = tag_v ? layout->flex.evenly.x : layout->flex.evenly.y;
    
    /* 步骤1: 采样子控件主/副轴尺寸与组号 */
    scui_coord_t  child_num = widget->child_num;
    scui_coord_t *main_size = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * child_num);
    scui_coord_t *vice_size = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * child_num);
    scui_coord_t *group_chd = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * child_num);
    scui_coord_t *group_lst = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * child_num);
    scui_multi_t  group_idx = 0;
    scui_coord_t  group_cnt = 0;
    
    scui_widget_child_list_btra(widget, child_idx) {
        scui_handle_t  handle_c = widget->child_list[child_idx];
        scui_widget_t *widget_c = scui_handle_source_check(handle_c);
        scui_layout_flex_node_t *node = scui_layout_flex_find(layout, handle_c);
        
        main_size[child_idx] = tag_v ? widget_c->clip.h : widget_c->clip.w;
        vice_size[child_idx] = tag_v ? widget_c->clip.w : widget_c->clip.h;
        group_chd[child_idx] = (node != NULL) ? node->group : 0;
    }
    
    /* 步骤2: 组号去重统计(升序) */
    scui_widget_child_list_btra(widget, child_idx) {
        scui_coord_t group_val = group_chd[child_idx];
        for (group_idx = 0; group_idx < group_cnt; group_idx++)
            if (group_lst[group_idx] == group_val)
                break;
        
        if (group_idx < group_cnt)
            continue;
        
        scui_multi_t idx_ins = group_cnt;
        for (idx_ins = group_cnt; idx_ins > 0 && group_lst[idx_ins - 1] > group_val; idx_ins--)
            group_lst[idx_ins] = group_lst[idx_ins - 1];
        
        group_lst[idx_ins] = group_val;
        group_cnt++;
    }
    
    scui_coord_t *track_main_size = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    scui_coord_t *track_child_num = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    scui_coord_t *track_vice_max  = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    scui_coord_t *track_main_gap  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    scui_coord_t *track_main_cur  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    scui_coord_t *track_vice_pos  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    
    /* 步骤3: 组内统计(主轴合计/元素数, 副轴最大) */
    scui_widget_child_list_btra(widget, child_idx) {
        for (group_idx = 0; group_idx < group_cnt; group_idx++)
            if (group_lst[group_idx] == group_chd[child_idx])
                break;
        
        track_main_size[group_idx] += main_size[child_idx];
        track_child_num[group_idx]++;
        
        if (vice_size[child_idx] > track_vice_max[group_idx])
            track_vice_max[group_idx] = vice_size[child_idx];
    }
    
    /* 步骤4: 轨道内基础排布(主轴尺寸始终含span; 均分不参与统计) */
    scui_coord_t auto_main = 0;
    scui_coord_t auto_vice = 0;
    
    for (group_idx = 0; group_idx < group_cnt; group_idx++) {
        scui_coord_t group_main = track_main_size[group_idx];
        group_main += main_span * (track_child_num[group_idx] - 1);
        track_main_size[group_idx] = group_main;
        
        if (group_main > auto_main)
            auto_main = group_main;
        
        auto_vice += track_vice_max[group_idx];
    }
    auto_vice += vice_span * (group_cnt - 1);
    
    /* 步骤5: 自适应尺寸 */
    scui_coord_t width  = widget->state.layout_w ? (tag_v ? auto_vice : auto_main) : widget->clip.w;
    scui_coord_t height = widget->state.layout_h ? (tag_v ? auto_main : auto_vice) : widget->clip.h;
    if (width != widget->clip.w || height != widget->clip.h)
        scui_widget_adjust_size(handle, width, height);
    
    /* 步骤6: 主/副轴可用空间与对齐位(主轴位看主方向, 副轴位看另一方向) */
    scui_coord_t main_extent = tag_v ? widget->clip.h : widget->clip.w;
    scui_coord_t vice_extent = tag_v ? widget->clip.w : widget->clip.h;
    scui_coord_t vice_free = vice_extent - auto_vice;
    if (vice_free < 0) vice_free = 0;
    
    scui_opt_dir_t align_main = layout->flex.align_o & (tag_v ? scui_opt_pos_ver : scui_opt_pos_hor);
    scui_opt_dir_t align_vice = layout->flex.align_o & (tag_v ? scui_opt_pos_hor : scui_opt_pos_ver);
    scui_opt_dir_t align_item = layout->flex.align_i & (tag_v ? scui_opt_pos_hor : scui_opt_pos_ver);
    
    /* 步骤7: 副轴排布(轨道堆叠: 均分剩余 或 固定间距+整体对齐) */
    scui_coord_t vice_gap = vice_span;
    scui_coord_t vice_off = 0;
    
    if (even_vice && group_cnt > 1) {
        /* 均分: 基础间距+剩余均分到轨道间缝隙(AUTO无剩余退化为span) */
        vice_gap = vice_span + vice_free / (group_cnt - 1);
    } else {
        if (align_vice == scui_opt_pos_r || align_vice == scui_opt_pos_d)
            vice_off = vice_free;
        else if (align_vice == scui_opt_pos_hor || align_vice == scui_opt_pos_ver)
            vice_off = vice_free / 2;
    }
    
    scui_coord_t vice_cur = 0;
    for (group_idx = 0; group_idx < group_cnt; group_idx++) {
        track_vice_pos[group_idx] = vice_off + vice_cur;
        vice_cur += track_vice_max[group_idx] + vice_gap;
    }
    
    /* 步骤8: 主轴排布(轨道内间距: 均分剩余 或 固定span; 轨道整体: 逐轨道对齐) */
    scui_coord_t main_gap_extra = 0;
    
    if (even_main) {
        /* 均分: 整块主轴剩余均分到所有轨道内缝隙(AUTO无剩余退化为span) */
        scui_coord_t main_free = main_extent - auto_main;
        if (main_free < 0) main_free = 0;
        
        scui_coord_t main_slots = 0;
        for (group_idx = 0; group_idx < group_cnt; group_idx++)
            main_slots += track_child_num[group_idx] - 1;
        
        if (main_slots > 0)
            main_gap_extra = main_free / main_slots;
    }
    
    for (group_idx = 0; group_idx < group_cnt; group_idx++) {
        track_main_gap[group_idx] = main_span + main_gap_extra;
        
        /* 轨道实际主轴宽(含均分增量) → 逐轨道剩余 → 按轨道间对齐定起点 */
        scui_coord_t track_use = track_main_size[group_idx] + main_gap_extra * (track_child_num[group_idx] - 1);
        scui_coord_t track_free = main_extent - track_use;
        if (track_free < 0) track_free = 0;
        
        scui_coord_t main_start = 0;
        if (align_main == scui_opt_pos_r || align_main == scui_opt_pos_d)
            main_start = track_free;
        else if (align_main == scui_opt_pos_hor || align_main == scui_opt_pos_ver)
            main_start = track_free / 2;
        
        track_main_cur[group_idx] = main_start;
    }
    
    /* 步骤9: 逐子摆放(主轴推进 + 轨道内交叉对齐) */
    scui_widget_child_list_btra(widget, child_idx) {
        for (group_idx = 0; group_idx < group_cnt; group_idx++)
            if (group_lst[group_idx] == group_chd[child_idx])
                break;
        
        scui_coord_t child_main_pos = track_main_cur[group_idx];
        track_main_cur[group_idx] += main_size[child_idx] + track_main_gap[group_idx];
        
        scui_coord_t item_vice_free = track_vice_max[group_idx] - vice_size[child_idx];
        if (item_vice_free < 0) item_vice_free = 0;
        
        scui_coord_t item_vice_off = 0;
        if (align_item == scui_opt_pos_r || align_item == scui_opt_pos_d)
            item_vice_off = item_vice_free;
        else if (align_item == scui_opt_pos_hor || align_item == scui_opt_pos_ver)
            item_vice_off = item_vice_free / 2;
        
        scui_point_t point = {
            .x = tag_v ? track_vice_pos[group_idx] + item_vice_off : child_main_pos,
            .y = tag_v ? child_main_pos : track_vice_pos[group_idx] + item_vice_off,
        };
        scui_widget_move_pos(widget->child_list[child_idx], &point, false);
    }
    
    SCUI_MEM_FREE(main_size);
    SCUI_MEM_FREE(vice_size);
    SCUI_MEM_FREE(group_chd);
    SCUI_MEM_FREE(group_lst);
    
    SCUI_MEM_FREE(track_main_size);
    SCUI_MEM_FREE(track_child_num);
    SCUI_MEM_FREE(track_vice_max);
    SCUI_MEM_FREE(track_main_gap);
    SCUI_MEM_FREE(track_main_cur);
    SCUI_MEM_FREE(track_vice_pos);
}

/*@brief 布局控件(方向/间距/均分/对齐)
 *@param handle 布局控件句柄
 *@param way    轨道方向(0:水平;1:垂直)
 *@param span   间距(水平/垂直)
 *@param evenly 均分(水平/垂直)(与span对照, 1:该轴均分剩余; AUTO下无剩余退化为span)
 *@param alig_o 轨道间对齐
 *@param alig_i 轨道内对齐
 */
void scui_layout_flex_way(scui_handle_t handle, bool way, scui_point_t span,
    scui_point_t evenly, scui_opt_pos_t alig_o, scui_opt_pos_t alig_i)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_layout));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_layout_t *layout = (void *)widget;
    
    if (layout->type != scui_layout_type_flex) {
        SCUI_LOG_WARN("unmatch type");
        return;
    }
    
    layout->flex.way     = way;
    layout->flex.span    = span;
    layout->flex.evenly  = evenly;
    layout->flex.align_o = alig_o;
    layout->flex.align_i = alig_i;
    
    /* 标记布局更新 */
    scui_widget_layout_refr(handle);
}

/*@brief 布局控件子控件分组
 *@param handle   布局控件句柄
 *@param handle_c 子控件句柄
 *@param group    组编号
 */
void scui_layout_flex_group(scui_handle_t handle, scui_handle_t handle_c, scui_coord_t group)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_layout));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_layout_t *layout = (void *)widget;
    
    if (layout->type != scui_layout_type_flex) {
        SCUI_LOG_WARN("unmatch type");
        return;
    }
    
    SCUI_ASSERT(scui_widget_parent(handle_c) == handle);
    scui_layout_flex_node_t *node = scui_layout_flex_claim(layout, handle_c);
    if (node == NULL) return;
    
    node->group = group;
    
    /* 标记布局更新 */
    scui_widget_layout_refr(handle);
}

/*@brief 布局控件grid子控件分组登记项定位
 *@param layout 布局控件实例
 *@param handle 子控件句柄
 *@retval 登记项
 */
static scui_layout_grid_node_t *scui_layout_grid_claim(scui_layout_t *layout, scui_handle_t handle)
{
    if (layout->grid.list == NULL) return NULL;
    
    for (scui_handle_t idx = 0; idx < layout->grid.num; idx++) {
        scui_layout_grid_node_t *node = &layout->grid.list[idx];
        if (node->use && node->handle == handle) return node;
        if (node->use) continue;
        
        node->use    = true;
        node->handle = handle;
        return node;
    }
    return NULL;
}

/*@brief 布局控件(列/行模板)
 *@param handle 布局控件句柄
 *@param way    轨道(0:横向/列; 1:纵向/行)
 *@param size   轨道尺寸数组(长度num)
 *@param num    轨道数量
 *@param gap    轨道间距
 */
void scui_layout_grid_way(scui_handle_t handle, bool way,
    const scui_coord_t *size, scui_coord_t num, scui_coord_t gap)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_layout));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_layout_t *layout = (void *)widget;
    
    if (layout->type != scui_layout_type_grid) {
        SCUI_LOG_WARN("unmatch type");
        return;
    }
    
    scui_coord_t **tar = way ? &layout->grid.row_size : &layout->grid.col_size;
    scui_coord_t  *cnt = way ? &layout->grid.row_num  : &layout->grid.col_num;
    scui_coord_t  *gzz = way ? &layout->grid.row_gap  : &layout->grid.col_gap;
    
    /* 回收旧轨道并拷贝定值模板 */
    SCUI_MEM_FREE(*tar);
    *cnt = 0;
    *gzz = gap;
    if (num > 0 && size != NULL) {
        scui_multi_t sz = num * sizeof(scui_coord_t);
        scui_coord_t *mem = SCUI_MEM_ALLOC(scui_mem_type_mix, sz);
        for (scui_coord_t i = 0; i < num; i++)
            mem[i] = size[i];
            
        *tar = mem;
        *cnt = num;
    }
    
    /* 标记布局更新 */
    scui_widget_layout_refr(handle);
}

/*@brief 布局控件落位
 *@param handle   布局控件句柄
 *@param handle_c 子控件句柄
 *@param pos      行列(x列;y行)
 *@param span     跨度(x列;y行)
 *@param align    对齐(cell)
 *@param stretch  拉伸方向(hor|ver)
 */
void scui_layout_grid_cell(scui_handle_t handle, scui_handle_t handle_c,
    scui_point_t pos, scui_point_t span, scui_opt_pos_t align, scui_opt_dir_t stretch)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_layout));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_layout_t *layout = (void *)widget;
    
    if (layout->type != scui_layout_type_grid) {
        SCUI_LOG_WARN("unmatch type");
        return;
    }
    
    SCUI_ASSERT(scui_widget_parent(handle_c) == handle);
    scui_layout_grid_node_t *node = scui_layout_grid_claim(layout, handle_c);
    if (node == NULL) return;
    
    node->pos    = pos;
    node->span.x = span.x > 0 ? span.x : 1;
    node->span.y = span.y > 0 ? span.y : 1;
    node->align  = align;
    node->st_x   = scui_opt_bits_equal(stretch, scui_opt_dir_hor);
    node->st_y   = scui_opt_bits_equal(stretch, scui_opt_dir_ver);
    
    scui_widget_layout_refr(handle);
}

/*@brief grid布局执行
 *@param layout 布局控件实例
 */
static void scui_layout_grid_exec(scui_layout_t *layout)
{
    scui_widget_t *widget = &layout->widget;
    scui_handle_t  handle =  widget->myself;
    if (layout->grid.num == 0) return;
    if (layout->grid.col_num == 0) return;
    if (layout->grid.row_num == 0) return;
    
    /* 步骤1: 轨道累计偏移(列/行) */
    
    scui_coord_t  col_acc = 0;
    scui_coord_t *col_ofs = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * (layout->grid.col_num + 1));
    for (scui_coord_t col_idx = 0; col_idx < layout->grid.col_num; col_idx++) {
        col_ofs[col_idx] = col_acc; col_acc += layout->grid.col_size[col_idx] + layout->grid.col_gap;
    }
    col_ofs[layout->grid.col_num] = col_acc;
    
    scui_coord_t  row_acc = 0;
    scui_coord_t *row_ofs = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * (layout->grid.row_num + 1));
    for (scui_coord_t row_idx = 0; row_idx < layout->grid.row_num; row_idx++) {
        row_ofs[row_idx] = row_acc; row_acc += layout->grid.row_size[row_idx] + layout->grid.row_gap;
    }
    row_ofs[layout->grid.row_num] = row_acc;
    
    /* 步骤2: auto 尺寸 */
    scui_coord_t width  = widget->state.layout_w ? col_ofs[layout->grid.col_num] : widget->clip.w;
    scui_coord_t height = widget->state.layout_h ? row_ofs[layout->grid.row_num] : widget->clip.h;
    if (width != widget->clip.w || height != widget->clip.h)
        scui_widget_adjust_size(handle, width, height);
    
    /* 步骤3: 逐cell摆放(网格区域 -> 拉伸接管尺寸 -> 内对齐定位置) */
    for (scui_handle_t cell_idx = 0; cell_idx < layout->grid.num; cell_idx++) {
        scui_layout_grid_node_t *node = &layout->grid.list[cell_idx];
        if (!node->use) continue;
        
        scui_handle_t  handle_c = node->handle;
        scui_widget_t *widget_c = scui_handle_source_check(handle_c);
        
        /* 网格区域(跨列/跨行, 越界夹取到有效轨道) */
        scui_coord_t c0 = node->pos.x;
        scui_coord_t r0 = node->pos.y;
        scui_coord_t c1 = c0 + node->span.x;
        scui_coord_t r1 = r0 + node->span.y;
        
        if (c0 <  0) c0 = 0;
        if (r0 <  0) r0 = 0;
        if (c0 >= layout->grid.col_num) c0 = layout->grid.col_num - 1;
        if (r0 >= layout->grid.row_num) r0 = layout->grid.row_num - 1;
        if (c1 <= c0) c1 = c0 + 1;
        if (r1 <= r0) r1 = r0 + 1;
        if (c1 >  layout->grid.col_num) c1 = layout->grid.col_num;
        if (r1 >  layout->grid.row_num) r1 = layout->grid.row_num;
        
        /* 网格区域(跨越的列/行像素范围) */
        scui_coord_t area_l = col_ofs[c0];
        scui_coord_t area_w = col_ofs[c1] - col_ofs[c0];
        scui_coord_t area_t = row_ofs[r0];
        scui_coord_t area_h = row_ofs[r1] - row_ofs[r0];
        
        /* 子控件实际宽高 */
        scui_coord_t child_w = widget_c->clip.w;
        scui_coord_t child_h = widget_c->clip.h;
        
        /* 拉伸轴: 接管尺寸为网格区域 */
        if (node->st_x && child_w != area_w)
            scui_widget_adjust_size(handle_c, area_w, child_h);
        if (node->st_y && child_h != area_h)
            scui_widget_adjust_size(handle_c, child_w, area_h);
            
        child_w = widget_c->clip.w;
        child_h = widget_c->clip.h;
        
        /* 未拉伸轴: 按cell内对齐定位置 */
        scui_coord_t off_x = area_l;
        scui_coord_t off_y = area_t;
        scui_coord_t free;
        
        if (!node->st_x) {
            free = area_w - child_w;
            if (free < 0) free = 0;
            switch (node->align & scui_opt_pos_hor) {
            case scui_opt_pos_r:   off_x = area_l + free;      break;
            case scui_opt_pos_hor: off_x = area_l + free / 2;  break;
            default:               off_x = area_l;             break;   /* l / 无 */
            }
        }
        
        if (!node->st_y) {
            free = area_h - child_h;
            if (free < 0) free = 0;
            switch (node->align & scui_opt_pos_ver) {
            case scui_opt_pos_d:   off_y = area_t + free;      break;
            case scui_opt_pos_ver: off_y = area_t + free / 2;  break;
            default:               off_y = area_t;             break;   /* u / 无 */
            }
        }
        
        scui_point_t off = {off_x, off_y};
        scui_widget_move_pos(handle_c, &off, false);
    }
    
    SCUI_MEM_FREE(col_ofs);
    SCUI_MEM_FREE(row_ofs);
}

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_layout_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_layout_t *layout = widget;
    scui_layout_maker_t *layout_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_layout));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    layout->type   = layout_maker->type;
    layout->use    = layout_maker->use;
    layout->align  = layout_maker->align;
    layout->offset = layout_maker->offset;
    
    switch (layout->type) {
    case scui_layout_type_item: {
        layout->item.num  = widget->child_num;
        
        if (layout->item.num > 0) {
            scui_multi_t size = layout->item.num * sizeof(scui_layout_item_node_t);
            layout->item.list = SCUI_MEM_ZALLOC(scui_mem_type_mix, size);
        }
        break;
    }
    case scui_layout_type_flex: {
        layout->flex.num  = widget->child_num;
        
        if (layout->flex.num > 0) {
            scui_multi_t size = layout->flex.num * sizeof(scui_layout_flex_node_t);
            layout->flex.list = SCUI_MEM_ZALLOC(scui_mem_type_mix, size);
        }
        break;
    }
    case scui_layout_type_grid: {
        layout->grid.num  = widget->child_num;
        
        if (layout->grid.num > 0) {
            scui_multi_t size = layout->grid.num * sizeof(scui_layout_grid_node_t);
            layout->grid.list = SCUI_MEM_ZALLOC(scui_mem_type_mix, size);
        }
        break;
    }
    default:
        SCUI_ASSERT(false);
        break;
    }
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_layout_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_layout));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_layout_t *layout = (void *)widget;
    
    switch (layout->type) {
    case scui_layout_type_item: {
        /* 回收布局资源 */
        SCUI_MEM_FREE(layout->item.list);
        break;
    }
    case scui_layout_type_flex: {
        /* 回收分组登记资源 */
        SCUI_MEM_FREE(layout->flex.list);
        break;
    }
    case scui_layout_type_grid: {
        /* 回收单元格登记与轨道模板资源 */
        SCUI_MEM_FREE(layout->grid.list);
        SCUI_MEM_FREE(layout->grid.col_size);
        SCUI_MEM_FREE(layout->grid.row_size);
        break;
    }
    default:
        SCUI_ASSERT(false);
        break;
    }
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_layout_invoke(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_layout_t *layout = (void *)widget;
    
    switch (event->type) {
    case scui_event_layout: {
        /* 有子控件才执行布局 */
        if (widget->child_num != 0) {
            switch (layout->type) {
            case scui_layout_type_item:
                scui_layout_item_exec(layout);
                break;
            case scui_layout_type_flex:
                scui_layout_flex_exec(layout);
                break;
            case scui_layout_type_grid:
                scui_layout_grid_exec(layout);
                break;
            default:
                SCUI_ASSERT(false);
                break;
            }
        }
        
        /* 最后自身相对父控件对齐 */
        SCUI_ASSERT(widget->clip.w > 0);
        SCUI_ASSERT(widget->clip.h > 0);
        if (layout->use) scui_widget_align_pos(widget->myself,
            SCUI_HANDLE_INVALID, layout->align, &layout->offset);
        
        break;
    }
    default:
        break;
    }
}
