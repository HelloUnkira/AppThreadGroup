#ifndef LVGL_VIEW_STASK_H
#define LVGL_VIEW_STASK_H

// 场景栈深
#define LVGL_VIEW_STACK_NEST            (20)

uint32_t lvgl_view_stack_nest(void);
uint32_t lvgl_view_stack_node(uint32_t top);
uint32_t lvgl_view_stack_last(void);
uint32_t lvgl_view_stack_top(void);

// 最近记录:0:当前;1:前一界面;以此类推
uint32_t lvgl_view_stack_rcd_last(uint32_t idx);



bool lvgl_view_stack_reset_by(uint32_t page, bool reserve,
    uint32_t transform_type, lv_dir_t dir);
bool lvgl_view_stack_goback_by(uint32_t page, uint32_t transform_type,
    lv_dir_t dir);
bool lvgl_view_stack_cover_by(uint32_t page, uint32_t transform_type,
    lv_dir_t dir);
bool lvgl_view_stack_add_by(uint32_t page, bool reserve,
    uint32_t transform_type, lv_dir_t dir);
bool lvgl_view_stack_del_by(uint32_t page, uint32_t transform_type,
    lv_dir_t dir);

bool lvgl_view_stack_add_prio_by(uint32_t page, bool reserve,
    uint32_t transform_type, lv_dir_t dir,
    bool (*prio_compare)(uint32_t page, uint32_t page_new));




static inline bool lvgl_view_stack_reset(uint32_t page, bool reserve)
{
    return lvgl_view_stack_reset_by(page, reserve, lvgl_view_tr_auto, LV_DIR_NONE);
}
static inline bool lvgl_view_stack_goback(uint32_t page)
{
    return lvgl_view_stack_goback_by(page, lvgl_view_tr_auto, LV_DIR_NONE);
}
static inline bool lvgl_view_stack_cover(uint32_t page)
{
    return lvgl_view_stack_cover_by(page, lvgl_view_tr_auto, LV_DIR_NONE);
}
static inline bool lvgl_view_stack_add(uint32_t page, bool reserve)
{
    return lvgl_view_stack_add_by(page, reserve, lvgl_view_tr_auto, LV_DIR_NONE);
}
static inline bool lvgl_view_stack_del(uint32_t page)
{
    return lvgl_view_stack_del_by(page, lvgl_view_tr_auto, LV_DIR_NONE);
}

static inline bool lvgl_view_stack_add_prio(uint32_t page, bool reserve,
    bool (*prio_compare)(uint32_t page, uint32_t page_new))
{
    return lvgl_view_stack_add_prio_by(page, reserve, lvgl_view_tr_auto,
            LV_DIR_NONE, prio_compare);
}



/* 额外扩充语义, 将add,del融合在一起的效果, 可以不使用层级管理理念设计界面层级跳转 */
static inline bool lvgl_view_stack_jump_by(uint32_t page,
    uint32_t transform_type, lv_dir_t dir)
{
    if (lvgl_view_stack_goback_by(page, transform_type, dir))
    {
        true;
    }
    
    return lvgl_view_stack_add_by(page, false, transform_type, dir);
}

/* 额外扩充语义, 将add,del融合在一起的效果, 可以不使用层级管理理念设计界面层级跳转 */
static inline bool lvgl_view_stack_jump(uint32_t page)
{
    return lvgl_view_stack_jump_by(page, lvgl_view_tr_auto, LV_DIR_NONE);
}

#endif
