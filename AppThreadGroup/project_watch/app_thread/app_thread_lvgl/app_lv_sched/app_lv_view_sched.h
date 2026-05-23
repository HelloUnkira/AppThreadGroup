#ifndef APP_LV_VIEW_SCHED_H
#define APP_LV_VIEW_SCHED_H

#include "lvgl.h"

// 补充宏定义
#define app_lv_view_mabs(x, m)          (((x) % (m) + (m)) % (m))
#define app_lv_view_dist(x, y)          ((x) > (y) ? (x) - (y) : (y) - (x))

// 界面创建销毁,焦点获取与丢失回调:
typedef void (*app_lv_view_create_t)(lv_obj_t *page);
typedef void (*app_lv_view_destroy_t)(lv_obj_t *page);
typedef void (*app_lv_view_focus_get_t)(lv_obj_t *page);
typedef void (*app_lv_view_focus_lost_t)(lv_obj_t *page);

typedef struct {
    app_lv_view_create_t  create;       // 创建自己回调
    app_lv_view_destroy_t destroy;      // 回收资源回调
    uint32_t page_l;                    // 左临近窗口
    uint32_t page_r;                    // 右临近窗口
    uint32_t page_t;                    // 上临近窗口
    uint32_t page_b;                    // 下临近窗口
    uint32_t transform_l;               // 左临近窗口特效交互
    uint32_t transform_r;               // 右临近窗口特效交互
    uint32_t transform_t;               // 上临近窗口特效交互
    uint32_t transform_b;               // 下临近窗口特效交互
    uint32_t enc_l:1;                   // 编码器旋转唤出左窗口
    uint32_t enc_r:1;                   // 编码器旋转唤出右窗口
    uint32_t enc_t:1;                   // 编码器旋转唤出上窗口
    uint32_t enc_b:1;                   // 编码器旋转唤出下窗口
    lv_dir_t enc_scroll_way;            // 编码器滚动方向控制(默认先上下, 后左右)
} app_lv_view_config_t;

#include "app_lv_view_transform.h"
#include "app_lv_view_config.h"
#include "app_lv_view_stack.h"
#include "app_lv_view_plug.h"

typedef struct {
    lv_obj_t    *slide_effect;  /* 永恒量 */
    // lv_obj_t    *canvas;        /* 特效渲染时临时构建 */
    // lv_img_dsc_t snapshot_c;    /* 中间状态 */
    // lv_img_dsc_t snapshot_t;    /* 中间状态 */
    
    lv_dir_t   slide_dir;
    lv_point_t slide_dist;
    lv_point_t touch_press;
    lv_point_t touch_last;
    lv_point_t touch_dist;
    lv_point_t touch_curr;
    lv_coord_t touch_skip:1;
    
    bool anim_lock;
    bool move_lock;
    bool jump_lock;
    
    // void *buffer_c;
    // void *buffer_t;
    // void *buffer_n;
    // size_t buffer_size;
    
    // void *buffer_c_static;
    // void *buffer_t_static;
    // void *buffer_n_static;
    // size_t buffer_size_static;
    
    uint32_t page_c;
    uint32_t page_t;
    lv_obj_t *view_c;
    lv_obj_t *view_t;
    lv_point_t point_c;
    lv_point_t point_t;
    lv_coord_t pct;
    
    lv_event_cb_t view_event_cb;
    
    uint32_t transform_type;
    lv_anim_path_cb_t anim_path_cb;
    lv_anim_t *anim;
    
    uint32_t slide_type_cfg;
    uint32_t jump_type_cfg;
    lv_dir_t jump_dir_cfg;
    lv_dir_t jump_dir;
    
    // 场景栈
    uint32_t stack[APP_LV_VIEW_STACK_NEST];
    uint32_t stack_rcd[APP_LV_VIEW_STACK_NEST];
    uint32_t stack_top;
    
} app_lv_view_sched_t;

void app_lv_view_sched_ready(void);
void app_lv_view_sched_config_page(uint32_t page);
bool app_lv_view_sched_obj_scrollable_bubble(lv_obj_t *obj, lv_dir_t dir, lv_obj_t **obj_scrollable, lv_coord_t *obj_dist);
bool app_lv_view_sched_obj_scrollable(lv_obj_t *obj, lv_dir_t dir, lv_obj_t **obj_scrollable, lv_coord_t *obj_dist);
void app_lv_view_sched_view_event_cb_register(lv_event_cb_t view_event_cb);

void app_lv_view_id_ready(void);
void app_lv_view_id_adjust(app_lv_view_id_t type, app_lv_view_config_t *config);
app_lv_view_config_t *app_lv_view_id_inst(uint32_t type);

void app_lv_view_sched_config_slide(uint32_t type);
void app_lv_view_sched_config_jump(uint32_t type, lv_dir_t dir);

bool app_lv_view_sched_jump(uint32_t page, uint32_t transform_type, lv_dir_t dir);
void app_lv_view_stack_update(uint32_t handle, uint8_t type);

#endif
