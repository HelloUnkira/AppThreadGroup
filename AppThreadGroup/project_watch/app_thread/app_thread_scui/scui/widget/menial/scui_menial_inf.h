#ifndef SCUI_MENIAL_INF_H
#define SCUI_MENIAL_INF_H

/*@brief 仆从控件:样式资源(子类型)
 */
typedef struct {
    scui_object_type_t part;     /* 部件(bg/edge/box/sha) */
    scui_color_t       color[2]; /* 颜色(未选中[0]选中[1]; l->d) */
    scui_coord_t       width;    /* 边界(实心:<=0;空心:>0) */
    scui_coord_t       radius;   /* 圆角半径(最大:<0) */
    scui_coord_t       time;     /* 动画时间(ms) */
    scui_coord_t       lim;      /* 缩小限制(pct) */
} scui_menial_btn_res_t;

/*@brief 仆从控件:样式资源(子类型)
 */
typedef struct {
    scui_object_type_t part;      /* 部件(bg/fg) */
    scui_point_t       center;    /* 弧心 */
    scui_coord_t       width;     /* 弧宽(扇形:<= 0;弧型:>0) */
    scui_coord_t       radius;    /* 半径(>0) */
    scui_coord3_t      angle_s;   /* 起始角度(默认:0) */
    scui_coord3_t      angle_e;   /* 结束角度(默认:360) */
    scui_color_t       color[2];  /* 颜色(背景[0]前景[1]) */
    scui_coord_t       time;      /* 动画时间(ms) */
    scui_sbitfd_t      round:1;   /* 端点圆角(可选) */
    scui_sbitfd_t      gradw:1;   /* 渐变方向(水平:0;垂直:1) */
    scui_sbitfd_t      grad:1;    /* 渐变(可选)(s->e) */
} scui_menial_arc_res_t;

/*@brief 仆从控件:样式资源(子类型)
 */
typedef struct {
    scui_object_type_t part;      /* 部件(bg/fg) */
    scui_color_t       color[2];  /* 颜色(背景[0]前景[1]) */
    scui_coord_t       radius;    /* 圆角半径(最大:<0) */
    scui_coord_t       time;      /* 动画时间(ms) */
    scui_sbitfd_t      grad:1;    /* 渐变(可选)(s->e) */
} scui_menial_bar_res_t;

/*@brief 仆从控件:样式资源(子类型)
 */
typedef struct {
    scui_object_type_t part;      /* 部件(rect_item/line_item) */
    scui_sbitfd_t      round:1;   /* 端点圆角 */
    scui_color_t       color;     /* 颜色 */
    scui_coord_t       width;     /* 线宽 */
} scui_menial_cht_res_t;

/*@brief 控件样式应用(子类型)
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_menial_btn_style(scui_handle_t handle, scui_menial_btn_res_t *res);

/*@brief 控件样式应用(子类型)
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_menial_arc_style(scui_handle_t handle, scui_menial_arc_res_t *res);

/*@brief 控件样式应用(子类型)
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_menial_bar_style(scui_handle_t handle, scui_menial_bar_res_t *res);

/*@brief 控件样式应用(子类型)
 *@param handle 控件句柄
 *@param res    样式资源
 */
void scui_menial_cht_style(scui_handle_t handle, scui_menial_cht_res_t *res);

/******************************************************************************/

/*@brief 仆从控件子类型
 *@param handle 控件句柄
 *@param type   子类型
 */
void scui_menial_type_sub(scui_handle_t handle, scui_menial_type_t *type);

/*@brief 仆从控件数据实例
 *@param handle 控件句柄
 *@param data   数据实例
 */
void scui_menial_data_inst(scui_handle_t handle, scui_menial_data_t **data);

/******************************************************************************/

/*@brief 控件当前值(子类型)
 *@param handle 控件句柄
 *@param angle  目标角度
 */
void scui_menial_arc_current_angle(scui_handle_t handle, scui_coord3_t *angle);

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param angle  目标角度
 *@param anim   动画更新
 */
void scui_menial_arc_update_angle(scui_handle_t handle, scui_coord3_t angle, bool anim);

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度[0.0f, 100.0f]
 *@param anim   动画更新
 */
void scui_menial_arc_update_value(scui_handle_t handle, scui_coord3_t value, bool anim);

/*@brief 控件当前值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度
 */
void scui_menial_bar_current_value(scui_handle_t handle, scui_coord3_t *value);

/*@brief 控件更新值(子类型)
 *@param handle 控件句柄
 *@param value  目标进度[0.0f, value_lim]
 *@param anim   动画更新
 */
void scui_menial_bar_update_value(scui_handle_t handle, scui_coord3_t value, bool anim);

/*@brief 控件数据列表更新(子类型)
 *@param handle    控件句柄
 *@param vlist_min 数据列表
 *@param vlist_max 数据列表
 */
void scui_menial_cht_hist_data(scui_handle_t handle, scui_coord_t *vlist_min, scui_coord_t *vlist_max);

/*@brief 控件数据列表更新(子类型)
 *@param handle    控件句柄
 *@param vlist_dot 数据列表
 */
void scui_menial_cht_line_data(scui_handle_t handle, scui_coord_t *vlist_dot);

#endif
