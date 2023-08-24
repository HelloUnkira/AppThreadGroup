
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

#include "ff.h"
#include "diskio.h"

#define APP_FATFS_EXT_MEM   0   /* 抽象外存文件系统 */

DSTATUS disk_status(BYTE pdrv)
{
    if (pdrv == APP_FATFS_EXT_MEM) {
        return RES_OK;
    } else {
        // APP_SYS_ASSERT(RES_OK == STA_NOINIT);
        APP_SYS_LOG_ERROR("pdrv:%u", pdrv);
        return STA_NOINIT;
    }
}

DSTATUS disk_initialize(BYTE pdrv)
{
    if (pdrv == APP_FATFS_EXT_MEM) {
        return RES_OK;
    } else {
        // APP_SYS_ASSERT(RES_OK == STA_NOINIT);
        APP_SYS_LOG_ERROR("pdrv:%u", pdrv);
        return STA_NOINIT;
    }
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv == APP_FATFS_EXT_MEM) {
        uint8_t  *buffer = (void *)buff;
        uintptr_t offset = FF_MAX_SS * (uintptr_t)sector;
        uintptr_t size   = FF_MAX_SS * (uintptr_t)count;
        
        const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("fat_fs");
        if (app_sys_ext_mem_read(ext_mem, offset, buffer, size))
            return RES_OK;
        else {
            APP_SYS_ASSERT(RES_OK == RES_ERROR);
            return RES_ERROR;
        }
    } else {
        // APP_SYS_ASSERT(RES_OK == RES_ERROR);
        APP_SYS_LOG_ERROR("pdrv:%u", pdrv);
        return RES_ERROR;
    }
}

#if FF_FS_READONLY == 0

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if (pdrv == APP_FATFS_EXT_MEM) {
        uint8_t  *buffer = (void *)buff;
        uintptr_t offset = FF_MAX_SS * (uintptr_t)sector;
        uintptr_t size   = FF_MAX_SS * (uintptr_t)count;
        
        const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("fat_fs");
        if (app_sys_ext_mem_write(ext_mem, offset, buffer, size))
            return RES_OK;
        else {
            APP_SYS_ASSERT(RES_OK == RES_ERROR);
            return RES_ERROR;
        }
    } else {
        // APP_SYS_ASSERT(RES_OK == RES_ERROR);
        APP_SYS_LOG_ERROR("pdrv:%u", pdrv);
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
            *(LBA_t *)buff = ext_mem->chunk_size / FF_MAX_SS;
            return RES_OK;
        }
        case GET_SECTOR_SIZE: {
            *(WORD  *)buff = FF_MAX_SS;
            return RES_OK;
        }
        case GET_BLOCK_SIZE: {
            *(DWORD *)buff = 1;
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

#if !FF_FS_READONLY && !FF_FS_NORTC
DWORD get_fattime(void)
{
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    
    union {
        DWORD retval;
        struct {
            uint32_t year:7;
            uint32_t month:4;
            uint32_t day:5;
            uint32_t hour:5;
            uint32_t minute:6;
            uint32_t second:5;
        } clock;
    } dtime = {
        .clock.year   = clock.year,
        .clock.month  = clock.month,
        .clock.day    = clock.day,
        .clock.hour   = clock.hour,
        .clock.minute = clock.minute,
        .clock.second = clock.second,
    };
    
    return dtime.retval;
}
#endif
