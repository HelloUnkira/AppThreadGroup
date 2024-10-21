/*需要适配...
 *    JS上下文
 */

#include "app_ext_lib.h"
#include "jerryscript-port.h"

#if 0
#elif     APP_ARCH_IS_PC

#ifndef JERRY_GLOBAL_HEAP_SIZE
#define JERRY_GLOBAL_HEAP_SIZE 512
#endif

static jerry_context_t *current_context_p = NULL;

/* 创建JS上下文空间 */
size_t JERRY_ATTR_WEAK jerry_port_context_alloc(size_t context_size)
{
    if (current_context_p != NULL) {
        app_mem_free(current_context_p);
        current_context_p  = NULL;
    }
    size_t total_size = context_size + JERRY_GLOBAL_HEAP_SIZE * 1024;
    current_context_p = app_mem_alloc(total_size);
    return total_size;
}

/* 获得JS上下文空间 */
jerry_context_t *JERRY_ATTR_WEAK jerry_port_context_get(void)
{
    return current_context_p;
}

/* 释放JS上下文空间 */
void JERRY_ATTR_WEAK jerry_port_context_free(void)
{
    app_mem_free(current_context_p);
}

#else
#error "jerry port context is not adapted yet"
#endif
