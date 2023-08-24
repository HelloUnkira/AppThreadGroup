/*实现目标:
 *    环形队列测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 环形队列测试
 *       迭代位置过大,需要单独测试索引
 */
void app_sys_rbuf_test_rewind(void)
{
    for (uint32_t idx = 0; idx < 5; idx++)
        if (idx >= 0x80000000) {
            uint32_t rewind = 0x80000000;
            rewind /= 1000;
            rewind *= 1000;
            APP_SYS_LOG_INFO("idx:%u rewind:%u max:%u", idx, rewind, 0x80000000);
            idx -= rewind;
            APP_SYS_LOG_INFO("idx:%u rewind:%u max:%u", idx, rewind, 0x80000000);
        }
}

/*@brief 环形队列测试
 */
void app_sys_rbuf_test(void)
{
    //暴力测试索引回退算法正确性
    //app_sys_rbuf_test_rewind();
    
    app_sys_rbuf_t rbuf1;
    app_sys_rbuf_t rbuf2;
    app_sys_rbuf_t rbuf4;
    app_sys_rbuf_t rbuf8;
    
    uint8_t  buffer1[1024] = {0}, temp1[1024] = {0};
    uint16_t buffer2[1024] = {0}, temp2[1024] = {0};
    uint32_t buffer4[1024] = {0}, temp4[1024] = {0};
    uint64_t buffer8[1024] = {0}, temp8[1024] = {0};
    
    app_sys_rbuf_ready(&rbuf1, 1, buffer1, 1024);
    app_sys_rbuf_ready(&rbuf2, 2, buffer2, 1024);
    app_sys_rbuf_ready(&rbuf4, 4, buffer4, 1024);
    app_sys_rbuf_ready(&rbuf8, 8, buffer8, 1024);
    
    for (uint32_t idx = 0; idx < 1024; idx++) {
        temp1[idx] = idx;
        temp2[idx] = idx;
        temp4[idx] = idx;
        temp8[idx] = idx;
    }
    
    for (uint32_t idx = 0; idx < 10; idx++) {
        /* 暴力测试 */
        app_sys_rbuf_puts(&rbuf1, temp1, 1024);
        app_sys_rbuf_puts(&rbuf2, temp2, 1024);
        app_sys_rbuf_puts(&rbuf4, temp4, 1024);
        app_sys_rbuf_puts(&rbuf8, temp8, 1024);

        app_sys_rbuf_gets(&rbuf1, temp1, 1024);
        app_sys_rbuf_gets(&rbuf2, temp2, 1024);
        app_sys_rbuf_gets(&rbuf4, temp4, 1024);
        app_sys_rbuf_gets(&rbuf8, temp8, 1024);
    }
}
