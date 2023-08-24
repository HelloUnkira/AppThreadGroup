
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#include "ff.h"

/* 动态内存适配接口 */

#if FF_USE_LFN == 3

void * ff_memalloc(UINT msize)
{
	return app_mem_alloc((uint32_t)msize);
}

void ff_memfree(void *mblock)
{
	app_mem_free(mblock);
}

#endif

/* 多线程同步适配接口 */

#if FF_FS_REENTRANT

static app_mutex_t app_mutex_ffsystem[FF_VOLUMES + 1] = {0};

int ff_mutex_create(int vol)
{
    app_mutex_process(&app_mutex_ffsystem[vol], app_mutex_create);
    return (int)true;
}

void ff_mutex_delete(int vol)
{
    app_mutex_process(&app_mutex_ffsystem[vol], app_mutex_destroy);
}

int ff_mutex_take(int vol)
{
    app_mutex_process(&app_mutex_ffsystem[vol], app_mutex_take);
    return (int)true;
}

void ff_mutex_give(int vol)
{
    app_mutex_process(&app_mutex_ffsystem[vol], app_mutex_give);
}

#endif
