#ifndef SCUI_OBJECT_H
#define SCUI_OBJECT_H

/*对象控件(lvgl式)
 *它会吸收lvgl控件设计部分思想
 *最终为用于menial控件继承使用
 */

typedef enum {
    scui_object_type_none = 0,
    
    /* field part: */
    scui_object_part_s  = 0x100,
    scui_object_part_crect_bg,  /* bg */
    scui_object_part_crect_bd,  /* border */
    scui_object_part_crect_ol,  /* outline */
    scui_object_part_crect_sd,  /* shadow */
    scui_object_part_main,
    scui_object_part_item,
    scui_object_part_e  = 0x199,
    /* field state: */
    scui_object_state_s = 0x200,
    scui_object_state_def,
    scui_object_state_pre,
    scui_object_state_chk,
    scui_object_state_e = 0x299,
    /* field style: */
    scui_object_style_s = 0x300,
    scui_object_style_crect_alpha,
    scui_object_style_crect_color_s,
    scui_object_style_crect_color_e,
    scui_object_style_crect_width,
    scui_object_style_crect_radius,
    scui_object_style_crect_multi,
    scui_object_style_width,
    scui_object_style_height,
    scui_object_style_radius,
    scui_object_style_color_bg,
    scui_object_style_color_fg,
    scui_object_style_alpha,
    scui_object_style_e = 0x399,
} scui_object_type_t;

typedef union {
    scui_alpha_t   alpha;
    scui_multi_t   number;
    scui_color32_t color32;
    
    /* 组合值: */
    struct {
    scui_sbitfd_t shadow:1;
    scui_sbitfd_t grad_w:1;
    scui_sbitfd_t grad:1;
    } multi;
} scui_object_data_t;

/* property */
typedef struct {
    scui_object_type_t part;    /* 部件 */
    scui_object_type_t state;   /* 状态 */
    scui_object_type_t style;   /* 样式 */
    scui_object_data_t data;    /* 属性数据 */
    scui_sbitfd_t      use:1;   /* 有效 */
} scui_object_prop_t;

/* transition */
typedef struct {
    scui_object_type_t part;    /* 部件 */
    scui_object_type_t state_p; /* 状态(prev) */
    scui_object_type_t state_n; /* 状态(next) */
    scui_object_type_t style;   /* 样式 */
    scui_object_data_t data_p;  /* 属性数据 */
    scui_object_data_t data_n;  /* 属性数据 */
    
    scui_map_cb_t path;
    scui_coord_t  time;
    scui_coord_t  delay;
    scui_coord_t  tick_d;       /* 内部 */
    scui_coord_t  tick_t;       /* 内部 */
    scui_coord_t  pct_c;        /* 内部:百分比 */
    scui_sbitfd_t use:1;        /* 内部:有效 */
} scui_object_tran_t;

typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord_t        check:1;        /* 使用chk状态 */
    /* 内部域: */
    scui_object_type_t  state_l;        /* 上一状态(last) */
    scui_object_type_t  state_c;        /* 当前状态(curr) */
    scui_object_prop_t *prop_list;      /* 属性列表 */
    scui_object_tran_t *tran_list;      /* 过渡列表 */
    scui_coord_t        prop_num;       /* 属性数量 */
    scui_coord_t        tran_num;       /* 过渡数量 */
    scui_coord_t        prop_cur;       /* 属性数量 */
    scui_coord_t        tran_cur;       /* 过渡数量 */
} scui_object_t;

#pragma pack(push, 1)
typedef struct {
    /* 继承域: */
    SCUI_EXTEND_FIELD_S
    scui_widget_maker_t widget;
    SCUI_EXTEND_FIELD_E
    /* 外部域: */
    scui_coord_t        check:1;        /* 使用chk状态 */
} scui_object_maker_t;
#pragma pack(pop)

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_object_make(void *inst, void *inst_maker, scui_handle_t *handle);

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_object_burn(scui_handle_t handle);

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_object_invoke(scui_event_t *event);

#endif
