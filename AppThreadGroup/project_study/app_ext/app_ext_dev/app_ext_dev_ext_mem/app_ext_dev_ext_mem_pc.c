/* 实现目标:
 *     APP需要对系统驱动进行的封装体
 *     这是一个参考样板
 */

#include "app_ext_lib.h"

#if APP_ARCH_IS_PC

/* 设备ext_mem抽象操作参数 */
typedef struct {
    void *args;
} app_dev_ext_mem_cfg_t;

/*@brief ext_mem设备初始化
 *@param driver 设备实例
 *@retval 失败返回负数
 */
static int32_t app_dev_ext_mem_hal_ready(app_dev_t *driver)
{
    const app_dev_ext_mem_cfg_t *cfg = driver->cfg;
    app_dev_ext_mem_data_t *data = driver->data;
    
    memset(data, 0, sizeof(app_dev_ext_mem_data_t));
    return 0;
}

/*@brief ext_mem设备读取
 *@param driver 设备实例
 *@retval 失败返回负数
 */
static int32_t app_dev_ext_mem_hal_read(app_dev_t *driver)
{
    const app_dev_ext_mem_cfg_t *cfg = driver->cfg;
    app_dev_ext_mem_data_t *data = driver->data;
    
    if (data->rw_args.buffer == NULL || data->rw_args.size == 0) {
        APP_DEV_LOG_MSG_COM("data->rw_args.buffer:%p", data->rw_args.buffer);
        APP_DEV_LOG_MSG_COM("data->rw_args.size:%x",   data->rw_args.size);
        return -1;
    }
    
    /* 检查溢出 */
    if (data->rw_args.offset + data->rw_args.size > data->ext_mem.chunk_size) {
        APP_DEV_LOG_MSG_COM("data->ext_mem.chunk_size:%x", data->ext_mem.chunk_size);
        APP_DEV_LOG_MSG_COM("data->rw_args.offset:%x",     data->rw_args.offset);
        APP_DEV_LOG_MSG_COM("data->rw_args.size:%x",       data->rw_args.size);
        return -2;
    }
    
    size_t retval = -1;
    /* chunk_base == 0x02000000, 保留内存 */
    if (data->ext_mem.chunk_base == 0x02000000) {
        FILE *file = fopen("ext_mem_static", "rb+");
        fseek(file, data->ext_mem.chunk_offset + data->rw_args.offset, SEEK_SET);
        retval = fread(data->rw_args.buffer, data->rw_args.size, 1, file);
        fclose(file);
    }
    /* chunk_base == 0x08000000, 闪存 */
    if (data->ext_mem.chunk_base == 0x08000000) {
        FILE *file = fopen("ext_mem_flash", "rb+");
        fseek(file, data->ext_mem.chunk_offset + data->rw_args.offset, SEEK_SET);
        retval = fread(data->rw_args.buffer, data->rw_args.size, 1, file);
        fclose(file);
    }
    /* chunk_base == 0x10000000, SD卡 */
    if (data->ext_mem.chunk_base == 0x10000000) {
        FILE *file = fopen("ext_mem_sd_card", "rb+");
        fseek(file, data->ext_mem.chunk_offset + data->rw_args.offset, SEEK_SET);
        retval = fread(data->rw_args.buffer, data->rw_args.size, 1, file);
        fclose(file);
    }
    
    memset(data, 0, sizeof(app_dev_ext_mem_data_t));
    return retval == 1 ? 0 : -3;
}

/*@brief ext_mem设备写入
 *@param driver 设备实例
 *@retval 失败返回负数
 */
static int32_t app_dev_ext_mem_hal_write(app_dev_t *driver)
{
    const app_dev_ext_mem_cfg_t *cfg = driver->cfg;
    app_dev_ext_mem_data_t *data = driver->data;
    
    if (data->rw_args.buffer == NULL || data->rw_args.size == 0) {
        APP_DEV_LOG_MSG_COM("data->rw_args.buffer:%p", data->rw_args.buffer);
        APP_DEV_LOG_MSG_COM("data->rw_args.size:%x",   data->rw_args.size);
        return -1;
    }
    
    /* 检查溢出 */
    if (data->rw_args.offset + data->rw_args.size > data->ext_mem.chunk_size) {
        APP_DEV_LOG_MSG_COM("data->ext_mem.chunk_size:%x", data->ext_mem.chunk_size);
        APP_DEV_LOG_MSG_COM("data->rw_args.offset:%x",     data->rw_args.offset);
        APP_DEV_LOG_MSG_COM("data->rw_args.size:%x",       data->rw_args.size);
        return -2;
    }
    
    size_t retval = -1;
    /* chunk_base == 0x02000000, 保留内存 */
    if (data->ext_mem.chunk_base == 0x02000000) {
        FILE *file = fopen("ext_mem_static", "rb+");
        fseek(file, data->ext_mem.chunk_offset + data->rw_args.offset, SEEK_SET);
        retval = fwrite(data->rw_args.buffer, data->rw_args.size, 1, file);
        fclose(file);
    }
    /* chunk_base == 0x08000000, 闪存 */
    if (data->ext_mem.chunk_base == 0x08000000) {
        FILE *file = fopen("ext_mem_flash", "rb+");
        fseek(file, data->ext_mem.chunk_offset + data->rw_args.offset, SEEK_SET);
        retval = fwrite(data->rw_args.buffer, data->rw_args.size, 1, file);
        fclose(file);
    }
    /* chunk_base == 0x10000000, SD卡 */
    if (data->ext_mem.chunk_base == 0x10000000) {
        FILE *file = fopen("ext_mem_sd_card", "rb+");
        fseek(file, data->ext_mem.chunk_offset + data->rw_args.offset, SEEK_SET);
        retval = fwrite(data->rw_args.buffer, data->rw_args.size, 1, file);
        fclose(file);
    }
    
    memset(data, 0, sizeof(app_dev_ext_mem_data_t));
    return retval == 1 ? 0 : -3;
}

/*@brief ext_mem设备数据流地址映射
 *@param driver 设备实例
 *@retval 返回值(app_dev_ext_mem_data_t *)类型地址
 */
static void * app_dev_ext_mem_hal_data_addr(app_dev_t *driver)
{
    const app_dev_ext_mem_cfg_t *cfg = driver->cfg;
    app_dev_ext_mem_data_t *data = driver->data;
    
    return data;
}

/* 静态配置的设备操作参数 */
static const app_dev_ext_mem_cfg_t app_dev_ext_mem_cfg = {
    .args = NULL,
};

/* 静态配置的设备操作集合 */
static const app_dev_ext_mem_api_t app_dev_ext_mem_api = {
    .ready     = app_dev_ext_mem_hal_ready,
    .read      = app_dev_ext_mem_hal_read,
    .write     = app_dev_ext_mem_hal_write,
    .data_addr = app_dev_ext_mem_hal_data_addr,
};

/* 动态的设备操作数据 */
static app_dev_ext_mem_data_t app_dev_ext_mem_data = {
    .ext_mem = {
        .chunk_base   = 0x00000000,
        .chunk_size   = 0,
        .chunk_offset = 0,
    },
    .rw_args = {
        .offset = 0x00000000,
        .buffer = 0,
        .size   = 0,
    },
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_ext_mem = {
    .name = "app_dev_ext_mem",
    .cfg  = &app_dev_ext_mem_cfg,
    .api  = &app_dev_ext_mem_api,
    .data = &app_dev_ext_mem_data,
};

#endif
