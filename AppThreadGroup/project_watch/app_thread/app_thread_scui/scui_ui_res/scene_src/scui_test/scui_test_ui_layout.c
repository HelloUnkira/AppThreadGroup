/*实现目标:
 *    测试(widget layout · flex)
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t layout;       /* 布局控件句柄 */
    scui_coord_t  build_idx;    /* 构建配置索引 */
} * scui_ui_res_local = NULL;

/*@brief 纯色自定义子控件构造基座(随机背景颜色)
 *@param parent 父控件句柄
 *@retval 构造器
 */
static scui_custom_maker_t scui_test_ui_layout_custom_base(scui_handle_t parent)
{
    scui_custom_maker_define(custom_maker);
    
    custom_maker.widget.style.sched_widget = true;
    custom_maker.widget.style.fully_bg     = true;
    custom_maker.widget.parent = parent;
    custom_maker.widget.color.color.ch.a = 0xFF;
    custom_maker.widget.color.color.ch.r = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
    custom_maker.widget.color.color.ch.g = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
    custom_maker.widget.color.color.ch.b = scui_rand(10) % 2 == 0 ? 0 : 0xFF;
    /* 保证至少一个通道为0xFF: 避免纯黑色块在暗背景上不可见 */
    if (custom_maker.widget.color.color.ch.r == 0 &&
        custom_maker.widget.color.color.ch.g == 0 &&
        custom_maker.widget.color.color.ch.b == 0)
        custom_maker.widget.color.color.ch.r = 0xFF;
    return custom_maker;
}

/*@brief grid布局构建(按子索引切换展示)
 *  core: 固定列/行模板 + cell落位(pos/span) + cell内对齐/拉伸
 */
static void scui_test_ui_layout_grid_create(uint8_t idx)
{
    scui_handle_t layout_handle = SCUI_HANDLE_INVALID;
    scui_layout_maker_define(layout_maker);
    
    layout_maker.widget.style.sched_widget = true;
    layout_maker.widget.style.fully_bg     = true;
    layout_maker.widget.clip.x = (SCUI_HOR_RES - SCUI_HOR_RES * 707 / 1000) / 2;
    layout_maker.widget.clip.y = (SCUI_VER_RES - SCUI_VER_RES * 707 / 1000) / 2;
    layout_maker.widget.clip.w = SCUI_HOR_RES * 707 / 1000;
    layout_maker.widget.clip.h = SCUI_VER_RES * 707 / 1000;
    layout_maker.widget.parent    = SCUI_UI_SCENE_TEST_UI_LAYOUT;
    layout_maker.widget.child_num = 6;
    layout_maker.widget.color.color.full = 0xFF6F6F6F;  /* 亮灰 */
    layout_maker.type  = scui_layout_type_grid;
    layout_maker.align = scui_align_itl;    /* 相对父左上 */
    layout_maker.offset = (scui_point_t){0};
    
    scui_widget_create(&layout_maker, &layout_handle);
    scui_ui_res_local->layout = layout_handle;
    
    /* 列/行轨道模板(定值) */
    const scui_coord_t col_size[] = {SCUI_HOR_RES / 5, SCUI_HOR_RES / 5, SCUI_HOR_RES / 5};
    const scui_coord_t row_size[] = {SCUI_HOR_RES / 8, SCUI_HOR_RES / 8, SCUI_HOR_RES / 8};
    scui_layout_grid_way(layout_handle, false, col_size, 3, 8);
    scui_layout_grid_way(layout_handle, true,  row_size, 3, 8);
    
    /* 6个cell: 由idx决定落位/跨格/对齐/拉伸 */
    for (uint8_t k = 0; k < 6; k++) {
        scui_custom_maker_t custom_maker = scui_test_ui_layout_custom_base(layout_handle);
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        custom_maker.widget.clip.w = SCUI_HOR_RES / 10;
        custom_maker.widget.clip.h = SCUI_HOR_RES / 10;
        scui_widget_create(&custom_maker, &custom_handle);
        
        scui_coord_t  col = 0, row = 0, cspan = 1, rspan = 1;
        scui_opt_pos_t align = 0;
        scui_opt_dir_t stretch = 0;
        switch (idx) {
        case 0: /* 基础: 每个格子左/上对齐 */
            col = k % 3; row = k / 3;
            break;
        case 1: /* 含跨列 + cell内对齐 */
            if (k == 0) {col = 0; row = 0; cspan = 3;}
            else if (k == 1) {col = 0; row = 1; align = scui_opt_pos_l | scui_opt_pos_ver;}
            else if (k == 2) {col = 1; row = 1; align = scui_opt_pos_r | scui_opt_pos_ver;}
            else if (k == 3) {col = 0; row = 2; align = scui_opt_pos_hor | scui_opt_pos_ver;}
            else if (k == 4) {col = 1; row = 2; align = scui_opt_pos_l | scui_opt_pos_d;}
            else             {col = 2; row = 1;}
            break;
        case 2: /* 含拉伸 + 右下对齐 */
            if (k == 0) {col = 0; row = 0; stretch = scui_opt_dir_ver;}
            else if (k == 1) {col = 1; row = 0; align = scui_opt_pos_hor | scui_opt_pos_ver;}
            else if (k == 2) {col = 2; row = 0; align = scui_opt_pos_r | scui_opt_pos_d;}
            else if (k == 3) {col = 0; row = 1; stretch = scui_opt_dir_hor; align = scui_opt_pos_ver;}
            else if (k == 4) {col = 1; row = 1; cspan = 2; align = scui_opt_pos_l | scui_opt_pos_u;}
            else             {col = 2; row = 2;}
            break;
        default:
            break;
        }
        scui_point_t span = {cspan, rspan};
        scui_layout_grid_cell(layout_handle, custom_handle,
            col, row, span, align, stretch);
    }
    
    SCUI_LOG_WARN("layout build grid cfg: %u", idx);
}

/*@brief 构建配置: 索引0为 item(手排/auto) 布局
 *  后续索引为 flex 布局参数(方向 x 间距 x 对齐 x 分组轨道)
 */
static void scui_test_ui_layout_create(uint8_t cfg)
{
    /* grid 布局: 固定轨道模板 + cell落位(空位在flex之后, 为后续扩展) */
    if (cfg >= 11) {
        scui_test_ui_layout_grid_create(cfg - 11);
        return;
    }
    
    /* item 布局: 子控件手工xy摆放, 部分登记相对对齐 */
    if (cfg == 0) {
        scui_handle_t  layout_handle = SCUI_HANDLE_INVALID;
        scui_layout_maker_define(layout_maker);
        
        layout_maker.widget.style.sched_widget = true;
        layout_maker.widget.style.fully_bg     = true;
        layout_maker.widget.clip.x = (SCUI_HOR_RES - SCUI_HOR_RES * 707 / 1000) / 2;
        layout_maker.widget.clip.y = (SCUI_VER_RES - SCUI_VER_RES * 707 / 1000) / 2;
        layout_maker.widget.clip.w = SCUI_HOR_RES * 707 / 1000;
        layout_maker.widget.clip.h = SCUI_VER_RES * 707 / 1000;
        layout_maker.widget.parent    = SCUI_UI_SCENE_TEST_UI_LAYOUT;
        layout_maker.widget.child_num = 6;
        layout_maker.widget.color.color.full = 0xFF6F6F6F;  /* 亮灰 */
        layout_maker.type  = scui_layout_type_item;
        layout_maker.align = scui_align_itl;    /* 相对父左上对齐(in, 避免out越界) */
        layout_maker.offset = (scui_point_t){0};
        
        scui_widget_create(&layout_maker, &layout_handle);
        scui_ui_res_local->layout = layout_handle;
        
        /* 子控件: 手工xy摆放成3*2网点(加大, 足够醒目) */
        scui_coord_t cell = SCUI_HOR_RES / 5;
        scui_coord_t span = 16;
        scui_handle_t first = SCUI_HANDLE_INVALID;
        for (uint8_t idx = 0; idx < 6; idx++) {
            scui_custom_maker_t custom_maker = scui_test_ui_layout_custom_base(layout_handle);
            scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
            
            custom_maker.widget.clip.x = (idx % 3) * (cell + span);
            custom_maker.widget.clip.y = (idx / 3) * (cell * 2 / 3 + span);
            custom_maker.widget.clip.w = cell;
            custom_maker.widget.clip.h = cell * 2 / 3;
            scui_widget_create(&custom_maker, &custom_handle);
            if (idx == 0) first = custom_handle;
        }
        
        /* 登记相对对齐: 最后一个元素相对第一个元素右下外对齐 */
        {
            scui_handle_t last = SCUI_HANDLE_INVALID;
            /* 遍历找到最后一个句柄 */
            scui_widget_t *widget_l = scui_handle_source_check(layout_handle);
            if (widget_l->child_num >= 6) {
                last = widget_l->child_list[5];
                /* 相对第一个元素外右下对齐, 偏移5像素 */
                scui_point_t off = {5, 5};
                scui_layout_item_align(layout_handle, first, last, scui_align_obl, &off);
            }
        }
        
        SCUI_LOG_WARN("layout build idx: 0 -> Item(manual xy+relative align)");
        return;
    }
    
    scui_handle_t layout_handle = SCUI_HANDLE_INVALID;
    scui_layout_maker_define(layout_maker);
    
    layout_maker.widget.style.sched_widget = true;
    layout_maker.widget.style.fully_bg     = true;
    layout_maker.widget.clip.x = (SCUI_HOR_RES - SCUI_HOR_RES * 707 / 1000) / 2;
    layout_maker.widget.clip.y = (SCUI_VER_RES - SCUI_VER_RES * 707 / 1000) / 2;
    layout_maker.widget.clip.w = SCUI_HOR_RES * 707 / 1000;
    layout_maker.widget.clip.h = SCUI_VER_RES * 707 / 1000;
    layout_maker.widget.parent    = SCUI_UI_SCENE_TEST_UI_LAYOUT;
    layout_maker.widget.child_num = 6;      /* 每行/列容纳多个且不溢出 */
    layout_maker.widget.color.color.full = 0xFF6F6F6F;   /* 亮灰 */
    layout_maker.type = scui_layout_type_flex;  /* 设置为flex, 规避unmatch type */
    
    /* flex布局配置表: {方向, 对齐, 间距, 分组} */
    typedef struct {
        bool          way;    /* 0:水平(行);1:垂直(列) */
        scui_opt_pos_t align; /* 对齐(x水平|y垂直) */
        scui_point_t  span;   /* 间距(x水平;y垂直); (-1)EVENLY=均等 */
        bool          group;  /* 分组轨道(演示换行/换列) */
    } flex_cfg_t;
    static const flex_cfg_t flex_cfg[] = {
        /* 行: 固定间距, 左/上对齐 */
        {.way = 0, .align = scui_opt_pos_l | scui_opt_pos_u,
         .span = {.x = 10, .y = 10}, .group = false},
        /* 行: 主轴均等铺开 */
        {.way = 0, .align = scui_opt_pos_l | scui_opt_pos_u,
         .span = {.x = -1 /*EVENLY*/, .y = 10}, .group = false},
        /* 行: 两组轨道(下方再排一行), 副轴均等 */
        {.way = 0, .align = scui_opt_pos_l | scui_opt_pos_ver,
         .span = {.x = 10, .y = 12}, .group = true},
        /* 列: 固定间距, 上/左对齐 */
        {.way = 1, .align = scui_opt_pos_u | scui_opt_pos_l,
         .span = {.x = 10, .y = 10}, .group = false},
        /* 列: 主轴均等铺开 */
        {.way = 1, .align = scui_opt_pos_u | scui_opt_pos_l,
         .span = {.x = 10, .y = -1 /*EVENLY*/}, .group = false},
        /* 行列均等, 整体居中 */
        {.way = 0, .align = scui_opt_pos_hor | scui_opt_pos_ver,
         .span = {.x = -1 /*EVENLY*/, .y = -1 /*EVENLY*/},
         .group = false},
        /* 行: 终点对齐(右/下) */
        {.way = 0, .align = scui_opt_pos_r | scui_opt_pos_d,
         .span = {.x = 10, .y = 10}, .group = false},
        /* 行: 水平垂直居中, 大间距 */
        {.way = 0, .align = scui_opt_pos_hor | scui_opt_pos_ver,
         .span = {.x = 28, .y = 8}, .group = false},
        /* 行: 主轴均等+右对齐 */
        {.way = 0, .align = scui_opt_pos_r | scui_opt_pos_ver,
         .span = {.x = -1 /*EVENLY*/, .y = 8}, .group = false},
        /* 列: 副轴均等, 垂直贴上 */
        {.way = 1, .align = scui_opt_pos_u | scui_opt_pos_ver,
         .span = {.x = -1 /*EVENLY*/, .y = 10}, .group = false},
    };
    static const char * const flex_tip[] = {
        "Row: fix span, TL align",
        "Row: main even",
        "Row: multi-track group",
        "Col: fix span, TL align",
        "Col: main even",
        "Both even, center",
        "Row: end align (R/D)",
        "Row: center, big span",
        "Row: main even + right",
        "Col: cross even",
    };
    SCUI_ASSERT(scui_arr_len(flex_cfg) == scui_arr_len(flex_tip));
    const flex_cfg_t *fcfg = &flex_cfg[(cfg - 1) % scui_arr_len(flex_cfg)];
    
    scui_widget_create(&layout_maker, &layout_handle);
    scui_ui_res_local->layout = layout_handle;
    
    /* flex方向/对齐/间距(合并设置, 执行在draw子流程) */
    scui_layout_flex_way(layout_handle, fcfg->way, fcfg->align, fcfg->span);
    
    /* 子控件: 统一宽高色块(方), 单行/列容纳多个且不溢出 */
    for (uint8_t idx = 0; idx < 6; idx++) {
        scui_custom_maker_t custom_maker = scui_test_ui_layout_custom_base(layout_handle);
        scui_handle_t custom_handle = SCUI_HANDLE_INVALID;
        
        custom_maker.widget.clip.w = SCUI_HOR_RES / 10;
        custom_maker.widget.clip.h = SCUI_HOR_RES / 10;
        scui_widget_create(&custom_maker, &custom_handle);
        
        /* 分组: 每3个分一组 -> 副轴堆叠出两条轨道 */
        if (fcfg->group)
            scui_layout_flex_group(layout_handle, custom_handle, idx / 3);
    }
    
    SCUI_LOG_WARN("layout build idx: %u -> %s", cfg, flex_tip[(cfg - 1) % scui_arr_len(flex_tip)]);
}

/*@brief 窗口事件响应回调
 *@param event 事件
 */
void scui_test_ui_layout_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        scui_test_ui_layout_create(scui_ui_res_local->build_idx);
        break;
    case scui_event_destroy:
        break;
    case scui_event_key_click: {
        if (event->key_id != scui_event_key_val_enter)
            break;
        
        /* 接口不支持运行时变更参数, 销毁重建更替构建配置 */
        scui_widget_destroy(scui_ui_res_local->layout);
        /* total: 1(item) + 10(flex) + 3(grid) = 14 configurations */
        scui_ui_res_local->build_idx = (scui_ui_res_local->build_idx + 1) % 14;
        scui_test_ui_layout_create(scui_ui_res_local->build_idx);
        
        scui_widget_draw(event->object, NULL, false, 0);
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}