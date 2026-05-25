#ifndef LVGL_VIEW_SCHED_H
#define LVGL_VIEW_SCHED_H

#include "lvgl.h"

// 一次性全局导入头文件
#include "lvgl_view_sched.h"
#include "lvgl_view_config.h"
#include "lvgl_view_transform.h"
#include "lvgl_view_stack.h"
#include "lvgl_view_plug.h"

// 资源缓存方案
// 若使用canvas,则为假渲染方案,需大内存支持
// 不使用canvas,则为真渲染方案,需transform样式支持
// 不使用canvas要支持lv_obj_set_style_transform_xxx(lv 9以上版本支持)
#define LVGL_VIEW_CVS_USE             (1)

// 补充宏定义
#define lvgl_view_mabs(x, m)          (((x) % (m) + (m)) % (m))
#define lvgl_view_dist(x, y)          ((x) > (y) ? (x) - (y) : (y) - (x))

// 界面创建销毁,焦点获取与丢失回调:
typedef void (*lvgl_view_create_t)(lv_obj_t *page);
typedef void (*lvgl_view_destroy_t)(lv_obj_t *page);
typedef void (*lvgl_view_focus_get_t)(lv_obj_t *page);
typedef void (*lvgl_view_focus_lost_t)(lv_obj_t *page);

typedef struct
{
    lvgl_view_create_t     create;        // 界面创建
    lvgl_view_destroy_t    destroy;       // 界面销毁(无需销毁控件)
    lvgl_view_focus_get_t  focus_get;     // 界面焦点获取
    lvgl_view_focus_lost_t focus_lost;    // 界面焦点丢失
    uint32_t page_l;        // 左临近窗口
    uint32_t page_r;        // 右临近窗口
    uint32_t page_t;        // 上临近窗口
    uint32_t page_b;        // 下临近窗口
    uint32_t transform_l;   // 左临近窗口特效交互
    uint32_t transform_r;   // 右临近窗口特效交互
    uint32_t transform_t;   // 上临近窗口特效交互
    uint32_t transform_b;   // 下临近窗口特效交互
    uint32_t enc_l: 1; // 编码器旋转唤出左窗口
    uint32_t enc_r: 1; // 编码器旋转唤出右窗口
    uint32_t enc_t: 1; // 编码器旋转唤出上窗口
    uint32_t enc_b: 1; // 编码器旋转唤出下窗口
    lv_dir_t enc_s_way;   // 编码器滚动方向控制(默认先上下, 后左右)
} lvgl_view_config_t;

typedef struct
{
    lv_obj_t    *view_root;         /* 永恒量 */

#if LVGL_VIEW_CVS_USE
    lv_obj_t    *view_cvs;          /* 特效渲染时临时构建 */
    lv_img_dsc_t view_sps_c;        /* 中间状态 */
    lv_img_dsc_t view_sps_t;        /* 中间状态 */
    lv_img_cf_t view_cvs_cf;
    void  *buffer_c;    /* 资源画布 */
    void  *buffer_t;    /* 资源画布 */
    void  *buffer_n;    /* 资源画布 */
    size_t buffer_size; /* 资源画布(size) */
#endif

    lv_point_t slide_dist;
    lv_point_t touch_press;
    lv_point_t touch_last;
    lv_point_t touch_dist;
    lv_point_t touch_curr;
    lv_coord_t touch_skip: 1;

    bool anim_lock;
    bool move_lock;
    bool jump_lock;

    lvgl_view_id_t page_c;
    lvgl_view_id_t page_t;
    lv_obj_t *view_c;
    lv_obj_t *view_t;
    lv_point_t point_c;
    lv_point_t point_t;
    lv_coord_t pct;

    // 全局通配的注入回调
    lv_event_cb_t view_event_cb;

    // 交互动画信息
    lv_anim_path_cb_t anim_path_cb;
    lv_anim_t *anim;

    // 交互风格和方向(当前)
    lvgl_view_tr_t transform_type;
    lv_dir_t slide_dir;
    lv_dir_t jump_dir;

    // 交互风格和方向(配置)
    lvgl_view_tr_t slide_type_cfg;
    lvgl_view_tr_t jump_type_cfg;
    lv_dir_t jump_dir_cfg;

    // 场景栈
    uint32_t stack[LVGL_VIEW_STACK_NEST];
    uint32_t stack_rcd[LVGL_VIEW_STACK_NEST];
    uint32_t stack_top;

    // 配置表
    lvgl_view_config_t *list_config;

} lvgl_view_sched_t;

void lvgl_view_sched_ready(void);
void lvgl_view_sched_config_slide(uint32_t type);
void lvgl_view_sched_config_jump(uint32_t type, lv_dir_t dir);
void lvgl_view_event_cb_register(lv_event_cb_t view_event_cb);

bool lvgl_view_obj_bubble_scrollable(lv_obj_t *obj, lv_dir_t dir,
                                     lv_obj_t **obj_scrollable, lv_coord_t *obj_dist);
bool lvgl_view_obj_tree_scrollable(lv_obj_t *obj, lv_dir_t dir,
                                   lv_obj_t **obj_scrollable, lv_coord_t *obj_dist);

lvgl_view_config_t *lvgl_view_id_config(uint32_t type);
void lvgl_view_id_adjust(lvgl_view_id_t type, lvgl_view_config_t *config);

bool lvgl_view_sched_jump(uint32_t page, uint32_t transform_type, lv_dir_t dir);
void lvgl_view_stack_update(uint32_t handle, uint8_t type);

#endif
