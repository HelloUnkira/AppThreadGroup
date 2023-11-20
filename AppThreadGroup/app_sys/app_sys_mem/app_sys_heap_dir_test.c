/*实现目标:
 *    双端分配堆测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

void app_sys_heap_dir_test(void)
{
    #define APP_SYS_HEAP_DIR_COUNT      (1024)
    #define APP_SYS_HEAP_DIR_BASE       (512)

    uintptr_t align[] = {sizeof(uintptr_t), sizeof(uintptr_t) * 2, sizeof(uintptr_t) * 4,};
    uintptr_t size = APP_SYS_HEAP_DIR_COUNT * APP_SYS_HEAP_DIR_BASE;
    uint8_t  *buffer = app_mem_alloc(size);
    void     *list[APP_SYS_HEAP_DIR_COUNT] = {0};
    
    app_sys_heap_dir_t heap_dir = {0};
    app_sys_heap_dir_ready(&heap_dir, (uintptr_t)buffer, size);
    
    
    
    /* 将内存尽可能的榨干 */
    for (uint32_t idx = 0; idx < APP_SYS_HEAP_DIR_COUNT; idx++) {
         uint32_t pos = 0, bse = rand() % APP_SYS_HEAP_DIR_COUNT;
         for (uint32_t ofs = 0; ofs < APP_SYS_HEAP_DIR_COUNT; ofs++) {
              if (list[ (ofs + bse) % APP_SYS_HEAP_DIR_COUNT] == NULL) {
                  pos = (ofs + bse) % APP_SYS_HEAP_DIR_COUNT;
                  break;
              }
         }
         /* 找到了一个随机的pos,现在随机生成 */
         if (list[pos] == NULL) {
             uint32_t size_block = APP_SYS_HEAP_DIR_BASE;
             if (rand() % 2 == 0)
                 size_block += rand() % (APP_SYS_HEAP_DIR_BASE / 2);
             else
                 size_block -= rand() % (APP_SYS_HEAP_DIR_BASE / 2);
             list[pos] = app_sys_heap_dir_alloc(&heap_dir, size_block, rand() % 2);
             /* 首地址对齐检查 */
             APP_SYS_ASSERT(app_sys_align_check(list[pos], sizeof(uintptr_t)));
         }
    }
    app_sys_heap_dir_check(&heap_dir);
    /* 然后随机放回 */
    for (uint32_t idx = 0; idx < APP_SYS_HEAP_DIR_COUNT; idx++) {
         app_sys_heap_dir_free(&heap_dir, list[idx]);
         list[idx] = NULL;
    }
    app_sys_heap_dir_check(&heap_dir);
    
    
    
    /* 将内存尽可能的榨干 */
    for (uint32_t idx = 0; idx < APP_SYS_HEAP_DIR_COUNT; idx++) {
         uint32_t pos = 0, bse = rand() % APP_SYS_HEAP_DIR_COUNT;
         for (uint32_t ofs = 0; ofs < APP_SYS_HEAP_DIR_COUNT; ofs++) {
              if (list[ (ofs + bse) % APP_SYS_HEAP_DIR_COUNT] == NULL) {
                  pos = (ofs + bse) % APP_SYS_HEAP_DIR_COUNT;
                  break;
              }
         }
         /* 找到了一个随机的pos,现在随机生成 */
         if (list[pos] == NULL) {
             uint32_t size_block = APP_SYS_HEAP_DIR_BASE;
             if (rand() % 2 == 0)
                 size_block += rand() % (APP_SYS_HEAP_DIR_BASE / 2);
             else
                 size_block -= rand() % (APP_SYS_HEAP_DIR_BASE / 2);
             uint32_t size_align = rand() % 3;
             list[pos] = app_sys_heap_dir_alloc_align(&heap_dir, size_block, align[size_align], rand() % 2);
             /* 首地址对齐检查 */
             APP_SYS_ASSERT(app_sys_align_check(list[pos], align[size_align]));
         }
    }
    app_sys_heap_dir_check(&heap_dir);
    /* 然后随机放回 */
    for (uint32_t idx = 0; idx < APP_SYS_HEAP_DIR_COUNT; idx++) {
         app_sys_heap_dir_free(&heap_dir, list[idx]);
         list[idx] = NULL;
    }
    app_sys_heap_dir_check(&heap_dir);
    
    
    
    app_mem_free(buffer);
}
