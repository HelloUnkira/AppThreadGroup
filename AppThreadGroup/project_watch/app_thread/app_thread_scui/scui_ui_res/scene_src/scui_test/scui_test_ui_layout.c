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

/*@brief 色块 */
static scui_handle_t scui_test_blk(scui_handle_t parent, scui_coord_t w, scui_coord_t h)
{
    scui_custom_maker_t m = scui_test_ui_layout_custom_base(parent);
    scui_handle_t hid = SCUI_HANDLE_INVALID;
    m.widget.clip.w = w;
    m.widget.clip.h = h;
    scui_widget_create(&m, &hid);
    return hid;
}

/*@brief 通用layout子盒子(type/尺寸/子数/底色/外部对齐) */
static scui_handle_t scui_test_subbox(scui_handle_t parent, scui_layout_type_t type,
    scui_coord_t w, scui_coord_t h, scui_coord_t cn, uint32_t bg, bool use, scui_align_t align)
{
    scui_layout_maker_define(m);
    m.widget.style.sched_widget = true;
    m.widget.style.fully_bg     = true;
    m.widget.clip.x = 0;
    m.widget.clip.y = 0;
    m.widget.clip.w = w;
    m.widget.clip.h = h;
    m.widget.parent = parent;
    m.widget.child_num = cn;
    m.widget.color.color.full = bg;
    m.type   = type;
    m.use    = use;
    m.align  = align;
    m.offset = (scui_point_t){0};
    scui_handle_t hid = SCUI_HANDLE_INVALID;
    scui_widget_create(&m, &hid);
    return hid;
}

/*@brief 填充一个flex子布局(内含色块), room=false走AUTO, 支持分组轨道; ai=轨道内交叉对齐 */
static scui_handle_t scui_test_flex_fill(scui_handle_t parent, bool way, scui_opt_pos_t align,
    scui_point_t span, scui_point_t evenly, scui_coord_t num, scui_coord_t grp, bool room,
    scui_coord_t rw, scui_coord_t rh, scui_opt_pos_t ai)
{
    scui_handle_t sub = scui_test_subbox(parent, scui_layout_type_flex,
        room ? rw : SCUI_WIDGET_AUTO_W, room ? rh : SCUI_WIDGET_AUTO_H,
        num, 0xFF2A2A2A, false, 0);
    for (scui_coord_t b = 0; b < num; b++) {
        /* 演示intra-cross时用高低错落方块, 否则统一方块 */
        scui_coord_t bh = ai ? (b % 2 ? SCUI_HOR_RES / 15 : SCUI_HOR_RES / 24) : SCUI_HOR_RES / 18;
        scui_handle_t blk = scui_test_blk(sub, SCUI_HOR_RES / 18, bh);
        if (grp > 0) scui_layout_flex_group(sub, blk, b / grp);
    }
    scui_layout_flex_way(sub, way, span, evenly, align, ai);
    return sub;
}

/*@brief 3层嵌套flex: 外层flex > 3个中层flex > 每中层2个子flex(各含2色块), Auto/固定混用 */
static scui_handle_t scui_test_flex_deep(scui_handle_t parent,
    bool oway, scui_opt_pos_t oalign, scui_point_t ospan, scui_point_t oev, bool oroom,
    scui_coord_t rw, scui_coord_t rh,
    bool iway, bool iroom)
{
    scui_coord_t mw = rw * 9 / 10;
    scui_coord_t mh = rh * 9 / 10;
    scui_handle_t outer = scui_test_subbox(parent, scui_layout_type_flex,
        oroom ? rw : SCUI_WIDGET_AUTO_W, oroom ? rh : SCUI_WIDGET_AUTO_H,
        3, 0xFF2A2A2A, false, 0);
    for (uint8_t i = 0; i < 3; i++) {
        scui_handle_t mid = scui_test_subbox(outer, scui_layout_type_flex,
            iroom ? mw : SCUI_WIDGET_AUTO_W, iroom ? mh : SCUI_WIDGET_AUTO_H,
            2, 0xFF4A4A4A, false, 0);
        for (uint8_t j = 0; j < 2; j++) {
            scui_handle_t sub2 = scui_test_subbox(mid, scui_layout_type_flex,
                SCUI_WIDGET_AUTO_W, SCUI_WIDGET_AUTO_H, 2, 0xFF5A5A5A, false, 0);
            scui_test_blk(sub2, SCUI_HOR_RES / 22, SCUI_HOR_RES / 22);
            scui_test_blk(sub2, SCUI_HOR_RES / 22, SCUI_HOR_RES / 22);
            scui_layout_flex_way(sub2, iway, (scui_point_t){4, 4}, (scui_point_t){0, 0},
                j == 0 ? (scui_opt_pos_l | scui_opt_pos_u) :
                (j == 1 ? (scui_opt_pos_hor | scui_opt_pos_u) :
                          (scui_opt_pos_r | scui_opt_pos_u)), 0);
        }
        scui_layout_flex_way(mid, iway, (scui_point_t){6, 6}, (scui_point_t){0, 0}, scui_opt_pos_l | scui_opt_pos_u, 0);
    }
    scui_layout_flex_way(outer, oway, ospan, oev, oalign, 0);
    return outer;
}

/*@brief 左右合并的双组合: 水平wrapper内 左一个子flex + 右一个子flex, 填充格子避免留白 */
static scui_handle_t scui_test_flex_pair(scui_handle_t parent, scui_coord_t rw, scui_coord_t rh,
    bool lway, scui_opt_pos_t lal, scui_point_t lsp, scui_point_t lve, bool lroom,
    bool rway, scui_opt_pos_t ral, scui_point_t rsp, scui_point_t rve, bool rroom)
{
    scui_handle_t wr = scui_test_subbox(parent, scui_layout_type_flex, rw, rh, 2, 0xFF2A2A2A, false, 0);
    scui_coord_t hw = rw / 2 - 6;
    scui_coord_t hh = rh - 20;
    scui_test_flex_fill(wr, lway, lal, lsp, lve, 4, 2, lroom, hw, hh, 0);
    scui_test_flex_fill(wr, rway, ral, rsp, rve, 4, 2, rroom, hw, hh, 0);
    scui_layout_flex_way(wr, 0, (scui_point_t){6, 6}, (scui_point_t){0, 0}, scui_opt_pos_l | scui_opt_pos_ver, 0);
    return wr;
}

/*@brief item布局: 子控件手工xy摆放, 部分登记相对对齐 */
static void scui_test_ui_layout_item_create(void)
{
    scui_handle_t layout_handle = SCUI_HANDLE_INVALID;
    scui_layout_maker_define(layout_maker);
    
    layout_maker.widget.style.sched_widget = true;
    layout_maker.widget.style.fully_bg     = true;
    layout_maker.widget.clip.x = 0;
    layout_maker.widget.clip.y = 0;
    layout_maker.widget.clip.w = SCUI_HOR_RES * 707 / 1000;
    layout_maker.widget.clip.h = SCUI_VER_RES * 707 / 1000;
    layout_maker.widget.parent    = SCUI_UI_SCENE_TEST_UI_LAYOUT;
    layout_maker.widget.child_num = 6;
    layout_maker.widget.color.color.full = 0xFF6F6F6F;  /* 亮灰 */
    layout_maker.type  = scui_layout_type_item;
    layout_maker.use   = true;          /* 开启外部对齐 */
    layout_maker.align = scui_align_icc; /* 全局居中 */
    layout_maker.offset = (scui_point_t){0};
    
    scui_widget_create(&layout_maker, &layout_handle);
    scui_ui_res_local->layout = layout_handle;
    
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
    
    /* 末元素相对首元素外右下对齐(演示item_align) */
    {
        scui_widget_t *widget_l = scui_handle_source_check(layout_handle);
        if (widget_l->child_num >= 6) {
            scui_handle_t last = widget_l->child_list[5];
            scui_point_t off = {5, 5};
            scui_layout_item_align(layout_handle, first, last, scui_align_obl, &off);
        }
    }
    
    SCUI_LOG_WARN("[item] manual xy + relative align");
}

/*@brief flex段: 每页2x2大grid, 每格一flex(显式room/auto), 含嵌套页 */
static void scui_test_ui_layout_flex_nested(uint8_t page)
{
    scui_handle_t layout_handle = SCUI_HANDLE_INVALID;
    scui_layout_maker_define(layout_maker);
    
    layout_maker.widget.style.sched_widget = true;
    layout_maker.widget.style.fully_bg     = true;
    layout_maker.widget.clip.x = 0;
    layout_maker.widget.clip.y = 0;
    layout_maker.widget.clip.w = SCUI_HOR_RES * 707 / 1000;
    layout_maker.widget.clip.h = SCUI_VER_RES * 707 / 1000;
    layout_maker.widget.parent    = SCUI_UI_SCENE_TEST_UI_LAYOUT;
    layout_maker.widget.child_num = 4;
    layout_maker.widget.color.color.full = 0xFFBEBEBE;  /* 浅灰(大) */
    layout_maker.type  = scui_layout_type_grid;
    layout_maker.use   = true;
    layout_maker.align = scui_align_icc;
    layout_maker.offset = (scui_point_t){0};
    
    scui_widget_create(&layout_maker, &layout_handle);
    scui_ui_res_local->layout = layout_handle;
    
    scui_coord_t cell_w = SCUI_HOR_RES * 707 / 1000 / 2;
    scui_coord_t cell_h = SCUI_VER_RES * 707 / 1000 / 2;
    scui_coord_t col_size[] = {cell_w, cell_w};
    scui_coord_t row_size[] = {cell_h, cell_h};
    scui_layout_grid_way(layout_handle, false, col_size, 2, 0);
    scui_layout_grid_way(layout_handle, true,  row_size, 2, 0);
    scui_coord_t room_w = cell_w * 85 / 100;
    scui_coord_t room_h = cell_h * 85 / 100;
    
    /* 4页: 0=单轨基础, 1=嵌套, 2=多行, 3=深层 */
    for (uint8_t k = 0; k < 4; k++) {
        scui_handle_t sub = SCUI_HANDLE_INVALID;
        scui_point_t pos  = {(scui_coord_t)(k % 2), (scui_coord_t)(k / 2)};
        scui_point_t span = {1, 1};

        switch (page) {
        case 0: { /* 单轨基础: 行/列 x 左/中/右/even */
            static const struct {
                bool way; scui_opt_pos_t align; scui_point_t sp; scui_point_t ev; scui_opt_pos_t ai; const char* tip;
            } b[4] = {
                {0, scui_opt_pos_l   | scui_opt_pos_u, {8, 8},  {0, 0},  0, "row left"},
                {0, scui_opt_pos_hor | scui_opt_pos_u, {8, 8},  {1, 0},  scui_opt_pos_ver, "row even mid"},
                {1, scui_opt_pos_u   | scui_opt_pos_l, {8, 8},  {0, 0},  0, "col top"},
                {1, scui_opt_pos_ver | scui_opt_pos_l, {8, 8},  {0, 1},  scui_opt_pos_ver, "col even mid"},
            };
            sub = scui_test_flex_fill(layout_handle, b[k].way, b[k].align, b[k].sp, b[k].ev,
                3, 0, true, room_w, room_h, b[k].ai);
            SCUI_LOG_WARN("[flex] p%u cell%d: %s", page, k + 1, b[k].tip);
            break;
        }
        case 1: { /* 嵌套: 外层flex(固定)含3个内层flex(Auto单行), 对照不同主对齐 */
            typedef struct { bool oway; scui_opt_pos_t oalign; scui_point_t osp; scui_point_t oev; bool iway; const char* tip; } nest_t;
            static const nest_t n[4] = {
                {1, scui_opt_pos_ver | scui_opt_pos_l, {8, 8},  {0, 0},  0, "col center span x row"},
                {1, scui_opt_pos_ver | scui_opt_pos_l, {8, 8},  {0, 1},  0, "col center even x row"},
                {0, scui_opt_pos_l   | scui_opt_pos_u, {8, 8},  {1, 0},  1, "row left even x col"},
                {0, scui_opt_pos_r   | scui_opt_pos_u, {8, 8},  {1, 0},  1, "row right even x col"},
            };
            static const scui_opt_pos_t ials_rh[3] = {scui_opt_pos_l|scui_opt_pos_u, scui_opt_pos_hor|scui_opt_pos_u, scui_opt_pos_r|scui_opt_pos_u};
            static const scui_opt_pos_t ials_cv[3] = {scui_opt_pos_u|scui_opt_pos_l, scui_opt_pos_ver|scui_opt_pos_l, scui_opt_pos_d|scui_opt_pos_l};
            const scui_opt_pos_t *ia = n[k].iway ? ials_cv : ials_rh;
            const nest_t *nc = &n[k];
            sub = scui_test_subbox(layout_handle, scui_layout_type_flex, room_w, room_h,
                3, 0xFF2A2A2A, false, 0);
            for (uint8_t i = 0; i < 3; i++)
                scui_test_flex_fill(sub, nc->iway, ia[i], (scui_point_t){5, 5}, (scui_point_t){0, 0}, 3, 0, false, 0, 0, 0);
            scui_layout_flex_way(sub, nc->oway, nc->osp, nc->oev, nc->oalign, 0);
            SCUI_LOG_WARN("[flex] p%u cell%d: NEST %s", page, k + 1, nc->tip);
            break;
        }
        case 2: { /* 多行合并: 每格 子layout 左=行多(左) 右=列多(右), 填充避免留白 */
            typedef struct {
                bool lway; scui_opt_pos_t lal; scui_point_t lsp; scui_point_t lve; bool lroom;
                bool rway; scui_opt_pos_t ral; scui_point_t rsp; scui_point_t rve; bool rroom;
                const char* tip;
            } pair_t;
            static const pair_t p[4] = {
                {0, scui_opt_pos_hor | scui_opt_pos_ver, {8, 8},  {1, 1},  true,
                 1, scui_opt_pos_ver | scui_opt_pos_hor, {8, 8},  {1, 1},  true,  "rowEven | colEven"},
                {0, scui_opt_pos_hor | scui_opt_pos_u,   {8, 8},  {0, 0},  false,
                 1, scui_opt_pos_ver | scui_opt_pos_l,   {8, 8},  {0, 0},  false, "rowSpan | colSpan(auto)"},
                {0, scui_opt_pos_r   | scui_opt_pos_hor, {8, 8},  {1, 0},  true,
                 1, scui_opt_pos_ver | scui_opt_pos_hor, {8, 8},  {0, 1},  true,  "rowEvenR | colEvenM"},
                {0, scui_opt_pos_hor | scui_opt_pos_ver, {8, 8},  {1, 1},  false,
                 0, scui_opt_pos_ver | scui_opt_pos_l,   {8, 8},  {0, 1},  false, "rowBoth | colEven(auto)"},
            };
            sub = scui_test_flex_pair(layout_handle, room_w, room_h,
                p[k].lway, p[k].lal, p[k].lsp, p[k].lve, p[k].lroom,
                p[k].rway, p[k].ral, p[k].rsp, p[k].rve, p[k].rroom);
            SCUI_LOG_WARN("[flex] p%u cell%d: %s", page, k + 1, p[k].tip);
            break;
        }
        default: { /* 深层嵌套(额外): 3层 Auto/固定混用 */
            typedef struct { bool oway; scui_opt_pos_t oalign; scui_point_t osp; scui_point_t oev; bool oroom;
                             bool iway; bool iroom; const char* tip; } deep_t;
            static const deep_t d[4] = {
                {1, scui_opt_pos_ver | scui_opt_pos_l, {8, 8},  {0, 1},  true,  0, false, "L3 colEven(auto mid)"},
                {0, scui_opt_pos_hor | scui_opt_pos_u, {8, 8},  {0, 1},  false, 1, true,  "L3 rowEven(room mid)"},
                {1, scui_opt_pos_ver | scui_opt_pos_l, {8, 8},  {1, 0},  true,  1, true,  "L3 colCrossEven(room)"},
                {0, scui_opt_pos_hor | scui_opt_pos_ver, {8, 8},  {1, 1},  true,  0, false, "L3 rowBothEven(auto)"},
            };
            sub = scui_test_flex_deep(layout_handle, d[k].oway, d[k].oalign, d[k].osp, d[k].oev, d[k].oroom,
                room_w, room_h, d[k].iway, d[k].iroom);
            SCUI_LOG_WARN("[flex] p%u cell%d: %s", page, k + 1, d[k].tip);
            break;
        }
        }

        scui_layout_grid_cell(layout_handle, sub, pos, span,
            scui_opt_pos_hor | scui_opt_pos_ver, 0);
    }

    SCUI_LOG_WARN("[flex] page%u grid 2x2 wrapping flex", page);
}

/*@brief grid段: 每格一个复杂嵌套布局(grid套grid / flex入格) 测试多种组合 */
static void scui_test_ui_layout_grid_combo(uint8_t idx)
{
    scui_handle_t layout_handle = SCUI_HANDLE_INVALID;
    scui_layout_maker_define(layout_maker);
    
    layout_maker.widget.style.sched_widget = true;
    layout_maker.widget.style.fully_bg     = true;
    layout_maker.widget.clip.x = 0;
    layout_maker.widget.clip.y = 0;
    layout_maker.widget.clip.w = SCUI_HOR_RES * 707 / 1000;
    layout_maker.widget.clip.h = SCUI_VER_RES * 707 / 1000;
    layout_maker.widget.parent    = SCUI_UI_SCENE_TEST_UI_LAYOUT;
    layout_maker.widget.child_num = 4;
    layout_maker.widget.color.color.full = 0xFFBEBEBE;  /* 浅灰(大) */
    layout_maker.type  = scui_layout_type_grid;
    layout_maker.use   = true;
    layout_maker.align = scui_align_icc;
    layout_maker.offset = (scui_point_t){0};
    
    scui_widget_create(&layout_maker, &layout_handle);
    scui_ui_res_local->layout = layout_handle;
    
    scui_coord_t cell_w = SCUI_HOR_RES * 707 / 1000 / 2;
    scui_coord_t cell_h = SCUI_VER_RES * 707 / 1000 / 2;
    scui_coord_t col_size[] = {cell_w, cell_w};
    scui_coord_t row_size[] = {cell_h, cell_h};
    scui_layout_grid_way(layout_handle, false, col_size, 2, 0);
    scui_layout_grid_way(layout_handle, true,  row_size, 2, 0);
    
    for (uint8_t k = 0; k < 4; k++) {
        scui_handle_t sub = SCUI_HANDLE_INVALID;
        scui_point_t pos  = {(scui_coord_t)(k % 2), (scui_coord_t)(k / 2)};
        scui_point_t span = {1, 1};
        
        if (idx == 0) {
            /* 每格一个2x2小grid(深灰), 不同cell对齐/拉伸 + 一个flex */
            scui_coord_t cw = cell_w * 80 / 100, chh = cell_h * 80 / 100;
            const scui_coord_t g2[] = {cw / 2, cw / 2};
            const scui_coord_t h2[] = {chh / 2, chh / 2};
            sub = scui_test_subbox(layout_handle, scui_layout_type_grid, cw, chh, 4, 0xFF3A3A3A, false, 0);
            scui_layout_grid_way(sub, false, g2, 2, 6);
            scui_layout_grid_way(sub, true,  h2, 2, 6);
            /* 4格: 左上 / 居中 / 右下 / 拉伸 */
            scui_point_t cpos[4] = {{0,0},{1,0},{0,1},{1,1}};
            scui_point_t csp[4]  = {{1,1},{1,1},{1,1},{1,1}};
            scui_opt_pos_t cal_[4] = {scui_opt_pos_l|scui_opt_pos_u, scui_opt_pos_hor|scui_opt_pos_ver,
                                      scui_opt_pos_r|scui_opt_pos_d, 0};
            scui_opt_dir_t cst_[4] = {0,0,0,scui_opt_dir_hor};
            for (uint8_t q = 0; q < 4; q++) {
                scui_handle_t bk = scui_test_blk(sub, cw / 4, chh / 4);
                scui_layout_grid_cell(sub, bk, cpos[q], csp[q], cal_[q], cst_[q]);
            }
        } else {
            /* grid套flex: 每格一格grid, 内含一flex(横) + flex(纵) 混合 */
            scui_coord_t cw = cell_w * 80 / 100, chh = cell_h * 80 / 100;
            const scui_coord_t g2[] = {cw, cw};
            const scui_coord_t h2[] = {chh * 3 / 10, chh * 7 / 10};
            sub = scui_test_subbox(layout_handle, scui_layout_type_grid, cw, chh, 2, 0xFF3A3A3A, false, 0);
            scui_layout_grid_way(sub, false, g2, 2, 6);
            scui_layout_grid_way(sub, true,  h2, 2, 6);
            /* 上格: flex横 3块; 下格: flex纵 3块(拉伸占据) */
            scui_handle_t hf = scui_test_flex_fill(sub, 0, scui_opt_pos_hor | scui_opt_pos_u,
                (scui_point_t){6, 6}, (scui_point_t){1, 0}, 3, 0, true, cw - 30, chh * 3 / 10 - 30, 0);
            scui_handle_t vf = scui_test_flex_fill(sub, 1, scui_opt_pos_ver | scui_opt_pos_l,
                (scui_point_t){6, 6}, (scui_point_t){0, 1}, 3, 0, true, cw - 30, chh * 7 / 10 - 30, 0);
            scui_layout_grid_cell(sub, hf, (scui_point_t){0, 0}, (scui_point_t){2, 1}, 0, 0);
            scui_layout_grid_cell(sub, vf, (scui_point_t){0, 1}, (scui_point_t){2, 1}, 0, 0);
        }
        
        scui_layout_grid_cell(layout_handle, sub, pos, span, scui_opt_pos_hor | scui_opt_pos_ver, 0);
        SCUI_LOG_WARN("[grid] page%u cell%d: sub grid-combo", idx, k + 1);
    }
    
    SCUI_LOG_WARN("[grid] page%u grid 2x2 complex combos", idx);
}

/*@brief 构建配置分派: item(1) + flex(4页) + grid(2页) */
static void scui_test_ui_layout_create(uint8_t cfg)
{
    switch (cfg) {
    case 0:
        scui_test_ui_layout_item_create();
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        scui_test_ui_layout_flex_nested(cfg - 1);
        break;
    default:
        scui_test_ui_layout_grid_combo(cfg - 5);
        break;
    }
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
        
        /* 接口不支持运行时变更参数, 销毁重建翻页: item + 4*flex + 2*grid = 7 */
        scui_widget_destroy(scui_ui_res_local->layout);
        scui_ui_res_local->build_idx = (scui_ui_res_local->build_idx + 1) % 7;
        scui_test_ui_layout_create(scui_ui_res_local->build_idx);
        
        scui_widget_draw(event->object, NULL, false, 0);
        scui_event_mask_over(event);
        break;
    }
    default:
        break;
    }
}