/*实现目标:
 *    转存到外存的追踪日志信息测试
 */

#define APP_SYS_LOG_RECORD_LIMIT    1
#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 日志追踪队列转储和加载测试
 *       此测试模式只能单独使用
 */
void app_sys_log_text_test(void)
{
    /* 复位日志追踪队列 */
    app_sys_log_text_reset();
    
    /* 改一下这里即可 */
    uint32_t time_s = 5;        // s
    uint32_t peroid = 1000;     // us
    uint32_t target = 1000 * 1000 / peroid;  //1s执行次数(无需修改)
    
    /* 测试(注意:I/O可能被占满,导致时间不准) */
    for (uint32_t count = 0; count < time_s * target; count++) {
        
        if (count % target == 0)
            APP_SYS_LOG_WARN("log test count:%d", count);
        
        static uint32_t offset = 0;
        uint8_t log_text_i[APP_SYS_LOG_TEXT_LIMIT * 2] = {0};
        uint8_t log_text_o[APP_SYS_LOG_TEXT_LIMIT * 2] = {0};
        
        for (uint32_t idx = 0; idx < APP_SYS_LOG_TEXT_LIMIT; idx++)
            log_text_i[idx] = log_text_i[idx + APP_SYS_LOG_TEXT_LIMIT] = '0' + idx % 10;
        
        app_sys_log_text_dump(log_text_i + offset, true);
        app_sys_log_text_load(log_text_o);
        
        if (memcmp(log_text_o, log_text_i + offset, APP_SYS_LOG_TEXT_LIMIT) != 0)
            APP_SYS_LOG_ERROR("log error");
        
        offset += 1;
        offset %= APP_SYS_LOG_TEXT_LIMIT;
        
        app_delay_us(peroid);
    }
}
