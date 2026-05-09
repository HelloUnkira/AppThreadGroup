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
 *@param cfg_type 窗口切换风格
 */
void scui_window_switch_get_cfg_type(scui_window_switch_type_t *cfg_type);

/*@brief 设置窗口切换风格
 *@param cfg_type 窗口切换风格
 */
void scui_window_switch_set_cfg_type(scui_window_switch_type_t cfg_type);

/*@brief 获取窗口切换方向
 *@param cfg_dir 窗口切换方向
 */
void scui_window_switch_get_cfg_dir(scui_opt_dir_t *cfg_dir);

/*@brief 设置窗口切换方向
 *@param cfg_dir 窗口切换方向
 */
void scui_window_switch_set_cfg_dir(scui_opt_dir_t cfg_dir);

/*@brief 获取窗口切换参数
 *@param cfg_args 窗口切换参数
 */
void scui_window_switch_get_cfg_args(scui_window_switch_args_t *cfg_args);

/*@brief 设置窗口切换参数
 *@param cfg_args 窗口切换参数
 */
void scui_window_switch_set_cfg_args(scui_window_switch_args_t *cfg_args);

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

/*@brief 窗口栈入栈出栈语义
 *@param handle       窗口句柄
 *@param switch_type  切换类型
 *@param switch_dir   切换方向
 *@param add          常规入栈
 *@param del          常规出栈
 *@param prio         优先级入栈
 *@param clean        清空入栈
 *@param cover        出栈入栈
 *@param goback       回退出栈
 *@param prio_compare 优先级比较(返回:arg1 > arg2)
 *@param reserve      保留栈顶
 *@retval 成功失败
 */
bool scui_window_stack_jump(scui_handle_t handle,
    scui_window_switch_type_t switch_type, scui_opt_dir_t switch_dir,
    bool add, bool del, bool prio, bool clean, bool cover, bool goback,
    bool (*prio_compare)(scui_handle_t, scui_handle_t), bool reserve);

/*************************************************************************************************/
/* stack的一些扩充语义 */



#define scui_window_stack_add_by(handle, switch_type, switch_dir, reserve)                          \
        scui_window_stack_jump(handle, switch_type, switch_dir,                                     \
         true, false, false, false, false, false, NULL, reserve)                                    \

#define scui_window_stack_del_by(handle, switch_type, switch_dir)                                   \
        scui_window_stack_jump(handle, switch_type, switch_dir,                                     \
        false,  true, false, false, false, false, NULL, false)                                      \

#define scui_window_stack_prio_by(handle, switch_type, switch_dir, compare, reserve)                \
        scui_window_stack_jump(handle, switch_type, switch_dir,                                     \
        false, false,  true, false, false, false, compare, reserve)                                 \

#define scui_window_stack_reset_by(handle, switch_type, switch_dir, reserve)                        \
        scui_window_stack_jump(handle, switch_type, switch_dir,                                     \
        false, false, false,  true, false, false, NULL, reserve)                                    \

#define scui_window_stack_cover_by(handle, switch_type, switch_dir)                                 \
        scui_window_stack_jump(handle, switch_type, switch_dir,                                     \
        false, false, false, false,  true, false, NULL, false)                                      \

#define scui_window_stack_goback_by(handle, switch_type, switch_dir)                                \
        scui_window_stack_jump(handle, switch_type, switch_dir,                                     \
        false, false, false, false, false,  true, NULL, false)                                      \



#define scui_window_stack_add(handle, reserve)                                                      \
scui_window_stack_add_by(handle, scui_window_switch_auto, scui_opt_dir_none, reserve)               \

#define scui_window_stack_del(handle)                                                               \
scui_window_stack_del_by(handle, scui_window_switch_auto, scui_opt_dir_none)                        \

#define scui_window_stack_prio(handle, reserve, compare)                                            \
scui_window_stack_prio_by(handle, scui_window_switch_auto, scui_opt_dir_none, compare, reserve)     \

#define scui_window_stack_reset(handle, reserve)                                                    \
scui_window_stack_reset_by(handle, scui_window_switch_auto, scui_opt_dir_none, reserve)             \

#define scui_window_stack_cover(handle)                                                             \
scui_window_stack_cover_by(handle, scui_window_switch_auto, scui_opt_dir_none)                      \

#define scui_window_stack_goback(handle)                                                            \
scui_window_stack_goback_by(handle, scui_window_switch_auto, scui_opt_dir_none)                     \

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/*@brief 窗口跳转
 *@param handle 窗口句柄
 *@param type   窗口切换风格
 *@param dir    窗口切换方向
 *@retval 成功失败
 */
bool scui_window_switch_jump(scui_handle_t handle, scui_window_switch_type_t type, scui_opt_dir_t dir);

/* 窗口跳转:自动模式 */
static inline bool scui_window_switch_jump_auto(scui_handle_t handle)
{return scui_window_switch_jump(handle, scui_window_switch_auto, scui_opt_dir_none);}

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

#endif
