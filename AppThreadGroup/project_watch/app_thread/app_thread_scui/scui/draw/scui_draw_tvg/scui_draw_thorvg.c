/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

void scui_draw_thorvg_assert(void *ptr)
{
    SCUI_ASSERT(ptr != NULL);
}

void * scui_draw_thorvg_alloc(int size)
{
    return SCUI_MEM_ALLOC(scui_mem_type_graph, size);
}

void * scui_draw_thorvg_zalloc(int size)
{
    return SCUI_MEM_ZALLOC(scui_mem_type_graph, size);
}

void * scui_draw_thorvg_realloc(void *ptr, int new_size)
{
    if (ptr == NULL)
        return scui_draw_thorvg_alloc(new_size);
        
    if (new_size == 0) {
        scui_draw_thorvg_free(ptr);
        return NULL;
    }
    
    /* 这种不该出现, 会导致设计多义 */
    if (scui_mem_size_ptr(ptr) >= new_size + 40)
        return ptr;
    
    void *ptr_new = scui_draw_thorvg_alloc(new_size);
    memcpy(ptr_new, ptr, new_size);
    scui_draw_thorvg_free(ptr);
    return ptr_new;
}

void scui_draw_thorvg_free(void *ptr)
{
    SCUI_MEM_FREE(ptr);
}

char * scui_draw_thorvg_strdup(const char * src)
{
    size_t len = strlen(src) + 1;
    char * dst = scui_draw_thorvg_alloc(len);
    if(dst == NULL) return NULL;
    
    /*memcpy is faster than strncpy when length is known*/
    memcpy(dst, src, len);
    return dst;
}
