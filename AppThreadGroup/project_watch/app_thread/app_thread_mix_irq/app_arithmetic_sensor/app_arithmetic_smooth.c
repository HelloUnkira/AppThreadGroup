/*实现目标:
 *    算法组件:平滑滤波器
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_arithmetic_lib.h"

/*@brief 平滑滤波器重置
 *@param smooth 实例
 */
void app_arithmetic_smooth_reset(app_arithmetic_smooth_t *smooth)
{
    memset(smooth, 0, sizeof(app_arithmetic_smooth_t));
}

/*@brief 平滑滤波器重置
 *@param smooth 实例
 *@param data   新数据
 *@retval 滤波后数据
 */
int32_t app_arithmetic_smooth_execute(app_arithmetic_smooth_t *smooth, int32_t data)
{
    /* 重置当前数据,更新为第一个 */
    if (smooth->sta == false) {
        memset(smooth, 0, sizeof(app_arithmetic_smooth_t));
        smooth->sta  = true;
        smooth->sum  = data * APP_ARITHMETIC_SMOOTH_SIZE;
        for(uint8_t idx = 0; idx < APP_ARITHMETIC_SMOOTH_SIZE; idx++)
            smooth->buf[idx] = data;
    }
    /* 更新索引 */
    if (smooth->pos >= APP_ARITHMETIC_SMOOTH_SIZE)
        smooth->pos  = 0;
    /* 将旧数据更新为新数据然后迭代,计算均值 */
    smooth->sum -= smooth->buf[smooth->pos];
    smooth->sum += smooth->buf[smooth->pos] = data;
    smooth->avg  = smooth->sum / APP_ARITHMETIC_SMOOTH_SIZE;
    smooth->pos++;
    
    return smooth->avg;
}
