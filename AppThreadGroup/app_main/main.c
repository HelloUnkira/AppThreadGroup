
#include "app_func_test.h"

int main(int argc, char *argv[])
{
    /* 启动APP调度策略 */
    app_thread_group_schedule();
    /* 主线程滚动阻塞 */
    while (true) {
        /* 测试中我们在主线程 */
        /* 模拟发送1ms定时器中断事件 */
        /* 我是实在没想到这种方式 */
        /* 居然是最简单的做法...... */
        app_main_1ms_loop();
    }
    return 0;
}
