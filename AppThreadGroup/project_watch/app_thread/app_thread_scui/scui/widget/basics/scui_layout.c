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
    scui_coord_t wide = 0;
    scui_coord_t tall = 0;
    
    if (layout->item.num == 0)
        return;
    
    /* 阶段1: 统计子控件最大宽高 */
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t   handle_c = widget->child_list[idx];
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
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t handle_c = widget->child_list[idx];
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
    scui_multi_t   num    =  widget->child_num;
    
    bool way = layout->flex.way;
    scui_coord_t span_main = way ? layout->flex.span.y : layout->flex.span.x;
    scui_coord_t span_vice = way ? layout->flex.span.x : layout->flex.span.y;
    bool even_main = (span_main == -1);     /* 主轴 -1: 均等铺开 */
    bool even_vice = (span_vice == -1);     /* 副轴 -1: 均等铺开 */
    
    
    if (layout->flex.num == 0)
        return;
    
    /* 子控件主轴/副轴尺寸与分组 */
    scui_coord_t *main_sz = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * num);
    scui_coord_t *vice_sz = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * num);
    scui_coord_t *group_v = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * num);
    
    scui_widget_child_list_btra(widget, idx) {
        scui_handle_t  handle_c = widget->child_list[idx];
        scui_widget_t *widget_c = scui_handle_source_check(handle_c);
        scui_layout_flex_node_t *node = scui_layout_flex_find(layout, handle_c);
        
        main_sz[idx] = way ? widget_c->clip.h : widget_c->clip.w;
        vice_sz[idx] = way ? widget_c->clip.w : widget_c->clip.h;
        group_v[idx] = (node != NULL) ? node->group : 0;
    }
    
    /* 去重统计组编号(升序) */
    scui_multi_t  group_cnt = 0;
    scui_coord_t *group_uni = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * num);
    
    scui_widget_child_list_btra(widget, idx) {
        scui_coord_t gv = group_v[idx];
        scui_multi_t  k = 0;
        
        for (k = 0; k < group_cnt; k++)
            if (group_uni[k] == gv)
                break;
        
        if (k < group_cnt)
            continue;
        
        scui_multi_t ins = group_cnt;
        for (ins = group_cnt; ins > 0 && group_uni[ins - 1] > gv; ins--)
            group_uni[ins] = group_uni[ins - 1];
        
        group_uni[ins] = gv;
        group_cnt++;
    }
    
    /* 组内统计: 主轴合计/元素数, 副轴最大 */
    scui_coord_t *track_main = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    scui_coord_t *track_vice = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    scui_coord_t *track_cnt  = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    
    scui_widget_child_list_btra(widget, idx) {
        scui_multi_t g = 0;
        for (g = 0; g < group_cnt; g++)
            if (group_uni[g] == group_v[idx])
                break;
        
        track_main[g] += main_sz[idx];
        track_cnt[g]++;
        
        if (vice_sz[idx] > track_vice[g])
            track_vice[g] = vice_sz[idx];
    }
    
    /* 阶段2: auto尺寸(均等移除固定span, 无自由空间时退化为贴合) */
    scui_coord_t auto_main = 0;
    scui_coord_t auto_vice = 0;
    scui_multi_t g;
    for (g = 0; g < group_cnt; g++) {
        scui_coord_t gm = track_main[g] + (even_main ? 0 : span_main) * (track_cnt[g] - 1);
        if (gm > auto_main)
            auto_main = gm;
        
        auto_vice += track_vice[g];
    }
    
    auto_vice += (even_vice ? 0 : span_vice) * (group_cnt - 1);
    scui_coord_t width  = widget->state.layout_w ? (way ? auto_vice : auto_main) : widget->clip.w;
    scui_coord_t height = widget->state.layout_h ? (way ? auto_main : auto_vice) : widget->clip.h;
    if (width != widget->clip.w || height != widget->clip.h)
        scui_widget_adjust_size(handle, width, height);
    
    scui_coord_t main_total = way ? widget->clip.h : widget->clip.w;
    scui_coord_t vice_total = way ? widget->clip.w : widget->clip.h;
    
    /* 主轴/副轴对齐: 从align中按位分离
     * x轴(水平位) = 左/右/水平居中; y轴(垂直位) = 上/下/垂直居中
     * 主轴看主方向位, 副轴看另一方向位 */
    scui_opt_dir_t align_main = layout->flex.align & (way ? scui_opt_pos_ver : scui_opt_pos_hor);
    scui_opt_dir_t align_vice = layout->flex.align & (way ? scui_opt_pos_hor : scui_opt_pos_ver);
    
    /* 均等边缘: 依据对齐分离起点/终点侧是否各补一份
     * 起始对齐: 终点侧补1; 末端对齐: 起点侧补1; 居中: 两侧各补1 */
    scui_coord_t es_m = 0, en_m = 0;   /* 主轴起点/终点边缘缝隙 */
    scui_coord_t es_c = 0, en_c = 0;   /* 副轴起点/终点边缘缝隙 */
    switch (align_main) {
    case scui_opt_pos_r:
    case scui_opt_pos_d:   es_m = 1; en_m = 0; break;   /* 末端对齐: 起点侧多补 */
    case scui_opt_pos_hor:
    case scui_opt_pos_ver: es_m = 1; en_m = 1; break;   /* 居中: 两侧各补 */
    default:               es_m = 0; en_m = 1; break;   /* 起始对齐: 终点侧多补 */
    }
    switch (align_vice) {
    case scui_opt_pos_r:
    case scui_opt_pos_d:   es_c = 1; en_c = 0; break;
    case scui_opt_pos_hor:
    case scui_opt_pos_ver: es_c = 1; en_c = 1; break;
    default:               es_c = 0; en_c = 1; break;
    }
    
    /* 主轴缝隙: 均等铺满 or 固定span+整体偏移 */
    scui_coord_t *gap_main = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    scui_coord_t  start = 0;
    if (even_main) {
        /* 每track独立均等铺满主轴(内容不同, 自由空间各异) */
        for (g = 0; g < group_cnt; g++) {
            scui_coord_t free  = main_total - track_main[g];
            scui_coord_t slots = (track_cnt[g] - 1) + es_m + en_m;
            gap_main[g] = (free > 0 && slots > 0) ? free / slots : 0;
        }
    } else {
        scui_coord_t free_main = main_total - auto_main;
        if (free_main < 0) free_main = 0;
        switch (align_main) {
        case scui_opt_pos_r:
        case scui_opt_pos_d:   start = free_main;      break;
        case scui_opt_pos_hor:
        case scui_opt_pos_ver: start = free_main / 2;  break;
        default:   /* none/l/u */
            break;
        }
        for (g = 0; g < group_cnt; g++)
            gap_main[g] = span_main;
    }
    
    /* 副轴缝隙/偏移: 均等铺满 or 固定span+整体对齐 */
    scui_coord_t off_c = 0;
    scui_coord_t gap_c = span_vice;
    scui_coord_t free_vice = vice_total - auto_vice;
    if (free_vice < 0) free_vice = 0;
    if (even_vice) {
        scui_coord_t slots = (group_cnt - 1) + es_c + en_c;
        gap_c = (free_vice > 0 && slots > 0) ? free_vice / slots : 0;
        off_c = es_c * gap_c;
    } else {
        switch (align_vice) {
        case scui_opt_pos_r:
        case scui_opt_pos_d:   off_c = free_vice;      break;
        case scui_opt_pos_hor:
        case scui_opt_pos_ver: off_c = free_vice / 2;  break;
        default:   /* none/l/u */
            break;
        }
    }
    
    scui_coord_t *track_pos = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    scui_coord_t *cur_main  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_coord_t) * group_cnt);
    
    scui_coord_t vice_pos = 0;
    for (g = 0; g < group_cnt; g++) {
        track_pos[g] = off_c + vice_pos;
        vice_pos += track_vice[g] + gap_c;
        cur_main[g] = start + es_m * gap_main[g];
    }
    
    scui_widget_child_list_btra(widget, idx) {
        scui_multi_t g  = 0;
        for (; g < group_cnt; g++) if (group_uni[g] == group_v[idx]) break;
        
        scui_coord_t main = cur_main[g];
        cur_main[g] += main_sz[idx] + gap_main[g];
        
        scui_point_t point = {0};
        if (way) {point.x = track_pos[g]; point.y = main;}
        else     {point.x = main;         point.y = track_pos[g];}
        /* move_pos 收的是独立画布坐标, 而 point 是布局内相对坐标;
         * 经 align_pos(内左上 + 偏移) 换算成布局的画布绝对位置再摆放 */
        scui_widget_align_pos(widget->child_list[idx], handle, scui_align_itl, &point);
    }
    
    SCUI_MEM_FREE(main_sz);
    SCUI_MEM_FREE(vice_sz);
    SCUI_MEM_FREE(group_v);
    SCUI_MEM_FREE(group_uni);
    SCUI_MEM_FREE(track_main);
    SCUI_MEM_FREE(track_vice);
    SCUI_MEM_FREE(track_cnt);
    SCUI_MEM_FREE(track_pos);
    SCUI_MEM_FREE(cur_main);
    SCUI_MEM_FREE(gap_main);
}

/*@brief 布局控件(方向/对齐/间距)
 *@param handle 布局控件句柄
 *@param way    摆放方向(0:水平;1:垂直)
 *@param align  对齐(水平/垂直)
 *@param span   间距(水平/垂直)(EVNELY:-1)
 */
void scui_layout_flex_way(scui_handle_t handle, bool way, scui_opt_pos_t align, scui_point_t span)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_layout));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_layout_t *layout = (void *)widget;
    
    if (layout->type != scui_layout_type_flex) {
        SCUI_LOG_WARN("unmatch type");
        return;
    }
    
    layout->flex.way   = way;
    layout->flex.align = align;
    layout->flex.span  = span;
    
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
