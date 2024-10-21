
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#include "ff.h"

/*@brief 文件系统基本信息
 *@param path 根路径
 */
void app_third_fatfs_info(char *path)
{
    FATFS *fs = NULL;
    FRESULT retval = FR_OK;
    char label[12] = {0}, fs_type[5] = {0}, cwd[256] = {0};
    DWORD free_clusters = 0, free_sectors = 0, total_sectors = 0;
    
    if ((retval = f_getfree(path, &free_clusters, &fs)) != FR_OK) {
        APP_SYS_LOG_WARN("f_getfree failed:%d", retval);
        return;
    }
    
    free_sectors  = fs->csize * (free_clusters);
    total_sectors = fs->csize * (fs->n_fatent - 2);
    
    if ((retval = f_getlabel(path, label, NULL)) != FR_OK)
        sprintf(label, "%s", "N/A");
    if ((retval = f_getcwd(cwd, sizeof(cwd))) != FR_OK)
        sprintf(cwd, "%s", "N/A");
    
    APP_SYS_LOG_INFO("");
    APP_SYS_LOG_INFO("Volume label:%s",     label);
    APP_SYS_LOG_INFO("Current work dir:%s", cwd);
    APP_SYS_LOG_INFO("Free  space: %lu KB", free_sectors  / 2);
    APP_SYS_LOG_INFO("Total space: %lu KB", total_sectors / 2);
    
    APP_SYS_LOG_DEBUG("");
    APP_SYS_LOG_DEBUG("fs->fs_type:%s",     fs->fs_type == FS_FAT12 ? "FAT12" :
                                            fs->fs_type == FS_FAT16 ? "FAT16" :
                                            fs->fs_type == FS_FAT32 ? "FAT32" :
                                            fs->fs_type == FS_EXFAT ? "exFAT" : "N/A");
    APP_SYS_LOG_DEBUG("fs->id: %x",         fs->id);
    APP_SYS_LOG_DEBUG("fs->pdrv: %x",       fs->pdrv);
    APP_SYS_LOG_DEBUG("fs->ldrv: %x",       fs->ldrv);
    APP_SYS_LOG_DEBUG("fs->csize: %x",      fs->csize);
    APP_SYS_LOG_DEBUG("fs->wflag: %x",      fs->wflag);
    APP_SYS_LOG_DEBUG("fs->fsi_flag: %x",   fs->fsi_flag);
    APP_SYS_LOG_DEBUG("fs->n_fats: %x",     fs->n_fats);
    APP_SYS_LOG_DEBUG("fs->n_rootdir: %x",  fs->n_rootdir);
    APP_SYS_LOG_DEBUG("fs->n_fatent: %x",   fs->n_fatent);
    APP_SYS_LOG_DEBUG("fs->fsize: %x",      fs->fsize);
    APP_SYS_LOG_DEBUG("fs->volbase: %x",    fs->volbase);
    APP_SYS_LOG_DEBUG("fs->fatbase: %x",    fs->fatbase);
    APP_SYS_LOG_DEBUG("fs->dirbase: %x",    fs->dirbase);
    APP_SYS_LOG_DEBUG("fs->database: %x",   fs->database);
    APP_SYS_LOG_DEBUG("");
}

/*@brief 递归遍历文件系统的文件列表
 *@param path 根路径
 */
void app_third_fatfs_walk(char *path)
{
    static FILINFO fno;
    FRESULT retval = FR_OK;
    
    DIR dir;
    APP_SYS_LOG_INFO(path);
    if ((retval = f_opendir(&dir, path)) != FR_OK)
        APP_SYS_LOG_WARN("f_opendir fail:%d", retval);
    
    while (true) {
        if ((retval = f_readdir(&dir, &fno)) != FR_OK || fno.fname[0] == 0)
            break;
        
        if (strcmp(fno.fname, ".") == 0 || strcmp(fno.fname, "..") == 0)
            continue;
        
        if (fno.fattrib & AM_DIR) {
            char path_next[256] = {0};
            sprintf(path_next, "%s/%s", path, fno.fname);
            app_third_fatfs_walk(path_next);
        } else {
            char path_file[256] = {0};
            sprintf(path_file, "%s/%s", path, fno.fname);
            APP_SYS_LOG_INFO(path_file);
            
            FIL file = {0};
            if ((retval = f_open(&file, path_file, FA_READ | FA_WRITE)) != FR_OK)
                APP_SYS_LOG_WARN("f_open fail:%d", retval);
            if ((retval = f_close(&file)) != FR_OK)
                APP_SYS_LOG_WARN("f_close fail:%d", retval);
        }
    }
    f_closedir(&dir);
}

/*@brief 初始化FatFS
 */
void app_third_fatfs_init(void)
{
    FRESULT retval = FR_OK;
    
    /* 挂载文件系统 */
    static FATFS fat_fs = {0};
    if ((retval = f_mount(&fat_fs, "", 1)) != FR_OK)
        APP_SYS_LOG_WARN("f_mount fail:%d", retval);
    
    /* 输出文件系统基本信息 */
    app_third_fatfs_info("");
    /* 递归遍历文件系统的文件列表 */
    // app_third_fatfs_walk("");
    
    #if APP_THIRD_FATFS_USE_TEST_MODE
    for (uint32_t idx = 0; idx < 1000; idx++)
        app_third_fatfs_walk("");
    #endif
}

/*@brief 反初始化FatFS
 */
void app_third_fatfs_deinit(void)
{
    /* 卸载文件系统 */
    FRESULT retval = FR_OK;
    if ((retval = f_unmount("")) != FR_OK)
        APP_SYS_LOG_WARN("f_unmount fail:%d", retval);
}

/*@brief 通过文件系统将文件打包到外存映射中
 *       注意:仅在PC上构建
 *@param path 根路径
 */
static void app_third_fatfs_remake_recursion(char *path)
{
    #if APP_ARCH_IS_PC && APP_OS_IS_WINDOWS
    
    FRESULT retval = FR_OK;
    if ((retval = f_mkdir(path)) != FR_OK)
        APP_SYS_LOG_WARN("f_mkdir fail:%d", retval);
    
    WIN32_FIND_DATAA fdata;
    char path_curr[256] = {0};
    sprintf(path_curr, "%s\\*", path);
    HANDLE dir = FindFirstFileA(path_curr, &fdata);
    
    do {
        if (strcmp(fdata.cFileName,  ".") == 0 ||
            strcmp(fdata.cFileName, "..") == 0)
            continue;
        
        if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            char path_next[256] = {0};
            sprintf(path_next, "%s\\%s", path, fdata.cFileName);
            APP_SYS_LOG_INFO(path_next);
            app_third_fatfs_remake_recursion(path_next);
        } else {
            UINT file_old_size = 0;
            char path_file[256] = {0};
            sprintf(path_file, "%s\\%s", path, fdata.cFileName);
            APP_SYS_LOG_INFO(path_file);
            
            FILE *file_old = fopen(path_file, "rb+");
            fseek(file_old, 0, SEEK_END);
            file_old_size = ftell(file_old);
            fseek(file_old, 0, SEEK_SET);
            uint8_t *buffer1 = malloc(file_old_size);
            if (fread(buffer1, file_old_size, 1, file_old) != 1)
                APP_SYS_LOG_WARN("fread fail");
            fclose(file_old);
            
            FIL file_new = {0};
            UINT file_new_size = 0;
            char path_file_new[256] = {0};
            sprintf(path_file_new, "%s/%s", path, fdata.cFileName);
            BYTE mode = FA_CREATE_NEW | FA_READ | FA_WRITE;
            if ((retval = f_open(&file_new, path_file_new, mode)) != FR_OK)
                APP_SYS_LOG_WARN("f_open fail:%d", retval);
            f_truncate(&file_new);
            if ((retval = f_write(&file_new, buffer1, file_old_size, &file_new_size)) != FR_OK || file_old_size != file_new_size)
                APP_SYS_LOG_WARN("f_write fail:%d", retval);
            if ((retval = f_close(&file_new)) != FR_OK)
                APP_SYS_LOG_WARN("f_close fail:%d", retval);
            
            free(buffer1);
        }
    } while (FindNextFileA(dir, &fdata));
    
    #endif
}

/*@brief 通过文件系统将文件打包到外存映射中
 *       注意:仅在PC上构建
 *@param path 路径
 *@param num  路径数量
 */
void app_third_fatfs_remake(char *path[], int8_t num)
{
    #if APP_ARCH_IS_PC && APP_OS_IS_WINDOWS
    
    FRESULT retval = FR_OK;
    /* 重构文件系统 */
    BYTE work_station[1024 * 64] = {0};
    const MKFS_PARM opt = {FM_ANY, 0, 0, 0, 0};
    if ((retval = f_mkfs("", &opt, work_station, sizeof(work_station))) != FR_OK)
        APP_SYS_LOG_WARN("f_mkfs fail:%d", retval);
    /* 挂载文件系统 */
    FATFS fat_fs = {0};
    if ((retval = f_mount(&fat_fs, "", 1)) != FR_OK)
        APP_SYS_LOG_WARN("f_mount fail:%d", retval);
    
    /* 通过文件系统将文件打包到外存映射中 */
    for (int8_t idx = 0; idx < num; idx++)
        app_third_fatfs_remake_recursion(path[idx]);
    
    /* 输出文件系统基本信息 */
    /* 递归遍历文件系统的文件列表 */
    app_third_fatfs_info("");
    app_third_fatfs_walk("");
    
    if ((retval = f_unmount("")) != FR_OK)
        APP_SYS_LOG_WARN("f_unmount fail:%d", retval);
    
    #else
    #error "unknown"
    #endif
}
