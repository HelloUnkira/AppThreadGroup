#ifndef APP_MODULE_XFER_FILE_H
#define APP_MODULE_XFER_FILE_H

/* 通用文件传输:公共常量与类型定义(实现见app_nanopb_xfer_file.c) */

#define APP_MODULE_XFER_FILE_BUFFER_SIZE    1024   /* 文件内容缓冲 */
#define APP_MODULE_XFER_FILE_CHUNK_SIZE     512    /* 分包负载大小 */

/* 文件类型 */
typedef enum {
    APP_MODULE_XFER_FILE_TYPE_LOG = 0,
    APP_MODULE_XFER_FILE_TYPE_FW  = 1,
    APP_MODULE_XFER_FILE_TYPE_CWF = 2,
    APP_MODULE_XFER_FILE_TYPE_SWF = 3,
} app_module_xfer_file_type_t;

/* 传输阶段 */
typedef enum {
    APP_MODULE_XFER_FILE_STAGE_GO    = 0,
    APP_MODULE_XFER_FILE_STAGE_START = 1,
    APP_MODULE_XFER_FILE_STAGE_END   = 2,
} app_module_xfer_file_stage_t;

/* 传输结果码 */
typedef enum {
    APP_MODULE_XFER_FILE_RESULT_OK        = 0,
    APP_MODULE_XFER_FILE_RESULT_CHK_FAIL = 1,
    APP_MODULE_XFER_FILE_RESULT_OTHER    = 2,
} app_module_xfer_file_result_t;

/* 子对象(用于应答tag映射) */
typedef enum {
    APP_MODULE_XFER_FILE_SUB_DES  = 0,
    APP_MODULE_XFER_FILE_SUB_PKG  = 1,
    APP_MODULE_XFER_FILE_SUB_XFER = 2,
} app_module_xfer_file_sub_t;

#endif
