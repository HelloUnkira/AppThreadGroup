
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_table.h"

#include "ff.h"
#include "diskio.h"

#define APP_FATFS_EXT_MEM           0       /* 抽象外存文件系统 */
#define APP_FATFS_EXT_MEM_SECTOR    4096    /* 抽象外存文件系统的扇区尺寸 */
#define APP_FATFS_EXT_MEM_BLOCK     512     /* 抽象外存文件系统的块尺寸 */

DSTATUS disk_status(BYTE pdrv)
{
    if (pdrv == APP_FATFS_EXT_MEM) {
        /* 无事可做 */
        return RES_OK;
    } else {
        APP_SYS_ASSERT(RES_OK == STA_NOINIT);
        return STA_NOINIT;
    }
}

DSTATUS disk_initialize(BYTE pdrv)
{
    if (pdrv == APP_FATFS_EXT_MEM) {
        /* 无事可做 */
        return RES_OK;
    } else {
        APP_SYS_ASSERT(RES_OK == STA_NOINIT);
        return STA_NOINIT;
    }
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv == APP_FATFS_EXT_MEM) {
        
        uint8_t  *buffer = (void *)buff;
        uintptr_t offset = APP_FATFS_EXT_MEM_SECTOR * (uintptr_t)sector;
        uintptr_t size   = APP_FATFS_EXT_MEM_SECTOR * (uintptr_t)count;
        
        const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("fat_fs");
        if (app_sys_ext_mem_read(ext_mem, offset, buffer, size))
            return RES_OK;
        else
            return RES_ERROR;
        
    } else {
        APP_SYS_ASSERT(RES_OK == RES_ERROR);
        return RES_ERROR;
    }
}

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv == APP_FATFS_EXT_MEM) {
        
        uint8_t  *buffer = (void *)buff;
        uintptr_t offset = APP_FATFS_EXT_MEM_SECTOR * (uintptr_t)sector;
        uintptr_t size   = APP_FATFS_EXT_MEM_SECTOR * (uintptr_t)count;
        
        const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("fat_fs");
        if (app_sys_ext_mem_write(ext_mem, offset, buffer, size))
            return RES_OK;
        else
            return RES_ERROR;
        
    } else {
        APP_SYS_ASSERT(RES_OK == RES_ERROR);
        return RES_ERROR;
    }
}

#endif

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    if (pdrv == APP_FATFS_EXT_MEM) {
        
        switch (cmd) {
        case CTRL_SYNC: {
            return RES_OK;
        }
        case GET_SECTOR_COUNT: {
            const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("fat_fs");
            *(DWORD *)buff = ext_mem->chunk_size / APP_FATFS_EXT_MEM_SECTOR;
            return RES_OK;
        }
        case GET_SECTOR_SIZE: {
            *(DWORD *)buff = APP_FATFS_EXT_MEM_SECTOR;
            return RES_OK;
        }
        case GET_BLOCK_SIZE: {
            *(DWORD *)buff = APP_FATFS_EXT_MEM_BLOCK;
            return RES_OK;
        }
        default: {
            APP_SYS_ASSERT(RES_OK == RES_PARERR);
            return RES_PARERR;
        }
        }
        
    } else {
        APP_SYS_ASSERT(RES_OK == RES_PARERR);
        return RES_PARERR;
    }
}