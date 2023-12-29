#ifndef SCUI_HANDLE_H
#define SCUI_HANDLE_H

/*@brief 句柄
 */
typedef uint32_t scui_handle_t;

/*@brief 句柄表
 */
typedef struct {
    void        **source;
    scui_handle_t number;
    scui_handle_t offset;
} scui_handle_table_t;

/*@brief 句柄偏移量,非法句柄,系统句柄
 *       0号句柄为非法句柄
 */
#define SCUI_HANDLE_INVALID     (0)
#define SCUI_HANDLE_SYSTEM      (~(scui_handle_t)0)

/*@brief 句柄表注册
 *@param table 句柄表
 */
void scui_handle_table_register(scui_handle_table_t *table);

/*@brief 通过句柄判断属于哪一个偏移
 *       通过这种方式可以确认句柄所在类型
 *@param handle 句柄
 */
scui_handle_t scui_handle_check(scui_handle_t handle);

/*@brief 生成一个句柄
 *@retval 句柄
 */
scui_handle_t scui_handle_new(void);

/*@brief 销毁一个句柄
 *@param handle 句柄
 *@retval 成功失败
 */
bool scui_handle_del(scui_handle_t handle);

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
