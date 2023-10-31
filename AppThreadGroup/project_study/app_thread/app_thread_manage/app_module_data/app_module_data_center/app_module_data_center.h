#ifndef APP_MODULE_DATA_CENTER_H
#define APP_MODULE_DATA_CENTER_H

/*备注:
 *    将各种各样的零碎数据统一打包到此结构中
 *    通过不同的结构块区分不同类型的数据区域
 *    并以一个完全统一的接口对需要持久化的资源统一访问
 *    因为数据中心使用了缓存机制,为了提升缓存性能
 *    尽可能的将大数据块切碎成各种各样类型的小集合
 *    那么缓存的热缓冲机制能充分被发挥出效果
 */
typedef enum {
    app_module_data_center_none = 0,
    app_module_data_center_module_source,           /* 模组资源 */
    app_module_data_center_num,
} app_module_data_center_type_t;

typedef struct {
    uint32_t crc32; /* 公共字段,以做校验 */
    union {
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源占位标识(内部偏移计算使用) */
        uintptr_t offset;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源(module source) */
        /* 模组专用字段,模组内部使用:静止访问该字段 */
        struct {
            /* 模组专用字段,模组内部使用:静止访问该字段 */
            app_module_clock_t                  clock;
            /* keep adding... */
        } module_source;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源:keep adding */
        /* --------------------------------------------------------------------- */
    };
} app_module_data_center_t;

/*@brief    更换目标数据类型并锁定数据中心
 *param[in] type 数据中心类型(app_module_data_center_t(type))
 *@retval   数据中心的缓存地址
 */
app_module_data_center_t * app_module_data_center_take(uint32_t type);

/*@brief 解锁数据中心
 */
void app_module_data_center_give(void);

/*@brief 刷新数据中心
 */
void app_module_data_center_reflush(void);

/*@brief     回收数据中心资源
 *@param[in] force 强制回收数据中心资源(不建议使用)
 *           不建议使用,这会导致非正常的逻辑
 */
void app_module_data_center_recycle(bool force);

/*@brief 数据中心模组初始化
 *       内部使用: 被manage线程使用
 */
void app_module_data_center_ready(void);

#endif
