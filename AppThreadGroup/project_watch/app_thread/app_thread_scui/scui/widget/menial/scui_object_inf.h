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
/* 自定义的经典效果: */

typedef struct {
    scui_object_type_t part;
    scui_object_type_t state;
    
    scui_area_t   area;         /* 部件宽高 */
    scui_coord_t  index;        /* 部件索引[0, 3] */
    scui_alpha_t  alpha[4];     /* 部件透明度(背景,边界,外部线,阴影) */
    scui_color_t  color[4];     /* 部件源色调(s->e) */
    scui_coord_t  width[4];     /* 部件宽度(边框) */
    scui_coord_t  radius;       /* 部件圆角(背景) */
    scui_sbitfd_t grad_w:1;     /* 渐变方向 */
    scui_sbitfd_t grad:1;       /* 渐变标记 */
    
} scui_object_rect_t;

/*@brief 对象控件绘制圆角矩形
 *@param handle 对象控件句柄
 *@param prop   属性(part;state)
 *@retval 成功失败
 */
bool scui_object_draw_rect(scui_handle_t handle, scui_object_prop_t *prop);

/*@brief 对象控件添加经典圆角矩形属性
 *@param handle 对象控件句柄
 *@param rect   圆角矩形属性
 */
void scui_object_prop_rect(scui_handle_t handle, scui_object_rect_t *rect);

/*@brief 对象控件绘制圆角矩形
 *@param handle 对象控件句柄
 *@param prop   属性(state)
 *@retval 成功失败
 */
bool scui_object_draw_rect4(scui_handle_t handle, scui_object_prop_t *prop);

/*@brief 对象控件添加经典圆角矩形属性
 *@param handle 对象控件句柄
 *@param rect   圆角矩形属性
 */
void scui_object_prop_rect4(scui_handle_t handle, scui_object_rect_t *rect);

#endif
