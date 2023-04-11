#ifndef APP_EXT_ARCH_H
#define APP_EXT_ARCH_H

/* APP arch adaptor: */
#define APP_ARCH_IS_UNKNOWN     1
#define APP_ARCH_IS_PC          1

/* arch custom adaptor */
#include "app_ext_arch_pc.h"
/* arch ext mem adaptor */
#include "app_ext_arch_ext_mem.h"
/* arch driver abstract adaptor */
#include "app_ext_arch_dev.h"
#include "app_ext_arch_dev_pc_empty.h"

#endif
