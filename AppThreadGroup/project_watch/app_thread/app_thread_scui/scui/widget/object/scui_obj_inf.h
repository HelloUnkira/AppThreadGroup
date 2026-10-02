#ifndef SCUI_OBJ_INF_H
#define SCUI_OBJ_INF_H

/*@brief 样式资源
 */
typedef struct {
    scui_object_type_t part;        /* 关键部分(rect_bg) */
    scui_object_type_t form;        /* 层级(all/base/edge/box/sha) */
    scui_color_t       color[4];    /* 颜色(def[0]/pre[1]/chk[2]/pre[3]; s状态色->e渐变) */
    scui_coord_t       width;       /* 边界(实心:<=0;空心:>0; 该层stroke) */
    scui_coord_t       radius;      /* 圆角半径(最大:<0; 基准all) */
    scui_alpha_t       alpha;       /* 透明度(默认cover) */
    scui_opt_pos_t     align;       /* 对齐(默认中心; 基准all) */
    scui_coord_t       time;        /* 动画时间(ms) */
    scui_coord_t       lim;         /* 缩小限制(pct) */
    scui_sbitfd_t      grad:1;      /* 渐变(可选) */
    scui_sbitfd_t      gradw:1;     /* 渐变方向(水平:0;垂直:1) */
} scui_obj_btn_res_t;

/*@brief 样式资源
 */
typedef struct {
    scui_object_type_t part;        /* 关键部分(arc_bg/arc_fg/arc_knob) */
    scui_object_type_t form;        /* 层级(all/base/edge/box/sha) */
    scui_point_t       center;      /* 弧心(基准all) */
    scui_coord_t       width;       /* 弧宽(扇形:<= 0;弧型:>0; 该层stroke) */
    scui_coord_t       radius;      /* 半径(>0; 基准all) */
    scui_coord3_t      angle_s;     /* 起始角度(默认:0; 基准all) */
    scui_coord3_t      angle_e;     /* 结束角度(默认:360; 基准all) */
    scui_color_t       color;       /* 颜色(s状态色->e渐变) */
    scui_coord_t       time;        /* 动画时间(ms) */
    scui_alpha_t       alpha;       /* 透明度(默认cover) */
    scui_sbitfd_t      round:1;     /* 端点圆角(可选) */
    scui_sbitfd_t      gradw:1;     /* 渐变方向(水平:0;垂直:1) */
    scui_sbitfd_t      grad:1;      /* 渐变(可选)(s->e) */
} scui_obj_arc_res_t;

/*@brief 样式资源
 */
typedef struct {
    scui_object_type_t part;        /* 关键部分(rect_bg/rect_fg/rect_knob) */
    scui_object_type_t form;        /* 层级(all/base/edge/box/sha) */
    scui_color_t       color;       /* 颜色(s状态色->e渐变) */
    scui_coord_t       width;       /* 边界(实心:<=0;空心:>0; 该层stroke) */
    scui_coord_t       radius;      /* 圆角半径(最大:<0; 基准all) */
    scui_alpha_t       alpha;       /* 透明度(默认cover) */
    scui_opt_pos_t     align;       /* 对齐(默认左上; 基准all) */
    scui_coord_t       time;        /* 动画时间(ms) */
    scui_sbitfd_t      grad:1;      /* 渐变(可选)(s->e) */
    scui_sbitfd_t      gradw:1;     /* 渐变方向(水平:0;垂直:1) */
} scui_obj_bar_res_t;

/*@brief 样式资源
 */
typedef struct {
    scui_object_type_t part;        /* 关键部分(line_item/rect_fg) */
    scui_object_type_t form;        /* 层级(base; 线条无层级恒0) */
    scui_sbitfd_t      round:1;     /* 端点圆角 */
    scui_sbitfd_t      grad:1;      /* 折线阴影 */
    scui_color_t       color;       /* 颜色 */
    scui_coord_t       width;       /* 线宽 */
    scui_alpha_t       alpha;       /* 透明度(默认cover) */
} scui_obj_cht_res_t;

/*@brief 样式资源
 */
typedef struct {
    scui_object_type_t part;        /* 关键部分(rect_bg) */
    scui_object_type_t form;        /* 层级(all/base/sha) */
    scui_color_t       color_on;    /* 点亮色(状态色->渐变) */
    scui_color_t       color_off;   /* 熄灭色(淡白) */
    scui_coord_t       width;       /* 边界(实心:<=0;空心:>0; 该层stroke) */
    scui_coord_t       radius;      /* 圆角半径(最大:<0; 基准all) */
    scui_alpha_t       alpha;       /* 透明度(默认cover) */
    scui_opt_pos_t     align;       /* 对齐(默认中心; 基准all) */
    scui_coord_t       brightness;  /* 亮度(0-100, 默认100) */
    scui_sbitfd_t      grad:1;      /* 渐变(可选) */
    scui_sbitfd_t      gradw:1;     /* 渐变方向(水平:0;垂直:1) */
} scui_obj_led_res_t;

/*@brief 样式资源
 */
typedef struct {
    scui_object_type_t part;        /* 关键部分(rect_item) */
    scui_object_type_t form;        /* 层级(all/base/edge/box/sha) */
    scui_color_t       color[2];    /* 颜色(def[0]/pre[1]; s状态色->e渐变) */
    scui_coord_t       width;       /* 边界(实心:<=0;空心:>0; 该层stroke) */
    scui_coord_t       radius;      /* 圆角半径(最大:<0; 基准all) */
    scui_alpha_t       alpha;       /* 透明度(默认cover) */
    scui_opt_pos_t     align;       /* 对齐(默认左上; 基准all) */
    scui_sbitfd_t      grad:1;      /* 渐变(可选) */
    scui_sbitfd_t      gradw:1;     /* 渐变方向(水平:0;垂直:1) */
} scui_obj_bmat_res_t;

/*@brief 样式资源
 */
typedef struct {
    scui_object_type_t part;        /* 关键部分(line_item) */
    scui_sbitfd_t      round:1;     /* 端点圆角 */
    scui_sbitfd_t      grad:1;      /* 折线阴影 */
    scui_color_t       color;       /* 颜色 */
    scui_coord_t       width;       /* 线宽 */
    scui_alpha_t       alpha;       /* 透明度(默认cover) */
} scui_obj_line_res_t;

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_btn_style(scui_handle_t handle, scui_obj_btn_res_t *res);

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_arc_style(scui_handle_t handle, scui_obj_arc_res_t *res);

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_bar_style(scui_handle_t handle, scui_obj_bar_res_t *res);

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_cht_style(scui_handle_t handle, scui_obj_cht_res_t *res);

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_led_style(scui_handle_t handle, scui_obj_led_res_t *res);

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_bmat_style(scui_handle_t handle, scui_obj_bmat_res_t *res);

/*@brief 控件样式应用
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_obj_line_style(scui_handle_t handle, scui_obj_line_res_t *res);

/******************************************************************************/

/*@brief 控件当前值
 *@param handle 控件句柄
 *@param angle  目标角度
 */
void scui_obj_arc_current_angle(scui_handle_t handle, scui_coord3_t *angle);

/*@brief 控件更新值
 *@param handle 控件句柄
 *@param angle  目标角度
 *@param anim   动画更新
 */
void scui_obj_arc_update_angle(scui_handle_t handle, scui_coord3_t angle, bool anim);

/*@brief 控件更新值
 *@param handle 控件句柄
 *@param value  目标进度[0.0f, 100.0f]
 *@param anim   动画更新
 */
void scui_obj_arc_update_value(scui_handle_t handle, scui_coord3_t value, bool anim);

/*@brief 控件当前值
 *@param handle 控件句柄
 *@param value  目标进度
 */
void scui_obj_bar_current_value(scui_handle_t handle, scui_coord3_t *value);

/*@brief 控件更新值
 *@param handle 控件句柄
 *@param value  目标进度[0.0f, value_lim]
 *@param anim   动画更新
 */
void scui_obj_bar_update_value(scui_handle_t handle, scui_coord3_t value, bool anim);

/*@brief 控件类型
 *@param handle 控件句柄
 *@param type   子类型
 */
void scui_obj_cht_type(scui_handle_t handle, scui_coord_t *type);

/*@brief 控件数据列表更新
 *@param handle    控件句柄
 *@param vlist_min 数据列表
 *@param vlist_max 数据列表
 */
void scui_obj_cht_hist_data(scui_handle_t handle, scui_coord_t *vlist_min, scui_coord_t *vlist_max);

/*@brief 控件数据列表更新
 *@param handle    控件句柄
 *@param vlist_dot 数据列表
 */
void scui_obj_cht_line_data(scui_handle_t handle, scui_coord_t *vlist_dot);

/*@brief 控件循环推送(环上写入一个样本)
 *@param handle  控件句柄
 *@param value_1 数据值(hist:最小值; line:数据值)
 *@param value_2 数据值(hist:最大值; line:忽略)
 */
void scui_obj_cht_loop_push(scui_handle_t handle, scui_coord_t value_1, scui_coord_t value_2);

/*@brief 控件点亮颜色设置
 *@param handle    控件句柄
 *@param color_on  点亮颜色
 *@param color_off 熄灭颜色
 */
void scui_obj_led_color(scui_handle_t handle, scui_color32_t color_on, scui_color32_t color_off);

/*@brief 控件亮度设置
 *@param handle 控件句柄
 *@param level  亮度(0-100)
 */
void scui_obj_led_level(scui_handle_t handle, scui_coord_t level);

/*@brief 控件亮灭设置
 *@param handle 控件句柄
 *@param toggle 切换(亮<->灭)
 *@param onoff  亮灭(非切换时生效)
 */
void scui_obj_led_onoff(scui_handle_t handle, bool toggle, bool onoff);

/*@brief 对象控件标记获取
 *@param handle 控件句柄
 *retval 对象控件标记
 */
bool scui_obj_chk_fixed(scui_handle_t handle);

/*@brief 对象控件状态获取(特殊语义)
 *@param handle 控件句柄
 *@param state  对象控件状态
 */
void scui_obj_chk_state(scui_handle_t handle, scui_object_type_t *state);

/*@brief 条目序列设置
 *@param handle    控件句柄
 *@param item_num  条目数量(<=构造上限)
 *@param row_num   轨道数量
 *@param item_unit 条目宽度(单位数; 空=等分)
 *@param item_row  条目轨道(升序; 空=单轨)
 */
void scui_obj_bmat_item_set(scui_handle_t handle, scui_coord_t item_num, scui_coord_t row_num,
    scui_coord_t *item_unit, scui_coord_t *item_row);

/*@brief 条目区域
 *@param handle    控件句柄
 *@param idx       条目号
 *@param item_area 条目区域(控件相对)
 */
void scui_obj_bmat_item_area(scui_handle_t handle, scui_coord_t idx, scui_area_t *item_area);

/*@brief 最近点击条目
 *@param handle 控件句柄
 *@retval 条目号(-1:无)
 */
scui_coord_t scui_obj_bmat_click_item(scui_handle_t handle);

/*@brief 按下条目
 *@param handle 控件句柄
 *@retval 条目号(-1:无)
 */
scui_coord_t scui_obj_bmat_press_item(scui_handle_t handle);

/*@brief 端点序列设置
 *@param handle  控件句柄
 *@param seg_num 线段数量(<=构造上限; 1:连续)
 *@param dot_num 端点数量(<=构造上限)
 *@param seg_dot 线段端点数(空=等分)
 *@param vpos    端点序列(坐标:控件相对; 极坐标:角度+半径)
 */
void scui_obj_line_data_set(scui_handle_t handle, scui_coord_t seg_num,
    scui_coord_t dot_num, scui_coord_t *seg_dot, scui_point_t *vpos);

#endif
