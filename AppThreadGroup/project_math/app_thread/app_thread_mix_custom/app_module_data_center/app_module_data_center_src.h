#ifndef APP_MODULE_DATA_CENTER_SRC_H
#define APP_MODULE_DATA_CENTER_SRC_H

/*备注:
 *    将各种各样的零碎数据统一打包到此结构中
 *    通过不同的结构块区分不同类型的数据区域
 *    并以一个完全统一的接口对需要持久化的资源统一访问
 *    因为数据中心使用了缓存机制,为了提升缓存性能
 *    尽可能的将大数据块切碎成各种各样类型的小集合
 *    那么缓存的热缓冲机制能充分被发挥出效果
 */
typedef enum {
    app_module_data_center_src_none = 0,
    app_module_data_center_src_module_source,           /* 模组资源 */
    app_module_data_center_src_system_profile,          /* 系统配置 */
    app_module_data_center_src_num,
} app_module_data_center_src_type_t;

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
        /* 数据中心管理资源(system profile) */
        struct {
            uint64_t ver_major:8;       // 主版本号
            uint64_t ver_minor:8;       // 次版本号
            uint64_t ver_patch:8;       // 补版本号
            uint8_t  system_mode;       // 设备工作模式
        } system_profile;
        /* --------------------------------------------------------------------- */
        /* 数据中心管理资源:keep adding */
        /* --------------------------------------------------------------------- */
    };
} app_module_data_center_src_t;

typedef struct {
    app_module_data_center_src_type_t type;
    const char *data_name;
    uint32_t data_size;
} app_module_data_center_src_table_t;

#endif
