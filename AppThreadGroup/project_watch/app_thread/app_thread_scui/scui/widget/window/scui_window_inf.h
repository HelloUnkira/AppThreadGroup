#ifndef SCUI_WINDOW_INF_H
#define SCUI_WINDOW_INF_H

/*@brief 窗口配置参数获取
 *@param handle  窗口控件句柄
 *@param sibling 配置参数
 */
void scui_window_sibling_get(scui_handle_t handle, scui_handle_t sibling[4]);

/*@brief 窗口配置参数设置
 *@param handle  窗口控件句柄
 *@param sibling 配置参数
 */
void scui_window_sibling_set(scui_handle_t handle, scui_handle_t sibling[4]);

/*@brief 窗口配置参数获取
 *@param handle      窗口控件句柄
 *@param switch_type 配置参数
 */
void scui_window_switch_type_get(scui_handle_t handle, scui_window_switch_type_t switch_type[4]);

/*@brief 窗口配置参数设置
 *@param handle      窗口控件句柄
 *@param switch_type 配置参数
 */
void scui_window_switch_type_set(scui_handle_t handle, scui_window_switch_type_t switch_type[4]);

/*@brief 窗口配置参数获取
 *@param handle     窗口控件句柄
 *@param switch_enc 配置参数
 */
void scui_window_switch_enc_get(scui_handle_t handle, scui_opt_pos_t *switch_enc);

/*@brief 窗口配置参数设置
 *@param handle     窗口控件句柄
 *@param switch_enc 配置参数
 */
void scui_window_switch_enc_set(scui_handle_t handle, scui_opt_pos_t switch_enc);

/*@brief 窗口配置参数获取
 *@param handle     窗口控件句柄
 *@param switch_key 配置参数
 */
void scui_window_switch_key_get(scui_handle_t handle, scui_opt_pos_t *switch_key);

/*@brief 窗口配置参数设置
 *@param handle     窗口控件句柄
 *@param switch_key 配置参数
 */
void scui_window_switch_key_set(scui_handle_t handle, scui_opt_pos_t switch_key);

/*@brief 窗口配置参数获取
 *@param handle         窗口控件句柄
 *@param switch_enc_way 配置参数
 */
void scui_window_switch_enc_way_get(scui_handle_t handle, scui_opt_dir_t *switch_enc_way);

/*@brief 窗口配置参数设置
 *@param handle         窗口控件句柄
 *@param switch_enc_way 配置参数
 */
void scui_window_switch_enc_way_set(scui_handle_t handle, scui_opt_dir_t switch_enc_way);

/*@brief 窗口配置参数获取
 *@param handle        窗口控件句柄
 *@param switch_key_id 配置参数
 */
void scui_window_switch_key_id_get(scui_handle_t handle, scui_coord_t switch_key_id[4]);

/*@brief 窗口配置参数设置
 *@param handle        窗口控件句柄
 *@param switch_key_id 配置参数
 */
void scui_window_switch_key_id_set(scui_handle_t handle, scui_coord_t switch_key_id[4]);

/*@brief 窗口绘制锁获取
 *@param handle    窗口控件句柄
 *@param draw_lock 窗口绘制锁
 */
void scui_window_draw_lock_get(scui_handle_t handle, bool *draw_lock);

/*@brief 窗口绘制锁设置
 *@param handle    窗口控件句柄
 *@param draw_lock 窗口绘制锁
 */
void scui_window_draw_lock_set(scui_handle_t handle, bool draw_lock);

/*@brief 窗口资源获取
 *@param handle    窗口控件句柄
 *@param local_res 窗口资源
 */
void scui_window_local_res_get(scui_handle_t handle, void **local_res);

/*@brief 窗口资源设置
 *@param handle 窗口控件句柄
 *@param size   窗口资源大小
 */
void scui_window_local_res_set(scui_handle_t handle, scui_handle_t size);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 获取窗口切换风格
 *@param move_type 窗口切换风格
 */
void scui_window_switch_get_move_type(scui_window_switch_type_t *move_type);

/*@brief 设置窗口切换风格
 *@param move_type 窗口切换风格
 */
void scui_window_switch_set_move_type(scui_window_switch_type_t move_type);

/*@brief 获取窗口切换风格
 *@param jump_type 窗口切换风格
 */
void scui_window_switch_get_jump_type(scui_window_switch_type_t *jump_type);

/*@brief 设置窗口切换风格
 *@param jump_type 窗口切换风格
 */
void scui_window_switch_set_jump_type(scui_window_switch_type_t jump_type);

/*@brief 获取窗口切换方向
 *@param jump_dir 窗口切换方向
 */
void scui_window_switch_get_jump_dir(scui_opt_dir_t *jump_dir);

/*@brief 设置窗口切换方向
 *@param jump_dir 窗口切换方向
 */
void scui_window_switch_set_jump_dir(scui_opt_dir_t jump_dir);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 窗口激活
 *@param handle 窗口句柄
 */
void scui_window_active(scui_handle_t handle);

/*@brief 获得活跃窗口句柄
 *@param index 0:当前活跃;1:上一活跃;2~n以此类推
 *@retval 窗口句柄
 */
scui_handle_t scui_window_active_last(scui_handle_t index);
#define scui_window_active_curr() (scui_window_active_last(0))

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 窗口栈深度
 *@param top 栈深
 */
void scui_window_stack_nest(scui_handle_t *top);

/*@brief 获得次级显示窗口
 *@param handle 窗口句柄
 */
void scui_window_stack_last(scui_handle_t *handle);

/*@brief 获得顶级显示窗口
 *@param handle 窗口句柄
 */
void scui_window_stack_top(scui_handle_t *handle);

/*@brief 获得窗口记录
 *@param handle 窗口句柄
 *@param index  索引(0:当前;1:最近;以此类推)
 */
void scui_window_stack_rcd(scui_handle_t *handle, scui_handle_t index);

/*@brief 窗口栈自定义更变
 *@param switch_type  切换类型
 *@param switch_dir   切换方向
 *@param adjust       更变回调(stack 窗口栈信息;top 栈顶信息)
 *@retval 成功失败
 */
bool scui_window_stack_adjust(
    scui_window_switch_type_t switch_type, scui_opt_dir_t switch_dir,
    void (*adjust)(scui_handle_t *stack, scui_handle_t *top));

/*@brief 窗口栈入栈出栈语义
 *@param handle       窗口句柄
 *@param semantic     语义类型[1,6][add, del, prio, clean, cover, goback]
 *@param switch_type  切换类型
 *@param switch_dir   切换方向
 *@param reserve      保留栈顶
 *@param prio_compare 优先级比较(返回:arg1 > arg2)
 *@retval 成功失败
 */
bool scui_window_stack_jump(scui_handle_t handle, scui_coord_t semantic,
    scui_window_switch_type_t switch_type, scui_opt_dir_t switch_dir,
    bool reserve, bool (*prio_compare)(scui_handle_t, scui_handle_t));

#define scui_window_stack_jump_auto(handle, semantic, reserve, prio_compare) \
        scui_window_stack_jump(handle, semantic, scui_window_switch_auto, scui_opt_dir_none, reserve, prio_compare)

/*************************************************************************************************/
/* stack的一些扩充语义 */

#define scui_window_stack_add_by(handle, switch_type, switch_dir, reserve)                          \
        scui_window_stack_jump(handle, 0x01, switch_type, switch_dir, reserve, NULL)                \

#define scui_window_stack_del_by(handle, switch_type, switch_dir)                                   \
        scui_window_stack_jump(handle, 0x02, switch_type, switch_dir, false, NULL)                  \

#define scui_window_stack_prio_by(handle, switch_type, switch_dir, reserve, compare)                \
        scui_window_stack_jump(handle, 0x03, switch_type, switch_dir, reserve, compare)             \

#define scui_window_stack_reset_by(handle, switch_type, switch_dir, reserve)                        \
        scui_window_stack_jump(handle, 0x04, switch_type, switch_dir, reserve, NULL)                \

#define scui_window_stack_cover_by(handle, switch_type, switch_dir)                                 \
        scui_window_stack_jump(handle, 0x05, switch_type, switch_dir, false, NULL)                  \

#define scui_window_stack_goback_by(handle, switch_type, switch_dir)                                \
        scui_window_stack_jump(handle, 0x06, switch_type, switch_dir, false, NULL)                  \

/*************************************************************************************************/
/* stack的一些扩充语义 */

#define scui_window_stack_add(handle, reserve)                                                      \
        scui_window_stack_jump_auto(handle, 0x01, reserve, NULL)                                    \

#define scui_window_stack_del(handle)                                                               \
        scui_window_stack_jump_auto(handle, 0x02, false, NULL)                                      \

#define scui_window_stack_prio(handle, reserve, compare)                                            \
        scui_window_stack_jump_auto(handle, 0x03, reserve, compare)                                 \

#define scui_window_stack_reset(handle, reserve)                                                    \
        scui_window_stack_jump_auto(handle, 0x04, reserve, NULL)                                    \

#define scui_window_stack_cover(handle)                                                             \
        scui_window_stack_jump_auto(handle, 0x05, false, NULL)                                      \

#define scui_window_stack_goback(handle)                                                            \
        scui_window_stack_jump_auto(handle, 0x06, false, NULL)                                      \

/*************************************************************************************************/

/*@brief 窗口跳转
 *@param handle 窗口句柄
 *@param type   窗口切换风格
 *@param dir    窗口切换方向
 *@retval 成功失败
 */
bool scui_window_switch_jump(scui_handle_t handle, scui_window_switch_type_t type, scui_opt_dir_t dir);

/* 窗口跳转:自适应 */
static inline bool scui_window_switch_jump_auto(scui_handle_t handle)
{return scui_window_switch_jump(handle, scui_window_switch_auto, scui_opt_dir_none);}

/*************************************************************************************************/

/*@brief 窗口切换参数配置
 */
void scui_window_transform_flap1_shadow(scui_handle_t shadow);

/*@brief 窗口切换参数配置
 */
void scui_window_transform_flap2_shadow(scui_handle_t shadow);

/*@brief 窗口切换参数配置
 */
void scui_window_transform_cube_shadow(scui_handle_t shadow);

/*************************************************************************************************/

#endif
