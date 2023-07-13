#ifndef APP_EXT_ARCH_TEMP_PC_H
#define APP_EXT_ARCH_TEMP_PC_H

#if APP_ARCH_IS_PC

/* 设备temperature抽象操作参数 */
typedef struct {
    /* 自动测量: */
    #define APP_ARCH_TEMP_AUTO_XS       (60)        //自动测量一次间隔
    #define APP_ARCH_TEMP_AUTO_X_MGE    (15)        //自动测量累积次数合并
    #define APP_ARCH_TEMP_AUTO_X_CNT    (24)        //自动测量累积次数
    /* 手动测量: */
    #define APP_ARCH_TEMP_MANUAL_XMS    (200)       //手动测量一次间隔
    #define APP_ARCH_TEMP_MANUAL_CNT    (5 * 5)     //手动测量缓存数据
    void *parameter;
} app_arch_temp_cfg_t;

/* 设备temperature抽象操作接口 */
typedef struct {
    void    (*open)(app_arch_dev_t *driver);
    void    (*close)(app_arch_dev_t *driver);
    void    (*auto_xs_measure)(app_arch_dev_t *driver);
    bool    (*manual_xms_measure)(app_arch_dev_t *driver);
    void *  (*data_addr)(app_arch_dev_t *driver);
} app_arch_temp_api_t;

/* 设备temperature抽象操作数据 */
typedef struct {
    /* 自动测量数据: */
    uint8_t auto_tmp_cnt;
    uint8_t auto_val_cnt;
    float   auto_tmp_arr[APP_ARCH_TEMP_AUTO_X_MGE];
    float   auto_val_arr[APP_ARCH_TEMP_AUTO_X_CNT];
    /* 手动测量数据: */
    uint8_t manual_tmp_cnt;
    float   manual_tmp_arr[APP_ARCH_TEMP_MANUAL_CNT];
    float   manual_val;
} app_arch_temp_data_t;

#endif

#endif
