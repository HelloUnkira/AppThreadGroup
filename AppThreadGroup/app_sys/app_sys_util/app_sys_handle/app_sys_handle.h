#ifndef APP_SYS_HANDLE_H
#define APP_SYS_HANDLE_H

/* 系统句柄资源管理器
 * 采用二级数组实现
 * 在保证查找效率为O(1)的条件下
 * 降低额外空间开销
 * 将时间成本转移到获取与释放部分
 * 因为句柄通常不需要频繁获取与释放
 */

/* 资源管理单元 */
typedef struct {
    uint32_t num;    //可使用句柄数量
    uint32_t len;    //句柄资源集合数量
    void   **src;    //句柄资源集合
} app_sys_handle_unit_t;

/* 资源管理 */
typedef struct {
    app_sys_handle_unit_t *unit;    //资源管理单元集合
    uint32_t num;                   //可使用资源管理单元数量
    uint32_t len;                   //句柄资源集合数量
} app_sys_handle_t;

#define APP_SYS_HANDLE_INVALID      (~0)    /* 非法句柄 */
#define APP_SYS_HANDLE_UINT_FACTOR  (1)     /* 伸缩因子:暴力测试:1,推荐使用:5 */
#define APP_SYS_HANDLE_UINT_LENGTH  (1)     /* 单元大小:暴力测试:1,推荐使用:50,100 */

/*@brief     释放一个句柄
 *@param[in] handle 句柄
 */
void app_sys_handle_give(uint32_t handle);

/*@brief      获取一个句柄
 *@param[out] handle 句柄
 */
void app_sys_handle_take(uint32_t *handle);

/*@brief     句柄绑定或更新资源
 *@param[in] handle 句柄
 *@param[in] src 资源
 */
void app_sys_handle_src_set(uint32_t handle, void *src);

/*@brief      句柄获取资源
 *@param[in]  handle 句柄
 *@param[out] src 资源
 */
void app_sys_handle_src_get(uint32_t handle, void **src);

#endif
