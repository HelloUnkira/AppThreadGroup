/*需要适配...
 *        JS OS data适配
 */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "jerryscript-port.h"

#if 0
#elif     APP_OS_IS_WINDOWS
#include <winbase.h>
#include <winnt.h>


#define UNIX_EPOCH_IN_TICKS 116444736000000000ull
#define TICKS_PER_MS        10000ull

/* UNIX时间转为文件时间 */
/* https://support.microsoft.com/en-us/help/167296/how-to-convert-a-unix-time-t-to-a-win32-filetime-or-systemtime */
static void unix_time_to_filetime(double t, LPFILETIME ft_p)
{
    LONGLONG ll = (LONGLONG)t * TICKS_PER_MS + UNIX_EPOCH_IN_TICKS;
    
    /* 纪元之前的文件时间值无效 */
    if (ll < 0)
        ll = 0;
    
    ft_p->dwLowDateTime  = (DWORD)(ll);
    ft_p->dwHighDateTime = (DWORD)(ll >> 32);
}

/* 文件时间转为UNIX时间 */
static double filetime_to_unix_time(LPFILETIME ft_p)
{
    ULARGE_INTEGER date;
    date.HighPart = ft_p->dwHighDateTime;
    date.LowPart  = ft_p->dwLowDateTime;
    return (double)(((LONGLONG) date.QuadPart - UNIX_EPOCH_IN_TICKS) / TICKS_PER_MS);
}

int32_t jerry_port_local_tza(double unix_ms)
{
    FILETIME utc;
    FILETIME local;
    SYSTEMTIME utc_sys;
    SYSTEMTIME local_sys;

    unix_time_to_filetime(unix_ms, &utc);

    if (FileTimeToSystemTime(&utc, &utc_sys) &&
        SystemTimeToTzSpecificLocalTime(NULL, &utc_sys, &local_sys) &&
        SystemTimeToFileTime(&local_sys, &local)) {
        double unix_local = filetime_to_unix_time (&local);
        return (int32_t)(unix_local - unix_ms);
    }

    return 0;
}

double jerry_port_current_time(void)
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return filetime_to_unix_time(&ft);
}

#else
#error "jerry port os data is not adapted yet"
#endif
