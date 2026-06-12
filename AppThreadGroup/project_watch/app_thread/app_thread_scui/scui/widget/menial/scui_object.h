#ifndef SCUI_OBJECT_H
#define SCUI_OBJECT_H

/*对象控件(lvgl式)
 *它会吸收lvgl控件设计部分思想
 *最终为用于menial控件继承使用
 */

typedef enum {
    scui_object_type_none  = 0,
    scui_object_type_part  = 0x1000,
    scui_object_type_state = 0x2000,
    scui_object_type_style = 0x3000,
    scui_object_type_limit = 0x0FFF,
    scui_object_type_mask  = 0x00FF,
    
    scui_object_type_sub   = 0x0000,
    scui_object_type_sub1  = 0x0100,
    scui_object_type_sub2  = 0x0200,
    scui_object_type_sub3  = 0x0300,
    scui_object_type_sub4  = 0x0400,
    
    /*************************************************************************/
    scui_object_state_sub = scui_object_type_state + scui_object_type_sub,
    
    scui_object_state_def,
    scui_object_state_pre,
    scui_object_state_chk,
    
    /*************************************************************************/
    scui_object_part_sub = scui_object_type_part + scui_object_type_sub,
    
    scui_object_part_main,      /* 全局 */
    scui_object_part_rect_bg,   /* 背景 */
    scui_object_part_rect_fg,   /* 前景 */
    scui_object_part_rect_edge, /* 边界(可选扩充) */
    scui_object_part_rect_box,  /* 盒子(可选扩充) */
    scui_object_part_rect_sha,  /* 阴影(可选扩充) */
    scui_object_part_arc_bg,    /* 背景 */
    scui_object_part_arc_fg,    /* 前景 */
    scui_object_part_line,      /*  */
    
    /*************************************************************************/
    scui_object_style_main_time_s = scui_object_type_style + scui_object_type_sub1,
    
    scui_object_style_main_time,
    
    scui_object_style_main_time_e,
    /*************************************************************************/
    /* 背景/前景/边界/盒子/阴影 */
    scui_object_style_rect_s = scui_object_type_style + scui_object_type_sub2,
    
    scui_object_style_rect_alpha,
    scui_object_style_rect_color,
    scui_object_style_rect_point,
    scui_object_style_rect_align,
    scui_object_style_rect_width,
    scui_object_style_rect_height,
    scui_object_style_rect_radius,
    scui_object_style_rect_side_width,
    scui_object_style_rect_color_grad,
    scui_object_style_rect_multi,
    
    scui_object_style_rect_e,
    /*************************************************************************/
    /* 弧形 */
    scui_object_style_arc_s = scui_object_type_style + scui_object_type_sub3,
    
    scui_object_style_arc_alpha,
    scui_object_style_arc_color,
    scui_object_style_arc_angle_s,
    scui_object_style_arc_angle_e,
    scui_object_style_arc_center,
    scui_object_style_arc_radius,
    scui_object_style_arc_side_width,
    scui_object_style_arc_color_grad,
    scui_object_style_arc_multi,
    
    scui_object_style_arc_e,
    /*************************************************************************/
    /* 线 */
    scui_object_style_line_s = scui_object_type_style + scui_object_type_sub4,
    
    scui_object_style_line_alpha,
    scui_object_style_line_color,
    scui_object_style_line_area,
    scui_object_style_line_vpos,
    scui_object_style_line_vpos_num,
    scui_object_style_line_side_width,
    scui_object_style_line_multi,
    
    scui_object_style_line_e,
    
} scui_object_type_t;

#define scui_object_part_idx(part)      (scui_object_type_mask & (scui_object_part_##part   - 1))
#define scui_object_state_idx(state)    (scui_object_type_mask & (scui_object_state_##state - 1))
#define scui_object_style_idx(style)    (scui_object_type_mask & (scui_object_style_##style - 1))

typedef union {
    scui_area_t  area;
    scui_point_t point;
    scui_alpha_t alpha;
    scui_multi_t number;
    scui_color32_t color32;
    scui_opt_pos_t align;
    void *pointer;
    
    /* 组合值: */
    struct {
    scui_sbitfd_t round:1;      /* arc */
    scui_sbitfd_t shadow:1;     /* rect */
    scui_sbitfd_t grad_w:1;     /* rect,arc */
    scui_sbitfd_t grad:1;       /* rect,arc */
    } multi;
} scui_object_data_t;

/* property */
typedef struct {
    scui_object_type_t part;    /* 部件 */
    scui_object_type_t state;   /* 状态 */
    scui_object_type_t style;   /* 样式 */
    scui_object_data_t data;    /* 属性数据 */
    scui_sbitfd_t      use:1;   /* 内部:有效 */
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
    /* 内部域: */
    scui_sbitfd_t       press:1;        /* 使用pre状态 */
    scui_sbitfd_t       check:1;        /* 使用chk状态 */
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
