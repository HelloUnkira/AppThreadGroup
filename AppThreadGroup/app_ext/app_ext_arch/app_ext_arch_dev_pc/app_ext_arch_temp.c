/* 实现目标:
 *     模拟温度计
 */

#include "app_ext_lib.h"

/*@brief     temperature设备获得当前温度值
 *@param[in] driver 设备实例
 *@retval    获得当前温度值
 */
static float app_arch_temp_get_curr(app_arch_dev_t *driver)
{
    const app_arch_temp_cfg_t *cfg = driver->cfg;
    /* 这里伪造一下数据即可,因为是模拟 */
    return 36.0 + rand() % 6000 / 1.0e-3;
}

/*@brief     temperature设备打开
 *@param[in] driver 设备实例
 */
static void app_arch_temp_hal_open(app_arch_dev_t *driver)
{
    const app_arch_temp_cfg_t *cfg = driver->cfg;
    /* 填充目标平台下的动作 */
}

/*@brief     temperature设备关闭
 *@param[in] driver 设备实例
 */
static void app_arch_temp_hal_close(app_arch_dev_t *driver)
{
    const app_arch_temp_cfg_t *cfg = driver->cfg;
    /* 填充目标平台下的动作 */
}

/*@brief     temperature设备每分钟自动定时器调用
 *@param[in] driver 设备实例
 */
static void app_arch_temp_hal_auto_xs_measure(app_arch_dev_t *driver)
{
    const app_arch_temp_cfg_t *cfg = driver->cfg;
    app_arch_temp_data_t *data = driver->data;
    /*  */
    
}

/*@brief     temperature设备X毫秒手动定时器调用
 *@param[in] driver 设备实例
 *@retval    测量是否完毕
 */
static bool app_arch_temp_hal_manual_xms_measure(app_arch_dev_t *driver)
{
    const app_arch_temp_cfg_t *cfg = driver->cfg;
    app_arch_temp_data_t *data = driver->data;
    /*  */
    if (data->manual_tmp_cnt >= APP_ARCH_TEMP_MANUAL_CNT) {
        data->manual_tmp_cnt  = 0;
        return true;
    } else {
        if (data->manual_tmp_cnt == 0) {
            for (uint8_t i = 0; i < data->manual_tmp_cnt; i++)
                data->manual_tmp_arr[i] = 0.0;
                data->manual_val        = 0.0;
        }
        /* 获得此刻温度测量数据 */
        float val = app_arch_temp_get_curr(driver);
        /* 暂存温度数据 */
        data->manual_tmp_arr[data->manual_tmp_cnt++] = val;
        /* 暂存温度中值滤波 */
        if (data->manual_tmp_cnt >= 3) {
            /* 简单排序一下 */
            for (uint8_t i = 0; i < data->manual_tmp_cnt; i++)
            for (uint8_t j = i + 1; j < data->manual_tmp_cnt; j++)
                if (data->manual_tmp_arr[i] > data->manual_tmp_arr[j]) {
                                        val = data->manual_tmp_arr[i];
                    data->manual_tmp_arr[i] = data->manual_tmp_arr[j];
                    data->manual_tmp_arr[j] = val;
                }
            /* 取中值 */
            if (data->manual_tmp_cnt % 2 == 0) {
                val  = 0.0;
                val += data->manual_tmp_arr[data->manual_tmp_cnt / 2];
                val += data->manual_tmp_arr[data->manual_tmp_cnt / 2 + 1];
                data->manual_val = val / 2;
            } else {
                val = data->manual_tmp_arr[data->manual_tmp_cnt / 2];
                data->manual_val = val;
            }
        }
        /*  */
        return false;
    }
}

/*@brief     空设备数据流地址映射
 *@param[in] driver 设备实例
 *@retval    动态数据映射首地址
 */
static inline void * app_arch_temp_hal_data_addr(app_arch_dev_t *driver)
{
    const app_arch_temp_cfg_t *cfg = driver->cfg;
    app_arch_temp_data_t *data = driver->data;
    /*  */
    return data;
}

/* 静态配置的设备操作参数 */
static const app_arch_temp_cfg_t app_arch_temp_cfg = {
    .parameter = NULL,
};

/* 静态配置的设备操作集合 */
static const app_arch_temp_api_t app_arch_temp_api = {
    .open               = app_arch_temp_hal_open,
    .close              = app_arch_temp_hal_close,
    .auto_xs_measure    = app_arch_temp_hal_auto_xs_measure,
    .manual_xms_measure = app_arch_temp_hal_manual_xms_measure,
    .data_addr          = app_arch_temp_hal_data_addr,
};

/* 动态的设备操作数据 */
static app_arch_temp_data_t app_arch_temp_data = {
    /* 自动测量数据: */
    .auto_tmp_cnt =  0,
    .auto_val_cnt =  0,
    .auto_tmp_arr = {0.0},
    .auto_val_arr = {0.0},
    /* 手动测量数据: */
    .manual_tmp_cnt =  0,
    .manual_tmp_arr = {0.0},
    .manual_val = 0,
};

/* 静态配置的设备实例 */
const app_arch_dev_t app_arch_temp = {
    .name = "app_arch_temp",
    .cfg  = &app_arch_temp_cfg,
    .api  = &app_arch_temp_api,
    .data = &app_arch_temp_data,
};
