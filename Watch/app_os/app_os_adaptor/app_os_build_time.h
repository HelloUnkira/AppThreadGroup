#ifndef APP_OS_BUILD_TIME_H
#define APP_OS_BUILD_TIME_H

static inline void app_os_build_time(void)
{
    APP_OS_PRINT("FW Build Time:%s,%s\n", __DATE__, __TIME__);
}

#endif
