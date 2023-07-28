
#include "app_ext_lib.h"
#include "app_sys_timer.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_table.h"
#include "app_sys_log_text.h"
#include "app_thread_group.h"
#include "app_module_clock.h"
#include "app_module_rtc.h"
#include "app_module_protocol.h"
#include "app_module_stopwatch.h"
#include "app_module_countdown.h"
#include "app_module_remind_group.h"
#include "app_module_remind_alarm.h"

#if APP_OS_IS_WINDOWS

#include "ff.h"

/* 生成物理外存映射,将其都刷为0xFF */
static inline void app_sys_ext_mem_pc_refr(void)
{
    uint8_t buffer[1024] = {0};
    for (uint32_t idx = 0; idx < 1024; buffer[idx] = 0xff, idx++);
    fclose(fopen("ext_mem_static",  "wb")); // 64K
    fclose(fopen("ext_mem_flash",   "wb")); // 8M
    fclose(fopen("ext_mem_sd_card", "wb")); // 16M
    FILE *file1 = fopen("ext_mem_static",  "rb+");
    FILE *file2 = fopen("ext_mem_flash",   "rb+");
    FILE *file3 = fopen("ext_mem_sd_card", "rb+");
    fseek(file1, 0, SEEK_SET);
    fseek(file2, 0, SEEK_SET);
    fseek(file3, 0, SEEK_SET);
    for (uint32_t idx = 0; idx < 64; idx++)
        fwrite(buffer, 1024, 1, file1);
    for (uint32_t idx = 0; idx < 1024 * 8; idx++)
        fwrite(buffer, 1024, 1, file2);
    for (uint32_t idx = 0; idx < 1024 * 16; idx++)
        fwrite(buffer, 1024, 1, file3);
    fclose(file1);
    fclose(file2);
    fclose(file3);
}

/* 通过文件系统将文件打包到外存映射中 */
static inline void app_sys_ext_mem_pc_refr_fs_pkg_recursion(char *path)
{
    FRESULT retval = FR_OK;
    if ((retval = f_mkdir(path)) != FR_OK)
        printf("f_mkdir fail:%d\n", retval);
    
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
            printf("%s\n", path_next);
            app_sys_ext_mem_pc_refr_fs_pkg_recursion(path_next);
        } else {
            char path_file[256] = {0};
            sprintf(path_file, "%s\\%s", path, fdata.cFileName);
            printf("%s\n", path_file);
            UINT file_old_size = 0;
            UINT file_new_size = 0;
            
            FILE *file_old = fopen(path_file, "rb+");
            fseek(file_old, 0, SEEK_END);
            file_old_size = ftell(file_old);
            fseek(file_old, 0, SEEK_SET);
            uint8_t *buffer1 = malloc(file_old_size);
            if (fread(buffer1, file_old_size, 1, file_old) != 1)
                printf("fread fail\n");
            fclose(file_old);
            
            FIL file_new = {0};
            BYTE mode = FA_CREATE_NEW | FA_READ | FA_WRITE;
            if ((retval = f_open(&file_new, path_file, mode)) != FR_OK)
                printf("f_open fail:%d\n", retval);
            f_truncate(&file_new);
            if ((retval = f_write(&file_new, buffer1, file_old_size, &file_new_size)) != FR_OK || file_old_size != file_new_size)
                printf("f_write fail:%d\n", retval);
            if ((retval = f_close(&file_new)) != FR_OK)
                printf("f_close fail:%d\n", retval);
            
            free(buffer1);
        }
    } while (FindNextFileA(dir, &fdata));
}

/* 通过文件系统将文件打包到外存映射中 */
static inline void app_sys_ext_mem_pc_refr_fs_chk_recursion(char *path)
{
    static FILINFO fno;
    FRESULT retval = FR_OK;
    
    DIR dir;
    retval = f_opendir(&dir, path);
    if (retval != FR_OK)
        printf("f_opendir fail:%d\n", retval);
    
    while (true) {
        retval = f_readdir(&dir, &fno);
        if (retval != FR_OK || fno.fname[0] == 0)
            break;
        
        if (strcmp(fno.fname,  ".") == 0 ||
            strcmp(fno.fname, "..") == 0)
            continue;
        
        if (fno.fattrib & AM_DIR) {
            char path_next[256] = {0};
            sprintf(path_next, "%s/%s", path, fno.fname);
            printf("%s\n", path_next);
            app_sys_ext_mem_pc_refr_fs_chk_recursion(path_next);
        } else {
            char path_file[256] = {0};
            sprintf(path_file, "%s/%s", path, fno.fname);
            printf("%s\n", path_file);
        }
    }
    f_closedir(&dir);
}

/* 通过文件系统将文件打包到外存映射中 */
static inline void app_sys_ext_mem_pc_refr_fs(char *path)
{
    FRESULT retval = FR_OK;
    /* 重构文件系统 */
    BYTE work_station[1024 * 64] = {0};
    const MKFS_PARM opt = {FM_FAT, 0, 0, 0, 0};
    retval = f_mkfs("", &opt, work_station, sizeof(work_station));
    if (retval != FR_OK)
        printf("f_mkfs fail:%d\n", retval);
    /* 挂载文件系统 */
    FATFS fat_fs = {0};
    retval = f_mount(&fat_fs, "", 1);
    if (retval != FR_OK)
        printf("f_mount fail:%d\n", retval);
    app_sys_ext_mem_pc_refr_fs_pkg_recursion(path);
    printf("\n\n\nfat fs recursion:\n\n\n");
    app_sys_ext_mem_pc_refr_fs_chk_recursion(path);
    retval = f_unmount("");
    if (retval != FR_OK)
        printf("f_unmount fail:%d\n", retval);
}

#endif

int main(int argc, char *argv[])
{
    /* 启动APP调度策略 */
    app_thread_group_schedule();
    /* 主线程滚动阻塞 */
    while (true) {
        #if 0
        #elif 0 && APP_OS_IS_WINDOWS
        app_sys_ext_mem_pc_refr();
        break;
        #elif 1 && APP_OS_IS_WINDOWS
        app_sys_ext_mem_pc_refr_fs("lvgl_ext_src");
        break;
        #elif 1
        #if 0
        /* 测试日志追踪 */
        app_sys_log_text_test();
        #endif
        /* 测试中我们在主线程 */
        /* 模拟发送1ms定时器中断事件 */
        /* 我是实在没想到这种方式 */
        /* 居然是最简单的做法...... */
        #if APP_ARCH_IS_PC
        /* fake hard clock 1ms irq */
        app_module_rtc_1ms_cb();
        app_delay_us(1000);
        #else
        #endif
        /* test:... */
        #if 0
        #elif 0
        /* test timer */
        if (count == 1000)
            app_sys_timer_test();
        #elif 0
        /* test reset load and dump */
        if (count % 5000 == 0) {
            app_module_system_delay_set(2);
            app_module_system_status_set(app_module_system_reset);
        }
        #elif 0
        /* test stopwatch */
        if (count == 1000) {
            app_module_stopwatch_reset();
            app_module_stopwatch_start();
        }
        #elif 0
        /* test countdown */
        if (count == 1000) {
            app_module_countdown_reset();
            app_module_countdown_t countdown = {
                .hour   = 0,
                .minute = 0,
                .second = 13,
            };
            app_module_countdown_set(&countdown);
            app_module_countdown_start();
        }
        #elif 0
        /* test alarm group */
        if (count == 1000)
            app_module_remind_alarm_test();
        #elif 0
        /* test protocol(7s later) */
        if (count == 1000 * 7) {
            app_module_protocol_t protocol = {
              //.notify.type = app_module_protocol_system_clock,
                .notify.type = app_module_protocol_trace_text,
            };
            app_module_protocol_notify(&protocol);
        }
        #else
        #endif
        #endif
    }
    return 0;
}
