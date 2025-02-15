#ifndef SCUI_HANDLE_H
#define SCUI_HANDLE_H

/*@brief 句柄
 */
typedef uint16_t scui_handle_t;

/*@brief 句柄表
 *       句柄资源重映射表:可选项
 *       它是一个资源数组,用于承装资源的动态重映射
 *       如果重映射表存在,句柄优先映射到重映射表之上
 *       如果重映射表资源为空,再映射到原资源表上
 */
typedef struct {
    void        **source;           /* 句柄资源表 */
    void        **source_remap;     /* 句柄资源重映射表 */
    scui_handle_t number;           /* 句柄资源表数量 */
    scui_handle_t offset;           /* 句柄资源表偏移 */
} scui_handle_table_t;

/*@brief 非法句柄,系统句柄
 *       0号句柄为非法句柄
 */
#define SCUI_HANDLE_INVALID     ((scui_handle_t)(0))
#define SCUI_HANDLE_SYSTEM      ((scui_handle_t)(~(scui_handle_t)0))

/*@brief 句柄表获取
 *@param handle 句柄
 *@retval 句柄表
 */
scui_handle_table_t * scui_handle_table_find(scui_handle_t handle);

/*@brief 句柄表注册
 *@param table 句柄表
 */
void scui_handle_table_register(scui_handle_table_t *table);

/*@brief 通过句柄判断属于哪一个偏移
 *       通过这种方式可以确认句柄所在类型
 *@param handle 句柄
 *@retval 句柄偏移量
 */
scui_handle_t scui_handle_check(scui_handle_t handle);

/*@brief 寻找一个动态句柄
 *@retval 句柄
 */
scui_handle_t scui_handle_find(void);

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

/*@brief 句柄是否重映射
 *@param handle 句柄
 *@retval 是否重映射
 */
bool scui_handle_remap(scui_handle_t handle);

/*@brief 句柄是否重映射
 *@param handle 句柄
 *@retval 是否重映射
 */
bool scui_handle_unmap(scui_handle_t handle);

/* 宏函数(内联版本):简化句柄到资源的访问(快速访问) */
static inline void * scui_handle_source_assert(scui_handle_t handle)    \
{void *source = scui_handle_get(handle);SCUI_ASSERT(source != NULL);return source;}

/* 简单的对句柄进行一次宏转义 */
/* 这里的get和set语义定义的不是特别好 */
#define scui_handle_linker                  scui_handle_set
#define scui_handle_source(handle)          scui_handle_get(handle)
#define scui_handle_source_check(handle)    scui_handle_source_assert(handle)
#define scui_handle_clear(handle)           scui_handle_set(handle, NULL)

#endif
