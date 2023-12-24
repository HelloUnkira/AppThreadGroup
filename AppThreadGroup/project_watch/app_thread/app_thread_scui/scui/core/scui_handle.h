#ifndef SCUI_HANDLE_H
#define SCUI_HANDLE_H

/*@brief 句柄
 */
typedef uint32_t scui_handle_t;

/*@brief 句柄偏移量,非法句柄,系统句柄
 *       0号句柄为非法句柄
 */
#define SCUI_HANDLE_OFFSET_S    (1)
#define SCUI_HANDLE_OFFSET_D    (0xFFFF)
#define SCUI_HANDLE_INVALID     (0)
#define SCUI_HANDLE_SYSTEM      (~(scui_handle_t)0)

/*@brief 生成一个句柄
 *@param dynamic 动态句柄,静态句柄
 *@retval 句柄
 */
scui_handle_t scui_handle_new(bool dynamic);

/*@brief 销毁一个句柄
 *@param dynamic 动态句柄,静态句柄
 *@param handle 句柄
 *@retval 成功失败
 */
bool scui_handle_del(bool dynamic, scui_handle_t handle);

/*@brief 句柄获得资源
 *@param handle 句柄
 *@retval 资源
 */
void * scui_handle_get(scui_handle_t handle);

/*@brief 句柄设置资源
 *@param handle 句柄
 *@param source 资源
 *@retval 成功失败
 */
bool scui_handle_set(scui_handle_t handle, void *source);

#endif
