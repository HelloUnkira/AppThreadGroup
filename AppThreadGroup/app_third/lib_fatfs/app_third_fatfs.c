
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_table.h"

#include "ff.h"

/*@brief 初始化FatFS
 */
void app_third_fatfs_init(void)
{
    /* 挂载文件系统 */
    FATFS fat_fs = {0};
    FRESULT retval = FR_OK;
    if ((retval = f_mount(&fat_fs, "", 1)) != FR_OK)
        APP_SYS_LOG_WARN("f_mount fail:%d\n", retval);
}

/*@brief 初始化FatFS
 */
void app_third_fatfs_deinit(void)
{
    FRESULT retval = FR_OK;
    if ((retval = f_unmount("")) != FR_OK)
        APP_SYS_LOG_WARN("f_unmount fail:%d\n", retval);
}
