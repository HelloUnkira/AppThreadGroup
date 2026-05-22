#ifndef SCUI_OBJECT_INF_H
#define SCUI_OBJECT_INF_H

/*@brief 对象控件移除属性
 *@param handle 控件句柄
 *@param prop   控件属性
 */
void scui_object_prop_del(scui_handle_t handle, scui_object_prop_t *prop);

/*@brief 对象控件添加属性
 *@param handle 控件句柄
 *@param prop   控件属性
 */
void scui_object_prop_add(scui_handle_t handle, scui_object_prop_t *prop);

/*@brief 对象控件同步属性
 *@param handle 控件句柄
 *@param prop   控件属性
 *@retval 成功失败
 */
bool scui_object_prop_sync(scui_handle_t handle, scui_object_prop_t *prop);

/*@brief 对象控件移除过渡
 *@param handle 控件句柄
 *@param tran   控件过渡
 */
void scui_object_tran_del(scui_handle_t handle, scui_object_tran_t *tran);

/*@brief 对象控件添加过渡
 *@param handle 控件句柄
 *@param tran   控件过渡
 */
void scui_object_tran_add(scui_handle_t handle, scui_object_tran_t *tran);

/*@brief 对象控件添加过渡(取属性)
 *@param handle 控件句柄
 *@param tran   控件过渡
 *@retval 缺属性
 */
bool scui_object_tran_add_by(scui_handle_t handle, scui_object_tran_t *tran);

/*@brief 对象控件过渡执行
 *@param handle 控件句柄
 *@param tran   控件过渡
 *@retval 未找到控件过渡
 */
bool scui_object_tran_work(scui_handle_t handle, scui_object_tran_t *tran);

/*@brief 对象控件状态获取
 *@param handle 控件句柄
 *@param press  控件状态
 */
void scui_object_press_get(scui_handle_t handle, bool *press);

/*@brief 对象控件状态设置
 *@param handle 控件句柄
 *@param press  控件状态
 */
void scui_object_press_set(scui_handle_t handle, bool press);

/*@brief 对象控件状态获取
 *@param handle 控件句柄
 *@param check  控件状态
 */
void scui_object_check_get(scui_handle_t handle, bool *check);

/*@brief 对象控件状态设置
 *@param handle 控件句柄
 *@param check  控件状态
 */
void scui_object_check_set(scui_handle_t handle, bool check);

/*@brief 对象控件状态获取
 *@param handle  控件句柄
 *@param state_l 控件状态
 */
void scui_object_state_l_get(scui_handle_t handle, scui_object_type_t *state_l);

/*@brief 对象控件状态获取
 *@param handle 控件句柄
 *@param state  控件状态
 */
void scui_object_state_get(scui_handle_t handle, scui_object_type_t *state);

/*@brief 对象控件状态设置
 *@param handle 控件句柄
 *@param state  控件状态
 */
void scui_object_state_set(scui_handle_t handle, scui_object_type_t state);

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
/* 简要宏化简数据类型转换: */

#define scui_object_data_area(val)          ((scui_object_data_t){.area     = val,})
#define scui_object_data_point(val)         ((scui_object_data_t){.point    = val,})
#define scui_object_data_alpha(val)         ((scui_object_data_t){.alpha    = val,})
#define scui_object_data_number(val)        ((scui_object_data_t){.number   = val,})
#define scui_object_data_color32(val)       ((scui_object_data_t){.color32  = val,})
#define scui_object_data_align(val)         ((scui_object_data_t){.align    = val,})
#define scui_object_data_pointer(val)       ((scui_object_data_t){.pointer  = val,})
#define scui_object_data_multi(val)         ((scui_object_data_t){.multi    = val,})

/*****************************************************************************/
/* 简要宏化简属性过渡的添加: */

/* scui_object_prop_add宏化简 */
#define scui_object_prop_new(handle_v, part_v, style_v, state_v, data_v)        \
do {                                                                            \
    scui_object_prop_t scui_op_i = {                                            \
        .part  = scui_object_part_##part_v,                                     \
        .style = scui_object_style_##style_v,                                   \
        .state = scui_object_state_##state_v,                                   \
        .data  = data_v,};                                                      \
    scui_object_prop_add(handle_v, &scui_op_i);                                 \
} while (0)                                                                     \

/* scui_object_tran_add宏化简 */
#define scui_object_tran_new(handle_v, part_v, style_v, state_p_v, state_n_v,   \
    data_p_v, data_n_v, path_v, time_v, delay_v)                                \
do {                                                                            \
    scui_object_tran_t scui_ot_i = {                                            \
        .part    = scui_object_part_##part_v,                                   \
        .style   = scui_object_style_##style_v,                                 \
        .state_p = scui_object_state_##state_p_v,                               \
        .state_n = scui_object_state_##state_n_v,                               \
        .data_p  = data_p_v,                                                    \
        .data_n  = data_n_v,                                                    \
        .path    = path_v,                                                      \
        .time    = time_v,                                                      \
        .delay   = delay_v,                                                     \
    };                                                                          \
    scui_object_tran_add(handle_v, &scui_ot_i);                                 \
} while (0)                                                                     \

/* scui_object_tran_add宏二次化简 */
#define scui_object_tran_new2(handle_v, part_v, style_v, state_p_v, state_n_v,  \
    data_p_v, data_n_v, path_v, time_v, delay_v)                                \
do {                                                                            \
    scui_object_tran_new(handle_v, part_v, style_v, state_p_v, state_n_v,       \
    data_p_v, data_n_v, path_v, time_v, delay_v);                               \
    scui_object_tran_new(handle_v, part_v, style_v, state_n_v, state_p_v,       \
    data_n_v, data_p_v, path_v, time_v, delay_v);                               \
} while (0)                                                                     \

/*****************************************************************************/
/* 标准部件样式属性配置及绘制: */

typedef struct {
    scui_object_type_t part;
    scui_object_type_t state;
    /* 本结构打包初始化无效??? */
    
    union {
    scui_object_data_t list[0];
    struct {
    scui_object_data_t alpha;           /* 部件透明度 */
    scui_object_data_t color;           /* 部件色调 */
    scui_object_data_t point;           /* 部件偏移点 */
    scui_object_data_t align;           /* 部件对齐 */
    scui_object_data_t width;           /* 部件宽 */
    scui_object_data_t height;          /* 部件高 */
    scui_object_data_t radius;          /* 部件圆角(背景) */
    scui_object_data_t side_width;      /* 部件宽度(边框) */
    scui_object_data_t color_grad;      /* 部件色调(渐变) */
    scui_object_data_t multi;           /* 部件渐变标记/方向 */
    } rect;
    struct {
    scui_object_data_t alpha;           /* 部件透明度 */
    scui_object_data_t color;           /* 部件色调 */
    scui_object_data_t angle_s;         /* 角度 */
    scui_object_data_t angle_e;         /* 角度 */
    scui_object_data_t center;          /* 中心 */
    scui_object_data_t radius;          /* 半径 */
    scui_object_data_t side_width;      /* 弧宽(扇形:<= 0;弧型:>0) */
    scui_object_data_t color_grad;      /* 部件色调(渐变) */
    scui_object_data_t multi;           /* 端点,渐变标记/方向 */
    } arc;
    struct {
    scui_object_data_t alpha;           /* 部件透明度 */
    scui_object_data_t color;           /* 部件色调 */
    scui_object_data_t area;            /* 区域 */
    scui_object_data_t vpos;            /* 端点序列 */
    scui_object_data_t vpos_num;        /* 端点序列数 */
    scui_object_data_t side_width;      /* 线宽 */
    scui_object_data_t multi;           /* 端点 */
    } line;
    };
} scui_object_sub_t;

/*@brief 对象控件添加经典矩形属性
 *@param handle 对象控件句柄
 *@param sub    矩形属性
 */
void scui_object_prop_rect(scui_handle_t handle, scui_object_sub_t *sub);

/*@brief 对象控件绘制矩形
 *@param handle 对象控件句柄
 *@param prop   属性(part;state)
 *@retval 成功失败
 */
bool scui_object_draw_rect(scui_handle_t handle, scui_object_prop_t *prop);

/*@brief 对象控件添加经典圆弧属性
 *@param handle 对象控件句柄
 *@param sub    圆弧属性
 */
void scui_object_prop_arc(scui_handle_t handle, scui_object_sub_t *sub);

/*@brief 对象控件绘制圆弧
 *@param handle 对象控件句柄
 *@param prop   属性(part;state)
 *@retval 成功失败
 */
bool scui_object_draw_arc(scui_handle_t handle, scui_object_prop_t *prop);

/*@brief 对象控件添加经典线条属性
 *@param handle 对象控件句柄
 *@param sub    线条属性
 */
void scui_object_prop_line(scui_handle_t handle, scui_object_sub_t *sub);

/*@brief 对象控件绘制线条
 *@param handle 对象控件句柄
 *@param prop   属性(part;state)
 *@retval 成功失败
 */
bool scui_object_draw_line(scui_handle_t handle, scui_object_prop_t *prop);

#endif
