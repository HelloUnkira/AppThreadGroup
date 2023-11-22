/*实现目标:
 *    一级隔离策略分配器测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

void app_sys_mem_olsf_test(void)
{
    #define APP_SYS_MEM_OLSF_COUNT     (1024)
    #define APP_SYS_MEM_OLSF_BASE      (512)
    #define APP_SYS_MEM_OLSF_LOOP      (1024)

    uintptr_t align[] = {sizeof(uintptr_t), sizeof(uintptr_t) * 2, sizeof(uintptr_t) * 4,};
    uintptr_t size = APP_SYS_MEM_OLSF_COUNT * APP_SYS_MEM_OLSF_BASE;
    uint8_t  *buffer = app_mem_alloc(size);
    void     *list[APP_SYS_MEM_OLSF_COUNT] = {0};
    
    app_sys_mem_olsf_t *mem_olsf = app_sys_mem_olsf_ready((void *)buffer, size);
    app_sys_mem_olsf_check(mem_olsf);
    
    
    
    for (uint32_t loop = 0; loop < APP_SYS_MEM_OLSF_LOOP; loop++) {
        /* 将内存尽可能的榨干 */
        for (uint32_t idx = 0; idx < APP_SYS_MEM_OLSF_COUNT; idx++) {
             uint32_t pos = 0, bse = rand() % APP_SYS_MEM_OLSF_COUNT;
             for (uint32_t ofs = 0; ofs < APP_SYS_MEM_OLSF_COUNT; ofs++) {
                  if (list[ (ofs + bse) % APP_SYS_MEM_OLSF_COUNT] == NULL) {
                      pos = (ofs + bse) % APP_SYS_MEM_OLSF_COUNT;
                      break;
                  }
             }
             /* 找到了一个随机的pos,现在随机生成 */
             if (list[pos] == NULL) {
                 uint32_t size_block = APP_SYS_MEM_OLSF_BASE;
                 if (rand() % 2 == 0)
                     size_block += rand() % (APP_SYS_MEM_OLSF_BASE / 2);
                 else
                     size_block -= rand() % (APP_SYS_MEM_OLSF_BASE / 2);
                 list[pos] = app_sys_mem_olsf_alloc(mem_olsf, size_block);
                 /* 首地址对齐检查 */
                 APP_SYS_ASSERT(app_sys_align_check(list[pos], sizeof(uintptr_t)));
             }
        }
        #if APP_SYS_MEM_OLSF_LOOP == 1
        app_sys_mem_olsf_check(mem_olsf);
        #endif
        /* 然后随机放回 */
        for (uint32_t idx = 0; idx < APP_SYS_MEM_OLSF_COUNT; idx++) {
             app_sys_mem_olsf_free(mem_olsf, list[idx]);
             list[idx] = NULL;
        }
        #if APP_SYS_MEM_OLSF_LOOP == 1
        app_sys_mem_olsf_check(mem_olsf);
        #endif
        
        /* 将内存尽可能的榨干 */
        for (uint32_t idx = 0; idx < APP_SYS_MEM_OLSF_COUNT; idx++) {
             uint32_t pos = 0, bse = rand() % APP_SYS_MEM_OLSF_COUNT;
             for (uint32_t ofs = 0; ofs < APP_SYS_MEM_OLSF_COUNT; ofs++) {
                  if (list[ (ofs + bse) % APP_SYS_MEM_OLSF_COUNT] == NULL) {
                      pos = (ofs + bse) % APP_SYS_MEM_OLSF_COUNT;
                      break;
                  }
             }
             /* 找到了一个随机的pos,现在随机生成 */
             if (list[pos] == NULL) {
                 uint32_t size_block = APP_SYS_MEM_OLSF_BASE;
                 if (rand() % 2 == 0)
                     size_block += rand() % (APP_SYS_MEM_OLSF_BASE / 2);
                 else
                     size_block -= rand() % (APP_SYS_MEM_OLSF_BASE / 2);
                 uint32_t size_align = rand() % 3;
                 list[pos] = app_sys_mem_olsf_alloc_align(mem_olsf, size_block, align[size_align]);
                 /* 首地址对齐检查 */
                 APP_SYS_ASSERT(app_sys_align_check(list[pos], align[size_align]));
             }
        }
        #if APP_SYS_MEM_OLSF_LOOP == 1
        app_sys_mem_olsf_check(mem_olsf);
        #endif
        /* 然后随机放回 */
        for (uint32_t idx = 0; idx < APP_SYS_MEM_OLSF_COUNT; idx++) {
             app_sys_mem_olsf_free(mem_olsf, list[idx]);
             list[idx] = NULL;
        }
        #if APP_SYS_MEM_OLSF_LOOP == 1
        app_sys_mem_olsf_check(mem_olsf);
        #endif
    }
    app_sys_mem_olsf_check(mem_olsf);
    
    
    
    app_mem_free(buffer);
}
