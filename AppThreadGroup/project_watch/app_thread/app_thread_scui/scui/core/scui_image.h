#ifndef SCUI_IMAGE_H
#define SCUI_IMAGE_H

/* 图片像素字节占用偏移 */
#define SCUI_IMAGE_PIXEL_OFFSET     (8)

/* 图片像素格式: */
/* 枚举值为(byte << 8) | index */
typedef enum {
    scui_image_format_bmp565  = 0x200,
    scui_image_format_png8565 = 0x310,
    scui_image_format_png6666 = 0x311,
} scui_image_format_t;

/* 图片像素数据: */
typedef struct {
    uintptr_t data;         // 图片数据源(外存地址)
    uintptr_t width;        // 图片宽
    uintptr_t height;       // 图片高
    uintptr_t size_raw;     // 图片数据源(外存源大小)
    uintptr_t size_mem;     // 图片数据源(在内存中预期源大小)
} scui_image_pixel_t;

/* 图片数据来源: */
typedef enum {
    scui_image_from_mem = 0,    // 图片数据在内存中
    scui_image_from_ext,        // 图片数据在外存(静态资源)
    scui_image_from_cloud,      // 图片数据在外存(云存储资源)
    scui_image_from_num,
} scui_image_from_t;

/* 图片状态: */
typedef enum {
    scui_image_status_none = 0,     // 默认图片
    scui_image_status_lz4,          // 压缩图片
    scui_image_status_num,
} scui_image_status_t;

/* 图片数据: */
typedef struct {
    scui_image_format_t format;     // 图片格式
    scui_image_status_t status;     // 图片状态
    scui_image_pixel_t  pixel;      // 图片色彩数据
    scui_image_from_t   from;       // 图片来源
    scui_handle_t handle;           // 图片句柄
    uintptr_t data;                 // 图片数据源(内存地址)
} scui_image_t;

/* 内存图片节点信息项: */
typedef struct {
    app_sys_list_dln_t dl_node;     // 优先队列节点
    app_sys_table_rbsn_t ht_node;   // 哈希表节点
    scui_image_t image;             // 图片数据
    uint16_t count:7;
    uint16_t lock:7;
} scui_image_unit_t;

/* 内存图片节点信息表: */
typedef struct {
    app_sys_list_dll_t dl_list;
    app_sys_table_rbsl_t ht_list[SCUI_IMAGE_LIMIT_HASH];
    app_sys_table_rbst_t ht_table;
    uint32_t usage;     /* 缓存对内存资源占用情况 */
    uint32_t total;     /* 缓存对内存资源占用总门限 */
    uint32_t cnt_hit;   /* 命中次数 */
    uint32_t cnt_unhit; /* 非命中次数 */
} scui_image_cache_t;

/*@brief 图片初始化
 */
void scui_image_ready(void);

/*@brief 图片资源清除
 */
void scui_image_cache_clear(void);

/*@brief 图片资源卸载
 */
void scui_image_cache_unload(scui_image_t *image, bool unload);

/*@brief 图片资源加载
 */
void scui_image_cache_load(scui_image_t *image, bool load);

/*@brief 图片合法性检查
 *@param image 图片信息
 *@retval 合法或不合法
 */
bool scui_image_check(scui_image_t *image);

#endif
